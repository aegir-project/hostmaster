<?php

/**
 * Main queue processing command for hostmaster.
 *
 * This is a single command, which will (based on configuration) run all the other
 * queue commands (cron, backup, tasks, stats). This is done so that there
 * is only one cron job to configure, and allow the frequency of calls to be configured
 * from the interface.
 */
function hosting_dispatch() {
  $now = mktime();
  variable_set("hosting_dispatch_last_run", $now);
  if (variable_get('hosting_dispatch_enabled', false)) {
    $queues = hosting_get_queues();
    foreach ($queues as $queue => $info) {
      if ($info['enabled']) {
        if (($now - $info["last"]) >= $info["calc_frequency"]) {
          system(hosting_generate_command("hosting", $queue, array(),
            array('items' => $info['calc_items'], 'background' => TRUE)));
        }
      }
    }
  }
}

/**
 * Retrieve a list of queues that need to be dispatched
 *
 * Generate a list of queues, and the frequency / amount of items
 * that need to be processed for each of them.
 */
function hosting_get_queues($refresh = false) {
  static $cache = null;

  if (is_null($cache) || $refresh) {
    $cache = array();
    $defaults = array(
      'type' => 'serial',
      'max_threads' => 6,
      'threshold' => '100',
      'min_threads' => 1,
      'timeout' => strtotime("10 minutes", 0),
      'frequency' => strtotime("5 minutes", 0),
      'items' => 5,
      'enabled' => TRUE,
      'singular' => t('item'),
      'plural' => t('items')
    );  
    $queues = module_invoke_all("hosting_queues");
    foreach ($queues as $key => $queue) {
      $queue = array_merge($defaults, $queue);

      // Configurable settings. 
      $configured = array(
        'frequency' => variable_get('hosting_queue_' . $key . '_frequency', $queue['frequency']),
        'items' => variable_get('hosting_queue_' . $key . '_items', $queue['items']),
        'enabled' => variable_get('hosting_queue_' . $key . '_enabled', $queue['enabled']),
        'last_run' => variable_get('hosting_queue_' . $key . '_last_run', false),
        'running' => variable_get('hosting_queue_' . $key . '_running', false),
        'interval' => variable_get('hosting_queue_' . $key . '_interval', false),
      );
      $queue = array_merge($queue, $configured);

      if ($queue['type'] == 'batch') {
        $threads = $queue['total_items'] / $queue['threshold'];
        if ($threads <= $queue['min_threads']) {
          $threads = $queue['min_threads'];
        } elseif ($thread > $queue['max_threads']) {
          $threads = $queue['max_threads'];
        }
        $queue['calc_threads'] = $threads;
        $queue['calc_frequency'] = ceil($queue['frequency'] / $threads);
        $queue['calc_items'] = ceil($queue['total_items'] / $threads);
      }
      else {
        $queue['calc_frequency'] = $queue['frequency'];
        $queue['calc_items'] = $queue['items'];
      }

      $queue['last'] = variable_get('hosting_queue_' . $key . '_last_run', 0);
      $queues[$key] = $queue;
    }
    $cache = $queues;
  }
  
  return $cache;
}


/**
 * Implementation of hook_hosting_queues
 *
 * Return a list of queues that this module needs to manage.
 */
function hosting_hosting_queues() {
  $queue['tasks'] = array(
    'name' => t('Task queue'),
    'description' => t('Process the queue of outstanding hosting tasks.'),
    'type' => 'serial',  # run queue sequentially. always with the same parameters.
    'frequency' => strtotime("5 minutes", 0),  # run queue every 300 seconds (5 minutes)
    'items' => 5, # process 5 queue items per execution.
    'total_items' => hosting_task_count(),
    'singular' => t('task'),
    'plural' => t('tasks'),
  );
  return $queue;
}

/**
 * Run a queue specified by hook_hosting_queues
 *
 * Run an instance of a queue processor. This function contains all the book keeping
 * functionality needed to ensure that the queues are running as scheduled.
 */
function hosting_run_queue() {
  global $args;
  $queue = $args['commands'][1];
  $count = drush_get_option(array('i', 'items'), 5); # process a default of 5 items at a time.

  variable_set('hosting_queue_' . $queue . '_last_run', $t = mktime());
  variable_set('hosting_queue_' . $queue . '_running', $t);

  $func = "hosting_" . $queue . "_queue";

  if (function_exists($func)) {
    $func($count);
  }

  variable_del('hosting_queue_' . $queue . '_running');
}

/**
 * Generate a drush command.
 *
 * Generates a drush command for either the provision or hosting system.
 */
