<?php

/**
  * Control structure for stepping through the installation wizard.
  *
  * This couldn't be simpler. It generates a set of menu items for each
  * of the steps, based on the key of the array.
  *
  * Each step has it's own form, that is a configuration form.
  *
  * Multiple levels are supported, as field/sub gets turned into a
  * hosting_wizard_field_sub() form callback.
  *
  * Simpler, and easier to maintain than writing actual _menu items,
  * as this lowers our upgrade dependencies for D6, by only having 
  * 2 items that need to be reimplemented.
 */
function hosting_wizard_steps($step = null) {
  static $steps = array();
  global $user;

  if (!sizeof($steps)) {
    // todo: perhaps add a hook to allow modules to add features into the system
    $steps['intro'] = array( 'title' => t("Introduction"), 'heading' => t("Welcome to your new hosting system"), 'type' => MENU_DEFAULT_LOCAL_TASK);

    if ($user->uid == 1) {
      $steps['account'] = array('title' => t("User account"), 'heading' => t("Configure your user account"),
        'message' => t('An administrator account has been registered for you. Please change your password to continue with the configuration process.'));
    }
    $steps['provision'] = array('title' => t("Provisioning"), 'heading' => t("Configure your provisioning framework"),
      'message' => t('To be able to create sites using Hostmaster, we need some information about your server.'), );

    $steps['provision/paths'] = array('title' => t("Paths"), 'heading' => t("Configure where to store provisioning data"),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'message' => t('To be able to create sites using Hostmaster, provisioning needs access to the file system.'), );

    $steps['provision/web'] = array('title' => t("Web server"), 'heading' => t("Configure your web server"),
      'message' => t("You will need somewhere to server your files from."));

    $steps['provision/db'] = array('title' => t('Database server'), 'heading' => t('Configure your database server'),
      'message' => t("You need to be able to create databases and database users."));

    $steps['hosting'] = array( 'title' => t('Hosting'), 'heading' => t('Set up your hosting framework'), 
      'message' => t("Finalize your setup by ensuring communication between the front end and back end."));

    $steps['hosting/init'] = array('title' => t('Initialize'), 'heading' => t('Initialize the hosting system'),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'message' => t("Finalize your setup by ensuring communication between the front end and back end."));

    $steps['hosting/verify'] = array('title' => t('Verify platform'), 'heading' => t('Verify platform settings'),
      'message' => t("Verify that the provisioning system is correctly set up, and new sites can be hosted on it."));

    $steps['hosting/import'] = array('title' => t('Import sites'), 'heading' => t('Import your existing sites'),
      'message' => t("Details of any existing sites found and imported into Hostmaster."));

    $steps['hosting/features'] = array('title' => t('Features'), 'heading' => t('Enable hosting features'),
      'message' => t("Enable or disable any of the features available."));


    $steps['complete'] = array('title' => t("Complete"), 'heading' => t('Complete your installation'),
    'message' => t("Congratulations, you have finished installation of hostmaster. If you have any questions, go to groups.drupal.org/hm2."));

    // @todo: figure out what would go on a features page.
    // $steps['features'] = array('title' => t("Features"), 'message' => t("."));
   }

  if ($step && $steps[$step]) {
    return $steps[$step];
  }
  return $steps;
}

/**
 * Wizard menu items.
 * 
 * Branches off from hosting using the referenced &$items parameter.
 * I'm not entirely sure the wizard is big enough to need it's own
 * module yet.
 */
