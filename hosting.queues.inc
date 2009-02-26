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
  hosting_log('hosting_dispatch', t("dispatching queues"));
  $platform = node_load(HOSTING_OWN_PLATFORM);
  $root = $platform->publish_path;

  if (variable_get('hosting_dispatch_enabled', false)) {
    $queues = hosting_get_queues();
    foreach ($queues as $queue => $info) {
      if ($info['enabled']) {
        if (($now - $info["last"]) >= $info["calc_frequency"]) {
          $data = array('items' => $info['calc_items'], 'root' => $root, 'quiet' => TRUE);
          $cmd = _drush_backend_generate_command("hosting " . $queue, $data);
          $cmd .= ' &';

          hosting_log('hosting_dispatch', t("dispatching command !cmd", array('!cmd' => $cmd)));
          system($cmd);
        } else {
          hosting_log(false, t("too early for queue @queue", array('@queue' => $queue)));
        }
      } else {
        hosting_log(false, t("queue @queue disabled", array('@queue' => $queue)));
      }
    }
  } else {
    hosting_log('hosting_dispatch', t("dispatching disabled"));
  }
  exit(DRUSH_SUCCESS);
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
  exit(DRUSH_SUCCESS);
}

/**
 * Generate a drush command.
 *
 * Generates a drush command for either the provision or hosting system.
 */
function hosting_generate_command($task) {
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
  $result = db_query("SELECT nid FROM {hosting_task_queue} WHERE status=%d ORDER BY timestamp, nid ASC LIMIT %d", DRUSH_NOT_COMPLETED, $limit);
  while ($node = db_fetch_object($result)) {
    $return[$node->nid] =  node_load($node->nid);
  }
  return $return;
}

/**
 * Check if a certain task is still in the queue
 * 
 * @param nid
 *    The nid of the task being checked
 * @return
 *    A boolean depending on whether the task is still queued.
 */

function _hosting_task_in_queue($nid) {
  return (db_result(db_query("SELECT nid FROM {hosting_task_queue} WHERE status=%d AND nid=%d", DRUSH_NOT_COMPLETED, $nid))) ? TRUE : FALSE;
}
/**
 * Process the hosting task queue.
 *
 * Iterates through the list of outstanding tasks, and execute the commands on the back end.
 */
function hosting_tasks_queue($count = 20) {
  global $provision_errors;

  hosting_log('hosting_tasks', t("Running tasks queue"));
  $tasks = _hosting_get_new_tasks($count);

  foreach ($tasks as $task) {
    hosting_process_task($task);
  }
}

function hosting_process_task($task) {
  if (is_numeric($task)) {
    $task = node_load($task);
  }
  if (($task->type == 'task') && ($task->task_status != DRUSH_SUCCESS)) {
    // create a new revision
    $task->changed = mktime();
    $task->executed = mktime();
    $task->revision = true;
    node_save($task);
    hosting_task_log($task->vid, "queue", t("Task starts processing"));

    // execute the hosting_pre_${task_type} hook.
    module_invoke_all("hosting_pre_" . $task->task_type, $task);

    $data = module_invoke_all('provision_args', $task, $task->task_type); 

    $hostname = null;
    $username = null;

    if (isset($data['web_id']) && ($data['web_id'] != HOSTING_OWN_WEB_SERVER)) {
      $username = $data['script_user'];
      $hostname = $data['web_host'];
    }
    $drush_path = $data['drush_path'];
    $data['root'] = $data['publish_path'];

    $cmd = _drush_backend_generate_command("provision " . $task->task_type, $data, 'GET', $drush_path, $hostname, $username);
    hosting_task_log($task->vid, 'command', 'Running:' . escapeshellcmd("$cmd"));

    $output = _drush_backend_invoke($cmd, $data, FALSE);

    if (!is_array($output)) {
      hosting_task_log($task->vid, 'error', t("The command could not be executed succesfully"));
      hosting_task_log($task->vid, 'notice', $output);
      $code = DRUSH_FRAMEWORK_ERROR;
    }
    else {
      $code = $output['error_status'];
    }

    _hosting_backend_integrate($task, $output);

    // record status 
    $task->task_status = $code;
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
    hosting_task_log($task->vid, "queue", t("Removing task from hosting queue"), 0);
    db_query("UPDATE {hosting_task_queue} SET status=%d WHERE nid=%d", $code, $task->nid);
  }
}

function _hosting_backend_integrate($task, $data = array()) {
  if (is_array($data)) {
    foreach ((array) $data['log'] as $log) {
      hosting_task_log($task->vid, $log['type'], $log['message'], $log['timestamp']);
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

