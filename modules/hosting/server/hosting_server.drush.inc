<?php

function hosting_server_context_options(&$task) {
  $task->context_options['remote_host'] = $task->ref->title;
  foreach (hosting_server_services() as $type => $info) {
    if (isset($task->ref->services[$type])) {
      $service = $task->ref->services[$type];
      if ($service->available) {
        $service->context_options($task->task_type, $task->ref->type, $task);
      }
    }
    else {
      $task->context_options["{$type}_service_type"] = '0';
    }
  }
}

/**
 * Resave the target node to trigger reverifications.
 */
function hosting_server_post_hosting_verify_task($task, $data) {
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}
