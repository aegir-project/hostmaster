<?php
// $Id$

function hostmaster_task_intro($current) {
  drupal_set_title(t("Welcome to the Aegir hosting system."));

 $form['introduction']['#value'] = 
    t("<p>I am a configuration wizard designed to help you through configuring the Aegir provisioning system.</p>
    <p>Once properly configured, this system will help you manage your Drupal sites. 
    It will even be able to manage them across multiple versions of Drupal and across multiple servers.</p>
    <p>Before we get started with the configuration, please make sure that your system meets the following requirements:</p>");
 $form['drupal'] = hostmaster_requirement_help('basic_drupal');
 $form['unix'] = hostmaster_requirement_help('basic_unix');
 $form['server'] = hostmaster_requirement_help('basic_server');

 return hostmaster_form($form, $current);
}

//====================================================================
//====================================================================
//====================================================================

function hostmaster_task_server() {
  drupal_set_title(t("Basic server configuration"));
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $form = array();
  $form['#validate'][] = 'hostmaster_task_server_validate';
  $form['#submit'][] = 'hostmaster_task_server_submit';
  $form['#nid'] = $node->nid;
  $node->services['server']->form($form);

  $form['ip_address']['#type'] = 'value';
  
  $form['script_user']['#weight'] = 1;
  $form['user_help'] = hostmaster_requirement_help('user', array(
    '#weight' => 0,
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_COLLAPSED
  ));

  // drush path field

  $form['drush_path']['#weight'] = 5;
  $form['drush_path_help'] = hostmaster_requirement_help('drush_path', array(
    '#weight' => 4, 
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_DISABLED
  ));

  // config path field
  $form['config_path']['#weight'] = 10;
  $form['config_path_help'] = hostmaster_requirement_help('config_path', array(
    '#weight' => 9, 
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_COLLAPSED
  ));

  // backup path field
  $form['backup_path']['#weight'] = 15;  
  $form['backup_path_help'] = hostmaster_requirement_help('backup_path', array(
    '#weight' => 14, 
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_COLLAPSED
  ));


  return hostmaster_form($form);
}


function hostmaster_task_server_validate($form, $form_state) {
  $node = node_load($form['#nid']); 
  $node->services['server']->setValues($form_state['values']);
  $node->services['server']->validate($node, $form);
}

function hostmaster_task_server_submit($form, $form_state) {
  $node = node_load($form['#nid']);
  $node->services['server']->setValues($form_state['values']);
  $node->services['server']->save($node);
  $node->services['server']->update($node);
}

//====================================================================
//====================================================================
//====================================================================


function hostmaster_task_dbserver() {
  drupal_set_title(t("Database server configuration"));
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $form = array();
  $form['#validate'][] = 'hostmaster_task_dbserver_validate';
  $form['#submit'][] = 'hostmaster_task_dbserver_submit';
  $form['#nid'] = $node->nid;
  $node->services['db']->form($form);


  $own_link = _hosting_node_link(HOSTING_OWN_DB_SERVER);

  $form['introduction']['#weight'] = -9;
  $form['introduction']['#value'] = 
    t('<p>You need at least one fully functional database server to be able to manage sites.</p> 
        <p>Aegir also requires a database user capable of creating more databases. It is advisable to not use the
        same database user that you configured for the main aegir database at the start of this installation, but instead
        create an "<strong>aegir_root</strong>" superuser with stronger privileges.</p>
        <p>Consult the "How do I configure this" section below to make use of the example GRANT statement provided.
        Then fill in the form with the credentials of that superuser.</p>',
        array('!own_server' => $own_link, '!edit_page' => l(t("edit page"), 'node/' . HOSTING_OWN_DB_SERVER . '/edit' )));

 
  $form['db_details'] = array(
    '#type' => 'fieldset',
    '#collapsed' => false,
    '#title' => t('Database account details'),
  );
  $form['db_details']['help'] = hostmaster_requirement_help('mysql_user', array('#configuration' => HOSTING_HELP_COLLAPSED, '#summary' => HOSTING_HELP_COLLAPSED));

  $form['db_details']['db_user'] = $form['db_user'];
  unset($form['db_user']);
  $form['db_details']['db_passwd'] = $form['db_passwd'];
  unset($form['db_passwd']);


  return hostmaster_form($form);
}



function hostmaster_task_dbserver_validate($form, $form_state) {
  $node = node_load($form['#nid']); 
  $node->services['db']->setValues($form_state['values']);
  $node->services['db']->validate($node, $form);
}

function hostmaster_task_dbserver_submit($form, $form_state) {
  $node = node_load($form['#nid']);
  $node->services['db']->setValues($form_state['values']);
  $node->services['db']->save($node);
  $node->services['db']->update($node);
}

/**
 * Configures web_server_node_form minus almost all the forms.
 */
function hostmaster_task_webserver() {
  drupal_set_title(t("Web server configuration"));
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $form = array();
  $form['#validate'][] = 'hostmaster_task_webserver_validate';
  $form['#submit'][] = 'hostmaster_task_webserver_submit';
  $form['#nid'] = $node->nid;
  $node->services['http']->form($form);

  $form['group_help'] = hostmaster_requirement_help('group', array(
    '#weight' => 0,
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_COLLAPSED
  ));
  $form['web_group']['#weight'] = 1;

  $form['restart_cmd_help'] = hostmaster_requirement_help('visudo', array(
    '#weight' => 4,
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_COLLAPSED
  ));
  $form['restart_cmd']['#weight'] = 5;

  $form['ports_help'] = hostmaster_requirement_help('ports', array(
    '#weight' => 9,
    '#summary' => HOSTING_HELP_ENABLED,
  ));
  $form['ports']['#weight'] = 10;


  // httpd.conf description
  $form['httpd_conf'] = hostmaster_requirement_help('httpd_conf', array(
    '#weight' => 14,
    '#summary' => HOSTING_HELP_ENABLED, 
    '#configuration' => HOSTING_HELP_ENABLED,
  ));


  return hostmaster_form($form);
}


function hostmaster_task_webserver_validate($form, $form_state) {
  $node = node_load($form['#nid']); 
  $node->services['http']->setValues($form_state['values']);
  $node->services['http']->validate($node, $form);
}

function hostmaster_task_webserver_submit($form, $form_state) {
  $node = node_load($form['#nid']);
  $node->services['http']->setValues($form_state['values']);
  $node->services['http']->save($node);
  $node->services['http']->update($node);
}



//====================================================================
//====================================================================
//====================================================================

function hostmaster_task_features() {
  drupal_set_title(st('Enable hosting features'));
  $form = drupal_retrieve_form('hosting_features_form', $form_state);
  unset($form['buttons']);
  return hostmaster_form($form);
}

function hostmaster_task_features_submit($form, &$form_state) {
  hosting_features_form_submit($form_state['values']['form_id'], $values);
}

//====================================================================
//====================================================================
//====================================================================


/**
 * Form that confirms that cron is running, and that the platform has been successfuly verified
 *
 * This page is essentially one huge page of contextual help, to ensure that everything is up and running.
 */
function hostmaster_task_init() {
  drupal_set_title(st('Initialize the hosting system'));
  // @ TODO: make this coherent, and add error reporting about what went wrong, and how it needs to be fixed.
  $form = array();
  $form['intro']['#value'] = t('<p>The hosting framework provides the user interface used to administer your sites.
    All components of the managed sites from the database and web servers, to the packages installed
    on them, to the sites themselves are represented by the hosting framework as nodes.
    This step is going to ensure that it is correctly initialized.</p>');

  $options = array('#configuration' => HOSTING_HELP_COLLAPSIBLE);

  if (!variable_get('hosting_dispatch_enabled', FALSE)) {
    $options['#status'] = HOSTING_STATUS_WARNING;
    $options['#message'] = t('Please follow the configuration help below to initialize the system.');
  }
  elseif (!variable_get('hosting_dispatch_last_run', false)) {
    $options['#status'] = HOSTING_STATUS_ERROR;
    $options['#message'] = t('The dispatch process has not been run. Please follow the instructions below.');
  }
  else {
    $options['#status'] = HOSTING_STATUS_SUCCESS;
    $options['#message'] = t('The dispatch process has been successfully executed');
  }

  $form['cron'] = hostmaster_requirement_help('hosting_setup', $options);
  return hostmaster_form($form);
}

function hostmaster_task_init_validate($form, &$form_state) {
  if (!variable_get('hosting_dispatch_last_run', false)) {
    form_set_error("submit");
  }
}

//====================================================================
//====================================================================
//====================================================================



function hostmaster_task_verify() {
  drupal_set_title(st('Verify your configuration'));
  $form = array();
  $node = hosting_get_most_recent_task(HOSTING_OWN_PLATFORM, 'verify');
  $form['provision'] = array('#type' => 'requirement_help', '#requirement' => 'platform');
  if ($node->task_status == HOSTING_TASK_SUCCESS) {
    $form['provision']['#status'] = HOSTING_STATUS_SUCCESS;
    $form['provision']['#message'] = t('Your platform has been successfully configured. You can now create sites.');

    $form['import']['#value'] = t('On the next page we will import any existing sites on this platform.');
  }
  else {
    $form['provision']['#status'] = HOSTING_STATUS_ERROR;
    $form['provision']['#message'] = t('One or more errors have occurred during platform verification.');
    $form['retry_id'] = array('#type' => 'hidden', '#value' => $node->nid);
    $form['log_tip']['#value'] = t('Underneath is the log output and status of your verification task.
      Check your settings on the previous pages of the wizard, and follow the applicable examples from the configuration
      tips.');
    $view = hosting_task_view($node);
    $form['task_status'] = $view->content['status'];
    $form['task_status']['#title'] = t('Errors');
    $form['last_attempt'] = array('#type' => 'item', '#title' => t('Last attempt'), '#value' => hosting_format_interval($node->changed));
    $form['log'] = $view->content['hosting_log'];
    $next = t('Check again');
  }
  return hostmaster_form($form, $next);
}

function hostmaster_task_verify_validate($form, &$form_state) {
  $node = hosting_get_most_recent_task(HOSTING_OWN_PLATFORM, 'verify');
  if (!($node->task_status == HOSTING_TASK_SUCCESS)) {
    form_set_error("submit");
  }
}

//====================================================================
//====================================================================
//====================================================================


function hostmaster_task_import() {
  drupal_set_title(st('Import your existing sites'));
  $form = array();

  $sites = hosting_get_sites_by_status(HOSTING_OWN_PLATFORM, HOSTING_SITE_IMPORTED);
  if ($list = hosting_site_list('platform', HOSTING_OWN_PLATFORM)) {
    $form['intro']['#value'] = t('<p>Below is a list of sites which were imported into the Hosting framework. Each of these pre-existing sites have had an import task created for them, and will be imported by the task queue.</p>');
    $form['list']['#value'] = $list;
  }
  else {
    $form['intro']['#value'] = t('<p>Hosting has not found any sites to import.</p>')
      . t('<p>In the future, any additional platforms you create will also automatically have existing sites imported, and at any time you can request an verify task on any existing platform to find and import any sites that may have been added manually.</p>');
  }
  return hostmaster_form($form);
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

  variable_set('install_profile_task', 'profile-finished');
  return hostmaster_form(array());
}
