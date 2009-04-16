<?php

function _hosting_task_log($entry) {
  $task = drush_get_context('HOSTING_TASK');
  if ($task->vid) {
    hosting_task_log($task->vid, $entry['type'], $entry['message'], $entry['error'], $entry['timestamp']);
  }
  else {
    _drush_print_log($entry);
  }
}

function hosting_hosting_task_validate($task) {
  if (is_numeric($task)) {
    $task = node_load($task);
  }
  if ($task->type == 'task') {
    $task->ref = node_load($task->rid);
    $task->changed = mktime();
    $task->executed = mktime();
    $task->revision = true;
    node_save($task);

    $task->options = array();
    $task->args = array();
    // create a new revision
    drush_set_context('HOSTING_TASK', $task);
    drush_set_context('DRUSH_LOG_CALLBACK', '_hosting_task_log');
    drush_log(dt("Task starts processing"), 'queue');
  }
  else {
    drush_set_error('HOSTING_INVALID_TASK', t("This task is not valid"));
  }
}

function hosting_hosting_task() {
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
  $output = drush_backend_invoke("provision " . $task->task_type, $data, TRUE, 'POST', $drush_path, $hostname, $username);

  drush_set_context('HOSTING_DRUSH_OUTPUT', $output);
  $code = drush_get_error();
  // record status 
  $task->task_status = ($code) ? $code : DRUSH_SUCCESS;
  $task->queued = 0;
    // New revision is created at the beginning of function.
  $task->revision = false;
  node_save($task);
  drush_log(dt("Removing task from hosting queue"), 'queue');
}

function hosting_hosting_task_rollback() {
  $task =& drush_get_context('HOSTING_TASK');
  module_invoke_all(sprintf("hosting_%s_task_rollback", $task->task_type), $task, drush_get_context('HOSTING_DRUSH_OUTPUT'));
}


function hosting_post_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  module_invoke_all(sprintf("post_hosting_%s_task", $task->task_type), $task, drush_get_context('HOSTING_DRUSH_OUTPUT'));
}
