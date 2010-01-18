<?php

/**
 * @file Implementation of the hostmaster resume command
 */

include_once('hosting.inc');

function drush_hosting_hostmaster_resume($url) {
  drush_log('bootstrapping drupal');
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  $platform_root = drush_get_context('DRUSH_DRUPAL_ROOT');

  $nid = db_result(db_query('SELECT nid FROM {node} WHERE type = "site" AND status = 1 AND title = "%s"', $url));
  $node = node_load($nid);

  if (!($platform_id = db_result(db_query('SELECT nid FROM {hosting_platform} WHERE publish_path = "%s"', $platform_root)))) {
     drush_log(dt('Platform not found for path %path, adding platform node', array('%path' => $platform_root)));
     // inherit settings from current platform node
     $platform = node_load($node->platform);
     unset($platform->nid);
     unset($platform->vid);
     $platform->publish_path = $platform_root;
     $platform->verified = 0;
     $platform->title = basename(rtrim($platform_root, '/'));
     node_save($platform);
     $platform_id = $platform->nid;
  }
  else {
    drush_log(dt('Platform found for path %path: #%id', array('%path' => $platform, '%id' => $platform_id)));
  }

  drush_log('fixing platform for site node');
  if ($platform_id = db_result(db_query('SELECT nid FROM {hosting_platform} WHERE publish_path = "%s"', $platform_root))) {
    $node->verified = 0;
    $node->platform = $platform_id;
    node_save($node);
    drush_backend_invoke('hosting-setup', array('uri' => $url));
  } else {
    return drush_set_error('PROVISION_FRAMEWORK_ERROR', dt("Could not find the node for platform path %platform", array('%platform' => $platform_root)));
  }
  // TODO: post migration tasks
  // - save the drush log to the migrate task and set proper status
}
