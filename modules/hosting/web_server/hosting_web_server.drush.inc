<?php
// $Id$
function drush_hosting_web_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  if (in_array($task->ref->type, array('server', 'site', 'platform'))) {
    //todo necessary for site/platform?
    $server = _hosting_get_web_server($task);
    foreach ($server->services as $service) {
      $service->options($task->task_type, $task->ref->type, $task);
    }
  }
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
