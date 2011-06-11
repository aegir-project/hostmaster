<?php

/**
 * @file
 *   Drush include for the Hosting module's hosting pause command.
 */

/**
 * Make sure Aegir is ready to be upgraded.
 *
 * @todo wait for tasks to complete
 */
function drush_hosting_pause_validate($url) {
}

/**
 * Drush command to pause the Aegir frontend queues.
 *
 * This is really just deleting the crontab.
 */
function drush_hosting_pause($url) {
  // wipe out cron entry
  exec('crontab -r');
  // we can't rely on update.php because it runs too late
  // @deprecated this can be removed in 0.5 and above
  provision_backend_invoke(drush_get_option('site_name'), 'sqlq', array("UPDATE {system} SET weight = 0 WHERE type='module' AND name='hosting';"));
}
