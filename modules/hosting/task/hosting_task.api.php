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
 *   - 'provision_save' (optional, defaults to FALSE) A flag that tells
 *      provision that a "provision-save" command needs to happen before this
 *      task can be run, used for tasks like Verify, Install, and Import.
 *      If you implement this option, you should implement
 *      hook_hosting_TASK_OBJECT_context_options() in order to pass parameters
 *      to the provision-save command.
 *
 * @see hosting_available_tasks()
 */
function hook_hosting_tasks() {
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
 * Alter front-end tasks defined by other modules.
 *
 * @param $tasks
 *   An array of tasks defined by other modules. Keys of the outer array are the
 *   types of objects that the tasks operate on, e.g. 'site', 'platform' or
 *   'server', values are arrays of the tasks that apply to those objects.
 *
 * @see hook_hosting_tasks
 */
function hook_hosting_tasks_alter(&$tasks) {
  // Change the title of the site's clone task.
  if (isset($tasks['site']['clone'])) {
    $tasks['site']['clone']['title'] = t('Site clone');
  }
}

/**
 * @} End of "addtogroup hooks".
 */
