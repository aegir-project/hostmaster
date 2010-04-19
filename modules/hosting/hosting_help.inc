<?php

// $Id$

/**
 * @file Hosting help subsystem
 */

/**
 * Constants used to generate the requirement help documentation.
 */

/**
 * Status is irrelevant. Can't be checked
 */
define('HOSTING_STATUS_NONE', 0);

/**
 * Requirement is met
 */
define('HOSTING_STATUS_SUCCESS', 1);

/**
 * Requirement not met. Less severe than an error.
 */
define('HOSTING_STATUS_WARNING', 2);

/**
 * Requirement failed. Fix before site will function
 */
define('HOSTING_STATUS_ERROR', 4);

/**
 * Returns a class name for the output of the form element.
 */
function _hosting_status_class($status)  {
  static $map;
 
  if (!sizeof($map)) {
   $map = array(
      HOSTING_STATUS_NONE => 'hosting-status-none',
      HOSTING_STATUS_SUCCESS => 'hosting-status-success',
      HOSTING_STATUS_WARNING => 'hosting-status-warning',
      HOSTING_STATUS_ERROR => 'hosting-status-error',
    );
  }

  return $map[$status];
}

/**
 * Constants used to configure contextual hosting help
 */

/**
 * Display component of help
 **/
define('HOSTING_HELP_ENABLED', 1);

/**
 * Don't show help component
 **/
define('HOSTING_HELP_DISABLED', ~HOSTING_HELP_ENABLED);

/**
 * These are internally used to test against. Use the COLLAPSIBLE AND
 * COLLAPSED instead
 **/
define('_HOSTING_HELP_CAN_COLLAPSE', 2);
define('_HOSTING_HELP_HAS_COLLAPSED', 4);

/**
 * Show help with the ability to collapse to save space. 
 * This automatically enables the help component
 **/
define('HOSTING_HELP_COLLAPSIBLE', HOSTING_HELP_ENABLED | _HOSTING_HELP_CAN_COLLAPSE);

/** 
 * Display help component with an initial collapsed state.
 * This automatically enables and sets the help component to be collapsible
 */
define('HOSTING_HELP_COLLAPSED', HOSTING_HELP_COLLAPSIBLE | _HOSTING_HELP_HAS_COLLAPSED);


/**
 * Returns a class name for the output of the form element
 */
function _hosting_help_class($state) {
  static $map;
 
  if (!sizeof($map)) {
    $map = array(
      HOSTING_HELP_ENABLED => 'hosting-help',
      HOSTING_HELP_COLLAPSIBLE => 'hosting-help-collapsible',
      HOSTING_HELP_COLLAPSED => 'hosting-help-collapsed'
    );
  }
  return $map[$state];
}

/**
 * Implementation of hook_help()
 */
function hosting_help($path, $arg) {
  switch ($path) {
    case 'admin/help/hosting':
      $output .= '<p>' . t('The Hostmaster system is a set of modules that provide an interface to the Provisioning framework.') . '</p>';
      $output .= '</dl>';      
      $output .= '<a name="requirements"></a><h3>' . t('Requirements') . '</h3></a>';
      // @TODO: make this more clear
      $output .= _hosting_requirements('provision');
      $output .= _hosting_requirements('hosting_setup');
      $output .= _hosting_requirements('cron');
      $output .= _hosting_requirements('user');
      $output .= _hosting_requirements('group');
      $output .= _hosting_requirements('backup_path');

      $output .= '<a name="glossary"></<h3>' . t('Glossary') . '</h3></a>';
      $output .= '<dl>';
      $types = node_get_types();

      foreach ($types as $type => $info) {
        if ($info->description) {
          $output .= "<dt><a name=\"glossary-$type\"></a>" . $info->name . '</dt>';
          $output .= '<dd>' . $info->description . '</dd>';
        }
      }
      return $output;
  }
}

/**
 * TODO: replace. this is OLD OLD OLD
 */
