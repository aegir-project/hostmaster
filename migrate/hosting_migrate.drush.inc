<?php
// $Id$

function hosting_migrate_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site' && $task->task_type == 'migrate') {
    $platform = node_load($task->task_args['target_platform']);
    $task->args[2] = $platform->publish_path;
  }
}


function hosting_migrate_post_hosting_migrate_task($task, $data) {
  if ($task->ref->type == 'site') {
    $target = $task->task_args['target_platform'];
    $task->ref->verified = 0;
    $task->ref->platform = $target;
    node_save($task->ref);
  }
}
