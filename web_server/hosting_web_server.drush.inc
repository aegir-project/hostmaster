<?php
// $Id$

function drush_hosting_web_server_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');
  if (in_array($task->ref->type, array('site', 'platform'))) {
    if ($task->ref->type == 'site') {
      $platform = node_load($task->ref->platform);
      $web = node_load($platform->web_server);
    }
    else {
      $web = node_load($task->ref->web_server);
    }
    $task->options['drush_path'] = $web->drush_path;
    $task->options['web_id'] = $web->nid;
    $task->options['web_host'] = $web->title;
    $task->options['web_ip'] = $web->ip_address;
    $task->options['script_user'] = $web->script_user;

    if ($task->task_type == 'verify' && $task->ref->type == 'platform') {
      $task->options['web_group'] = $web->web_group;
      $task->options['config_path'] = $web->config_path;
      $task->options['backup_path'] = $web->backup_path;
      $task->options['restart_cmd'] = $web->restart_cmd;
    }
  }
}
