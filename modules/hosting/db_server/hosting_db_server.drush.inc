<?php

function drush_hosting_db_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  if (in_array($task->ref->type, array('server', 'site', 'platform'))) {
    $server = _hosting_get_db_server($task);
    if ($server) {
      $server->services['db']->options($task->task_type, $task->ref->type, $task);
    }
  }
}

function _hosting_get_db_server($task) {
  if ($task->ref->type == 'site') {
    if ($task->ref->db_server) {
      return node_load($task->ref->db_server);
    }
  }
  elseif ($task->ref->type == 'platform') {
    // @todo - this becomes unecessary with a server verify task
    return node_load(HOSTING_OWN_DB_SERVER);
  }
  else {
    return $task->ref;
  }
  return false;
}