function hosting_generate_command($system, $command, $args = array(), $options = array(), $ref = null) {
  if (is_null($ref)) {
    $ref = node_load(HOSTING_OWN_PLATFORM);
  }

  $data = hosting_map_values($ref);
  $data = array_merge($data, $options);

  // Hack : add site to arguments if we're generating a command for a site.
  if ($data['site_url']) {
    $args[] = $data['site_url'];
  }

  $func = "hosting_cmd_" . $system . "_" . $command;
  if (function_exists($func)) {
    $func($args, $options, $data);
  }

  //escape shell argument
  foreach ($args as $key => $arg) {
    $args[$key] = escapeshellarg($arg); 
  }

  /**
   * A list of keys in $data that will be exported to the command line.
   *
   * This is defined in the map_values function for each of the types, and by
   * whatever $options were passed to the functions.
   */
  $export = array_merge($data['#export_' . $ref->type], array_keys($options));
  foreach ($export as $key) {
    if (!is_array($data[$key]) && !is_object($data[$key]) && !is_bool($data[$key]) && $data[$key]) {
      $option_str .= " --$key=" . escapeshellarg($data[$key]);
    }
  } 
 
  if ($options['backend']) {
    $option_str .= " --backend";
  }
  $drush_path = sprintf("%s/%s/drush.php", $data['publish_path'], drupal_get_path("module", "drush")); 
  // @TODO: Implement proper multi platform / multi server support.
  $cmd = sprintf(escapeshellcmd("%s %s %s %s --root=%s --uri=%s %s"), escapeshellcmd($drush_path) ,
    escapeshellarg($system), escapeshellarg($command), implode(" ", $args), escapeshellarg($data['publish_path']),
    escapeshellarg($data['web_host']), $option_str);

  if ($options['background']) {
    $cmd .= " &";
  }
  return $cmd;
}

function hosting_cmd_provision_restore(&$args, &$options, $data) {
  $backup = hosting_site_get_backup($data['task_args']['bid']);
  $args[] = $backup['filename'];
}


/**
 * Retrieve a list of outstanding tasks.
 *
 * @param limit
 *   The amount of items to return.
 * @return
 *   An associative array containing task nodes, indexed by node id.
 */
function _hosting_get_new_tasks($limit = 5) {
  $return = array();
  $result = db_query("SELECT nid FROM {hosting_task_queue} WHERE status=%d ORDER BY timestamp ASC LIMIT %d", PROVISION_QUEUED, $limit);
  while ($node = db_fetch_object($result)) {
    $return[$node->nid] =  node_load($node->nid);
  }
  return $return;
}

/**
 * Process the hosting task queue.
 *
 * Iterates through the list of outstanding tasks, and execute the commands on the back end.
 */
function hosting_tasks_queue($count) {
  global $provision_errors;

  $tasks = _hosting_get_new_tasks($count);

  foreach ($tasks as $task) {
    // create a new revision
    $task->changed = mktime();
    $task->executed = mktime();
    $task->revision = true;
    node_save($task);

    hosting_task_log($task->vid, "queue", t("Task starts processing"));

    // execute the hosting_pre_${task_type} hook.
    module_invoke_all("hosting_pre_" . $task->task_type, $task);

    // @todo Allow multiple commands to be run. 
    // For now tasks just map to one backend command, but that might need to change with additional complexity.
    $cmd = hosting_generate_command("provision", $task->task_type, array(), array('backend' => TRUE), $task);
    exec($cmd, $return, $code);
    $return = join("\n", $return);
    $data = unserialize($return);

    hosting_task_log($task->vid, 'command', escapeshellcmd("$cmd"));

    if (!is_array($data)) {
      hosting_task_log($task->vid, 'error', t("The command could not be executed succesfully (return: %return, data: %data, code: %code)", array("%return" => $return, "%data" => $data, "%code" =>  $code)));
      $code = PROVISION_FRAMEWORK_ERROR;
    }

    foreach ((array) $data['log'] as $log) {
      hosting_task_log($task->vid, $log['type'], $log['message'], $log['severity'], $log['timestamp']);
    }
    
    # Drupal message errors.
    foreach ((array) $data['messages']['error'] as $error) {
      if (preg_match("/^warning:/", $error)) {
        hosting_task_log($task->vid, "warning", ereg_replace("/^warning: /", '', $error), 0, $log['timestamp']);  
      }
      elseif (preg_match("/^user warning:/", $error)) {
        hosting_task_log($task->vid, "warning", ereg_replace("/^user warning: /", '', $error), 0, $log['timestamp']);  
      }
    }
    // record status 
    $task->task_status = $code;
    // New revision is created at the beginning of function.
    $task->revision = false;
    node_save($task);

    if ($code == PROVISION_SUCCESS) {
      # The task has been successful. Run the post hook.
      module_invoke_all("hosting_post_" . $task->task_type, $task, $data);

      // remove from queue
      hosting_task_log($task->vid, "queue", t("Removing task from hosting queue"), 0);
      db_query("UPDATE {hosting_task_queue} SET status=%d WHERE nid=%d", $code, $task->nid);
    }
  }
}

