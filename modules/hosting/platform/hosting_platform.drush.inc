<?php
// $Id$

function hosting_hosting_platform_context_options(&$task) {
  $task->context_options['server'] = '@server_master';
  $task->context_options['web_server'] = hosting_context_name($task->ref->web_server);
  // Trim the path so we dont pass extra spaces.
  $task->context_options['root'] = trim($task->ref->publish_path, " ");
  if ($task->ref->makefile) {
    $task->context_options['makefile'] = $task->ref->makefile;
  }
}

function hosting_platform_drush_context_import($context, &$node) {
  if ($context->type == 'platform') {
    $node->title = str_replace('platform_', '', trim($context->name, '@'));
    $node->web_server = hosting_drush_import($context->web_server);
    $node->publish_path = $context->root;
    $node->makefile = $context->makefile; 
  }
}

/**
 * Implementation hook_post_verify
 *
 * Sets the platform verified timestamp, to discren when it was verified.
 * Imports all the profiles and modules into package and package release nodes.
 */
function hosting_platform_post_hosting_verify_task($task, $data) {
  $node = $task->ref;
  if ($node->type == 'platform') {
    $context = $data['context'];
    $packages = $context['packages'];

    $node->verified = mktime(); // set verified flag on platform, to let it know it has been checked.
    /**
     * If we are verifying a Locked platform (i.e if the publish_path has changed), 
     * don't reset the status to Enabled. We don't need to check whether a platform 
     * is deleted here for the same reason, because we don't allow a deleted platform 
     * to be reverified.
     */
    if ($node->platform_status != HOSTING_PLATFORM_LOCKED) {
      $node->platform_status = HOSTING_PLATFORM_ENABLED;
    }
    $node->no_verify = TRUE;
    // Save the platform being verified
    node_save($node);

    hosting_package_sync($packages['base']);
    hosting_package_instance_sync($node->nid, $packages['base']);
    foreach ($context['profiles'] as $profile) {
      hosting_package_sync($packages['profiles'][$profile]);
      $instance = hosting_package_instance_load(
        array('p.short_name' => $profile, 'i.rid' => $node->nid, 'p.package_type' => 'profile'));
      hosting_package_instance_sync($instance->iid, $packages['base'], $packages['profiles'][$profile]);

      // @todo : package instances of modules to profiles.
    }
 
    foreach ($context['sites'] as $url) {
      if (!($site = hosting_get_site_by_url($url))) {
        // Import any sites that have not been created yet.
        $site = new StdClass();
        $site->type = 'site';
        $site->site_status = HOSTING_SITE_QUEUED;
        $site->import = TRUE;
        $site->title = $url;
        $site->platform = $node->nid;
        $site->client = HOSTING_DEFAULT_CLIENT;
        $site->db_server = HOSTING_DEFAULT_DB_SERVER;
        node_save($site);
        drush_log(dt("Imported existing site !domain", array('!domain' => _hosting_node_link($site->nid))));
      }
      hosting_package_instance_sync($site->nid, $packages['base']);
    }
  }
}

/**
 * implementation of hook_hosting_post_DELETE
 */
function hosting_platform_post_hosting_delete_task($task, $data) {
  $task->ref->platform_status = HOSTING_PLATFORM_DELETED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
  hosting_context_delete($task->ref->nid);
  db_query("DELETE FROM {hosting_platform_client_access} WHERE pid = %d", $task->ref->nid);
}

function hosting_platform_post_hosting_lock_task($task, $data) {
  $task->ref->platform_status = HOSTING_PLATFORM_LOCKED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

function hosting_platform_post_hosting_unlock_task($task, $data) {
  $task->ref->platform_status = HOSTING_PLATFORM_ENABLED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

