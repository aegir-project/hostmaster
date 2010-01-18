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
    $task->options = array();
    $task->args = array();
    $task->changed = mktime();
    $task->executed = mktime();
    // remove the task from the queue at the start of execution
    // this is to avoid concurrent task runs
    $task->task_status = HOSTING_TASK_PROCESSING;
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
  $hostname = null;
  $username = null;
  if (isset($task->options['web_id']) && ($task->options['web_id'] != HOSTING_OWN_WEB_SERVER)) {
    $username = $task->options['script_user'];
    $hostname = $task->options['web_host'];
  }
  $drush_path = $task->options['drush_path'];
  $task->options['root'] = $task->options['publish_path'];

  // make sure argument order is correct
  ksort($task->args);
  $data = array_merge($task->args, $task->options);
  $data['uri'] = null; // provision commands do not need the url passed to them.
  $mode = drush_get_option('debug', FALSE) ? 'GET' : 'POST';
  $output = drush_backend_invoke("provision-" . $task->task_type, $data, $mode, TRUE, $drush_path, $hostname, $username);

  drush_set_context('HOSTING_DRUSH_OUTPUT', $output);
  $code = drush_get_error();
  // record status 
  // We return 0 on success, so anything else is an error.
  $task->task_status = ($code) ? HOSTING_TASK_ERROR : HOSTING_TASK_SUCCESS;

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
