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
  drush_log('hosting_dispatch', t("dispatching queues"));
  $platform = node_load(HOSTING_OWN_PLATFORM);
  $root = $platform->publish_path;

  if (variable_get('hosting_dispatch_enabled', false)) {
    $queues = hosting_get_queues();
    foreach ($queues as $queue => $info) {
      if ($info['enabled']) {
        if (($now - $info["last"]) >= $info["calc_frequency"]) {
          drush_backend_fork("hosting", array($queue, 'items' => $info['calc_items']));
        } else {
          drush_log(dt("too early for queue @queue", array('@queue' => $queue)));
        }
      } else {
        drush_log(dt("queue @queue disabled", array('@queue' => $queue)));
      }
    }
  } else {
    drush_log(dt("dispatching disabled"));
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
      $queue['running'] = variable_get('hosting_queue_' . $key . '_running', 0);
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
    'frequency' => strtotime("1 minute", 0),  # run queue every minute.
    'items' => 20, # process 20 queue items per execution.
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
  $cmd = drush_get_command(); 
  $queue = $cmd['queue'];
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
 * Retrieve a list of outstanding tasks.
 *
 * @param limit
 *   The amount of items to return.
 * @return
 *   An associative array containing task nodes, indexed by node id.
 */
function _hosting_get_new_tasks($limit = 20) {
  $return = array();
  $result = db_query("SELECT nid FROM {hosting_task_queue} WHERE status=1 ORDER BY timestamp, nid ASC LIMIT %d", $limit);
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
function hosting_tasks_queue($count = 20) {
  global $provision_errors;

  drush_log(dt("Running tasks queue"));
  $tasks = _hosting_get_new_tasks($count);
  foreach ($tasks as $task) {
     drush_backend_fork("hosting task", array($task->nid));
  }
}

function hosting_process_task($task) {
  if (is_numeric($task)) {
    $task = node_load($task);
  }
  if ($task->type == 'task') {
    // create a new revision
    $task->changed = mktime();
    $task->executed = mktime();
    $task->revision = true;
    node_save($task);
    hosting_task_log($task->vid, "queue", t("Task starts processing"));

    // execute the hosting_pre_${task_type} hook.
    module_invoke_all("hosting_pre_" . $task->task_type, $task);

    $data = hosting_queues_get_arguments($task);

    $hostname = null;
    $username = null;

    if (isset($data['web_id']) && ($data['web_id'] != HOSTING_OWN_WEB_SERVER)) {
      $username = $data['script_user'];
      $hostname = $data['web_host'];
    }
    $drush_path = $data['drush_path'];
    $data['root'] = $data['publish_path'];

    $cmd = _drush_backend_generate_command("provision " . $task->task_type, $data, 'POST', $drush_path, $hostname, $username);
    hosting_task_log($task->vid, 'command', t('Task dispatcher running: %cmd', array('%cmd' => escapeshellcmd("$cmd"))));
    $output = _drush_backend_invoke($cmd, $data, FALSE);
    if (!is_array($output)) {
      hosting_task_log($task->vid, 'error', t("The command could not be executed succesfully"));
      hosting_task_log($task->vid, 'notice', $output);
      $code = DRUSH_FRAMEWORK_ERROR;
    }
    else {
      $code = $output['error_status'];
    }
    $code = ($code != DRUSH_SUCCESS) ? DRUSH_FRAMEWORK_ERROR : DRUSH_SUCCESS;
    drush_log("Task returned $code");

    _hosting_backend_integrate($task, $output);

    // record status 
    $task->task_status = $code;
    $task->queued = 0;
    // New revision is created at the beginning of function.
    $task->revision = false;
    node_save($task);

    if ($code == DRUSH_SUCCESS) {
      # The task has been successful. Run the post hook.
      module_invoke_all("hosting_post_" . $task->task_type, $task, $output);
    }
    else {
      module_invoke_all("hosting_failed_". $task->task_type, $task, $output, $code);
    }
    // remove from queue
    hosting_task_log($task->vid, "queue", t("Removing task from hosting queue"));
  }
}

function hosting_queues_get_arguments($task) {
  $data = module_invoke_all('provision_args', $task, $task->task_type); 
  foreach ($data as $key => $value) {
    if (substr($key, 0, 1) == '#') {
      $data[str_replace('#', '', $key)] = $value;
      unset($data[$key]);
    }
  }
  return $data;
}

function _hosting_backend_integrate($task, $data = array()) {
  if (is_array($data)) {
    foreach ((array) $data['log'] as $log) {
      hosting_task_log($task->vid, $log['type'], $log['message'], $log['error'], $log['timestamp']);
    }
    return $values; 
  }
}

function _hosting_backend_invoke($cmd, $task) {
  $proc = _drush_proc_open($cmd, FALSE);
  if ($proc['output']) {
    $values = drush_backend_parse_output($proc['output'], FALSE);

  }
  return FALSE;
}


function _hosting_queues_clean_output($return) {
  return filter_xss($return, array());
}


function _hosting_dispatch_cmd() {
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $cmd = sprintf("php %s hosting dispatch --root=%s", escapeshellarg($node->drush_path), 
      escapeshellarg(HOSTING_DEFAULT_DOCROOT_PATH));
  return $cmd;
}

function hosting_queues_cron_cmd() {
  return sprintf("*/1 * * * * (%s)", _hosting_dispatch_cmd());
}

