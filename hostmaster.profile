<?php
// $Id$

/**
 * Return an array of the modules to be enabled when this profile is installed.
 *
 * @return
 *  An array of modules to be enabled.
 */
function hostmaster_profile_modules() {
  return array(
    /* core */ 'block', 'color', 'filter', 'help', 'menu', 'node', 'system', 'user', 'watchdog',
    /* contrib */ 'drush', 'views', 'views_ui', 'cvs_deploy',
    /* custom */ 'provision', 'provision_apache', 'provision_mysql', 'provision_drupal', 'hosting');
}

/**
 * Return a description of the profile for the initial installation screen.
 *
 * @return
 *   An array with keys 'name' and 'description' describing this profile.
 */
function hostmaster_profile_details() {
  return array(
    'name' => 'Hostmaster',
    'description' => 'Select this profile to manage the installation and maintenance of hosted Drupal sites.'
  );
}

/**
 * Perform any final installation tasks for this profile.
 *
 * @return
 *   An optional HTML string to display to the user on the final installation
 *   screen.
 */
function hostmaster_profile_final() {
  /* Default node types and default node */
 $types =  node_types_rebuild();

  /**
  * Generate administrator account
  */
  $user = new stdClass();
  $edit['name'] = 'Administrator';
  $edit['pass'] = user_password();
  $edit['mail'] = valid_email_address($_SERVER['SERVER_ADMIN']) ? $_SERVER['SERVER_ADMIN'] : 'changeme@example.com';
  $edit['status'] = 1;
  $user = user_save($user,  $edit);
  $GLOBALS['user'] = $user;

  
  /* Default client */
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'client';
  $node->email = ($_SERVER['SERVER_ADMIN']) ? $_SERVER['SERVER_ADMIN'] : 'changeme@example.com';
  $node->name = 'Administrator';
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_client', $node->nid);  

  /* Default database server */
  global $db_url;
  $url = parse_url($db_url);

  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'db_server';
  $node->title = $url['host'];
  $node->db_type = $url['scheme'];
  if (_provision_mysql_can_create_database()) {
    $node->db_user = $url['user'];
    $node->db_passwd = $url['pass'];
  }
  else {
    $node->db_user = 'root';
    $node->db_passwd = 'password';    
  }
  $node->status = 1;

  node_save($node);
  variable_set('hosting_default_db_server', $node->nid);
  variable_set('hosting_own_db_server', $node->nid);
  
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'web_server';
  $node->title = $_SERVER['HTTP_HOST'];
  $node->script_user = provision_get_script_owner();
  $node->web_group = provision_get_group_name();
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_web_server', $node->nid);
 
  $node = new stdClass();
  $node->uid = 1;
  $node->title = 'Drupal';
  $node->type = 'package';
  $node->package_type = 'platform';
  $node->short_name = 'drupal';
  $node->status = 1;
  node_save($node);
  $package_id = $node->nid;

  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'package_release';
  $node->title = 'Drupal ' . VERSION;
  $node->package = $package_id;
  $node->version = VERSION;
  $node->schema_version = drupal_get_installed_schema_version('system');
  $node->status = 1;
  node_save($node);
  $release_id = $node->nid;

  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'platform';
  $node->title = "Drupal " . VERSION . ' on ' . $_SERVER['HTTP_HOST'];
  $node->publish_path = $_SERVER['DOCUMENT_ROOT'];
  $node->web_server = variable_get('hosting_default_web_server', 3);
  $node->release_id = $release_id;
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_platform', $node->nid);
  variable_set('hosting_own_platform', $node->nid);

  #verify platform
  hosting_add_action(variable_get('hosting_own_platform', 7), "verify");

  _hosting_add_block("views", "platforms", "garland", 1, 0, "right");
  _hosting_add_block("views", "servers", "garland", 1, 0, "right");

  #initial configuration of hostmaster - todo
  variable_set('site_frontpage', 'sites');

  // @todo create proper roles, and set up views to be role based
  hostmaster_install_set_permissions(hostmaster_install_get_rid('anonymous user'), array('access content', 'access all views'));
  hostmaster_install_set_permissions(hostmaster_install_get_rid('authenticated user'), array('access content', 'access all views'));
  views_invalidate_cache();
  menu_rebuild();

  drupal_goto('sites');
}

/**
 * Set the permission for a certain role
 */
function hostmaster_install_set_permissions($rid, $perms) {
  db_query('DELETE FROM {permission} WHERE rid = %d', $rid);
  db_query("INSERT INTO {permission} (rid, perm) VALUES (%d, '%s')", $rid, implode(', ', $perms));
}

/**
 * Get the role id for the role name
 */
function hostmaster_install_get_rid($name) {
  return db_result(db_query("SELECT rid FROM {role} WHERE name ='%s' LIMIT 1", $name));
}
