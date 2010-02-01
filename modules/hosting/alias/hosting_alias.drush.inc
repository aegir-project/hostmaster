<?php
// $Id$

function drush_hosting_alias_pre_hosting_task() {
  if (module_exists('hosting_alias')) {
    $task =& drush_get_context('HOSTING_TASK');
    if (($task->ref->type == 'site') && (in_array($task->task_type, array('install', 'verify', 'migrate', 'enable')))) {
      $task->options['aliases'] = implode(",", hosting_alias_get_aliases($task->ref));
      if (!$task->options['aliases']) {
        $task->options['no-aliases'] = 1;
      }
      $task->options['redirection'] = db_result(db_query("SELECT redirection FROM {hosting_site_alias} WHERE vid=%d", $task->ref->vid));
    }
  }
}
