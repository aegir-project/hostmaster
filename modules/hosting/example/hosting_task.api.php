<?php

/**
 * @file
 * Hooks provided by the hosting tasks module.
 */

/**
 * @addtogroup backend-frontend-IPC
 * @{
 */

/**
 * Define tasks that can be executed in the front-end.
 *
 * @return
 *   An array of arrays of tasks that can be executed by the front-end.
 *   The keys of the outer array should be the object that tasks operate on, for
 *   example 'site', 'platform' or 'server'. The values of the outer array
 *   should be an array of tasks keyed by task type, the value should be an
 *   array that defines the task. Valid keys for defining tasks are:
 *   - 'title': (required) The human readable name of the task.
 *   - 'description': (optional) The human readable description of the task.
 *   - 'weight': (optional) The weight of the task when displayed in lists.
 *   - 'dialog' (optional) Set to TRUE to indicate that this task requires a
 *      dialog to be shown to the user to confirm the execution of the task.
 *   - 'hidden' (optional) Set to TRUE to hide the task in the front-end UI, the
 *      task will still be available for execution by the front-end however.
 *   - 'access callback' (optional) An access callback to determine if the user
 *      can access the task, defaults to 'hosting_task_menu_access'.
 *
 * @see hosting_available_tasks()
 */
function hosting_example_hosting_tasks() {
  // From hosting_clone_hosting_tasks().
  $options = array();

  $options['site']['clone'] = array(
    'title' => t('Clone'),
    'description' => t('Make a copy of a site.'),
    'weight' => 5,
    'dialog' => TRUE,
  );
  return $options;
}

/**
 * @} End of "addtogroup hooks".
 */
