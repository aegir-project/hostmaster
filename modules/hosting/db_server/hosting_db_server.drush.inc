<?php

function drush_hosting_db_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
}

function _hosting_get_db_server($task) {
  if ($task->ref->type == 'site') {
    if ($task->ref->db_server) {
      return node_load($task->ref->db_server);
    }
  }
  else {
    return $task->ref;
  }
  return false;
}
