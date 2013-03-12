<?php
// $Id$

/**
 * @file Site backup functions.
 */

function hosting_task_backup_form($node) {
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#weight' => '-1',
    '#description' => "Describe the reasons for creating this backup.",
  );

  return $form;
}

/**
 * Add a site backup record.
 *
 * Builds a list of backups of the site that have been generated.
 */
function hosting_site_add_backup($site, $web_server, $filename, $description = '', $size = '') {
  $id = db_insert('hosting_site_backups')
  ->fields(array(
    'site' => $site,
    'web_server' => $web_server,
    'filename' => $filename,
    'description' => $description,
    'size' => $size,
    'timestamp' => REQUEST_TIME,
  ))
  ->execute();
  return $id;
}

/**
 * Delete a site backup record
 */
function hosting_site_delete_backup($bid) {
  db_delete('hosting_site_backups')
    ->condition('bid', $bid)
    ->execute();
}

/**
 * Get a site backup record
 */
function hosting_site_get_backup($bid) {
  return db_query("SELECT bid, site, web_server, filename, description, size, timestamp FROM {hosting_site_backups} WHERE bid = :bid", array(':bid' => $bid))->fetchAssoc();
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
  $result = db_query("SELECT bid, description, size, timestamp FROM {hosting_site_backups} WHERE site = :site ORDER BY timestamp DESC", array(':site' => $site));
  while ($object = $result->fetch()) {
    #needs to be cleaned up. but i am NOT generating a theme func for this right now.
    $backups[$object->bid] = '<strong>' . format_date($object->timestamp) . '</strong> - ' . format_size($object->size) . ' - ' . filter_xss($object->description);
  }
  return $backups;
}

