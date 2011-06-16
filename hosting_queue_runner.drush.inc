<?php

/**
 * @file
 * Dispatcher daemon
 *
 * This file is the heart of the dispatcher drush command. It
 * implements most of the backend functionality.
 */

/**
 * Implementation of hook_drush_command().
 */
function hosting_queue_runner_drush_command() {
  $items = array();

  $items['hosting-queue-runner'] = array(
    'description' => 'Runs the tasks queue',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'drupal dependencies' => array(
      'hosting_queue_runner',
    ),
    // We need this get around a drush bug, see: http://drupal.org/node/704848
    // But we remove it below for Drush 4.5 and above.
    'callback' => 'drush_hosting_queue_runner',
  );

  // We don't need the callback defined for Drush > 4.4
  if (defined('DRUSH_VERSION') && version_compare(DRUSH_VERSION, '4.4', '>')) {
    unset($items['hosting-queue-runner']['callback']);
  }

  return $items;
}

/**
 * Drush command to execute hosting tasks.
 */
function drush_hosting_queue_runner() {
  // Set a nice high time limit, if we can:
  if (function_exists('set_time_limit')) {
    @set_time_limit(0);
  }
  $end_time = variable_get('hosting_queue_runner_process_lifetime', 3600)  + time();

  // Record the fact that we're running, so we can give some feedback in the
  // frontend.
  variable_set('hosting_queue_runner_process_started', time());

  drush_log(dt('Starting Hosting queue runner, watching for available tasks'));

  while (TRUE) {
    // Sleep for a second before we look for new tasks.
    sleep(1);

    // Should we terminate.
    if (time() > $end_time) {
      drush_log(dt('Terminating Hosting queue runner (max run time reached)'));
      break;
    }

    // Try to get a single task to run.
    // Note that the node_load called by this function might return an old copy
    // of the node, and so it is not to be trusted!
    $tasks = _hosting_get_new_tasks(1);
    foreach ($tasks as $task) {
      // We sleep for a second just in case others want to run the task first.
      // This guards against other processes that want to add a hosting task
      // with arguments and run it immediately, they should be able to do this
      // without us getting in there first.
      drush_log(dt('Found task to execute. Pausing before execution.'));
      sleep(1);

      // Execute the task in the backend.
      drush_log('Executing task.');
      drush_backend_invoke("hosting-task", array($task->nid));
      drush_log(dt('Finished executing task.'));

      // Delay for a configurable amount of time.
      $delay = variable_get('hosting_queue_runner_post_task_delay', 0);
      if (!empty($delay)) {
        drush_log(dt('Going to sleep for @count seconds after completing task.', array('@count' => $delay)));
        sleep($delay);
      }

      // We're done with this task, set our reference to NULL, this might help
      // reduce memory usage.
      $task = NULL;
    }
    $tasks = NULL;
  }

  // restart the daemon to recycle leaked memory
  hosting_queue_runner_restart();
}

/**
 * Restart the dispatcher to work around memory leaks
 */
function hosting_queue_runner_restart() {
  $drush_command = drush_get_command();
  // should be hosting-queue-runner unless something is very wrong
  $command = $drush_command['command'];
  $options = drush_get_context('cli');

  if (function_exists('pcntl_exec')) {
    // this would usually be:
    // _drush_backend_generate_command_sitealias(drush_sitealias_get_record(d()->name), 'hosting-queue-runner', array(), $data);
    // but that generates --backend or > `tty` (if #interactive) which are both wrong
    $parts = explode(' ', DRUSH_COMMAND);
    $drush = array_shift($parts);
    $args = array(); // the arguments after argv0
    // we need to reimplement _drush_escape_option() because it adds
    // extra leading spaces
    foreach ($options as $key => $value) {
      if ($value !== TRUE) {
        $args[] = "--$key=" . escapeshellarg($value);
      }
      else {
        $args[] = "--$key";
      }
    }
    array_push($args, '@hostmaster', $command);
    drush_log(dt('Restarting queue runner with @drush @args', array('@drush' => $drush, '@args' => implode(" ", $args))));
    pcntl_exec($drush, $args);
    drush_log(dt('Exec failed'), 'warning');
    /* NOTREACHED */
  }
  else {
    // Call through to backend fork.
    drush_log(dt('Redispatch via backend fork'));
    drush_backend_fork($command, $options, $drush_path);
  }
}
