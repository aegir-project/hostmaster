<?php


function _hosting_watchdog($entry) {
  watchdog($entry['type'], $entry['message']);
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
  $result = db_query("SELECT t.nid FROM {hosting_task} t INNER JOIN {node} n ON t.vid = n.vid WHERE t.task_status = %d ORDER BY n.changed, n.nid ASC LIMIT %d", 0, $limit);
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
    drush_backend_fork("hosting-task", array($task->nid));
  }
}

function hosting_queues_get_arguments($task) {
  $data = module_invoke_all('provision_args', $task, $task->task_type); 
  foreach ($data as $key => $value) {
    if (substr($key, 0, 1) == '#') {
      $data[(int) str_replace('#', '', $key)] = $value;
      unset($data[$key]);
    }
  }
  
  ksort($data);
  return $data;
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
  $cmd = sprintf("%s %s hosting-dispatch ", DRUSH_COMMAND, escapeshellarg(d()->name));
  return $cmd;
}

function hosting_queues_cron_cmd() {
  $command = _hosting_dispatch_cmd();
  $return = <<<END
SHELL=/bin/bash
PATH=$_SERVER[PATH]
*/1 * * * * $command
END;
  return $return;
}