function hosting_wizard_menu($may_cache, &$items) {
  $items[] = array(
    'path' => 'hosting/wizard',
    'title' => t('Hosting wizard'),
    'description' => t('Configuration wizard'),
    'callback' => 'drupal_get_form',
    'callback arguments' => array('hosting_wizard_intro'),
    'type' => MENU_NORMAL_ITEM,
    'access' => user_access('access hosting wizard'), 
  );

  if (!$may_cache && (sprintf("%s/%s", arg(0), arg(1)) == 'hosting/wizard')) {
    hosting_wizard_dummy_theme_block();
    $steps = hosting_wizard_steps();
    $x = 1;
    foreach ($steps as $key => $step) {
      $form_id = array('hosting_wizard_' . str_replace("/", "_", $key));
      $wizard_items[$key] = array(
        'path' => 'hosting/wizard/' . $key,
        'title' => $step['title'],
        'description' => $step['message'],
        'callback' => 'drupal_get_form',
        'callback arguments' => $form_id,
        'type' => ($step['type']) ? $step['type'] : MENU_LOCAL_TASK,
        'access' => user_access('access hosting wizard'),
        'weight' => $x++
      );
      if ($step['type'] == MENU_DEFAULT_LOCAL_TASK) {
        $parts = explode("/", $key);
        if (sizeof($parts) >= 2) {
          $parent = array_shift($parts);
          $wizard_items[$parent]['callback arguments'] = $form_id;
        }
      }
    }
    $items =  array_merge($items, $wizard_items);
  }
}

/**
 * Introduction form.
 *
 * @todo: add an option for minimal configuration, which just hits the required pages?
 */
