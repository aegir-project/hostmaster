<?php
// $Id$

/**
 * @file Site backup functions.
 */

function hosting_task_backup_form($node) {
  $form['description'] = array (
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#weight' => '-1',
    '#description' => "Describe the reasons for creating this backup."
  );  

  return $form;
}

/**
 * Add a site backup record.
 *
 * Builds a list of backups of the site that have been generated.
 */
function hosting_site_add_backup($site, $web_server, $filename, $description = '') {
  db_query("INSERT INTO {hosting_site_backups} (site, web_server, filename, description, timestamp) VALUES (%d, %d, '%s', '%s', %d)",
    $site, $web_server, $filename, $description, mktime());
  $bid = db_last_insert_id('hosting_site_backups', 'bid');
  return $bid;
}

/**
 * Delete a site backup record
 */
function hosting_site_delete_backup($bid) {
  db_query("DELETE FROM {hosting_site_backups} WHERE bid=%d", $bid);
}

/**
 * Get a site backup record
 */
function hosting_site_get_backup($bid) {
  return db_fetch_array(db_query("SELECT bid, site, web_server, filename, description, timestamp FROM {hosting_site_backups} WHERE bid = %d", $bid));
}

/**
 * Retrieve a list of backup generated for a site.
 *
 * @param site
 *   The node if of the site backups are being retrieved for
 * @return
 *   An associative array of backups existing for the site, indexed by bid and sorted reverse chronologically.
 */
function hosting_site_backup_list($site) {
  $result = db_query("SELECT bid, description, timestamp FROM {hosting_site_backups} WHERE site=%d ORDER BY timestamp DESC", $site);
  while ($object = db_fetch_object($result)) {
    #needs to be cleaned up. but i am NOT generating a theme func for this right now.
    $backups[$object->bid] = '<strong>' . format_date($object->timestamp) . '</strong> - ' . filter_xss($object->description);
  }
  return $backups;
}

