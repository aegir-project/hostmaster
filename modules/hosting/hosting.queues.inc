<?php

/**
 * @file
 *   This file defines an API for defining new queues.
 */

/**
 * @defgroup backend-frontend-IPC
 * @{
 * Aegir is based on a very important distinction between the
 * "frontend" and the "backend". The frontend is the Drupal website
 * that creates the user interface that people manipulate. The backend
 * is (usually) a set a drush commands that do the actual operations
 * on the backend objects (servers, platforms, sites).
 *
 * The frontend and backend run as different users: the frontend runs
 * under the webserver user, while the backend run as a separate UNIX
 * user, which we call the backend sandbox. This distinction is
 * important as it brings an extra layer of security. It also allows
 * for commands to be ran on different servers more easily.
 *
 * The way the frontend communicates to the backend is through the
 * use of "queues", that are actually defined in the frontend
 * (hosting). The frontend also define drush commands that are ran
 * through a cronjob and therefore provide the glue to the backend.
 *
 * The point of entry is through the "dispatcher" (the
 * hosting-dispatch command), which runs the different queues
 * appropriately. There are different types of queues (see
 * hook_hosting_queues()), but the most important one is the "task"
 * queue, which runs the basic tasks like install and backup.
 *
 * When tasks are ran, a provision command is called with the same
 * name of the task type defined in the frontend (see
 * drush_hosting_task()). If we are running a special task like
 * install, verify or import, data from the task is actually saved in
 * the backend, through the backend provision-save command. The data
 * saved is from the associative array "context_options", defined from
 * the hook see hook_hosting_TASK_OBJECT_context_options(). A good
 * example of such a variable is the site name or platform it is on,
 * which are obviously saved in the backend.
 *
 * There is also an "options" array that are just one-time parameters
 * that are not stored in the backend context. A good example of that
 * is the site email address, that is just sent during the install or
 * password resets task but not saved.
 */

/**
 * Retrieve a list of queues that need to be dispatched
 *
 * Generate a list of queues, and the frequency / amount of items
 * that need to be processed for each of them.
 */
function hosting_get_queues($refresh = FALSE) {
  static $cache = NULL;

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
        'last_run' => variable_get('hosting_queue_' . $key . '_last_run', FALSE),
        'running' => variable_get('hosting_queue_' . $key . '_running', FALSE),
        'interval' => variable_get('hosting_queue_' . $key . '_interval', FALSE),
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
 * Run a queue specified by hook_hosting_queues()
 *
 * Run an instance of a queue processor. This function contains all the book keeping
 * functionality needed to ensure that the queues are running as scheduled.
 */
function hosting_run_queue() {
  $cmd = drush_get_command();
  $queue = $cmd['queue'];
  $count = drush_get_option(array('i', 'items'), 5); // process a default of 5 items at a time.

  variable_set('hosting_queue_' . $queue . '_last_run', $t = time());
  variable_set('hosting_queue_' . $queue . '_running', $t);

  $func = "hosting_" . $queue . "_queue";

  if (function_exists($func)) {
    $func($count);
  }

  variable_del('hosting_queue_' . $queue . '_running');
}

/**
 * Calculate the time at which the task queue will run next.
 *
 * There are two possibilities here - the task was never run, in which
 * case we compute the next time it was run.
 *
 * The second case is the case where the task wasn't run in the last
 * queue run even though it was scheduled, so we compute the next
 * iteration.
 *
 * @param $queue mixed
 *   the queue name or a queue array as returned by
 *   hosting_get_queues()
 *
 * @return integer
 *   the date at which the queue will run as a number of seconds since
 *   the UNIX epoch
 */
function _hosting_queue_next_run($queue) {
  if (!is_array($queue)) {
    $queues = hosting_get_queues();
    $queue = $queues[$queue];
  }
  $freq = $queue['frequency'];
  $last = $queue['last_run'];
  $now = time();
  return max($last + $freq,
    ( $now - ( ($now - $last) % $freq ) ) + $freq );
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

/**
 * @} End of "defgroup backend-frontend-IPC".
 */
