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

function pr($var) {
  echo "<pre>";
  print_r($var);
  echo "</pre>";
}

function hostmaster_profile_task_list() {
  return array(
    'intro' => st('Getting started'),
    'webserver' => st('Web server'),
    'filesystem' => st('Filesystem'),
    'dbserver' => st('Database server'),
    'features' => st('Features'),
    'init' => st('Initialize system'),
    'verify' => st('Verify settings'),
    'import' => st('Import sites'),
    'finalize' => st('Finalize')
  );
}

function hostmaster_get_task($task, $offset = 0) {
  static $tasks;
  static $keys;
  if (!$tasks) {
    $tasks = hostmaster_profile_task_list();
    $keys = array_keys($tasks);
  }


  if (($task == $keys[sizeof($keys) - 1]) && ($offset > 0)) {
    return 'profile-finished';
  }

  // finish if the task is the last one and the offset is positive
  if (($tid = array_search($task, $keys)) === FALSE ) {
    // at beginning 
    $tid = 0;
  }

  $tid = $tid + $offset;
  
  // reset to beginning if it tries to go back too far 
  if ($tid < 0) {
    $tid = 0;
  }


  return $keys[$tid];
}



function hostmaster_profile_tasks(&$task, $url) {
  include_once(dirname(__FILE__) . '/hostmaster.forms.inc');
  define("HOSTMASTER_FORM_REDIRECT", $url);
  if ($task == 'profile') {
    hostmaster_settings_php();
    hostmaster_bootstrap();
  }
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
  $task = hostmaster_get_task($task, variable_get('hostmaster_wizard_offset', 0));
  variable_del('hostmaster_wizard_offset');
  define("HOSTMASTER_CURRENT_TASK", $task);


   $func = sprintf("hostmaster_task_%s", $task);
  if (function_exists($func)) {
    return drupal_get_form($func);
  }


  return $task;
  return "123";


  if ($tid == (sizeof($keys) - 1)) {
  #  $task = 'profile-finished';
  }
}

function hostmaster_requirement_help($requirement, $options = array()) {
  $form = array_merge(_element_info('requirement_help'), $options);
  $form['#requirement'] = $requirement;
  $form['#help'] = hosting_get_requirement($requirement);
  $form['#value'] = theme_requirement_help($form);
  return $form;
}


function hostmaster_settings_php() {
  $settings_file = realpath(conf_path() . '/settings.php');
  drupal_verify_install_file($settings_file, FILE_READABLE|FILE_WRITABLE);
  $fp = fopen($settings_file, "a");
  fwrite($fp, "\n\n\$conf['install_profile'] = 'hostmaster';\n");
  fclose($fp);
  drupal_verify_install_file($settings_file, FILE_READABLE|FILE_NOT_WRITABLE);
}

/**
 * Form modifier similar to confirm_form
 *
 * Handles some bookkeeping like adding the js and css, 
 * embedded the right classes, and most importantly : adding the wizard_form_submit
 * to the #submit element. Without this, you would never be forwarded to the next
 * page.
 */
function hostmaster_form($form) {
  global $task;

  $form['#redirect'] = HOSTMASTER_FORM_REDIRECT;

  $form['#prefix'] = '<div id="hosting-wizard-form">';
  $form['#suffix'] = '</div>';


  $form['wizard_form'] = array(
    '#prefix' => '<div id="hosting-wizard-form-buttons">',
    '#suffix' => '</div>',
    '#weight' => 100
  );

  if (HOSTMASTER_CURRENT_TASK != 'intro') {
    // add a back button
    $button = array(
      '#type' => 'submit',
      '#value' =>  '<- Previous',
    );
    $button['#submit'][] = 'hostmaster_form_previous';

    $form['wizard_form']['back'] = $button;
  }

  // add a next button
  $button = array(
    '#type' => 'submit',
    '#value' =>  'Next ->',
  );

  // only validate when next is pressed
  // also inherit the whole form's validate callback
  $button['#validate'] = $form['#validate'];
  $button['#validate'][] = 'hostmaster_form_validate';
  unset($form['#validate']);

  $button['#submit'] = array();
  if ($form['#node']) {
    $button['#submit'][] = 'node_form_submit';
  }
  // hook the regular form submit hooks on the wizard submit hook
  if ($form['#submit']) {
    $button['#submit'] = array_merge($button['#submit'], $form['#submit']);
  }
  $button['#submit'][] = 'hostmaster_form_next';

  $form['wizard_form']['submit'] = $button;
  return $form;
}

function hostmaster_form_next($form, $form_state) {
  // move forward a page
  variable_set('hostmaster_wizard_offset', 1);
}

function hostmaster_form_previous($form, $form_state) {
  // move forward a page
  variable_set('hostmaster_wizard_offset', -1);
}

