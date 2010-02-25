<?php
// $Id$
function drush_hosting_web_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  if (in_array($task->ref->type, array('server', 'site', 'platform'))) {
    $server = _hosting_get_web_server($task);
    $server->services['server']->options($task->task_type, $task->ref->type, $task);
    $server->services['http']->options($task->task_type, $task->ref->type, $task);
  }
}


function _hosting_get_web_server($task) {
    if ($task->ref->type == 'site') {
      $platform = node_load($task->ref->platform);
      return node_load($platform->web_server);
    }
    elseif ($task->ref->type == 'platform') {
      // @todo - this becomes unecessary with a server verify task
      return node_load($task->ref->web_server);
    }
    else {
      return $task->ref;
    }
}
