<?php

/**
 * @file manual site migrate system
 */

function drush_hosting_hosting_migrate_validate($site, $platform) {
  // verify the target platform:
  $data = drush_get_merged_options(); // XXX: necessary because the remote
                                      // platform may not have been
                                      // initialised by the frontend (this
                                      // should be checked instead)
  $data['root'] = $platform;
  $data['r'] = $platform;
  drush_backend_invoke('provision verify', $data);
  // verify the current platform
  drush_backend_invoke('provision verify', array('root' => drush_get_option(array('r', 'root'), drush_locate_root())));
  // verify the site prior to migration
  drush_backend_invoke('provision verify', array($site));
}

function drush_hosting_pre_hosting_migrate($site, $platform) {
  return drush_backend_invoke('hosting park', array($site, $platform));
}

function drush_hosting_hosting_migrate($site, $platform) { 
  return drush_backend_invoke('provision clone', array($site, $site, $platform));
}

function drush_hosting_hosting_migrate_rollback($site, $platform) {
  return drush_backend_invoke('provision delete', array($site, 'root' => $platform));
}

function drush_hosting_post_hosting_migrate($site, $platform) {
  drush_backend_invoke('hosting unpark', array($site, $platform));
  if (!drush_get_error()) {
    drush_backend_invoke('provision delete', array($site));
    drush_backend_invoke('provision verify', array($site, 'root' => $platform, 'publish_path' => $platform));
  }
}
