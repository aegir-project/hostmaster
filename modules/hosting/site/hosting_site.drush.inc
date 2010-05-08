<?php

// $Id$

/**
 * Map values of site node into command line arguments
 */
function drush_hosting_site_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');

  if ($task->ref->type === 'site' && $task->task_type === 'named-context-save') {
    $db_server = node_load($task->ref->db_server);
    $task->options['parent'] = '@' . $db_server->hosting_name . '-' . $db_server->services['db']->type;
    $platform = node_load($task->ref->platform);
    $task->options['parent'] .= ',@' . $platform->hosting_name;

    $task->options['provision-uri'] = $task->ref->title;
    $task->options['site_id'] = $task->ref->nid;
    $task->options['language'] = $task->ref->site_language;
  }
  if ($task->task_type == 'restore') {
    $backup = hosting_site_get_backup($task->task_args['bid']);
    $task->args[1] = $backup['filename'];
  }
}


/**
 * implementation of the hosting_post_install hook
 */
function hosting_site_post_hosting_install_task($task, $data) {
  if ($task->ref->type == 'site') {
    $context = $data['context'];
    $packages = $context['packages'];
    hosting_package_sync($packages);
    hosting_package_instance_sync($task->ref->nid, $packages);
    $task->ref->site_status = HOSTING_SITE_ENABLED;
    $task->ref->no_verify = TRUE;
    $task->ref->verified = mktime();
    node_save($task->ref);

    if ($context['login_link']) {
      drush_log($context['login_link']);
      // we need to store the timestamp too because expire only means it can be deleted
      // after an amount of time, not will be deleted.
      $cache = array(
        'expire' => strtotime("+24 hours"),
        'link' => $context['login_link'],
      );
      cache_set('hosting:site:' . $task->ref->nid . ':login_link', $cache, 'cache', $cache['expire'] );
    }
  }
}

/**
 * Implementation of the hosting_failed_install hook
 */
function hosting_site_hosting_install_task_rollback($task, $data) {
  // @TODO : we need to check the returned list of errors, not the code. 
  if (drush_cmp_error('PROVISION_DRUPAL_SITE_INSTALLED')) {
    // Site has already been installed. Try to import instead.
    drush_log(dt("This site appears to be installed already. Generating an import task."));
    hosting_add_task($task->rid, 'import');
  }
  else {
    $task->ref->no_verify = TRUE;
    $task->ref->site_status = HOSTING_SITE_DISABLED;
    node_save($task->ref);
  }
}

/**
 * implementation of hook_hosting_post_disable
 * I am not very fond of the use of bitwise operators with negatives.
 */
function hosting_site_post_hosting_disable_task($task, $data) {
  if ($data['context']['backup_file'] && $data->ref->type == 'site') {
    $platform = node_load($task->ref->platform);
    hosting_site_add_backup($task->ref->nid, $platform->web_server, $data['context']['backup_file'], t('Generated before being disabled'));
  }
  $task->ref->site_status = HOSTING_SITE_DISABLED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

/**
 * implementation of hook_hosting_post_restore
 * I am not very fond of the use of bitwise operators with negatives.
 */
function hosting_site_post_hosting_restore_task($task, $data) {
  if ($data['context']['backup_file'] && $task->ref->type == 'site') {
    $platform = node_load($task->ref->platform);
    hosting_site_add_backup($task->ref->nid, $platform->web_server, $data['context']['backup_file'], t('Generated before being restored to a previous version'));
  }
}

/**
 * implementation of hook_hosting_post_enable
 */
function hosting_site_post_hosting_enable_task($task, $data) {
  $task->ref->site_status = HOSTING_SITE_ENABLED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

/**
 * implementation of hook_hosting_post_DELETE
 */
function hosting_site_post_hosting_delete_task($task, $data) {
  $task->ref->site_status = HOSTING_SITE_DELETED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

/**
 * Implementation of hook_hosting_post_backup
 *
 * Adds the data file that was saved to the site backup history.
 * This is needed to be able to restore.
 */
function hosting_site_post_hosting_backup_task($task, $data) {
  if ($data['context']['backup_file'] && $task->ref->type == 'site') {
    $platform = node_load($task->ref->platform);
    hosting_site_add_backup($task->ref->nid, $platform->web_server, $data['context']['backup_file'], t('Generated on request'));
  }
}


function hosting_site_post_hosting_import_task($task, $data) {
  hosting_import_site($task->ref->nid, $data['context'], $task->ref->platform);
}

function hosting_site_post_hosting_verify_task($task, $data) {
  if ($task->ref->type == 'site') {
    $task->ref->verified = mktime();
    $task->ref->no_verify = TRUE;
    node_save($task->ref);

    $context = $data['context'];
    $packages = $context['packages'];
    hosting_package_sync($packages);
    hosting_package_instance_sync($task->ref->nid, $packages);
  }
}

/**
 * Implementation of the hosting_failed_install hook
 */
function hosting_site_hosting_verify_task_rollback($task, $data) {
  if ($task->ref->type == 'site') {
    $task->ref->no_verify = TRUE;
    $task->ref->verified = 0;
    node_save($task->ref);
  }
}

/**
 * Implementation of the hosting_failed_install hook
 */
function hosting_site_hosting_import_task_rollback($task, $data) {
  if ($task->ref->type == 'site') {
    $task->ref->no_verify = TRUE;
    $task->ref->verified = 0;
    $task->ref->site_status = HOSTING_SITE_DISABLED;
    node_save($task->ref);
  }
}

/**
 * implementation of the hosting_post_login_reset hook
 */
function hosting_site_post_hosting_login_reset_task($task, $data) {
  if ($task->ref->type == 'site') {
    $task->ref->no_verify = TRUE;
    node_save($task->ref);

    $context = $data['context'];
    if ($context['login_link']) {
      // we need to store the timestamp too because expire only means it can be deleted
      // after an amount of time, not will be deleted.
      $cache = array(
        'expire' => strtotime("+24 hours"),
        'link' => $context['login_link'],
      );
      cache_set('hosting:site:' . $task->ref->nid . ':login_link', $cache, 'cache', $cache['expire'] );
    }
  }
}
