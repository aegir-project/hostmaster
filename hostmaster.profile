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
    /* aegir contrib */ 'hosting', 'hosting_task', 'hosting_client', 'hosting_db_server', 'hosting_package', 'hosting_platform', 'hosting_site', 'hosting_web_server',
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
  install_include(hostmaster_profile_modules());
  include_once(dirname(__FILE__) . '/hostmaster.forms.inc');
  define("HOSTMASTER_FORM_REDIRECT", $url);
  if ($task == 'profile') {
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

/**
 * modify the settings.php
 *
 * add the install_profile variable to the settings.php so that Drupal
 * picks up the theme in our install profile
 *
 * This is a bug in the Drupal core: http://drupal.org/node/330297
 */
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
  $node->no_verify = TRUE;
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

function hostmaster_form_alter($form, $values, $form_id) {
  if ($form_id == 'install_configure') {
    // Put the install profile into settings.php so that it will pick up
    // the eldir theme in profiles/hostmaster/themes
    if (!variable_get('hostmaster_settings_php_alter', FALSE)) {
      hostmaster_settings_php();
      variable_set('hostmaster_settings_php_alter', TRUE);
    }
  }
}
