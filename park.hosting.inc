<?php

include_once('hosting.inc');

/**
 * Make sure that AEgir is ready to be parked
 *
 * @todo make sure no task is running
 */
function drush_hosting_hosting_park_validate($url, $platform) {
  if ($platform_id = db_result(db_query('SELECT nid FROM {hosting_platform} WHERE publish_path = "%s"', hosting_path_normalize($platform)))) {
    drush_log(dt('Platform found for path %path: #%id', array('%path' => $platform, '%id' => $platform_id)));
  }
  else {
    return drush_set_error('PROVISION_FRAMEWORK_ERROR', dt("Cannot find target platform at @path before parking site. Make sure the platform exists in the frontend first.", array('@path' => $platform)));
  }
}

/**
 * TODO: prepare the site for migration
 * - disable the queue (variable_set...)
 * - disable the cronjob
 * - create a failed migrate task for logging
 * - disable the site
 */
function drush_hosting_hosting_park($url) {
  
}
