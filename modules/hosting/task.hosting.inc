<?php

function _hosting_task_log($entry) {
  $task = drush_get_context('HOSTING_TASK');
  if ($task->vid) {
    hosting_task_log($task->vid, $entry['type'], $entry['message'], $entry['error'], $entry['timestamp']);
  }
  else {
    return _drush_print_log($entry);
  }
  if (drush_get_option('debug', FALSE)) {
    return _drush_print_log($entry);
  }
}

function drush_hosting_hosting_task_validate($task) {
  drush_set_option('user', 1);
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_LOGIN);
  if (is_numeric($task)) {
    $task = node_load($task);
  }
  if ($task->type == 'task') {
    $task->ref = node_load($task->rid);
    $task->options = array('root' => null, 'uri' => null);
    $task->context_options = array(
      'context_type' => $task->ref->type,
      'master_url' => url('', array('absolute' => TRUE)),
      'root' => null,
      'uri' => null
    );
    $task->args = array();
    $task->changed = mktime();
    $task->executed = mktime();
    // remove the task from the queue at the start of execution
    // this is to avoid concurrent task runs
    $task->task_status = HOSTING_TASK_PROCESSING;
    $task->revision = TRUE;
    node_save($task);
    drush_set_context('HOSTING_TASK', $task);
    drush_set_context('DRUSH_LOG_CALLBACK', '_hosting_task_log');
    drush_log(dt("Task starts processing"), 'queue');
  }
  else {
    drush_set_error('HOSTING_INVALID_TASK', t("This task is not valid"));
  }
}

function drush_hosting_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  $output = array();
  $mode = drush_get_option('debug', FALSE) ? 'GET' : 'POST';

  // Make sure argument order is correct
  ksort($task->args);

  // On install/verify, save the named context
  if ($task->task_type === 'install' || $task->task_type === 'verify') {
    module_invoke('hosting_' . $task->ref->type, 'context_options', $task);
    $output = drush_backend_invoke_args('provision-named-context-save', array('@' . $task->ref->hosting_name), $task->context_options, $mode);
  }
  // Run the actual command. Adding alias here to work aorund Drush API.
  $output = drush_backend_invoke_args('@' . $task->ref->hosting_name . ' provision-' . $task->task_type, $task->args, $task->options, $mode);

  drush_set_context('HOSTING_DRUSH_OUTPUT', $output);
  $code = drush_get_error();
  // We return 0 on success, so anything else is an error.
  $task->task_status = ($code) ? HOSTING_TASK_ERROR : HOSTING_TASK_SUCCESS;

  // On succesful delete, remove the named context.
  if ($task->task_type === 'delete' && $task->task_status === HOSTING_TASK_SUCCESS) {
    $output = array_merge_recursive($output, drush_backend_invoke_args('provision-named-context-delete', array(), $task->context_options, $mode));
  }

  // New revision is created at the beginning of function.
  $task->revision = FALSE;
  $task->delta = mktime() - $task->executed;
  node_save($task);
}

function drush_hosting_hosting_task_rollback() {
  $task =& drush_get_context('HOSTING_TASK');
  module_invoke_all(sprintf("hosting_%s_task_rollback", $task->task_type), $task, drush_get_context('HOSTING_DRUSH_OUTPUT'));
}


function drush_hosting_post_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');

  module_invoke_all(sprintf("post_hosting_%s_task", $task->task_type), $task, drush_get_context('HOSTING_DRUSH_OUTPUT'));
}
