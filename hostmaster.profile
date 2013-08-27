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
    /* aegir contrib */ 'hosting', 'hosting_task', 'hosting_client', 'hosting_db_server', 'hosting_package', 'hosting_platform', 'hosting_site', 'hosting_web_server', 'hosting_server', 'hosting_clone', 'hosting_cron', 'hosting_migrate',
    /* other contrib */ 'install_profile_api', 'jquery_ui', 'jquery_update', 'modalframe', 'admin_menu', 'views', 'views_bulk_operations', 'actions_permissions',
    /* Aegir actual contrib */
    'hosting_platform_pathauto',
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

  // add support for nginx
  if (d()->platform->server->http_service_type === 'nginx') {
    drupal_install_modules(array('hosting_nginx'));
  }

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
  $node->title = drush_get_option('client_name', 'admin');
  $node->status = 1;
  node_save($node);
  variable_set('hosting_default_client', $node->nid);
  variable_set('hosting_admin_client', $node->nid);

  $client_id = $node->nid;

  /* Default server */
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'server';
  $node->title = php_uname('n');
  $node->status = 1;
  $node->hosting_name = 'server_master';
  $node->services = array();

  /* Make it compatible with more than apache and nginx */
  $master_server = d()->platform->server;
  hosting_services_add($node, 'http', $master_server->http_service_type, array(
   'restart_cmd' => $master_server->http_restart_cmd,
   'port' => urldecode($master_server->http_port),
   'available' => 1,
  ));

  /* examine the db server associated with the hostmaster site */
  $db_server = d()->db_server;
  $master_db = parse_url($db_server->master_db);
  /* if it's not the same server as the master server, create a new node
   * for it */
  if ($db_server->remote_host == $master_server->remote_host) {
    $db_node = $node;
  } else {
    $db_node = new stdClass();
    $db_node->uid = 1;
    $db_node->type = 'server';
    $db_node->title = $master_db['host'];
    $db_node->status = 1;
    $db_node->hosting_name = 'server_' . $db_server->remote_host;
    $db_node->services = array();
  }
  hosting_services_add($db_node, 'db', $db_server->db_service_type, array(
    'db_type' => $master_db['scheme'],
    'db_user' => urldecode($master_db['user']),
    'db_passwd' => urldecode($master_db['pass']),
    'port' => urldecode($master_db['port']),
    'available' => 1,
  ));

  drupal_set_message(st('Creating master server node'));
  node_save($node);
  if ($db_server->remote_host != $master_server->remote_host) {
    drupal_set_message(st('Creating db server node'));
    node_save($db_node);
  }
  variable_set('hosting_default_web_server', $node->nid);
  variable_set('hosting_own_web_server', $node->nid);

  variable_set('hosting_default_db_server', $db_node->nid);
  variable_set('hosting_own_db_server', $db_node->nid);

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
  $node->title = 'hostmaster';
  $node->publish_path = d()->root;
  $node->web_server = variable_get('hosting_default_web_server', 2);
  $node->status = 1;
  node_save($node);
  $platform_id = $node->nid;
  variable_set('hosting_own_platform', $node->nid);


  $instance = new stdClass();
  $instance->rid = $node->nid;
  $instance->version = VERSION;
  $instance->schema_version = drupal_get_installed_schema_version('system');
  $instance->package_id = $package_id;
  $instance->status = 0;
  hosting_package_instance_save($instance);

  // Create the hostmaster profile node
  $node = new stdClass();
  $node->uid = 1;
  $node->title = 'hostmaster';
  $node->type = 'package';
  $node->package_type = 'profile';
  $node->short_name = 'hostmaster';
  $node->status = 1;
  node_save($node);

  $profile_id = $node->nid;

  // Create the main Aegir site node
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'site';
  $node->title = d()->uri;
  $node->platform = $platform_id;
  $node->client = $client_id;
  $node->db_server = $db_node->nid;
  $node->profile = $profile_id;
  $node->import = true;
  $node->hosting_name = 'hostmaster';
  $node->status = 1;
  node_save($node);

  variable_set('site_frontpage', 'hosting/sites');

  // do not allow user registration: the signup form will do that
  variable_set('user_register', 0);

  // This is saved because the config generation script is running via drush, and does not have access to this value
  variable_set('install_url' , $GLOBALS['base_url']);
}

