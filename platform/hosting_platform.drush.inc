<?php
// $Id$

function hosting_platform_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');
  if ($task->ref->type == 'site') {
    $node = node_load($task->ref->platform);
  }
  elseif ($task->ref->type == 'platform') {
    $node = $task->ref;
  }
  $task->options['platform'] = $node->nid;
  $task->options['publish_path'] = $node->publish_path;
  if ($task->task_type == 'verify') {
    $task->options['master_url'] = variable_get('install_url', $GLOBALS['base_url']);
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
    // Save the platform being verified
    node_save($node);

    hosting_package_sync($packages['base']);
    hosting_package_instance_sync($node->nid, $packages['base']);
    foreach ($context['profiles'] as $profile) {
      hosting_package_sync($packages['profiles'][$profile]);
      $instance = hosting_package_instance_load(
        array('short_name' => $profile, 'rid' => $node->nid, 'package_type' => 'profile'));
      hosting_package_instance_sync($instance->iid, $packages['base'], $packages['profiles'][$profile]);

      // @todo : package instances of modules to profiles.
    }
 
    foreach ($context['sites'] as $url) {
      hosting_package_sync($packages['sites'][$url]);

      if (!($site = hosting_get_site_by_url($url))) {
        // Import any sites that have not been created yet.
        $site = new StdClass();
        $site->type = 'site';
        $site->import = TRUE;
        $site->title = $url;
        $site->platform = $node->nid;
        $site->client = HOSTING_DEFAULT_CLIENT;
        node_save($site);
        drush_log(dt("Imported existing site !domain", array('!domain' => _hosting_node_link($site->nid))));
      }
      hosting_package_instance_sync($site->nid, $packages['base'], $packages['sites'][$url]);
    }
  }
}

