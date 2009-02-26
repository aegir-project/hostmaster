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
    /* custom */ 'hosting', 'hosting_task', 'hosting_client', 'hosting_db_server', 'hosting_package', 'hosting_platform', 'hosting_site', 'hosting_web_server');
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

  // Initialize the hosting defines
  hosting_init();

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
  $node->client_name = 'Administrator';
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
  $node->db_user = $url['user'];
  $node->db_passwd = $url['pass'];
  $node->status = 1;

  node_save($node);
  variable_set('hosting_default_db_server', $node->nid);
  variable_set('hosting_own_db_server', $node->nid);
  
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'web_server';
  $node->title = $_SERVER['HTTP_HOST'];
  $node->script_user = HOSTING_DEFAULT_SCRIPT_USER;
  $node->web_group = HOSTING_DEFAULT_WEB_GROUP;
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
  $node->title = $_SERVER['HTTP_HOST'] . ' (Drupal ' . VERSION . ')';
  $node->publish_path = $_SERVER['DOCUMENT_ROOT'];
  $node->web_server = variable_get('hosting_default_web_server', 3);
  $node->release_id = $release_id;
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_platform', $node->nid);
  variable_set('hosting_own_platform', $node->nid);


  #initial configuration of hostmaster - todo
  variable_set('site_name', t('Hostmaster'));
  variable_set('site_frontpage', 'hosting/sites');

  // This is set to true, because the node/add/site form needs
  // to use AHAH to create a valid node, and ahah_forms requires clean_urls
  variable_set('clean_url', TRUE);

  // This is saved because the config generation script is running via drush, and does not have access to this value
  variable_set('install_url' , $GLOBALS['base_url']);

  // add default blocks
  hostmaster_install_add_block('hosting', 'hosting_summary', 'garland', 1, 10, 'left');
  hostmaster_install_add_block('hosting', 'hosting_queues', 'garland', 1, 0, 'right');
  hostmaster_install_add_block('hosting', 'hosting_queues_summary', 'garland', 1, 2, 'right');

  // @todo create proper roles, and set up views to be role based
  hostmaster_install_set_permissions(hostmaster_install_get_rid('anonymous user'), array('access content', 'access all views'));
  hostmaster_install_set_permissions(hostmaster_install_get_rid('authenticated user'), array('access content', 'access all views'));
  hostmaster_install_create_role('aegir client');
  // @todo we may need to have a hook here to consider plugins
  hostmaster_install_set_permissions(hostmaster_install_get_rid('aegir client'), array('access content', 'access all views', 'edit own client', 'view client', 'create site', 'delete site', 'view site', 'create backup task', 'create delete task', 'create disable task', 'create enable task', 'create restore task', 'view own tasks', 'view task'));
  menu_rebuild();

  node_access_rebuild();
  drupal_goto('hosting/wizard');
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


/**
 * Create a role
 */
function hostmaster_install_create_role($role_name) {
  db_query("INSERT INTO {role} (name) VALUES ('%s')", $role_name);
}

/**
 * Creates a new block.
 */
function hostmaster_install_add_block($module, $delta, $theme, $status, $weight, $region, $visibility = 0, $pages = '', $custom = 0, $throttle = 0, $title = '') {
  db_query("INSERT INTO {blocks} (module, delta, theme, status, weight, region, visibility, pages, custom, throttle, title) 
     VALUES ('%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, '%s')", 
     $module, $delta, $theme, $status, $weight, $region, $visibility, $pages, $custom, $throttle, $title);
  if ($module == 'block') {
    $box = db_fetch_object(db_query('SELECT * FROM {boxes} WHERE bid=%d', $delta));
    db_query("INSERT INTO {boxes} (bid, body, info, format) VALUES (%d, '%s', '%s', '%s')", $box->bid, $box->body, $box->info, $box->format);
  }
}

