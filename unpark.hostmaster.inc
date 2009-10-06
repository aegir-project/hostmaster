<?php

/**
 * @file Implementation of the hostmaster unpark command
 */

include_once('hosting.inc');

function drush_hosting_hostmaster_unpark($url) {
  drush_log('bootstrapping drupal');
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  $platform = drush_get_context('DRUSH_DRUPAL_ROOT');
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
