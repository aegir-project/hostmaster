<?php

/**
 * @file Implementation of the hosting unpark command
 */

function drush_hosting_hosting_unpark($url, $platform) {
  drush_log('bootstrapping drupal');
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  drush_log('fixing platform');
  if ($platform_id = db_result(db_query('SELECT nid FROM {hosting_platform} WHERE publish_path = "%s"', $platform))) {
    $nid = db_result(db_query('SELECT nid FROM {node} WHERE type = "site" AND status = 1 AND title = "%s"', $url));
    $node = node_load($nid);
    $node->verified = 0;
    $node->platform = $platform_id;
    node_save($node);
  } else {
    return drush_set_error('PROVISION_FRAMEWORK_ERROR', dt("Could not find the node for platform path %platform", array('%platform' => $platform)));
  }
  // TODO: post migration tasks
  // - turn on new cron job
  // - save the drush log to the migrate task and set proper status
}
