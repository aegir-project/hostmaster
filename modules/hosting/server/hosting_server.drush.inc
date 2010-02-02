<?php

function drush_hosting_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  if (in_array($task->ref->type, array('server', 'site', 'platform'))) {
    $servers = array();
    if ($task->ref->type == 'site') {
      $platform = node_load($task->ref->platform);
      $web_server = $platform->web_server;
      $servers[$web_server] = node_load($web_server);
      $db_server = $task->ref->db_server;
      if ($web_server != $db_server) {
        $servers[$db_server] = node_load($db_server);
      }
    }
    elseif ($task->ref->type == 'platform') {
      $web_server = $task->ref->web_server;
      $servers[$web_server] = node_load($web_server);
    }
    else {
      $servers[$task->ref->nid] = $task->ref;
    }
    foreach ($servers as $nid => $server) {
      hosting_server_invoke_services($server, 'options', $task->task_type, $task->ref->type,  $task);
    }
  }
}
