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
    /* contrib */ 'install_profile_api',
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
    'import' => st('Import sites')
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
    hostmaster_bootstrap();
  }
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
  $task = hostmaster_get_task($task, variable_get('hostmaster_wizard_offset', 0));
  variable_del('hostmaster_wizard_offset');
  define("HOSTMASTER_CURRENT_TASK", $task);

  install_include(hostmaster_profile_modules());

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

  if ($form['#node']) {
    $button['#submit'][] = 'node_form_submit';
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



function hostmaster_form_validate($form, &$form_state) {
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
 * Perform any final installation tasks for this profile.
 *
 * @return
 *   An optional HTML string to display to the user on the final installation
 *   screen.
 */
function hostmaster_profile_final() {
  // add default blocks
  hostmaster_install_add_block('hosting', 'hosting_summary', 'garland', 1, 10, 'left');
  hostmaster_install_add_block('hosting', 'hosting_queues', 'garland', 1, 0, 'right');
  hostmaster_install_add_block('hosting', 'hosting_queues_summary', 'garland', 1, 2, 'right');

  // enable the eldir theme, if present
  hostmaster_setup_theme('eldir');

  // Enable optional, yet recommended modules.
  hostmaster_setup_optional_modules();

  // @todo create proper roles, and set up views to be role based
  hostmaster_install_set_permissions(hostmaster_install_get_rid('anonymous user'), array('access content', 'access all views'));
  hostmaster_install_set_permissions(hostmaster_install_get_rid('authenticated user'), array('access content', 'access all views'));
  hostmaster_install_create_role('aegir client');
  // @todo we may need to have a hook here to consider plugins
  hostmaster_install_set_permissions(hostmaster_install_get_rid('aegir client'), array('access content', 'access all views', 'edit own client', 'view client', 'create site', 'delete site', 'view site', 'create backup task', 'create delete task', 'create disable task', 'create enable task', 'create restore task', 'view own tasks', 'view task'));
  hostmaster_install_create_role('aegir account manager');
  hostmaster_install_set_permissions(hostmaster_install_get_rid('aegir account manager'), array('create client', 'edit client users', 'view client'));

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
 * Enable a theme, if present
 *
 * @param mixed The theme name or an array of preferred themes, that
 * will be tried in order.
 */
function hostmaster_setup_theme($name) {
  if (!is_array($name)) {
    $name = array($name);
  }
  $themes = system_theme_data();
  // In preference descending order
  foreach ($name as $theme) {
    if (array_key_exists($theme, $themes)) {
      system_initialize_theme_blocks($theme);
      db_query("UPDATE {system} SET status = 1 WHERE type = 'theme' and name = '%theme'", array('%theme' => $theme));
      variable_set('theme_default', $theme);
    }
  }
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
      switch ($name) {
        case 'admin_menu' :
          variable_set('admin_menu_margin_top', 1);
          variable_set('admin_menu_position_fixed', 1);
          variable_set('admin_menu_tweak_menu', 0);
          variable_set('admin_menu_tweak_modules', 0);
          variable_set('admin_menu_tweak_tabs', 0);

          $menu_mid = hostmaster_create_menu(t('Administration'));
          $admin_mid = hostmaster_menu_get_mid('admin');
          hostmaster_update_menu_item($admin_mid, array('pid' => $menu_mid));
          hostmaster_disable_menu_item($admin_mid);
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

