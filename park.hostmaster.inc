<?php

include_once('hosting.inc');

/**
 * Make sure that AEgir is ready to be parked
 *
 * @todo make sure no task is running
 */
function drush_hosting_hostmaster_park_validate($url) {
  /**
   * In aegir 0.4 alpha 2 we were verifying that the platform already exists before adding it, but this caused errors during the upgrade process.
   * if you are using this you will need to add the platform manually, but it will automatically be added for you in alpha3 and onwards.
   */
}

/**
 * TODO: prepare the site for migration
 * - disable the queue (variable_set...)
 * - create a failed migrate task for logging
 * - disable the site
 */
function drush_hosting_hostmaster_park($url) {
  
}
