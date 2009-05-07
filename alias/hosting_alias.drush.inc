<?php
// $Id$

function drush_hosting_alias_pre_hosting_task() {
  if (module_exists('hosting_alias')) {
    $task =& drush_get_context('HOSTING_TASK');
    if (($task->ref->type == 'site') && (in_array($task->task_type, array('install', 'verify', 'migrate')))) {
      $task->options['aliases'] = implode(",", hosting_alias_get_aliases($task->ref));
    }
  }
}