/**
 * Page callback
 *
 * Configure the frequency of tasks.
 */
function hosting_queues_configure() {
  drupal_add_css(drupal_get_path('module', 'hosting') . '/hosting.css');
  $units = array(
    strtotime("1 second", 0) => t("Seconds"),
    strtotime("1 minute", 0) => t("Minutes"),
    strtotime("1 hour", 0) => t("Hours"),
    strtotime("1 day", 0) => t("Days"),
    strtotime("1 week", 0) => t("Weeks"),
  );
  
  $queues = hosting_get_queues();
  $form['#tree'] = TRUE;

  foreach ($queues as $queue => $info) {
    $form[$queue]['description'] = array(
      '#type' => 'item',
      '#value' => $info['name'],
      '#description' => $info['description']
    );

    $form[$queue]["enabled"] = array(
      '#type' => 'checkbox',
      '#default_value' => $info['enabled']
    );

    $form[$queue]["last_run"] = array(
      '#value' => hosting_format_interval(variable_get('hosting_queue_' . $queue . '_last_run', false))
    );
    $form[$queue]['frequency']['#prefix'] = "<div class='hosting-queue-frequency'>";
    $form[$queue]['frequency']['#suffix'] = "</div>";
    
    if ($info['type'] == 'batch') {
      $form[$queue]['frequency']['items'] = array(
        '#value' => t('%count %items every ', array("%count" => $info['calc_items'], "%items" => format_plural($info['calc_items'], $info['singular'], $info['plural']))),
      ); 
    }
    else {
      $form[$queue]['frequency']['items'] = array(
        '#type' => 'textfield',
        '#size' => 3,
        '#maxlength' => 3,
        '#default_value' => $info['items'],
        '#suffix' => t(' %items every ', array('%items' => $info['plural'])),
      );
    }
    foreach (array_reverse(array_keys($units)) as $length) {
      $unit = $units[$length];
      
      if (!($info['frequency'] % $length)) {
        $frequency_ticks = $info['frequency'] / $length;
        $frequency_length = $length;
        break;
      }
    }
    $form[$queue]['frequency']["ticks"] = array(
      '#type' => 'textfield',
      '#default_value' => $frequency_ticks,
      '#maxlength' => 5,
      '#size' => 5
    );
    $form[$queue]['frequency']["unit"] = array(
      '#type' => 'select',
      '#options' => $units,
      '#default_value' => $frequency_length,
    );
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));
  return $form;
}

function theme_hosting_queues_configure($form) {
  $queues = hosting_get_queues();

  $rows = array();
  $header = array('', t('Description'), 
    array('data' => t('Frequency'), 'class' => 'hosting-queue-frequency-head'),
    t('Last run'),);
  foreach ($queues as $key => $info) {
    $row = array();
    $row[] = drupal_render($form[$key]['enabled']); 
    $row[] = drupal_render($form[$key]['description']); 
    $row[] = drupal_render($form[$key]['frequency']);
    $row[] = drupal_render($form[$key]['last_run']); 
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form['submit']);
  $output .= drupal_render($form);
  return $output;
}

function hosting_queues_configure_submit($form_id, $values) {
  foreach (hosting_get_queues() as $queue => $info) {
    if ($values[$queue]) {
      variable_set("hosting_queue_" . $queue . "_enabled", $values[$queue]['enabled']);
      variable_set("hosting_queue_" . $queue . "_frequency", $values[$queue]['frequency']['ticks'] * $values[$queue]['frequency']['unit']);
      if ($info['type'] == 'serial') {
        variable_set("hosting_queue_" . $queue . "_items", $values[$queue]['items']);
      }

    }
  }
}

function hosting_queue_drush_path($platform) {


}

function hosting_queues_cron_cmd() {
  $drush_path = sprintf("%s/%s/drush.php", PROVISION_DOCROOT_PATH, drupal_get_path('module', 'drush'));
  $cron_cmd = sprintf("*/1 * * * * (php %s hosting dispatch --uri=%s --root=%s)", 
    escapeshellarg($drush_path), escapeshellarg(PROVISION_DOMAIN), escapeshellarg(PROVISION_DOCROOT_PATH));
  return $cron_cmd;
}
