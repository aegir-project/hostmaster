<?php
// $Id$

function drush_hosting_migrate_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site' && $task->task_type == 'migrate') {
    $platform = node_load($task->task_args['target_platform']);
    $task->args[2] = $platform->publish_path;


    // load the original install profile
    $profile = node_load($task->ref->profile);


    // get the profile instance we are migrating to.
    $profile_instance = hosting_package_instance_load(array(
      'i.rid' => $task->task_args['target_platform'],
      'p.short_name' => $profile->short_name,
    ));

    if (!sizeof($profile_instance)) {
      // get a possible upgrade path for the profile.
      $profile_instance = hosting_package_instance_load(array(
        'i.rid' => $task->task_args['target_platform'],
        'p.old_short_name' => $profile->short_name,
      ));
    }

    if (sizeof($profile_instance)) {
      $task->options['profile'] = $profile_instance->short_name;
    }
    else {
      drush_set_error("HOSTING_NO_VALID_PROFILE", dt("There are no valid install profiles on the target platform to migrate to"));
    }
  }
}


function hosting_migrate_post_hosting_migrate_task($task, $data) {
  if ($task->ref->type == 'site') {
    $target = $task->task_args['target_platform'];
    $site = $task->ref;

    $profile = hosting_package_instance_load(array(
      'rid' => $target,
      'short_name' => $data['context']['profile']
    ));
    if ($profile) {
      $site->profile = $profile->package_id;
    }

    $site->verified = 0;
    $site->platform = $target;
    node_save($site);

    $task->ref = $site;
  }
}
