<?php
// $Id$

function drush_hosting_clone_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site' && $task->task_type == 'clone') {
    $task->args[2] = '@' . $task->task_args['new_uri'];
    $platform = node_load($task->task_args['target_platform']);
    $task->args[3] = hosting_context_name($platform->nid);

    $profile_instance = _hosting_migrate_get_target_profile_name($task);

    if (sizeof($profile_instance)) {
      $task->options['profile'] = $profile_instance->short_name;
    }
    else {
      drush_set_error("HOSTING_NO_VALID_PROFILE", dt("There are no valid install profiles on the target platform to migrate to"));
    }

    if ($site->db_server != $task->task_args['new_db_server']) {
      $task->options['new_db_server'] = hosting_context_name($task->task_args['new_db_server']);
    }

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
    foreach (array('type', 'status', 'uid', 'comment', 'promote', 'moderate', 'sticky', 'name', 'format', 'client', 'db_server', 'profile', 'site_status', 'site_language') as $field) {
      $clone->$field = $task->ref->$field;
    }
    $clone->title = $task->task_args['new_uri'];
    $clone->hosting_name = $clone->title;
    $clone->platform = $target;
    $clone->import = 1; # make sure the site doesn't reinstall...
    $clone->verified = 0; # ... and it does verify
    $clone->aliases = $task->task_args['aliases'];
    $clone->redirection = $task->task_args['redirection'];
    
    if ($clone->db_server != $task->task_args['new_db_server']) {
      $clone->db_server = $task->task_args['new_db_server'];
    }

    $profile = hosting_package_instance_load(array(
      'i.rid' => $target,
      'p.short_name' => $data['context']['profile']
    ));
    if ($profile) {
      $clone->profile = $profile->package_id;
    }

    node_save($clone);
  }
}

