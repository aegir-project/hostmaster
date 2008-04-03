<?php
/**
 * @file Defines package node types
 *
 * Packages are somewhat analogous to Drupal.org projects. IE: components that can be installed onto Drupal sites.
 */

function hosting_get_package($short_name) {
  $result = db_query("SELECT nid FROM {hosting_package} WHERE short_name = '%s'", $short_name);
  if ($nid = db_result($result)) {
    return node_load($nid);
  }
  else {
    return false;
  }
}

function hosting_get_package_release($short_name, $version) {
  $result = db_query("SELECT r.nid FROM {hosting_package} p LEFT JOIN {hosting_package_release} r ON p.nid = r.package WHERE p.short_name = '%s' AND r.version = '%s'", $short_name, $version);
  if ($nid = db_result($result)) {
    return node_load($nid);
  }
  else {
    return false;
  }
}

function hosting_delete_package_instances($nid) {
  db_query("DELETE FROM {hosting_package_instances} WHERE rid = %d", $nid);
}

function hosting_add_package_instance($nid, $rid, $path) {
  $iid = db_next_id("{hosting_package_instance}_iid");
  db_query("INSERT INTO {hosting_package_instances} (iid, release_id, rid, path) VALUES (%d, %d, %d, '%s')", $iid, $nid, $rid, $path);
} 

/**
 * Implementation of hook_insert().
 */
function hosting_package_insert($node) {
  db_query("INSERT INTO {hosting_package} (vid, nid, package_type, short_name ) VALUES (%d, %d, '%s', '%s')",
    $node->vid, $node->nid, $node->package_type, $node->short_name );
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_package_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_package_insert($node);
  }
  else {
    db_query("UPDATE {hosting_package} SET package_type = '%s', short_name = '%s' WHERE nid=%d",
              $node->package_type, $node->short_name, $node->vid, $node->nid);
  }
}

function hosting_nodeapi_package_delete_revision(&$node) {
  db_query('DELETE FROM {hosting_package} WHERE vid = %d', $node->vid);
}

/**
 * Implementation of hook_delete().
 */