function hostmaster_bootstrap() {
  /* Default node types and default node */
  $types =  node_types_rebuild();

  variable_set('install_profile', 'hostmaster');
  global $user;
  // Initialize the hosting defines
  hosting_init();
  
  /* Default client */
  $node = new stdClass();
  $node->uid = 1;
  $node->type = 'client';
  $node->email = $user->mail;
  $node->client_name = $user->name;
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
  $node->type = 'platform';
  $node->title = $_SERVER['HTTP_HOST'] . ' (Drupal ' . VERSION . ')';
  $node->publish_path = $_SERVER['DOCUMENT_ROOT'];
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
          menu_link_save($admin);
          menu_rebuild();

          break;
      }
    }
  }
}


/**
 * Take a node form and get rid of all the crud that we don't
 * need on a wizard form, in a non destructive manner.
 */
function _hostmaster_clean_node_form(&$form) {
  $form['revision_information']['revision']['#type'] = 'value';
  $form['revision_information']['log']['#type'] = 'value';
  $form['revision_information']['#type'] = 'markup';

  $form['author']['name']['#type'] = 'value';
  $form['author']['date']['#type'] = 'value';
  $form['author']['#type'] = 'markup';

  $form['options']['sticky']['#type'] = 'value';
  $form['options']['status']['#type'] = 'value';
  $form['options']['promote']['#type'] = 'value';
  $form['options']['#type'] = 'markup';

  unset($form['buttons']);
}

/**
 * Enable a theme.
 *
 * @param $theme
 *   Unique string that is the name of theme.
 */
function install_enable_theme($theme) {
  system_theme_data();
  db_query("UPDATE {system} SET status = 1 WHERE type = 'theme' and name = '%s'", $theme);
  system_initialize_theme_blocks($theme);
}

/**
 * Disable a theme.
 *
 * @param $theme
 *   Unique string that is the name of theme.
 */
function install_disable_theme($theme) {
  system_theme_data();
  db_query("UPDATE {system} SET status = 0 WHERE type = 'theme' and name ='%s'", $theme);
}

/**
 * Set default theme.
 *
 * @param $theme
 *   Unique string that is the name of theme.
 */
function install_default_theme($theme) {
  install_enable_theme($theme);
  variable_set('theme_default', $theme);
}


/**
 * Add a new plain block provided by block module.
 */
