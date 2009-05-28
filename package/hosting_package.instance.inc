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
      $instance = hosting_package_instance_load(array(
        'rid' => $rid,
        'package_id' => $package['package_id']));
      if (!$instance) {
        $instance = new stdClass();
        $instance->rid = $rid;
        $instance->package_id = $package['package_id'];
      }
      $instance->languages = $package['info']['languages'];
      $instance->filename = $package['filename'];
      $instance->version = ($package['version']) ? $package['version'] : 'Unknown';
      $instance->schema_version = $package['schema_version'];
      if (array_key_exists('status', $package)) {
        $instance->status = $package['status'];
      }
      else {
        $instance->status = 0;
      }
      hosting_package_instance_save($instance);
    }
  }
  db_query("DELETE FROM {hosting_package_instance} WHERE rid=%d AND status=-1", $rid);
}

function _hosting_package_instance_languages($instance) {
  if (sizeof($instance->languages)) {
    db_query("DELETE FROM {hosting_package_languages} WHERE iid=%d", $instance->iid);
    foreach ($instance->languages as $language) {
      db_query("INSERT INTO {hosting_package_languages} (iid, language) VALUES (%d, '%s')", $instance->iid, $language);
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
   db_query("INSERT INTO {hosting_package_instance} (
      rid, package_id, filename,
      schema_version, version, status)
      VALUES (%d, %d, '%s', %d, '%s', %d)", 
      $instance->rid, $instance->package_id, $instance->filename, 
      $instance->schema_version, $instance->version, $instance->status);
   $instance->iid = db_last_insert_id('hosting_package_instance', 'iid');
   _hosting_package_instance_languages($instance);
}

function hosting_package_instance_load($param) {
  return _hosting_package_instances_load($param);
}

function hosting_package_instances_load($param) {
  return _hosting_package_instances_load($param, TRUE);
}

function _hosting_package_instances_load($param, $multiple = FALSE) {
  $arguments = array();
  if (is_numeric($param)) {
    $cond = 'iid = %d';
    $arguments[] = $param;
  }
  elseif (is_array($param)) {
    // Turn the conditions into a query.
    foreach ($param as $key => $value) {
      $cond[] = $key ." = '%s'";
      $arguments[] = $value;
    }
    $cond = implode(' AND ', $cond);
  }
  else {
    return FALSE;
  }
  $instances = array();
  $result = db_query("SELECT 
      n.title, r.type, p.package_type, p.nid,
      i.iid, i.version, i.schema_version,
      i.status, p.description, p.short_name, i.rid,
      i.package_id
      FROM {hosting_package_instance} i 
        LEFT JOIN {hosting_package} p ON p.nid=i.package_id 
        LEFT JOIN {node} n ON p.nid=n.nid
        LEFT JOIN {node} r ON r.nid=i.rid
      WHERE " . $cond, $arguments);

  while ($instance = db_fetch_object($result)) {
    $languages = array('en' => 'en');

    // load language options: if argument is null, load all language options
    $lang_result = db_query("SELECT DISTINCT(language) FROM {hosting_package_languages} WHERE iid = %d", $instance->iid);
    while ($language = db_fetch_object($lang_result)) {
      $languages[$language->language] = _hosting_language_name($language->language);
    }
    $instance->languages = $languages;
    if (!$multiple) {
      return $instance;
    }
    $instances[$instance->iid] = $instance;
  }
  return $instances;
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

function hosting_package_instance_version_compare($from, $to) {
  if ($from->version == $to->version) {
    return 0;
  }

#  if (isset($from->schema_version)) {
#    if ($to->schema_version > $from->schema_version) {
#      return 1;
#    }
#  }

  if (in_array($from->version, array('Unknown', 'HEAD'))) {
    if (!in_array($to->version, array('Unknown', 'HEAD'))) {
      return 1;
    }
  }
  _hosting_version_info_extract($from);
  _hosting_version_info_extract($to);
  $compare = version_compare($to->drupal_version, $from->drupal_version);
  if ($compare == 0) {
    $compare = version_compare($to->project_version, $from->project_version);
  }
  return $compare;
}

function _hosting_version_info_extract(&$instance) {
  $parts = explode('-', $instance->version);
  switch (sizeof($parts)) {
    case '3' :
      $instance->project_version = str_replace(".x", ".0", $parts[1]) . "." . $parts[2];
      $instance->drupal_version = str_replace(".x", ".0", $parts[0]);
      break;
    case '2' :
      $instance->project_version = str_replace(".x", ".0", $parts[1]);
      $instance->drupal_version = str_replace(".x", ".0", $parts[0]);
      break;
    case '1' :
      $instance->drupal_version = str_replace(".x", ".0", $parts[0]);
      break;
  }
}
