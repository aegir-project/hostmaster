<?php
// $Id$

/**
 * @file API for mapping packages to various Hosting node types
 */

/**
 * Generate instances to reference nodes to releases.
 *
 * This function uses extensible parameters, so you
 * can pass multiple groups of packages to reference
 * to the node.
 *
 * This mimics Drupal's module and theme override
 * functionality, in that only the top most item
 * will be referenced to the node.
 */
function hosting_package_instance_sync($rid) {
  db_query("UPDATE {hosting_package_instance} SET status = -1 where rid=%d", $rid);
  $map = _hosting_package_plural_map();
  $args = func_get_args();
  $rid = array_shift($args);
  foreach ($map as $plural => $key) {
    $merged = array();
    foreach ($args as $index => $arg) {
      if (array_key_exists($plural, $args[$index])) {
        $merged = array_merge($merged, $args[$index][$plural]);
      }
    }
    foreach ($merged as $name => $package) {
      $iid = db_result(db_query("SELECT iid FROM {hosting_package_instance} WHERE iid=%d AND package_id=%d", $rid, $package['package_id']));
      if ($iid) {
        $instance = hosting_package_instance_load($iid);
      }
      else {
        $instance = new stdClass();
        $instance->rid = $rid;
        $instance->package_id = $package['package_id'];
      }

      $instance->filename = $package['filename'];
      $instance->version = ($package['version']) ? $package['version'] : 'Unknown';
      $instance->schema_version = $package['schema_version'];
      $instance->status = $package['status'];
      hosting_package_instance_save($instance);
    }
  }
  db_query("DELETE FROM {hosting_package_instance} WHERE rid=%d AND status=-1", $rid);
}

function _hosting_package_instance_languages($instance) {
  if (sizeof($instance->languages)) {
    db_query("DELETE FROM {hosting_package_languages} WHERE iid=%d", $iid);
    foreach ($languages as $language) {
     db_query("INSERT INTO {hosting_package_languages} (iid, language) VALUES (%d, '%s')", $iid, $language);
    }
  }
}

function hosting_package_instance_update(&$instance) {
  db_query("UPDATE {hosting_package_instance} SET
    rid = %d, package_id = %d, filename = '%s',
    schema_version = %d, version = '%s', status = %d
    WHERE iid = %d", 
    $instance->rid, $instance->package_id, $instance->filename, 
    $instance->schema_version, $instance->version, $instance->status,
    $instance->iid);
   _hosting_package_instance_languages($instance);

}

function hosting_package_instance_create(&$instance) {
   $instance->iid = db_next_id("{hosting_package_instance}_iid");
    db_query("INSERT INTO {hosting_package_instance} (
      iid, rid, package_id, filename,
      schema_version, version, status)
      VALUES (%d, %d, %d, '%s', %d, '%s', %d)", 
      $instance->iid, $instance->rid, $instance->package_id, $instance->filename, 
      $instance->schema_version, $instance->version, $instance->status);
      _hosting_package_instance_languages($instance);
}

function hosting_package_instance_load($iid) {
  $result = db_query("SELECT * FROM {hosting_package_instance} WHERE iid=%d", $iid);
  if ($instance = db_fetch_object($result)) {
    $languages = array('en' => 'en');

    // load language options: if argument is null, load all language options
    $result = db_query("SELECT DISTINCT(language) FROM  {hosting_package_languages} WHERE iid = %d", $instance->iid);
    while ($language = db_fetch_object($result)) {
      $languages[$language->language] = _hosting_language_name($language->language);
    }
    $instance->languages = $languages;
    return $instance;
  }
  return false;
}

function hosting_package_instance_save(&$instance) {
  if (!isset($instance->iid)) {
    hosting_package_instance_create($instance);
  }
  else {
    hosting_package_instance_update($instance);
  }
}

function hosting_package_instance_delete($instance) {
  db_query("DELETE FROM {hosting_package_languages} WHERE iid=%d", $instance->iid);
  db_query("DELETE FROM {hosting_package_instance} WHERE iid = %d", $instance->iid);
}
