<?php

function drush_hosting_rename_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site' && $task->task_type == 'rename') {
    $task->args[2] = $task->task_args['new_name'];
    $platform = node_load($task->task_args['platform']);
    $task->args[3] = $platform->publish_path;
    if (module_exists('hosting_alias')) {
      $task->options['aliases'] = str_replace("\n", ",", $task->task_args['aliases']);
      $task->options['redirection'] = $task->task_args['redirection'];
    }
  }
}


function hosting_rename_post_hosting_rename_task($task, $data) {
  if ($task->ref->type == 'site') {
    $node = $task->ref;
    $node->title = $task->task_args['new_name'];
    $node->aliases = $task->task_args['aliases'];
    $node->redirection = $task->task_args['redirection'];
    /* @TODO fix Hosting so that it handles revisionning properly -
     * currently setting this to TRUE inserts a new row into hosting_site
     * with the new vid, and in most places we select from this table by nid
     *
     * $node->revision = TRUE;
     */
    $node->changed = mktime();
    node_save($node);
  }
}