function hosting_package_delete($node) {
  db_query('DELETE FROM {hosting_package} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 */
function hosting_package_load($node) {
  $additions = db_fetch_object(db_query('SELECT package_type, short_name FROM {hosting_package} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_package_view($node, $teaser = FALSE, $page = FALSE) {
  $node->content['package_type'] = array(
    '#type' => 'item',
    '#title' => t('Package Type'),
    '#value' => $node->package_type,
  );
  $node->content['short_name'] = array(
    '#type' => 'item',
    '#title' => t('Project name'),
    '#value' => $node->short_name,
  );

  $view = views_get_view('package_release_embed');
  $node->content['release_view'] = array(
    '#type' => 'item',
    '#title' => t("Releases"),
    '#value' => views_build_view('embed', $view, array($node->nid)), 
    '#weight' => 10
  );

  return $node;
}


function hosting_package_release_set_title(&$node) {
  $package = node_load($node->package);
  $node->title = $package->title . ' ' . $node->version;
  db_query("UPDATE {node} SET title='%s' WHERE nid=%d", $node->title, $node->nid);
  db_query("UPDATE {node_revisions} SET title='%s' WHERE vid=%d", $node->title, $node->vid);
}

/**
 * Implementation of hook_insert().
 */
function hosting_package_release_insert($node) {
  db_query("INSERT INTO {hosting_package_release} (vid, nid, package, schema_version, version ) VALUES (%d, %d, %d, %d, '%s')",
    $node->vid, $node->nid, $node->package, $node->schema_version, $node->version );
  hosting_package_release_set_title($node);
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_package_release_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_package_release_insert($node);
  }
  else {
    db_query("UPDATE {hosting_package_release} SET package = %d, schema_version = %d, version = '%s' WHERE nid=%d",
              $node->package, $node->schema_version, $node->version, $node->vid, $node->nid);
  }
  hosting_package_release_set_title($node);
}

function hosting_nodeapi_package_release_delete_revision(&$node) {
  db_query('DELETE FROM {hosting_package_release} WHERE vid = %d', $node->vid);
}

/**
 * Implementation of hook_delete().
 */
function hosting_package_release_delete($node) {
  db_query('DELETE FROM {hosting_package_release} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 */
function hosting_package_release_load($node) {
  $additions = db_fetch_object(db_query('SELECT package, schema_version, version FROM {hosting_package_release} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_package_release_view($node, $teaser = FALSE, $page = FALSE) {
  $node->content['package'] = array(
    '#type' => 'item',
    '#title' => t('Package'),
    '#value' => _hosting_node_link($node->package),
    '#weight' => -1
  );
  $node->content['version'] = array(
    '#type' => 'item',
    '#title' => t('Version'),
    '#value' => $node->version,
  );
  if ($node->schema_version) {
    $node->content['schema_version'] = array(
      '#type' => 'item',
      '#title' => t('Schema Version'),
      '#value' => $node->schema_version,
    );
  }
  return $node;
}

function hosting_package_views_tables() {

  $tables['hosting_package'] = array(
    'name' => 'hosting_package',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid',
      ),
      'right' => array(
        'field' => 'vid',
      ),
    ),
    'fields' => array(
      'package_type' => array(
        'name' => t('Package: Package Type'),
        'help' => t('Display the type of package.'),
      ),
      'short_name' => array(
        'name' => t('Package: Project Name'),
        'sortable' => TRUE,
        'help' => t('Display the name of the project.'),
      ),
    ),
  );

  $tables['hosting_package_release'] = array(
    'name' => 'hosting_package_release',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid',
      ),
      'right' => array(
        'field' => 'vid',
      ),
    ),
    'fields' => array(
      'package' => array(
        'name' => t('Package Release: Package'),
        'handler' => 'hosting_relation_link_handler',
        'help' => t('Display the name of the package the release is for.'),
      ),
      'version' => array(
        'name' => t('Package Release: Version'),
        'sortable' => TRUE,
        'help' => t('Display the version of the package.'),
      ),
      'schema_version' => array(
        'name' => t('Package Release: Schema Version'),
        'sortable' => TRUE,
        'help' => t('Display the database schema version of the module.'),
      ),
    ),
  );

  $tables['hosting_package_instance'] = array(
    'name' => 'hosting_package_instance',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'release_id',
      ),
    ),
    'fields' => array(
      'release_id' => array(
        'name' => t('Package Instance: Release'),
        'handler' => 'hosting_relation_link_handler',
        'help' => t('Display the name of the package linked to the platform/site.'),
      ),
      'rid' => array(
        'name' => t('Package Instance: Reference'),
        'sortable' => TRUE,
        'help' => t('Display the name of the package or site that this instance has been linked to.'),
      ),
      'path' => array(
        'name' => t('Package Instance: Path'),
        'sortable' => TRUE,
        'help' => t('Display where the package is in this instance.'),
      ),
    ),
  );
  return $tables;
}


/**
 * Views 1.x argument handling for package nodes.
 */
function hosting_package_views_arguments() {
  $arguments['hosting_package_reference'] = array(
    'name' => t('Package Release: Package'),
    'help' => t('Filter package releases by the package.'),
    'handler' => 'hosting_package_release_package_argument',
  );
  return $arguments;
}

/**
 * Views 1.x argument handler for filtering package nodes by reference.
 */
function hosting_package_release_package_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_package_release", "package", $op, $query, $a1, $a2);
}

function hosting_package_import_packages($rid, $type, $default_version, $data) {
  hosting_delete_package_instances($rid);
  // Import install profiles as package and release nodes
  foreach ($data as $short_name => $info) {
    $version = ($info->info['version']) ? $info->info['version'] : $default_version; // dirty hack. but ugh.
    if (!$package = hosting_get_package($short_name)) {
      $node = new stdClass();
      $node->type = 'package';
      $node->title = $info->info['name'];
      $node->package_type = $type;
      $node->short_name = $short_name;
      $node->status = 1;
      node_save($node);
      $package = $node;
    }
    if (!$release = hosting_get_package_release($short_name, $default_version)) {
      $node = new stdClass();
      $node->type = 'package_release';
      $node->package = $package->nid;
      $node->version = $version;
      $node->status = 1;
      node_save($node);
      $release = $node;
    }
    hosting_add_package_instance($release->nid, $rid, $info->filename); 
  }

}

