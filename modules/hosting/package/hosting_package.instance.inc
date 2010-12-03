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
        'i.rid' => $rid,
        'i.package_id' => $package['package_id']));
      if (!$instance) {
        $instance = new stdClass();
        $instance->rid = $rid;
        $instance->package_id = $package['package_id'];
      }
      $instance->languages = $package['info']['languages'];
      $instance->filename = $package['filename'];
      $instance->version = ($package['version']) ? $package['version'] : 'Unknown';
      $instance->version_code = hosting_package_instance_version_code($package['version']);
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
    schema_version = %d, version = '%s', version_code = %f, status = %d
    WHERE iid = %d", 
    $instance->rid, $instance->package_id, $instance->filename, 
    $instance->schema_version, $instance->version, $instance->version_code, $instance->status,
    $instance->iid);

  _hosting_package_instance_languages($instance);
}

function hosting_package_instance_create(&$instance) {
   db_query("INSERT INTO {hosting_package_instance} (
      rid, package_id, filename,
      schema_version, version, version_code, status)
      VALUES (%d, %d, '%s', %d, %f, %d, %d)", 
      $instance->rid, $instance->package_id, $instance->filename, 
      $instance->schema_version, $instance->version, $instance->version_code, $instance->status);
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
      i.iid, i.version, i.version_code, i.schema_version,
      i.status, p.description, p.short_name, p.old_short_name, i.rid,
      i.package_id
      FROM {hosting_package_instance} i 
        LEFT JOIN {hosting_package} p ON p.nid=i.package_id 
        LEFT JOIN {node} n ON p.nid=n.nid
        LEFT JOIN {node} r ON r.nid=i.rid
        LEFT JOIN {hosting_platform} h ON r.nid=h.nid
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


/**
 * Turn package version string into a number
 *
 * This function returns a float value for the number that represents the
 * version string.
 */
function hosting_package_instance_version_code($version) {

  $defaults = array(
    'platform_major' => 0,
    'platform_minor' => 0,
    'package_major' => 0,
    'package_minor' => 0,
    'package_version_type' => 'release',
    'package_patch_level' => 0,
  );

  $release_types = array(
    '' => 0,
    'dev' => 0,
    'unstable' => 1,
    'alpha' => 2,
    'beta' => 3,
    'rc' => 4,
    'release' => 5
  );

  $regex = array(
    'core' => '/^(?P<platform_major>\d?)\.(?P<platform_minor>[x\d]*)?(-(?P<package_version_type>dev|unstable|alpha|beta|rc)?(?P<package_patch_level>\d)*)?$/',
    'contrib' => "/(?P<platform_major>\d?)\.(?P<platform_minor>[x\d]*)?(-(?P<package_major>\d*)?\.(?P<package_minor>[x\d]*)?(-(?P<package_version_type>alpha|unstable|beta|rc|dev)?(?P<package_patch_level>[\d]*)?)?)?/");

  $matches = array();
  if (preg_match($regex['core'], $version, $matches)) {
    $matches = array_merge($defaults, $matches);
  }
  elseif (preg_match($regex['contrib'], $version, $matches)) {
    $matches = array_merge($defaults, $matches);
  }
  else {
    return 0;
  }
  // we use the float type because php generally has a maximum integer value to small to handle this value
  $result =  (float) sprintf("%d%02d%02d%02d%02d%02d", 
    $matches['platform_major'], $matches['platform_minor'], $matches['package_major'], 
    $matches['package_minor'], $release_types[$matches['package_version_type']], 
    $matches['package_patch_level']);

  return $result;
}

function hosting_package_comparison($current, $target) {

  _hosting_package_temporary_table("current", $current);
  _hosting_package_temporary_table("target", $target);

  $status = array();
  $result = db_query("SELECT count(c.nid) AS error FROM current c LEFT JOIN target t ON c.nid=t.nid WHERE (t.schema_version > 0) && (c.schema_version > t.schema_version) AND c.status=1");
  while ($obj = db_fetch_object($result)) {
    $status['error'] = $obj->error;
  }

  $result = db_query("SELECT COUNT(c.nid) as missing FROM current c LEFT JOIN target t ON c.nid=t.nid WHERE t.nid IS null AND c.status=1");
  while ($obj = db_fetch_object($result)) {
    $status['missing'] = $obj->missing;
  }

  $result = db_query("SELECT COUNT(c.nid) as upgrade FROM  current c LEFT JOIN target t ON c.nid=t.nid WHERE (c.version_code < t.version_code) OR (c.schema_version < t.schema_version) AND c.status=1");
  while ($obj = db_fetch_object($result)) {
    $status['upgrade'] = $obj->upgrade;
  }
  
  $result = db_query("SELECT count(c.nid) AS downgrade FROM current c LEFT JOIN target t ON c.nid=t.nid WHERE (c.version_code > t.version_code)");
  while ($obj = db_fetch_object($result)) {
    $status['downgrade'] = $obj->downgrade;
  }

  db_query("DROP TEMPORARY TABLE target");
  db_query("DROP TEMPORARY TABLE current");

  return $status;
}


function _hosting_package_temporary_table($name, $ref) {
  db_query("CREATE TEMPORARY TABLE %s AS SELECT nid, short_name, old_short_name, version_code, schema_version, version, status from {hosting_package} p left join {hosting_package_instance} i on p.nid = i.package_id where rid= %d", $name, $ref);
}
