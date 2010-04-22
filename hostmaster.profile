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
    /* core */ 'block', 'color', 'filter', 'help', 'menu', 'node', 'system', 'user',
    /* aegir contrib */ 'hosting', 'hosting_task', 'hosting_client', 'hosting_db_server', 'hosting_package', 'hosting_platform', 'hosting_site', 'hosting_web_server', 'hosting_server',
    /* other contrib */ 'install_profile_api' /* needs >= 2.1 */, 'jquery_ui', 'modalframe'
  );
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

function hostmaster_profile_tasks(&$task, $url) {
  // Install dependencies
  install_include(hostmaster_profile_modules());

  // Bootstrap and create all the initial nodes
  hostmaster_bootstrap();

  // Finalize and setup themes, menus, optional modules etc
  hostmaster_task_finalize();
}

function hostmaster_bootstrap() {
  /* Default node types and default node */
  $types =  node_types_rebuild();

  variable_set('install_profile', 'hostmaster');
  // Initialize the hosting defines
  hosting_init();
  
  /* Default client */
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'client';
  $node->email = drush_get_option('client_email', 'webmaster@localhost');
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_client', $node->nid);  
  variable_set('hosting_admin_client', $node->nid);

  /* Default server */
  global $db_url;
  $url = parse_url($db_url);


  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'server';
  $node->title = $_SERVER['HTTP_HOST'];
  $node->status = 1;
  $node->services = array();


  hosting_services_add($node, "server", array(
    'script_user' => drush_get_option('script_user'),
    'drush_path' => DRUSH_COMMAND,
    'available' => 1,
  ));

  hosting_services_add($node, "http", array(
   'web_group' => drush_get_option('web_group'),
   'restart_cmd' => drush_get_option('restart_cmd', _provision_default_restart_cmd()),
   'ports' => drush_get_option('web_port'),
   'config_path' => drush_get_option('config_path'),
   'backup_path' => drush_get_option('backup_path'),
   'available' => 1,
  ));

  node_save($node);
  variable_set('hosting_default_web_server', $node->nid);
  variable_set('hosting_own_web_server', $node->nid);


  if (!in_array($url['host'], array('localhost', '127.0.0.1', $_SERVER['HTTP_HOST']))) {
    $node = new stdClass();
    $node->uid = 1;
    $node->type = 'server';
    $node->title = $url['host'];
    $node->status = 1;
    $node->services = array();
  }

  hosting_services_add($node, "db", array(
    'db_type' => $url['scheme'],
    'available' => 1,
  ));

  node_save($node);
  variable_set('hosting_default_db_server', $node->nid);
  variable_set('hosting_own_db_server', $node->nid);


  // @todo - check if we need to create a separate server for the db service.



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
  $node->type = 'platform';
  $node->title = $_SERVER['HTTP_HOST'] . ' (Drupal ' . VERSION . ')';
  $node->publish_path = drush_locate_root(drush_get_option(array('r', 'root'), NULL));
  $node->web_server = variable_get('hosting_default_web_server', 3);
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_platform', $node->nid);
  variable_set('hosting_own_platform', $node->nid);


  $instance = new stdClass();
  $instance->rid = $node->nid;
  $instance->version = VERSION;
  $instance->schema_version = drupal_get_installed_schema_version('system');
  $instance->package_id = $package_id;
  $instance->status = 0;
  hosting_package_instance_save($instance);

  variable_set('site_frontpage', 'hosting/sites');

  // do not allow user registration: the signup form will do that
  variable_set('user_register', 0);

  // This is saved because the config generation script is running via drush, and does not have access to this value
  variable_set('install_url' , $GLOBALS['base_url']);

}


/**
 * Enable optional modules, if present
 */
function hostmaster_setup_optional_modules() {
  $optional = array('admin_menu');

  foreach ($optional as $name) {
    $exists = db_result(db_query("SELECT name FROM {system} WHERE type='module' and name='%s'", $name));

    drupal_install_modules(array($name));

    if ($exists == $name) {
      drupal_set_message(st("Enabling module !module", array('!module' => $name)));
      switch ($name) {
        case 'admin_menu' :
          variable_set('admin_menu_margin_top', 1);
          variable_set('admin_menu_position_fixed', 1);
          variable_set('admin_menu_tweak_menu', 0);
          variable_set('admin_menu_tweak_modules', 0);
          variable_set('admin_menu_tweak_tabs', 0);

          $menu_name = install_menu_create_menu(t('Administration'));
          $admin = install_menu_get_items('admin');
          $admin = install_menu_get_item($admin[0]['mlid']);
          $admin['menu_name'] = $menu_name;
          $admin['customized'] = 1;
          $admin['hidden'] = 0;
          menu_link_save($admin);
          menu_rebuild();

          break;
      }
    }
  }
}


function hostmaster_task_finalize() {
  variable_set('install_profile', 'hostmaster');
  drupal_set_title(st("Configuring roles, blocks and theme"));

  install_include(array('menu'));
  $menu_name = variable_get('menu_primary_links_source', 'primary-links');

  $items = install_menu_get_items('hosting/servers');
  $item = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE mlid = %d", $items[0]['mlid']));
  $item['menu_name'] = $menu_name;
  $item['customized'] = 1;
  $item['options'] = unserialize($item['options']);
  install_menu_update_menu_item($item);

  $items = install_menu_get_items('hosting/sites');
  $item = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE mlid = %d", $items[0]['mlid']));
  $item['menu_name'] = $menu_name;
  $item['customized'] = 1;
  $item['options'] = unserialize($item['options']);
  install_menu_update_menu_item($item);

  $items = install_menu_get_items('hosting/platforms');
  $item = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE mlid = %d", $items[0]['mlid']));
  $item['menu_name'] = $menu_name;
  $item['customized'] = 1;
  $item['options'] = unserialize($item['options']);
  install_menu_update_menu_item($item);

  menu_rebuild();


  // enable the eldir theme, if present
  $themes = system_theme_data();
  if ($themes['eldir'] || file_exists("profiles/hostmaster/themes/eldir/eldir.info")) {
    drupal_set_message(st("Enabling optional Eldir theme"));
    install_disable_theme('garland');
    install_default_theme('eldir');
    $theme = 'eldir';
  } else {
    $theme = 'garland';
    drupal_set_message(st("Could not find optional Eldir theme"));
  }
  // not sure this is necessary
  system_theme_data();
  db_query("DELETE FROM {cache}");

  drupal_set_message(st('Adding default blocks'));
  install_add_block('hosting', 'hosting_queues', $theme, 1, 5, 'right', 1);
  install_add_block('hosting', 'hosting_summary', $theme, 1, 10, 'right', 1);

  drupal_set_message(st('Setting up roles'));
  install_remove_permissions(install_get_rid('anonymous user'), array('access content', 'access all views'));
  install_remove_permissions(install_get_rid('authenticated user'), array('access content', 'access all views'));
  install_add_role('aegir client');
  // @todo we may need to have a hook here to consider plugins
  install_add_permissions(install_get_rid('aegir client'), array('access content', 'access all views', 'edit own client', 'view client', 'create site', 'delete site', 'view site', 'create backup task', 'create delete task', 'create disable task', 'create enable task', 'create restore task', 'view own tasks', 'view task'));
  install_add_role('aegir account manager');
  install_add_permissions(install_get_rid('aegir account manager'), array('create client', 'edit client users', 'view client'));

  drupal_set_message(st('Enabling optional, yet recommended modules'));
  hostmaster_setup_optional_modules();

  node_access_rebuild();

}

