<?php
// $Id$

function drush_hosting_clone_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site' && $task->task_type == 'clone') {
    $task->args[2] = $task->task_args['new_name'];
    $platform = node_load($task->task_args['target_platform']);
    $task->args[3] = $platform->publish_path;
    if (module_exists('hosting_alias')) {
      $task->options['aliases'] = str_replace("\n", ",", $task->task_args['aliases']);
      $task->options['redirection'] = $task->task_args['redirection'];
    }
  }
}


function hosting_clone_post_hosting_clone_task($task, $data) {
  if ($task->ref->type == 'site') {
    $target = $task->task_args['target_platform'];
    $clone = new stdClass();
    # copy some of the settings of the cloned site ($task->ref) to $clone
    foreach (array('type', 'status', 'uid', 'comment', 'promote', 'moderate', 'sticky', 'name', 'format', 'client', 'db_server', 'profile', 'site_status', 'port') as $field) {
      $clone->$field = $task->ref->$field;
    }
    $clone->title = $task->task_args['new_name'];
    $clone->platform = $target;
    $clone->import = 1; # make sure the site doesn't reinstall...
    $clone->verified = 0; # ... and it does verify
    $clone->aliases = $task->task_args['aliases'];
    $clone->redirection = $task->task_args['redirection'];
    node_save($clone);
  }
}

