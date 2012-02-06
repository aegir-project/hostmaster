<?php

/**
 * @file
 *   Drush include for the Hosting module's hosting resume command.
 */

include_once('hosting.inc');

/**
 * Drush command to resume the Aegir frontend site.
 */
function drush_hosting_resume() {
  drush_log('bootstrapping drupal');
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  $server_name = drush_get_option('server_name');
  $old_platform = drush_get_option('old_platform_name');
  $new_platform = drush_get_option('new_platform_name');

  $node = hosting_context_load(d()->name);

  // fix the old platform first
  $platform_id = db_result(db_query('SELECT nid FROM {hosting_platform} WHERE publish_path = "%s"', d($old_platform)->root));
  if ($platform_id) {
    hosting_context_register($platform_id, ltrim($old_platform, '@'));
  } else {
    drush_log(dt('Old platform not found in path %path, not setting context %context', array('%path' => d($old_platform)->root, '%context' => ltrim($old_platform, '@'))), 'warning');
  }

  // if the new platform doesn't exist, create it. 
  $platform_root = d()->root;
  if (!($platform_id = db_result(db_query('SELECT nid FROM {hosting_platform} WHERE publish_path = "%s"', $platform_root)))) {
     drush_log(dt('Platform not found for path %path, adding platform node', array('%path' => $platform_root)));
     // inherit settings from current platform node
     $platform = node_load($node->platform);
     $platform->type = 'platform';
     unset($platform->nid);
     unset($platform->vid);
     $platform->publish_path = $platform_root;
     $platform->verified = 0;
     $platform->title = basename(rtrim($platform_root, '/'));
     if (preg_match("/^([0-9]+)$/i", $platform->title)) {
       $platform->title = 'Aegir Hostmaster ' . $platform->title;
     }
     elseif (preg_match("/hostmaster/i", $platform->title)) {
       $platform->title = 'Aegir Hostmaster';
     }
     node_save($platform);
     $platform_id = $platform->nid;
  }
  else {
    drush_log(dt('Platform found for path %path: #%id', array('%path' => $platform_root, '%id' => $platform_id)));
    $platform = node_load($platform_id);
  }

  hosting_context_register($platform_id, ltrim($new_platform, '@'));

  drush_log('fixing platform for site node');
  $node->verified = 0;
  $node->platform = $platform_id;
  node_save($node);
  provision_backend_invoke(d()->name, 'hosting-setup');
}
