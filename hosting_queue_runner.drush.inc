<?php

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
  );

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

  while (TRUE) {

    // Should we terminate.
    if (time() > $end_time) {
      drush_log('Terminating hosting queue runner (max run time reached)');
      break;
    }

    // Try to get a single task to run.
    $tasks = _hosting_get_new_tasks(1);
    foreach ($tasks as $task) {
      // We sleep for a  couple of seconds just in case others want to run the
      // task first. This guards against other processes that want to add a
      // hosting task with arguments and run it immediately, they should be
      // able to do this without us getting in there first.
      sleep(2);
      // Execute the task on in the backend.
      drush_backend_invoke("hosting-task", array($task->nid));
    }
    // Sleep for a second before we look for new tasks.
    sleep(1);
  }

}