function hosting_wizard_intro() {
  $form['introduction']['#value'] = 
    t("<p>I am a configuration wizard designed to help you through configuring the Aegir provisioning system.</p>
    <p>Once properly configured, this system will help you manage your Drupal sites. 
    It will even be able to manage them across multiple versions of Drupal and across multiple servers.</p>
    <p>Before we get started with the configuration, please make sure that your system meets the following requirements:</p>");
  $form['drupal'] = array('#type' => 'requirement_help', '#requirement' => 'basic_drupal');
  $form['unix'] = array('#type' => 'requirement_help', '#requirement' => 'basic_unix');
  $form['server'] = array('#type' => 'requirement_help', '#requirement' => 'basic_server');

  return hosting_wizard_form('intro', $form);
}


/**
 * Account form.
 *
 * The fields were more easily copied from user_edit than trying to import it and get rid of the cruft
 */
function hosting_wizard_account() {
  global $user;
  $form = array();
  $form['introduction']['#value'] =
    t("<p>As part of the installation, we have already created an administrator account on this system for you. One of hostmaster's project goals is to configure itself to the best of it's abilities, so that you don't have to. Unfortunately there will be some information that we can't guess.</p>
    <p>So to ensure that you are actually able to log back in and use the system, we would appreciate it if you could please change your password to something we didn't generate randomly.</p>");

   $form['account']['name'] = array('#type' => 'textfield',
     '#title' => t('Username'),
     '#default_value' => $user->name,
     '#maxlength' => USERNAME_MAX_LENGTH,
     '#size' => 30,
     '#description' => t('Your preferred username; punctuation is not allowed except for periods, hyphens, and underscores.'),
     '#required' => TRUE,
   );

  $form['account']['#prefix'] = "<div id='hosting-wizard-account-details'>";
  $form['account']['#suffix'] = "</div>";
  $form['account']['mail'] = array('#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#default_value' => $user->mail,
    '#maxlength' => EMAIL_MAX_LENGTH,
     '#size' => 30,
    '#description' => t('A valid e-mail address. All e-mails from the system will be sent to this address. The e-mail address is not made public and will only be used if you wish to receive a new password or wish to receive certain news or notifications by e-mail.'),
    '#required' => TRUE,
  );

  $form['pass']['#prefix'] = "<div id='hosting-wizard-account-passwd'>";
  $form['pass']['#suffix'] = "</div><br style='clear:both' />"; //i had to use inline styles. ick
  $form['pass']['pass'] = array(
    '#type' => 'password_confirm',
    '#description' => t('Provide a password for the new account in both fields.'),
    '#required' => hosting_wizard_completed('account') ? FALSE : TRUE,
    '#size' => 30,
  );
  $client_help = hosting_node_help('client');
  $form['client_info']['#value'] = t("<p>This information will also be used by Hostmaster to create the <a id='hosting-wizard-client-help-show'>primary client.</a></p>
    <div id='hosting-wizard-client-help' class='hosting-wizard-glossary'>!client_help</div>", array('!client_help' => $client_help));
  
  return hosting_wizard_form('account', $form);
}

/**
 * Ensure a valid user
 */
function hosting_wizard_account_validate($form_id, $values, $form) {
  global $user;
  _user_edit_validate($user->uid, $values);
}

/**
 * Save the user account, and update the default client node
 */
function hosting_wizard_account_submit($form_id, $values) {
  global $user;
  user_save($user, $values);
  $node = node_load(HOSTING_DEFAULT_CLIENT);
  $node->client_name = $values['name'];
  $node->email = $values['mail'];
  node_save($node);
}

/**
 * Provision information page. Explains what you are about to do, uneccesary?
 */
function hosting_wizard_provision() {
  $form = array();
  $form['introduction']['#value'] = 
    t("<p>The Provisioning framework provides the back end of the system. It does all the nitty gritty server level things, so that you don't have to.</p>
    <p>Provision is very careful about security, and as such has been designed to run under the absolute minimum permissions it needs to accomplish it's job. For the security of your data, Hostmaster will not run unless it is running in this optimally secure configuration.</p>    <p>Because of these stringently enforced security requirements, it takes a bit longer to set up than would be ideal, however we feel it is a fair tradeoff.</p>");
  t("<p>We have tried to collect as much data about our current environment as we can, and provide these as defaults for server configuration. For a lot of cases this works adequately, however we will perform checks on every item, to ensure that it is in working order.</p>
    <p>In some cases, such as file permissions, we are incapable of testing items until the hosting front end has been succesfully configured. If we do find a problem, you will be directed back to this section to modify your configuration.</p>");

  return hosting_wizard_form('provision', $form);
}



/**
 * Configures web_server_node_form paths.
 *
 * @todo pull changes in the form upstream
 */
function hosting_wizard_provision_paths() {
  $form = drupal_retrieve_form('web_server_node_form', node_load(HOSTING_OWN_WEB_SERVER));
  _hosting_wizard_cleanse_node_form($form);

  $own_link = _hosting_node_link(HOSTING_OWN_WEB_SERVER);

  $form['script_user']['#type'] = 'value';
  $form['web_group']['#type'] = 'value';
  /*
  $form['introduction']['#weight'] = -45;
  $form['introduction']['#value'] = 
    t('<p>This form is actually the !edit_page for the !own_server web server node, which has been heavily annotated and a number of fields removed for simplicity.</p>
    <p>It is recommended to use the !own_server page to manage this information in the future.</p>
        ', 
        array('!own_server' => $own_link, '!edit_page' => l(t("edit page"), 'node/' . HOSTING_OWN_WEB_SERVER . '/edit' )));*/

  $form['title']['#type'] = 'value';
  $form['ip_address']['#type'] = 'value';
  $form['restart_cmd']['#type'] = 'value';

  $form['config_path_help'] = array('#type' => 'requirement_help', '#requirement' => 'config_path', '#weight' => 9, '#summary' => PROVISION_HELP_ENABLED, '#configuration' => PROVISION_HELP_COLLAPSED);
  $form['config_path']['#weight'] = 10;

  $form['backup_path_help'] = array('#type' => 'requirement_help', '#requirement' => 'backup_path', '#weight' => 14, '#summary' => PROVISION_HELP_ENABLED, '#configuration' => PROVISION_HELP_COLLAPSED);
  $form['backup_path']['#weight'] = 15;  

  // remove all the things i don't want from the node form.
  return hosting_wizard_form('provision/paths', $form);
}

/**
 * Save the changes to the default web server node
 */
function hosting_wizard_provision_paths_submit($form_id, $values) {
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $node->config_path = $values['config_path'];
  $node->restart_cmd = $values['restart_cmd'];
  $node->backup_path = $values['backup_path'];
  node_save($node);
}


/**
 * Configures web_server_node_form minus almost all the forms.
 *
 * @todo pull changes in the form upstream
 */
function hosting_wizard_provision_web() {
  $form = drupal_retrieve_form('web_server_node_form', node_load(HOSTING_OWN_WEB_SERVER));
  _hosting_wizard_cleanse_node_form($form);

  $own_link = _hosting_node_link(HOSTING_OWN_WEB_SERVER);

  /*
  $form['introduction']['#weight'] = -45;
  $form['introduction']['#value'] = 
    t('<p>You need to have one fully working web server configured to be able to create sites. The web server can run into problems with file permissions quite easily, and these are unfortunately undetectable to us until the hosting back end is configured.</p>
    <p>Like the database configuration, this form is actually the !edit_page for the !own_server database server node, which has been heavily annotated and a number of fields removed for simplicity.</p>
    <p>It is recommended to use the !own_server page to manage this server during daily use.</p>
        ', 
        array('!own_server' => $own_link, '!edit_page' => l(t("edit page"), 'node/' . HOSTING_OWN_WEB_SERVER . '/edit' ))); */

  $form['httpd_conf'] = array(
    '#type' => 'requirement_help', '#requirement' => 'httpd_conf',
    '#summary' => PROVISION_HELP_ENABLED, '#configuration' => PROVISION_HELP_ENABLED,
    /*    '#status' => PROVISION_STATUS_WARNING, '#message' => t('This needs to be configured for Apache to pick up new sites. We can not confirm that this is working correctly yet, so if your sites are installed but do not work from a web browser, please confirm that you have completed this step'),*/
    '#weight' => -1,);

  $form['title']['#type'] = 'value';
  $form['ip_address']['#type'] = 'value';
  $form['backup_path']['#type'] = 'value';  
  $form['config_path']['#type'] = 'value';  
  $form['acc_details'] = array(
    '#type' => 'fieldset',
    '#collapsed' => false,
    '#title' => t('System account details'),
    '#weight' => 0 
  );
  $form['acc_details']['acc_help'] = array('#type' => 'requirement_help', '#requirement' => 'group', '#weight' => -20, '#summary' => PROVISION_HELP_ENABLED, '#configuration' => PROVISION_HELP_COLLAPSED);
  $form['acc_details']['script_user'] = $form['script_user'];
  unset($form['script_user']);
  $form['acc_details']['web_group'] = $form['web_group'];
  unset($form['web_group']);


  $form['restart_cmd_help'] = array('#type' => 'requirement_help', '#requirement' => 'visudo', '#weight' => 19, '#summary' => PROVISION_HELP_ENABLED, '#configuration' => PROVISION_HELP_COLLAPSED);
  $form['restart_cmd']['#weight'] = 20;

  // remove all the things i don't want from the node form.
  return hosting_wizard_form('provision/web', $form);
}

/**
 * Save the changes to the default web server node
 */
function hosting_wizard_provision_web_submit($form_id, $values) {
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $node->title = $values['title'];
  $node->ip_address = $values['ip_address'];
  $node->script_user = $values['script_user'];
  $node->web_group = $values['web_group'];
  $node->restart_cmd = $values['restart_cmd'];
  node_save($node);
}

/**
 * Database configuration form
 *
 * Inherits the db_server_node_form, and shifts some things around.
 * Perhaps those changes need to go in upstream?
 */
function hosting_wizard_provision_db() {
  $form = drupal_retrieve_form('db_server_node_form', node_load(HOSTING_OWN_DB_SERVER));
  _hosting_wizard_cleanse_node_form($form);

  $own_link = _hosting_node_link(HOSTING_OWN_DB_SERVER);

  $form['introduction']['#weight'] = -9;
  $form['introduction']['#value'] = 
    t('<p>You need at least one fully functional database server to be able to manage sites. 
       Luckily, if you can read this, you are already most of the way there. <strong>If the user you specified when first configuring the Drupal can create databases, you probably do not need to change anything here.</strong></p>
        <p>This configuration form is actually the !edit_page for the !own_server database server node. 
        It has however been heavily annotated and slightly simplified for this installation wizard. And while
        the wizard can be run multiple times, it won\'t be able to edit any database servers other than the database
        this site is running on.</p>', 
        array('!own_server' => $own_link, '!edit_page' => l(t("edit page"), 'node/' . HOSTING_OWN_DB_SERVER . '/edit' )));

#  $form['host_help']['#weight'] = -8;
/**
 * not going to use this field on this form, but i might need this help text another time
  $form['title']['#description'] = t('<strong>A database server to connect to</strong>
    <p>You will need the hostname or ip address of the database server you are connecting to. Unless you are using an external database server, or managing multiple database servers, this will just be "localhost". This information is very reliably retrieved from the configuration of the main hosting site. So you should only need to modify this in special cases</p>');
 */

  $form['title']['#type'] = 'value';
  
  $form['db_details'] = array(
    '#type' => 'fieldset',
    '#collapsed' => false,
    '#title' => t('Database account details'),
  );
  $form['db_details']['help'] = array('#type' => 'requirement_help', '#requirement' => 'mysql_user', '#configuration' => PROVISION_HELP_COLLAPSED, '#summary' => PROVISION_HELP_COLLAPSED);
  $form['db_details']['db_user'] = $form['db_user'];
  unset($form['db_user']);
  $form['db_details']['db_passwd'] = $form['db_passwd'];
  unset($form['db_passwd']);

  return hosting_wizard_form('provision/db', $form);
}

/**
 * Save the modified db_server
 */
function hosting_wizard_provision_db_submit($form_id, $values) {
  $node = node_load(HOSTING_OWN_DB_SERVER);
  $node->title = $values['title'];
  $node->db_user = $values['db_user'];
  if ($values['db_passwd']) {
    $node->db_passwd = $values['db_passwd'];
  }
  node_save($node); 
}



/**
 * Form that confirms that cron is running, and that the platform has been successfuly verified
 *
 * This page is essentially one huge page of contextual help, to ensure that everything is up and running.
 */
function hosting_wizard_hosting_init() {
  // @ TODO: make this coherent, and add error reporting about what went wrong, and how it needs to be fixed.
  $form = array();
  $form['intro']['#value'] = t('<p>The hosting framework provides the user interface used to administer your sites.
    All components of the managed sites from the database and web servers, to the packages installed
    on them, to the sites themselves are represented by the hosting framework as nodes.
    This step is going to ensure that it is correctly initialized.</p>');
  $form['cron'] = array('#type' => 'requirement_help', '#requirement' => 'hosting_setup', '#configuration' => PROVISION_HELP_COLLAPSIBLE);

  if (!variable_get('hosting_dispatch_enabled', FALSE)) {
    $form['cron']['#status'] = PROVISION_STATUS_WARNING;
    $form['cron']['#message'] = t('Please follow the configuration help below to initialize the system.');
  }
  elseif (!variable_get('hosting_dispatch_last_run', false)) {
    $form['cron']['#status'] = PROVISION_STATUS_ERROR;
    $form['cron']['#message'] = t('The dispatch process has not been run. Please follow the instructions below.');
  }
  else {
    $form['cron']['#status'] = PROVISION_STATUS_SUCCESS;
    $form['cron']['#message'] = t('The dispatch process has been succesfully executed');
  }
  return hosting_wizard_form('hosting', $form);
}

function hosting_wizard_hosting_init_validate() {
  if (!variable_get('hosting_dispatch_last_run', false)) {
  drupal_goto('hosting/wizard/hosting');
  #  form_set_error('cron', t("The hosting setup command has not been run yet"));
  }
}

function hosting_wizard_hosting_verify() {
  $form = array();
  $node = hosting_get_most_recent_task(HOSTING_OWN_PLATFORM, 'verify');
  $form['provision'] = array('#type' => 'requirement_help', '#requirement' => 'platform');
  if ($node->task_status & PROVISION_SUCCESS) {
    $form['provision']['#status'] = PROVISION_STATUS_SUCCESS;
    $form['provision']['#message'] = t('Your platform has been succesfully configured. You can now create sites.');

    $form['import']['#value'] = t('On the next page we will import any existing sites on this platform.');
  }
  else {
    $form['provision']['#status'] = PROVISION_STATUS_ERROR;
    $form['provision']['#message'] = t('One of more errors has occurred during platform verification.');
    $form['log_tip']['#value'] = t('Underneath is the log output and status of your verification task.
      Check your settings on the previous pages of the wizard, and follow the applicable examples from the configuration
      tips. Dispatch will continue attempting to run the verification task until it succeeds, so clicking on the Check again button
      or just reloading the page will update the information below');
    $view = hosting_task_view($node);
    $form['task_status'] = $view->content['status'];
    $form['task_status']['#title'] = t('Errors');
    $form['last_attempt'] = array('#type' => 'item', '#title' => t('Last attempt'), '#value' => hosting_format_interval($node->changed));
    $form['log'] = $view->content['hosting_log'];
    $next = t('Check again');
  }
  return hosting_wizard_form('hosting/verify', $form, $next);
}

function hosting_wizard_hosting_verify_validate() {
  $node = hosting_get_most_recent_task(HOSTING_OWN_PLATFORM, 'verify');
  if (!($node->task_status & PROVISION_SUCCESS)) {
    drupal_goto('/hosting/wizard/hosting/verify');
  }
}

function hosting_wizard_hosting_import() {
  $form = array();
  $task = hosting_get_most_recent_task(HOSTING_OWN_PLATFORM, 'import');

  $sites = hosting_get_sites_by_status(HOSTING_OWN_PLATFORM, HOSTING_SITE_IMPORTED);
  if ($task->task_status & PROVISION_SUCCESS) {
    $form['intro']['#value'] = t('<p>Below is a list of sites which were imported into the Hosting framework. Each of these pre-existing sites is now being managed
      by Hosting, and has the same capabilities of sites that were created using the framework.</p>');
    $rows = array();
    foreach ($sites as $site) {
      $own = '';
      if (($site->title == PROVISION_DOMAIN) && ($site->platform == HOSTING_OWN_PLATFORM)) {
        $own = '*';
      }
      $rows[] = array( _hosting_node_link($site->nid) . $own, _hosting_node_link($site->platform), hosting_format_interval($site->created));
    }
    $form['sites'] = array('#type' => 'item', '#title' => t('Imported sites'), '#value' => theme('table', array(t('Site'), t('Platform'), t('Imported')), $rows));
    $form['own']['#value'] = t('* This is the site node for the Hosting site. This is the site that manages all the other sites, and should be regularly backed up.');
  }
  else {
    $form['no_sites']['#value'] = t('<p>Provision has not imported any sites yet. You may wait a few moments and reload this page, or click on the button to complete your installation.</p>');
  }
  return hosting_wizard_form('hosting/import', $form);
}

function hosting_wizard_hosting_features() {
  $features = module_invoke_all('hosting_features');
  $form['features']['#tree'] = TRUE;
  foreach ($features as $feature => $info) {
    $form['features'][$feature] = array(
      '#type' => 'checkbox',
      '#title' => $info['title'],
      '#description' => $info['description'],
      '#default_value' => hosting_feature($feature),
      '#required' => hosting_feature($feature) == HOSTING_FEATURE_REQUIRED,
    );

  }
  return hosting_wizard_form('hosting/features', $form);
}

function hosting_wizard_hosting_features_submit($form_id, $values) {
  foreach ($values['features'] as $feature => $value) {
    variable_set('hosting_feature_' . $feature, $value);
  }
  menu_rebuild();
}

/**
 * The last page.
 *
 * Essentially gives a very small summary, and links you to a few interesting things
 * to do next (configure your site, add your first site etc)
 *
 * When clicking on the button you will be forwarded to your site.
 */
function hosting_wizard_complete() {
  $form['configure']['#value'] .= 
    t('<p><strong>Configure your website</strong>
               Once logged in, visit the <a href="@admin">administration section</a>,
               where you can <a href="@config">customize and configure</a> all aspects
               of your website.</p>', 
               array('@admin' => url('admin'), '@config' => url('admin/settings'))) ;

  $form['create_site']['#value'] = t('<p><strong>Create your first hosted site.</strong> This system uses special site posts to store information about your sites, so you can simply <a href="@create_site">create a site post</a> to get your first hosted site running.</p>', array('@create_site' => url('node/add/site')));

  return hosting_wizard_form('complete', $form, t('Go to your site'));
}


/**
 * Helper functions for wizarding
 */

/**
 * Take a node form and get rid of all the crud that we don't
 * need on a wizard form, in a non destructive manner.
 */
function _hosting_wizard_cleanse_node_form(&$form) {
  $form['log']['#type'] = 'value';
  $form['author']['name']['#type'] = 'value';
  $form['author']['date']['#type'] = 'value';
  $form['options']['status']['#type'] = 'value';
  $form['options']['promote']['#type'] = 'value';
  $form['options']['sticky']['#type'] = 'value';
  $form['options']['revision']['#type'] = 'value';
  $form['options']['#type'] = 'markup';
  $form['author']['#type'] = 'markup';

  // get rid of buttons (we replace submit anyway)
  unset($form['submit']);
  unset($form['preview']);
  unset($form['delete']);
}

/**
 * Form modifier similar to confirm_form
 *
 * Handles some bookkeeping like adding the js and css, 
 * embedded the right classes, and most importantly : adding the wizard_form_submit
 * to the #submit element. Without this, you would never be forwarded to the next
 * page.
 */
function hosting_wizard_form($step, $form, $next = null) {

  drupal_add_css(drupal_get_path("module", "hosting") . '/hosting.wizard.css');
  drupal_add_css(drupal_get_path("module", "hosting") . '/hosting.wizard.js');
  // @todo: add a message to exit the wizard.
  hosting_wizard_set_title($step);
  $form['#submit']['hosting_wizard_' . str_replace('/', '_', $step) . '_submit'] = array(); 
  $form['#submit']['hosting_wizard_form_submit'] = array();
  $form['#prefix'] = '<div id="hosting-wizard-form">';
  $form['#suffix'] = '</div>';
  $form['step'] = array('#type' => 'value', '#value' => $step);

  $form['wizard_form'] = array(
    '#prefix' => '<div id="hosting-wizard-form-buttons">',
    '#suffix' => '</div>',
    '#weight' => 100
  );
  $info = hosting_wizard_steps(hosting_wizard_next_step());
  $text = ($next) ? $next : (($info['heading']) ? $info['heading'] : t('Continue'));
  $form['wizard_form']['submit'] = array(
    '#type' => 'submit',
    '#value' => $text . ' ->',
   ); 

  return $form;
}

/**
 * Additional submit handler that gets added to the end of 
 * all wizard forms.
 *
 * This redirects the user to the next step in the wizard
 */
function hosting_wizard_form_submit($form_id, $values) {
  $values = (array) $values;
  variable_set('hosting_wizard_completed_' . $values['step'], mktime());

  $next = hosting_wizard_next_step();
  if ($next) {
    return "hosting/wizard/" . $next;
  }
  else {
    return variable_get('site_frontpage', 'node');
  }
}


/**
 * Simple check to see if the user has completed a step yet.
 */
function hosting_wizard_completed($step) {
  return variable_get("hosting_wizard_completed_" . $step, false);
}

function hosting_wizard_set_title($step) {
  $info = hosting_wizard_steps($step);
   if ($info['heading']) {
     drupal_set_title($info['heading']);
   }
}

function hosting_wizard_init() {
}

/**
 * Clever hack to hide display of blocks during wizard.
 *
 * The wizard installation tries to keep things as simple and uncluttered
 * as possible, making basic use of the menus and forms.
 *
 * This uses eval to replace the theme_block function implementation.
 * It's done this way so it doesn't affect the interface of the rest
 * of the site, and just clicking on home will make the site render
 * normally.
 *
 * Only way this could break is if the theme already had a $theme_block,
 * which i consider unlikely, as most themes are phptemplate based these days.
 * 
 * This could probably work for a number of theme functions.
 */
function hosting_wizard_dummy_theme_block() {
  $func = $GLOBALS['conf']['theme_default'] . "_blocks";

  if (!function_exists($func)) {
    drupal_eval("<?php function $func() { return ' ';}");
  }
}

function hosting_wizard_current_step() {
  if (sprintf("%s/%s", arg(0), arg(1)) == 'hosting/wizard') {
    $current = trim(str_replace('hosting/wizard', '', $_GET['q']), '/');
    if (!$current) {
      $current = 'intro';
    }
    return $current;
  }
}

function hosting_wizard_next_step() {
  static $static = null;
  
  if (!$static) {
    $steps = hosting_wizard_steps();
    $names = array_keys($steps);
    reset($names); // just in case
    $current = current($names);
    while ($current != hosting_wizard_current_step()) {
      $current = next($names);
    }
    $static = next($names);

    // If the next page is the default tab, skip over to the next page.
    if ($steps[$static]['type'] == MENU_DEFAULT_LOCAL_TASK) {
      $static = next($names); // step over default tabs.
    }
  }
  return $static;
}
