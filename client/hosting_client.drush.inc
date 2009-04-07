<?php
// $Id$

function hosting_client_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site' && $task->task_type == 'install') {
    $client = node_load($task->ref->client);
    $task->options['client_id'] = $task->ref->client;
    $task->options['client_email'] = $client->email;
  }
}

