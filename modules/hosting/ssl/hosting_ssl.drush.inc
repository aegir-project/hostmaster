<?php

// $Id$

/**
 */
function drush_hosting_ssl_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');

  if ($task->ref->type == 'site') {
    $task->options['ssl'] = $task->ref->ssl;
    $task->options['ssl_redirect'] = $task->ref->ssl_redirect;
  }
}
