<?php

function hosting_server_context_options(&$task) {
  foreach ($task->ref->services as $service) {
    if ($service->available) {
      $service->context_options($task->task_type, $task->ref->type, $task);
    }
  }
}

function hosting_server_post_hosting_verify_task($task, $data) {
  $node = $task->ref;

  if ($node->type === 'server') {
    foreach ($node->services as $name => $service) {
      if ($node->services[$name]->available) {
        // todo set available to result
      }
    }
  }

  $node->no_verify = TRUE;
  node_save($node);
}
