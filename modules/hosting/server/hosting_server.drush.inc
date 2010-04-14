<?php

function drush_hosting_server_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  $node = $task->ref;

  // todo set up task
}

function hosting_server_post_hosting_verify_task($task, $data) {
  $node = $task->ref;

  if ($node->type === 'server') {
    foreach ($node->services as $name => $service) {
      if ($node->services[$name]['available']) {
        // todo set available to result
      }
    }
  }

  $node->no_verify = TRUE;
  node_save($node);
}
