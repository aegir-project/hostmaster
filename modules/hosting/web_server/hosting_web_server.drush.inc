<?php
// $Id$
function drush_hosting_web_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
}


function _hosting_get_web_server($task) {
  if ($task->ref->type == 'site') {
    $platform = node_load($task->ref->platform);
    return node_load($platform->web_server);
  }
  else {
    return $task->ref;
  }
}