function _hosting_introduction() {
  $default_message = t('<p>Please follow these steps to set up and start using your website:</p>');
  $default_message .= '<ol>';

  $default_message .= '<li>'. t('<strong>Configure your website</strong>
                  Once logged in, visit the <a href="@admin">administration section</a>,
                  where you can <a href="@config">customize and configure</a> all aspects of your website.', 
                  array('@admin' => url('admin'), '@config' => url('admin/settings'))) .'</li>';
  $default_message .= '<li>'. t('<strong>Configure the Hosting framework.</strong>
    You now have a configured provisioning framework, but there are some additional configuration options for the hosting framework that can be set.
    Please visit the <a href="@hosting_admin">hosting administration</a> section for more information.', array('@hosting_admin' => url('admin/hosting'))) .'</li>';
  $default_message .= '<li>'. t('<strong>Create your first hosted site.</strong> This system uses special site posts to store information about your sites, so you can simple <a href="@create_site">create a site post</a> to get your first hosted site running.', array('@create_site' => url('node/add/site'))) .'</li>';
  $default_message .= '</ol>';
  $default_message .= '<p>'. t('For more information, please refer to the <a href="@help">help section</a>, or the <a href="@handbook">online Drupal handbooks</a>. You may also post at the <a href="@forum">Drupal forum</a>, or view the wide range of <a href="@support">other support options</a> available.', array('@help' => url('admin/help'), '@handbook' => 'http://drupal.org/handbooks', '@forum' => 'http://drupal.org/forum', '@support' => 'http://drupal.org/support')) .'</p>';

  $output = '<div id="first-time">'. $default_message .'</div>';

  return $output;
  
}

/**
 * Per node type description text. To be stored in the node_type table.
 *
 * @param type
 *  The node type.
 * @return
 *  Description text for the node type.
 */
function hosting_node_help($type) {
  switch ($type) {
    case 'site' :
      return t("<strong>An instance of a hosted site.</strong>
                It contains information relating to the site, most notably the domain name, database server 
                and platform it is being published on. A site may also have several aliases for additional
                domains the site needs to be accessible on.");
      break;
    case 'platform' :
      return t("<strong>The file system location on a specific web server on which to publish sites.</strong>
                Multiple platforms can co-exist on the same web server, and need to do so for
                upgrades to be managed, as this is accomplished by migrating the site between platforms.
                Platforms are most commonly built for specific releases of Drupal.");
      break;
    case 'client' :
      return t("<strong>The person or group that runs the site.</strong> 
                This information is usually required for billing and access purposes, to ensure
                that only certain people are able to view the information for sites they run. 
                If you do not intend on having more than one client access the system, 
                you will not need to create any additional clients for your purposes.");
      break;
    case 'server' :
      return t("<strong>The physical machine which will provide various services to your site.</strong>
                Each server can have multiple services associated to it. To host a site you require a database service and a web service, which can either be provided by a single or multiple servers.");
      break;
    case 'task' :
      return t("<strong>The mechanism whereby Hostmaster keeps track of all changes that occur to the system.</strong>
                Each task acts as a command for the back-end, and contains a full log of all changes that have occurred.
                If a task should fail, the administrator will be notified with an explanation of exactly what went wrong,
                and how to fix it.");
      break;      
  }
}

/**
 * Page callback with in depth requirement documentation
 *
 * This will invoke hook_help() for every module implementing
 * hook_hosting_service()
 *
 * @see hook_hosting_service()
 * @see hook_hosting_help()
 */
function hosting_help_requirements() {
  $output .= _hosting_requirements("basic_drupal");  
  $output .= _hosting_requirements("basic_unix");  
  $output .= _hosting_requirements("basic_server");  
  $modules = module_implements('hosting_service');
  foreach ($modules as $module) {
    $service = module_invoke($module, 'hosting_service');
    $name = current($service);
    $help = module_invoke($module, 'help', 'admin/help/provision#requirements', null);
    if ($name && $help) {
      $output .= "<a href='requirements-$module'></a><h3>". t($name) .'</h3>';
      $output .=  $help;
    }
  }
  return $output;
}

/**
 * Helper function for displaying contextual help when not used in a form.
 */
function _hosting_requirements($req, $section = 'all') {
  $item = _element_info('requirement_help');
  $item['#requirement'] = $req;
  $item['#type'] = 'requirement_help';
/*
  foreach (array('#heading', '#summary', '#suggestion', '#configuration') as $key) {
    if (in_array($section, array('all', $key))) {
      $item['#'. $key] = HOSTING_HELP_ENABLED;
    }
    elseif ($section != 'all') {
      $item['#'. $key] = ($section == $key) ? HOSTING_HELP_ENABLED : HOSTING_HELP_DISABLED;
    }
  }
 */
  return theme("requirement_help", hosting_requirement_process($item));
}

/**
 * Find the requirements help document for the given element
 *
 * This will call hook_requirements() for the given element. The
 * function name should be _hosting_$req_requirements() for this to
 * function properly.
 *
 * @see hook_requirements()
 * @see _hosting_requirements()
 */
function hosting_get_requirement($req) {
  $func = '_hosting_'. $req .'_requirements';
  if (function_exists($func)) {
    $help = $func();
  }
  return $help;
}

/**
 * Places the various help components info the $element array
 *
 * @see hosting_get_requirement()
 * @return array a form item with '#help' properly filled in with hosting_get_requirement() analysis
 */
function hosting_requirement_process(&$element) {
  if (!$element['#requirement']) {
    return $element;
  }

  $element['#help'] = hosting_get_requirement($element['#requirement']);
  return $element;
}

/**
 * Implementation of hook_elements.
 *
 * Defines a number of form elements that are used for formatting the contextual help
 * in forms.
 */
function hosting_elements() {
  $type['requirement_help'] = array(
    '#requirement' => NULL,
    '#input' => FALSE,
    '#status' => HOSTING_STATUS_NONE,
    '#heading' => HOSTING_HELP_ENABLED,
    '#summary' => HOSTING_HELP_ENABLED,
    '#summary_prefix' => t("What is this?"),
    '#configuration' => HOSTING_HELP_COLLAPSED,
    '#configuration_prefix' => t("How do I configure this?"),
    '#default_messages' => array(
      HOSTING_STATUS_SUCCESS => t("You have met this requirement."),
      HOSTING_STATUS_WARNING => t("This requirement has a non critical error."),
      HOSTING_STATUS_ERROR => t("This requirement has a critical error. This system will not operate until it has been fixed")),
    '#process' => array('hosting_requirement_process'),
  );
  return $type;
}

/**
 * Theme function for displaying contextual help.
 *
 * Can control individual components of the help, for display in various places.
 */
function theme_requirement_help(&$element) {
  drupal_add_js(drupal_get_path('module', 'hosting') .'/hosting_help.js');
  drupal_add_css(drupal_get_path('module', 'hosting') .'/hosting_help.css');
  $req = $element['#requirement'];
  $help = $element['#help']; // this is just to make it easier to work with.

  //place anchor so user can be directed to right page.
  $output .= "<a name='hosting-help-$req'></a>";

  $output .= '<div class="hosting-requirement-help' . 
    ' '. _hosting_status_class($element['#status']) .'">';

  if ($element['#status'] != HOSTING_STATUS_NONE) {
    $output .= "<div class='message'>". (($element['#message']) ? $element['#message'] : $element['#default_messages'][$element['#status']]) ."</div>";
  }

  $components = array('summary', 'suggestion', 'configuration');
  foreach ($components as $key) {
    if (($element["#$key"] & HOSTING_HELP_ENABLED) && !is_null($help[$key])) {
      $display_type = _hosting_help_class($element["#$key"]);
      $output .= "<div class='hosting-help-$key $display_type'>";
      if (!is_array($help[$key])) {
        // it is simpler if there's only one way to print the component
        $help[$key] = array($help[$key]);  
      }
      if ($element["#$key".'_prefix'] && ($element["#$key"] & _HOSTING_HELP_CAN_COLLAPSE)) {
        $output .= '<a href="javascript:void(0)" class="hosting-help-toggle">'. $element["#$key".'_prefix'] .'</a>';
      }

      $output .= "<div class='hosting-help'><p>". implode("</p><p>", $help[$key]) ."</p></div>"; 
      $output .= '</div>';
    } 
  }
  $output .= '</div>';
  if (($element['#heading'] & HOSTING_HELP_ENABLED) && $help['title'] && !$element['#title']) {
    $element['#title'] = $help['title'];
  }
  $element['#value'] = $output;
  return theme('item', $element);
}

/**
 * @TODO: handle element children for requirement help
function _hosting_basic_requirements() {
  $help['drupal'] = _hosting_basic_drupal_requirements();
  $help['unix'] = _hosting_basic_unix_requirements();
  $help['server'] = _hosting_basic_server_requirements();

  return $help;
}
 */

function _hosting_backup_path_requirements() {
    $username = HOSTING_DEFAULT_SCRIPT_USER;
    $group = HOSTING_DEFAULT_WEB_GROUP;
    $backup_path = HOSTING_DEFAULT_BACKUP_PATH;
    $mkdir_cmd['@backup_path'] = $backup_path;      
    $mkdir_cmd['@mkdir_cmd'] = <<<EOF
    mkdir $backup_path
    chown $username:$username $backup_path
    chmod 0700 $backup_path
EOF;
  $help['title'] = t('Write access to a directory to store backups');
  $help['summary'] = t('The drush user (<a href="http://drupal.org/project/drush">http://drupal.org/project/drush</a>) needs to be able to maintain the backups repository to ensure that your site is backed up successfully. 
                        It is incredibly important that this path is not accessible via the web server, so that no undesirables can get their
                        hands on your database. The recommended path is directly above your platform path, but it can be anywhere.');

  $help['suggestion'] = t('Based on your server configuration we have determined that your path should be <code>@backup_path</code>', $mkdir_cmd);
  $help['configuration'] = t('Please enter the following commands : <pre>@mkdir_cmd</pre>', $mkdir_cmd);
  return $help;
}


function _hosting_user_requirements() {
  $username = HOSTING_DEFAULT_SCRIPT_USER;
  $userhome = HOSTING_DEFAULT_SCRIPT_USER_HOME;
  $add_cmd = <<<EOF
    sudo adduser --system --group --home $userhome $username
EOF;
  $help['title'] = t('A separate system account for the scripts');
  $help['summary'] = t('The provision framework requires that the scripts run as a non-root system account, to ensure that
    it can correctly set the file permissions on the hosted files. All existing files need to be changed to belong to this
    new system account.');
  $help['suggestion'] = t('Based on your server configuration, we have determined that your user account should be <code>@script_user</code>', array('@script_user' => HOSTING_DEFAULT_SCRIPT_USER));
  $help['configuration'][] = t('If your system supports it, run the adduser command (if this command is unavailable, please consult your operating system documentation on how to add new system accounts) : <pre>@cmd</pre>', array('@cmd' => $add_cmd));
  return $help;
}

function _hosting_group_requirements() {

    $username = HOSTING_DEFAULT_SCRIPT_USER;
    $group = HOSTING_DEFAULT_WEB_GROUP;
      

    $vigr_cmd = <<<EOF
    sudo adduser $username $group
EOF;
    $vigr1 = <<<EOF
    $group::99:
EOF;
    $vigr2 = <<<EOF
    $group::99:$username
EOF;
    $vigr3 = <<<EOF
    $group::99:anotheruser,$username
EOF;

    $su = <<<EOF
    su -
EOF;

  $help['title'] = t('The system group of the web server');
  $help['summary'] = t('For the provision framework to be able to ensure that the file permissions of the 
                          hosted sites are always as safe as can be, and especially to make sure that the web server does 
                          not have the ability to modify the code of the site, the configured system account needs to be a member
                          of the web server group, in order to be able to correctly set the file permissions.');

  $help['suggestion'] = t('Based on your server configuration we have determined that you should add the 
                        system account "<code>@username</code>" to the "<code>@group</code>" system group.', array("@username" => $username, "@group" => $group));

  $help['configuration'] =t('If your system account is not a member of the web group, you can add them by using the
                          <code>adduser</code> command:
                          <pre>@vigr_cmd</pre>
                          If that command is not available, you will need to edit the /etc/group file directly with
                          your vigr or your favorite editor.
                          Find the line that says : <pre>@vigr1</pre>
                          Then add the username to the end of the line, so that it looks like : <pre>@vigr2</pre>
                          If there were already users in the group, add your user to the group using a comma as separator :
                          <pre>@vigr3</pre>
                          Once you have changed this, you will need to log out and log back into your terminal session for this
                          setting to take effect. Alternatively you
                          can get a new login shell by typing : <pre>@su</pre>', 
                          array('@vigr_cmd' => $vigr_cmd, '@vigr1' => $vigr1, '@vigr2' => $vigr2, '@vigr3' => $vigr3, '@su' => $su));
  return $help;

}

/**
 * Implementation of hook_requirements()
 */
function _hosting_basic_drupal_requirements() {
  $help['title'] = t('A system capable of running Drupal');
  $help['summary'] = t('If you are reading this via the inline help, this would be kind of obvious. This system is entirely Drupal based, and has the same base requirements that Drupal does.');
  return $help;
}

/**
 * Implementation of hook_requirements()
 */
function _hosting_basic_server_requirements() {
  $help['title'] = t('Your own server');
  $help['summary'] = t('The level of access required to be able to configure this system is very far beyond what is commonly available to users with shared hosting.');
  return $help;
}

/**
 * Implementation of hook_requirements()
 */
function _hosting_basic_unix_requirements() {
  $help['title'] = t('A unix based operating system');
  $help['summary'] = t('The majority of functionality in this system occurs in the back-end, through command line scripting. There are several features (such as symlinks), that are not available to users on Windows. There are no plans currently to add Windows support.</p>');
  return $help;
}

/**
 * Implementation of hook_requirements()
 */
function _hosting_cron_requirements() {
  $cron_cmd['!queueconf'] = l(t("Queue administration"), "admin/settings/queues");
  $cron_cmd['@cron_line'] = hosting_queues_cron_cmd();
  $help['title'] = t('A cron entry for queue dispatcher.');
  $help['summary'][] = t('Changes to this system are executed via a back end script which needs to be called by a user other than 
    the web server user for security reasons at regular intervals.');
  $help['summary'][] = t('The intervals at which these commands are run can be customised by going to the !queueconf section.', $cron_cmd);
  $help['summary'][] = t('The <code>hosting-setup</code> command installs the required cron entry for you.', $cron_cmd);
  $help['configuration'] = t('Add the following line to your crontab : <code>@cron_line</code>', $cron_cmd);
  return $help;
}

/**
 * Implementation of hook_requirements()
 */
function _hosting_platform_requirements() {
  $platform = node_load(HOSTING_OWN_PLATFORM);
  $web_server = node_load(HOSTING_OWN_WEB_SERVER);
  $db_server = node_load(HOSTING_OWN_DB_SERVER);

  $task = hosting_get_most_recent_task($platform->nid, 'verify');
  
  $help['title'] = t('Provisioning framework status');
  $help['summary'][] = t('In order for the hosting system to be able to create sites, it needs a platform to publish these sites on. A platform is a Drupal site that has the provision back-end installed. Each platform also requires one correctly configured web server and one correctly configured database server. Once a new platform has been created, Hosting will automatically schedule a task that will verify the new platform is working properly.');
  $help['summary'][] = t('As hosting is also running on a platform, it is suggested that you successfully configure your <a href="@platform">primary platform</a>
    before creating additional platforms and servers.', array('@platform' => url('node/' . HOSTING_OWN_PLATFORM)));
  $help['summary'][] = t('Each platform must be verified at least once, but if you find your platform is not being verified, look at the error log on the <a href="@verify_link">Verify platform task of this platform</a>', array("@verify_link" => url("node/" . $task->nid)));

   return $help;

}