function install_add_block($module, $delta, $theme, $status, $weight = 0, $region = '', $visibility = 0, $pages = '', $custom = 0, $throttle = 0, $title = '') {
  if ($status) {
    db_query("INSERT INTO {blocks} (module, delta, theme, status, weight, region, visibility, pages, custom, throttle, title) 
       VALUES ('%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, '%s')", 
       $module, $delta, $theme, $status, $weight, $region, $visibility, $pages, $custom, $throttle, $title);
  }
  else {
    db_query("UPDATE {blocks} SET status = 0 WHERE module = '%s' AND delta = '%s' AND theme = '%s'", $module, $delta, $theme); 
  }
}

/**
 * Position an existing block inside a region of a theme.
 *
 * TIP: To identify the $module and $delta, go to an existing site and visit
 * the /admin/build/block page. Hover over the 'configure' links - the module
 * and delta are the last two parts of the target url.
 *
 * @param $theme
 *   A theme name, eg. 'garland'
 * @param $region
 *   Available region: usually one of 'header', 'footer', 'left', 'right', 'content'
 * @param $module
 *   The name of the module that provides the block
 * @param $delta
 *   The block id.
 * @param $weight
 *   Block order within the region.
 */
function install_set_block($module, $delta, $theme, $region, $weight = 0) {
  db_query("UPDATE {blocks} SET region = '%s', status = 1, weight = %d WHERE module = '%s' AND delta = '%s' AND theme = '%s'", $region, $weight, $module, $delta, $theme);
}

/**
* Disable a block within a theme.
*/
function install_disable_block($module, $delta, $theme) {
  db_query("UPDATE {blocks} SET region = '', status = 0 WHERE module = '%s' AND delta = '%s' AND theme = '%s'", $module, $delta, $theme);
}


/**
 * Add a role to the roles table.
 */
function install_add_role($name) {
  // Check to see if role already exists. If not, create it.
  $rid = install_get_rid($name);
  if (!$rid) {
    db_query("INSERT INTO {role} (name) VALUES ('%s')", $name);
  }
  
  return ($rid) ? $rid : install_get_rid($name);
}

/**
 * Get the role ID for the role name.
 */
function install_get_rid($name) {
  static $roles = array();
  if (empty($roles[$name])) {
    $roles[$name] = db_result(db_query_range("SELECT rid FROM {role} WHERE name ='%s'", $name, 0, 1));
  }
  return $roles[$name];
}

/**
 * Add the permission for a certain role.
 */
function install_add_permissions($rid, $perms) {
  // Retrieve the currently set permissions.
  $result = db_query("SELECT p.perm FROM {role} r INNER JOIN {permission} p ON p.rid = r.rid WHERE r.rid = %d ", $rid);
  $existing_perms = array();
  while ($row = db_fetch_object($result)) {
    $existing_perms += explode(', ', $row->perm);
  }
  // If this role already has permissions, merge them with the new permissions being set.
  if (count($existing_perms) > 0) { 
    $perms = array_unique(array_merge($perms, (array)$existing_perms));
  }

  // Update the permissions.
  db_query('DELETE FROM {permission} WHERE rid = %d', $rid);
  db_query("INSERT INTO {permission} (rid, perm) VALUES (%d, '%s')", $rid, implode(', ', $perms));
}


/**
 * Create a new top-level menu.
 *
 * @param $title
 *   The title of the menu as seen by users.
 * @param $menu_name
 *   Optional. System menu name.
 * @param $desc
 *   Optional. The description of the menu.
 * 
 * @return 
 *   Returns either FALSE if there was an error, or the system menu-name as stored in the db.
 */
function install_menu_create_menu($title, $menu_name = '', $desc = '') {
  $return = FALSE;

  if (module_exists('menu')) {
    $title            = $title;
    $description      = $desc;
    $mn               = (empty($menu_name)) ? $title : $menu_name;
    $menu_name        = str_replace(array(' ', '_'), '-', strtolower($mn));
    $menu_name_custom = 'menu-' . $menu_name;

    // Check the db for an existing menu by the same name.
    $menu = db_result(db_query_range("SELECT menu_name FROM {menu_custom} WHERE menu_name = '%s' OR menu_name = '%s'", $menu_name, $menu_name_custom, 0, 1));

    // If a menu does not exists then we create it.
    // Appending 'menu-' to the beginning is standard for the menu module
    // so we do the same thing.
    if (!$menu) {
      $menu = $menu_name_custom;
      
      // Set up data for menu_link table.
      $path = 'admin/build/menu-customize/';
      $link = array(
        'link_title'  => $title,
        'link_path'   => $path . $menu,
        'router_path' => $path . '%',
        'module'      => 'menu',
        'plid'        => db_result(db_query("SELECT mlid FROM {menu_links} WHERE link_path = '%s' AND module = '%s'", 'admin/build/menu', 'system')),
      );
      // Save link to menu_link table.
      menu_link_save($link);
      
      // Save menu to menu_custom table.
      db_query("INSERT INTO {menu_custom} (menu_name, title, description) VALUES ('%s', '%s', '%s')", $menu_name_custom, $title, $description);
    }
    
    $return = $menu;
  } else {
    drupal_set_message('Custom menu was not created. Please be sure that the menu module is a dependancy.');
  }

  return $return;
}

/**
 * Return a menu item based on the mlid.
 *
 * Good for testing the existence of a value or just getting the row data.
 *
 * @param $mlid
 *   The mlid requested.
 * @return
 *   A menu item if found, boolean FALSE otherwise.
 */
function install_menu_get_item($mlid) {
  return db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE mlid = %d", $mlid));
}

/**
 * Get menu item information based on path or other information.
 * 
 * @param $path
 *   Search on path information string.
 * @param $menu_link_title
 *   Search on $meny_link_title string.
 * @param $plid
 *   Search on parent menu mlid.
 * @param $menu_name
 *   Search on the base menu name.
 *
 * @return
 *   An array of menu items matching the specified criteria.
 */
function install_menu_get_items($path = NULL, $menu_link_title = NULL, $plid = NULL, $menu_name = NULL) {
  $return = array();
  $query  = "SELECT mlid FROM {menu_links} WHERE ";

  $and = FALSE;
  $args = array();

  if (!is_null($path)) { 
    $query .= "link_path = '%s'";
    $args[] = $path;
    $and = TRUE;
  }
  if (!is_null($menu_link_title)) {
    $query .= ($and) ? " AND " : "";
    $query .= "link_title = '%s'";
    $args[] = $menu_link_title;
    $and = TRUE;
  }
  if (!is_null($plid)) {
    $query .= ($and) ? " AND " : "";
    $query .= "plid = %d";
    $args[] = $plid;
    $and = TRUE;
  }
  if (!is_null($menu_name)) {
    $query .= ($and) ? " AND " : "";
    $query .= "menu_name = '%s'";
    $args[] = $menu_name;
  }

  $result = db_query($query, $args);
  while ($row = db_fetch_array($result)) {
    $return[] = $row;
  }

  return $return;
}