function hostmaster_task_finalize() {
  variable_set('install_profile', 'hostmaster');
  drupal_set_message(st('Configuring menu items'));

  install_include(array('menu'));
  $menu_name = variable_get('menu_primary_links_source', 'primary-links');

  // @TODO - seriously need to simplify this, but in our own code i think, not install profile api
  $items = install_menu_get_items('hosting/servers');
  $item = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE mlid = %d", $items[0]['mlid']));
  $item['menu_name'] = $menu_name;
  $item['customized'] = 1;
  $item['options'] = unserialize($item['options']);
  install_menu_update_menu_item($item);

  menu_rebuild();


  $theme = 'eldir';
  drupal_set_message(st('Configuring Eldir theme'));
  install_disable_theme('garland');
  install_default_theme('eldir');
  system_theme_data();

  db_query("DELETE FROM {cache}");

  drupal_set_message(st('Configuring default blocks'));
  install_add_block('user', 0, $theme, 1, -1, 'right', 1);
  install_add_block('user', 1, $theme, 1, 1, 'right', 1);
  install_add_block('hosting', 'hosting_queues', $theme, 1, 5, 'right', 1);
  install_add_block('views', 'hosting_task_list-block' , $theme, 1, 0, 'right', 0);
  // TODO: Set visibility in Views on Drupal 7 
  install_add_block('views', 'hosting_site_list-block_1' , $theme, 1, 0, 'content_bottom', 1, 'hosting/c/platform_*');
  install_add_block('views', 'hosting_site_list-block_profile' , $theme, 1, 0, 'content_bottom', 2, "<?php\n
return hosting_site_profile_block_visibility();
\n?>");
  install_add_block('views', 'hosting_site_list-block_client' , $theme, 1, 0, 'content_bottom', 2, "<?php\n
return hosting_site_client_admin_block_visibility();
\n?>");
  install_add_block('views', 'hosting_site_list-block_client2', $theme, 1, 0, 'content_bottom', 2, "<?php\n
return hosting_site_client_list_block_visibility();
\n?>");
  install_add_block('views', 'hosting_package_list-block_1' , $theme, 1, 0, 'content_bottom', 2, "<?php\n
return hosting_site_profile_block_visibility();\n
?>");

  drupal_set_message(st('Configuring roles'));
  install_remove_permissions(install_get_rid('anonymous user'), array('access content', 'access all views'));
  install_remove_permissions(install_get_rid('authenticated user'), array('access content', 'access all views'));
  install_add_permissions(install_get_rid('anonymous user'), array('access disabled sites'));
  install_add_permissions(install_get_rid('authenticated user'), array('access disabled sites'));
  install_add_role('aegir client');
  // @todo we may need to have a hook here to consider plugins
  install_add_permissions(install_get_rid('aegir client'), array('access content', 'edit own client', 'view client', 'create site', 'delete site', 'view site', 'create backup task', 'create delete task', 'create disable task', 'create enable task', 'create restore task', 'view own tasks', 'view task', 'cancel own tasks', 'view own quota', 'create site aliases', 'create verify task', 'create clone task', 'create migrate task', 'create ssl certificate', 'access task logs', 'view package', 'execute Site: Backup (hosting_site_op_backup)', 'execute Site: Delete (hosting_site_op_delete)', 'execute Site: Disable (hosting_site_op_disable)', 'execute Site: Enable (hosting_site_op_enable)', 'execute Site: Reset password (hosting_site_op_login_reset)', 'execute Site: Verify (hosting_site_op_verify)'), 'view platform', 'view revisions', 'edit site');
  install_add_role('aegir account manager');
  install_add_permissions(install_get_rid('aegir account manager'), array('create client', 'edit client users', 'view client', 'administer clients', 'access content', 'access user profiles', 'edit all quotas', 'view all quotas', 'view own quota', 'edit client uname'), 'view_site');
  install_add_role('aegir platform manager');
  install_add_permissions(install_get_rid('aegir platform manager'), array('create clone task', 'create migrate task', 'create platform', 'delete platform', 'edit platform', 'view locked platforms', 'view platform', 'create sites on locked platforms', 'create site', 'delete site', 'edit site', 'view site', 'access task logs', 'create lock task', 'create unlock task', 'view revisions', 'search content', 'use advanced search', 'view package', 'administer platforms', 'cancel own tasks', 'view own tasks', 'access content', 'view own tasks', 'view task', 'administer sites', 'create verify task', 'retry failed tasks', 'administer clients', 'execute Site: Delete (hosting_site_op_delete)', 'execute Site: Disable (hosting_site_op_disable)', 'execute Site: Enable (hosting_site_op_enable)', 'execute Site: Verify (hosting_site_op_verify)', 'execute Platform: Delete (hosting_platform_op_delete)', 'execute Platform: Lock (hosting_platform_op_lock)', 'execute Platform: Unlock (hosting_platform_op_unlock)', 'execute Platform: Verify (hosting_platform_op_verify)', 'administer tasks'));
  install_add_role('aegir administrator');
  install_add_permissions(install_get_rid('aegir administrator'), array('access disabled sites', 'access hosting wizard', 'administer hosting', 'administer hosting features', 'administer hosting queues', 'administer hosting settings', 'administer hosting aliases', 'create site aliases', 'administer clients', 'create client', 'delete own client', 'edit client uname', 'edit client users', 'edit own client', 'view client', 'create clone task', 'create migrate task', 'create package', 'delete package', 'edit package', 'view package', 'create platform', 'delete platform', 'edit platform', 'view locked platforms', 'view platform', 'create sites on locked platforms', 'edit all quotas', 'view all quotas', 'view own quota', 'create server', 'delete server', 'edit server', 'view server', 'access hosting signup form', 'administer sites', 'create site', 'delete site', 'edit site', 'view site', 'access task logs', 'administer tasks', 'cancel own tasks', 'create backup task', 'create backup-delete task', 'create delete task', 'create disable task', 'create enable task', 'create lock task', 'create login-reset task', 'create restore task', 'create unlock task', 'create verify task', 'retry failed tasks', 'view own tasks', 'view task', 'access administration menu', 'administer platforms', 'administer servers', 'access content', 'view revisions', 'administer hosting aliases', 'create site aliases', 'edit all quotas', 'view all quotas', 'view own quota', 'access hosting signup form', 'create ssl certificate', 'access user profiles', 'execute Site: Backup (hosting_site_op_backup)', 'execute Site: Delete (hosting_site_op_delete)', 'execute Site: Disable (hosting_site_op_disable)', 'execute Site: Enable (hosting_site_op_enable)', 'execute Site: Reset password (hosting_site_op_login_reset)', 'execute Site: Verify (hosting_site_op_verify)', 'execute Platform: Delete (hosting_platform_op_delete)', 'execute Platform: Lock (hosting_platform_op_lock)', 'execute Platform: Unlock (hosting_platform_op_unlock)', 'execute Platform: Verify (hosting_platform_op_verify)'), 'update status of tasks');

  node_access_rebuild();
}
