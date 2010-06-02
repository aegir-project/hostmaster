<?php

function hosting_server_context_options(&$task) {
  foreach ($task->ref->services as $service) {
    if ($service->available) {
      $service->context_options($task->task_type, $task->ref->type, $task);
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
