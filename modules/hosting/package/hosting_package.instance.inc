<?php
/**
 * @file
 *   API for mapping packages to various Hosting node types
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
  db_query("UPDATE {hosting_package_instance} SET status = -1 WHERE rid = %d", $rid);
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
      $instance->languages = isset($package['info']['languages']) ? $package['info']['languages'] : array();
      $instance->filename = isset($package['filename']) ? $package['filename'] : NULL;
      $instance->version = isset($package['version']) ? $package['version'] : 'Unknown';
      $instance->version_code = hosting_package_instance_version_code($package['version']);
      $instance->schema_version = isset($package['schema_version']) ? $package['schema_version'] : NULL;
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

  // Clear the static cache for languages of package instances.
  _hosting_package_instances_load_languages(NULL, TRUE);
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
      VALUES (%d, %d, '%s', %d, '%s', %f, %d)",
      $instance->rid, $instance->package_id, $instance->filename,
      $instance->schema_version, $instance->version, $instance->version_code, $instance->status);
   $instance->iid = db_last_insert_id('hosting_package_instance', 'iid');
   _hosting_package_instance_languages($instance);
}

/**
 * Get the first package instance object that maches given parameters.
 *
 * @see hosting_package_instances_load()
 */
function hosting_package_instance_load($param) {
  return _hosting_package_instances_load($param);
}

/**
 * Get an array of all package instance objects that match given parameters.
 *
 * @param mixed $param
 *   If $param is an integer it will be matched against an instance iid. If
 *   $param is an array then the key/value pairs will be used as a WHERE clause
 *   in the form "WHERE key0 = value0 AND ... AND keyN = valueN". Both the keys
 *   and values of the $param array must be strings.
 *
 * @return mixed
 *   Returns an array of instance objects keyed by the instance iid. If there
 *   are no matches an empty array will be returned. If there was an error FALSE
 *   will be returned.
 */
function hosting_package_instances_load($param) {
  return _hosting_package_instances_load($param, TRUE);
}

/**
 * Internal function for hosting_package_instance(s)_load.
 */
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
    $instance->languages = _hosting_package_instances_load_languages($instance->iid);
    if (!$multiple) {
      return $instance;
    }
    $instances[$instance->iid] = $instance;
  }
  return $instances;
}

/**
 * Get the languages associated with an instance of a package.
 */
function _hosting_package_instances_load_languages($instance_id = NULL, $reset = FALSE) {
  static $instances = array();
  if (!empty($reset)) {
    $instances = array();
  }

  // Build the array of languages if requested and we need to.
  if (!is_null($instance_id) && !isset($instances[$instance_id])) {
    $languages = array('en' => 'en');

    // load language options: if argument is null, load all language options
    $lang_result = db_query("SELECT DISTINCT(language) FROM {hosting_package_languages} WHERE iid = %d", $instance_id);
    while ($language = db_fetch_object($lang_result)) {
      $languages[$language->language] = _hosting_language_name($language->language);
    }

    $instances[$instance_id] = $languages;
  }

  // Return the array of languages if requested.
  return !is_null($instance_id) ? $instances[$instance_id] : NULL;

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

  $current_tbl = _hosting_package_temporary_table($current);
  $target_tbl = _hosting_package_temporary_table($target);

  $status = array();
  $result = db_query("SELECT count(c.nid) AS error FROM %s c LEFT JOIN %s t ON c.nid=t.nid WHERE (t.schema_version > 0) && (c.schema_version > t.schema_version) AND c.status=1", $current_tbl, $target_tbl);
  while ($obj = db_fetch_object($result)) {
    $status['error'] = $obj->error;
  }

  $result = db_query("SELECT COUNT(c.nid) as missing FROM %s c LEFT JOIN %s t ON c.nid=t.nid WHERE t.nid IS null AND c.status=1", $current_tbl, $target_tbl);
  while ($obj = db_fetch_object($result)) {
    $status['missing'] = $obj->missing;
  }

  $result = db_query("SELECT COUNT(c.nid) as upgrade FROM %s c LEFT JOIN %s t ON c.nid=t.nid WHERE (c.version_code < t.version_code) OR (c.schema_version < t.schema_version) AND c.status=1", $current_tbl, $target_tbl);
  while ($obj = db_fetch_object($result)) {
    $status['upgrade'] = $obj->upgrade;
  }

  $result = db_query("SELECT count(c.nid) AS downgrade FROM %s c LEFT JOIN %s t ON c.nid=t.nid WHERE (c.version_code > t.version_code)", $current_tbl, $target_tbl);
  while ($obj = db_fetch_object($result)) {
    $status['downgrade'] = $obj->downgrade;
  }

  return $status;
}


function _hosting_package_temporary_table($ref) {
  static $names = array();

  if (!isset($names[$ref])) {
    // Generate a unique table name for this $ref.
    $names[$ref] = '_hosting_comparision_' . trim($ref);

    // Create the temporary table.
    db_query("CREATE TEMPORARY TABLE %s AS SELECT p.nid, p.short_name, p.old_short_name, i.version_code, i.schema_version, i.version, i.status FROM {hosting_package} p LEFT JOIN {hosting_package_instance} i on p.nid = i.package_id WHERE i.rid = %d", $names[$ref], $ref);
  }


  return $names[$ref];
}
