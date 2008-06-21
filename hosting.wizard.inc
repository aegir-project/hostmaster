<?php

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
  $func = $GLOBALS['conf']['theme_default'] . "_block";

  if (!function_exists($func)) {
    drupal_eval("<?php function $func() { return ' ';}");
  }
}


/**
  * Control structure for stepping through the installation wizard.
  *
  * This couldn't be simpler. It generates a set of menu items for each
  * of the steps, based on the key of the array.
  *
  * Each step has it's own form, that is a configuration form.
  *
  * Multiple levels are supported, as field/sub gets turned into a
  * hosting_wizard_step_field_sub() form callback.
  *
  * Simpler, and easier to maintain than writing actual _menu items,
  * as this lowers our upgrade dependencies for D6, by only having 
  * 2 items that need to be reimplemented.
 */
function hosting_wizard_steps() {
  // todo: this could do with cleaning up.
  // todo: perhaps add a hook to allow modules to add features into the system
  $steps['intro'] = array(
    'title' => t("Introduction"),
    'heading' => t("Hello World!"),
    'message' => t('An administrator account has been registered for you. 
    Please change your password to continue with the configuration process.')
  );

  $steps['account'] = array(
    'title' => t("User account"),
    'heading' => t("Configure your user account"),
    'message' => t('An administrator account has been registered for you. 
    Please change your password to continue with the configuration process.')
  );

  $steps['provision'] = array(
    'title' => t("Provisioning"), 
    'message' => t('To be able to create sites using Hostmaster, we need some 
       information about your server.'),
  );

  $steps['provision/db'] = array(
    'title' => t('Database server'),
    'message' => t("You need to be able to create databases and database users.") 
  );

  $steps['provision/web'] = array(
    'title' => t("Web server"),
    'message' => t("You will need somewhere to server your files from.")
  );

  $steps['hosting'] = array(
    'title' => t('Hosting'),
    'message' => t("Hostmaster uses a back end script to accomplish the system level
      tasks it needs. This is a very important security requirement, as having this
      functionality accessible via the web could result in incredibly dangerous vulnerabilities.")
    );

 $steps['hosting'] = array(
    'title' => t('Hosting'),
    'message' => t("Hostmaster uses a back end script to accomplish the system level
      tasks it needs. This is a very important security requirement, as having this
      functionality accessible via the web could result in incredibly dangerous vulnerabilities.")
    );

  $steps['import'] = array(
    'title' => t('Import'),
    'message' => t("Hostmaster can import any sites that you are already managing, and start
      managing them for you. Any time you add an additional platform, hostmaster will check for
      additional sites to import")
    );

  /**
   * @todo: figure out what would go on a features page.
   *
   * I'd like there to be a simple mechanism for enabling hosting/provision modules like stats, for instance.
   */
  /**
   $steps['features'] = array(
     'title' => t("Features"),
     'message' => t("Hostmaster is an incredibly flexible system, but if your needs are simple, you 
     might not need to enable all the additional features that Hostmaster is capable of. You may
     also enable these features at a later time, as your needs grow.")
   );*/

    $steps['complete'] = array(
      'title' => t("Complete"),
      'message' => t("Congratulations, you have finished installation of hostmaster. 
      If you have any questions, go to groups.drupal.org/hm2.")
    );

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
    'callback arguments' => array('hosting_wizard_step_intro'),
    'type' => MENU_NORMAL_ITEM,
    'access' => user_access('access hosting wizard'), 
  );

  if (!$may_cache && (sprintf("%s/%s", arg(0), arg(1)) == 'hosting/wizard')) {
    hosting_wizard_dummy_theme_block();
    $steps = hosting_wizard_steps();
    $x = 1;
    foreach ($steps as $key => $step) {
      $items[] = array(
        'path' => 'hosting/wizard/' . $key,
        'title' => $step['title'],
        'description' => $step['message'],
        'callback' => 'drupal_get_form',
        'callback arguments' => array('hosting_wizard_step_' . str_replace("/", "_", $key)),
        'type' => ($x == 1) ? MENU_DEFAULT_LOCAL_TASK : MENU_LOCAL_TASK,
        'access' => user_access('access hosting wizard'),
        'weight' => $x++
      );
    }
  }
}

/**
 * Introduction form.
 *
 * @todo: add an option for minimal configuration, which just hits the required pages?
 */
function hosting_wizard_step_intro() {
  $form['introduction']['#value'] = 
    t("<p>I am a configuration wizard designed to help you through configuring the Hostmaster provisioning system.</p>
    <p>Once properly configured, this system will help you manage your Drupal sites. 
    It will even be able to manage them across multiple versions of Drupal, and even across multiple servers.</p>
    <p>Before we get started with the configuration, please make sure that you meet the following requirements.</p>");
  $form['requirements']['#value'] = _provision_requirements('basic');

  return hosting_wizard_form('intro', $form, t('Set up my user account'));
}

/**
 * Account form.
 *
 * The fields were more easily copied from user_edit than trying to import it and get rid of the cruft
 */
function hosting_wizard_step_account() {
  global $user;
  $form = array();
  $form['title']['#value'] = t('<h1></h1>');
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
    '#required' => TRUE,
    '#size' => 30,
  );
  $client_help = hosting_node_help('client');
  $form['client_info']['#value'] = t("<p>This information will also be used by Hostmaster to create the <a id='hosting-wizard-client-help-show'>primary client.</a></p>
    <div id='hosting-wizard-client-help' class='hosting-wizard-glossary'>!client_help</div>", array('!client_help' => $client_help));
  
  return hosting_wizard_form('account', $form, t("Save and go on to provisioning"));
}

/**
 * Ensure a valid user
 */
function hosting_wizard_step_account_validate($form_id, $values, $form) {
  global $user;
  _user_edit_validate($user->uid, $values);
}

/**
 * Save the user account, and update the default client node
 */
function hosting_wizard_step_account_submit($form_id, $values) {
  global $user;
  user_save($user, $values);
  $node = node_load(HOSTING_DEFAULT_CLIENT);
  $node->client_name = $values['name'];
  $node->email = $values['email'];
  node_save($node);
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

  $form['#submit']['hosting_wizard_step_' . str_replace('/', '_', $step) . '_submit'] = array(); 
  $form['#submit']['hosting_wizard_form_submit'] = array();
  $form['#prefix'] = '<div id="hosting-wizard-form">';
  $form['#suffix'] = '</div>';
  $form['step'] = array('#type' => 'value', '#value' => $step);

  $form['wizard_form'] = array(
    '#prefix' => '<div id="hosting-wizard-form-buttons">',
    '#suffix' => '</div>',
    '#weight' => 100
  );
  $form['wizard_form']['submit'] = array(
    '#type' => 'submit',
    '#value' => ($next) ? $next : t("Continue")
   );

  return $form;
}

/**
 * Provision information page. Explains what you are about to do, uneccesary?
 */
function hosting_wizard_step_provision() {
  $form = array();
  $form['title']['#value'] = t('<h1>Provisioning platform configuration</h1>');
  $form['introduction']['#value'] = 
    t("<p>The Provisioning framework provides the back end of the system. It does all the nitty gritty server level things, so that you don't have to.</p>
    <p>Provision is very careful about security, and as such has been designed to run under the absolute minimum permissions it needs to accomplish it's job. For the security of your data, Hostmaster will not run unless it is running in this optimally secure configuration.</p>    <p>Because of these stringently enforced security requirements, it takes a bit longer to set up than would be ideal, however we feel it is a fair tradeoff.</p>");
  t("<p>We have tried to collect as much data about our current environment as we can, and provide these as defaults for server configuration. For a lot of cases this works adequately, however we will perform checks on every item, to ensure that it is in working order.</p>
    <p>In some cases, such as file permissions, we are incapable of testing items until the hosting front end has been succesfully configured. If we do find a problem, you will be directed back to this section to modify your configuration.</p>");

  return hosting_wizard_form('provision', $form, t('Configure my database server'));
}

/**
 * Database configuration form
 *
 * Inherits the db_server_node_form, and shifts some things around.
 * Perhaps those changes need to go in upstream?
 */
function hosting_wizard_step_provision_db() {
  $form = drupal_retrieve_form('db_server_node_form', node_load(HOSTING_OWN_DB_SERVER));
  _hosting_wizard_cleanse_node_form($form);

  $form['heading']['#weight'] = -10;
  $form['heading']['#value'] = t('<h1>Configure database server</h1>');

  $own_link = _hosting_node_link(HOSTING_OWN_DB_SERVER);

  $form['introduction']['#weight'] = -9;
  $form['introduction']['#value'] = 
    t('<p>You need at least one fully functional database server to be able to manage sites. 
        Luckily, if you can read this, you are already most of the way there.</p>
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
    '#description' => _provision_requirements('mysql_user', 'summary'),
  );
  $form['db_details']['db_user'] = $form['db_user'];
  unset($form['db_user']);
  $form['db_details']['db_passwd'] = $form['db_passwd'];
  unset($form['db_passwd']);

  return hosting_wizard_form('provision/db', $form, t('Configure my web server'));
}

/**
 * Save the modified db_server
 */
function hosting_wizard_step_provision_db_submit($form_id, $values) {
  $node = node_load(HOSTING_OWN_DB_SERVER);
  $node->title = $values['title'];
  $node->db_user = $values['db_user'];
  $node->db_passwd = $values['db_passwd'];
  node_save($node); 
}

/**
 * Simple check to see if the user has completed a step yet.
 */
function hosting_wizard_completed($step) {
  return variable_get("hosting_wizard_completed_" . $step, false);
}

/**
 * Form that confirms that cron is running, and that the platform has been successfuly verified
 *
 * This page is essentially one huge page of contextual help, to ensure that everything is up and running.
 */
function hosting_wizard_step_hosting() {
  // @ TODO: make this coherent, and add error reporting about what went wrong, and how it needs to be fixed.
  $form = array();
  $form['header']['#value'] = t('<h1>Hosting framework configuration</h1>');
  $form['intro']['#value'] = t('<p>Hosting provides the user interface used to administrate your sites. The front end is provided by the Hostmaster install profile, and the Hosting contributed module. It defines a complete Drupal based data model for managing the various aspects of your installation.</p>
    <p>Each entity being managed by Hostmaster has a node created for it<p>');
  $form['cron']['#value'] = _provision_requirements('cron');

  if (variable_get('hosting_dispatch_last_run', false)) {
    $form['last_run']['#value'] = t('<p>The dispatch process has been succesfully executed</p>');
  }
  else {
    $form['last_run']['#value'] = t('<p>The dispatch process has not been run.</p>');
  }

  return hosting_wizard_form('hosting', $form, t('Done now'));
}



/**
 * Configures web_server_node_form minus almost all the forms.
 *
 * @todo pull changes in the form upstream
 */
function hosting_wizard_step_provision_web() {
  $form = drupal_retrieve_form('web_server_node_form', node_load(HOSTING_OWN_WEB_SERVER));
  _hosting_wizard_cleanse_node_form($form);

  $form['heading']['#weight'] = -50;
  $form['heading']['#value'] = t('<h1>Web server configuration</h1>');

  $own_link = _hosting_node_link(HOSTING_OWN_WEB_SERVER);

  $form['introduction']['#weight'] = -45;
  $form['introduction']['#value'] = 
    t('<p>You need to have one fully working web server configured to be able to create sites. The web server can run into problems with file permissions quite easily, and these are unfortunately undetectable to us until the hosting back end is configured.</p>
    <p>Like the database configuration, this form is actually the !edit_page for the !own_server database server node, which has been heavily annotated and a number of fields removed for simplicity.</p>
    <p>It is recommended to use the !own_server page to manage this server during daily use.</p>
        ', 
        array('!own_server' => $own_link, '!edit_page' => l(t("edit page"), 'node/' . HOSTING_OWN_WEB_SERVER . '/edit' )));

  $form['title']['#type'] = 'value';
  $form['ip_address']['#type'] = 'value';
  $form['backup_path']['#description'] = _provision_requirements('backup_path', 'summary');
  $form['config_path']['#description'] = _provision_requirements('config_path', 'summary');
  $form['restart_cmd']['#description'] = _provision_requirements('visudo', 'summary');

  $form['acc_details'] = array(
    '#type' => 'fieldset',
    '#collapsed' => false,
    '#title' => t('System account details'),
    '#description' => _provision_requirements('group', 'summary'),
    '#weight' => -30
  );
  $form['acc_details']['script_user'] = $form['script_user'];
  unset($form['script_user']);
  $form['acc_details']['web_group'] = $form['web_group'];
  unset($form['web_group']);

  // remove all the things i don't want from the node form.
  return hosting_wizard_form('provision/web', $form, t('Set up the Hosting framework'));
}

/**
 * Save the changes to the default web server node
 */
function hosting_wizard_step_provision_web_submit($form_id, $values) {
  $node = node_load(HOSTING_OWN_WEB_SERVER);
  $node->title = $values['title'];
  $node->ip_address = $values['ip_address'];
  $node->script_user = $values['script_user'];
  $node->web_group = $values['web_group'];
  $node->config_path = $values['config_path'];
  $node->backup_path = $values['backup_path'];
  $node->restart_cmd = $values['restart_cmd'];
  node_save($node);
}

/**
 * Configure things like user roles and extra modules.
 *
 * The actual step is commented out atm.
 */
function hosting_wizard_step_features() {
  $form['placeholder']['#value'] = t('<h1>Features placeholder page</h1>');
  $form['content']['#value'] = t("This page will contain some basic features at first,
    primarily a usage scenario");
  return hosting_wizard_form('features', $form, t("That's about it then."));
}

/**
 * The last page.
 *
 * Essentially gives a very small summary, and links you to a few interesting things
 * to do next (configure your site, add your first site etc)
 *
 * When clicking on the button you will be forwarded to your site.
 */
function hosting_wizard_step_complete() {
  $form['configure']['#value'] .= 
    t('<p><strong>Configure your website</strong>
               Once logged in, visit the <a href="@admin">administration section</a>,
               where you can <a href="@config">customize and configure</a> all aspects
               of your website.</p>', 
               array('@admin' => url('admin'), '@config' => url('admin/settings'))) ;

  $form['create_site']['#value'] = t('<p><strong>Create your first hosted site.</strong> This system uses special site posts to store information about your sites, so you can simply <a href="@create_site">create a site post</a> to get your first hosted site running.</p>', array('@create_site' => url('node/add/site')));

  return hosting_wizard_form('complete', $form);
}

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
 * Additional submit handler that gets added to the end of 
 * all wizard forms.
 *
 * This redirects the user to the next step in the wizard
 */
function hosting_wizard_form_submit($form_id, $values) {
  $values = (array) $values;
  $steps = hosting_wizard_steps();
  $names = array_keys($steps);
  variable_set('hosting_wizard_completed_' . $values['step'], mktime);

  reset($names); // just in case
  if (!$values['step']) {
    return "hosting/wizard/" . current($names);
  }
  while (current($names) != $values['step']) {
    next($names);
  }

  $name = next($names);

  if ($name) {
    return "hosting/wizard/" . current($names);
  }
  else {
    return variable_get('site_frontpage', 'node');
  }
}


