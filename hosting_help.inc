<?php
/**
 * @file Hosting help subsystem
 */
 
/**
 * Implementation of hook_help()
 */
function hosting_help($section) {

  switch ($section) {
    case 'admin/help#hosting':
      $output .= t('<p>The Hostmaster system is a set of modules that provide an interface to the <a href="@provision_help">Provisioning framework</a>.</p>', 
                  array('@provision_help' => url('admin/help/provision')));
      $output .= '</dl>';      
      $output .= '<a name=\'requirements\'></a><h3>' . t('Requirements') . '</h3></a>';
      // @TODO: make this more clear
      $output .= _provision_requirements('provision');
      $output .= _provision_requirements('hosting_setup');
      $output .=  _provision_requirements('cron');

      $output .= '<a name=\'requirements\'></a><h3>' . t('Glossary') . '</h3></a>';
      $output .= '<dl>';
      $types = node_get_types();

      foreach ($types as $type => $info) {
        if ($info->description) {
          $output .= "<dt><a name='glossary-$type'></a>" . $info->name . '</dt>';
          $output .= '<dd>' . $info->description . '</dd>';
        }
      }
      
      $output .= '<a name=\'commands\'><h3>' . t('Commands') . '</h3></a>';
      $commands = module_invoke_all('drush_command');
      $output .= "<dl>";
      foreach ($commands as $command => $info) {
        if (preg_match('/^hosting/', $command)) {
          if (sizeof($info['arguments'])) {
            $command .= ' ' . implode(' ', (array) key($info['arguments']));  
          }
          
          if (sizeof($info['optional arguments'])) {
            $command .= ' [' . implode('] [', (array) key($info['optional arguments'])) . ']';  
          }
          $output .= '<dt>' . "<code>drush.php $command</code>" . '</dt>';
          $output .= '<dd>' .  $info["description"] . '</dd>';
        }
      }
      $output .= "</dl>";
      return $output;

    case 'admin/settings/provision' :
      $output = t("These settings will also change the settings of the platform, database server and web server that this site is running on.");
    
      return $output;
  }
}

function _provision_hosting_setup_requirements() {
  $docroot = PROVISION_DOCROOT_PATH;
  $uri = PROVISION_BASE_URL;
  $drush_path = PROVISION_DRUSH_PATH;
  $setup_cmd = <<<EOF
    [$username@hm2 ~]$ cd $docroot
    [$username@hm2 ~]$ ./$drush_path hosting setup
EOF;
  $help['title'] = t('Perform initial configuration by running the <code>Hosting Setup</code> command');

  $help['summary'][] = t('The hosting setup command performs several useful installation functions, but it\'s primary responsibility
        is adding the crontab entry for the queue dispatcher and ensuring that has been correctly installed.');

  $help['summary'][] = t('The queue dispatcher runs every minute. It keeps track of what work needs to be done on the system 
    and instructs the provisioning framework to perform the needed tasks in the task queue.');
  $help['summary'][] = t('Otherwise said, without the dispatcher, changes on the front end will not translate in the required backend configuration preventing actual sites management.');
  $help['configuration'] = t('Ensure that you are logged into the shell as %script_user, and then execute the following commands :<pre>@setup_cmd</pre>', 
     array('%script_user' => PROVISION_SCRIPT_USER, '@setup_cmd' => $setup_cmd));

  return $help;
}
function _provision_cron_requirements() {
  $username = PROVISION_SCRIPT_USER;
  $cron_cmd['!queueconf'] = l(t("Queue administration"), "admin/settings/queues");
  $cron_cmd['@cron_line'] = hosting_queues_cron_cmd();
  $help['title'] = t('A cron entry for queue dispatcher.');
  $help['summary'][] = t('Changes to this system are executed via a back end script which needs to be called by a user other than 
    the web server user for security reasons at regular intervals.');
  $help['summary'][] = t('The intervals at which these commands are run can be customised by going to the !queueconf section.', $cron_cmd);
  $help['summary'][] = t('The <code>hosting setup</code> command installs the required cron entry for you.', $cron_cmd);
  $help['configuration'] = t('Add the following line to your crontab : <code>@cron_line</code>', $cron_cmd);
  return $help;
}


function _provision_platform_requirements() {
  $platform = node_load(HOSTING_OWN_PLATFORM);
  $web_server = node_load(HOSTING_OWN_WEB_SERVER);
  $db_server = node_load(HOSTING_OWN_DB_SERVER);

  $task = hosting_get_most_recent_task($platform->nid, 'verify');
  
  $help['title'] = t('Provisioning framework status');
  $help['summary'][] = t('In order for the hosting system to be able to create sites, it needs a platform to publish these sites on. A platform is a Drupal site that has the provision back end installed. Each platform also requires one correctly configured web server and one correctly configured database server. Once a new platform has been created, Hosting will automatically schedule a task that will verify the new platform is working properly.');
  $help['summary'][] = t('As hosting is also running on a platform, it is suggested that you succesfully configure your <a href="@platform">primary platform</a>
    before creating additional platforms and servers.', array('@platform' => url('node/' . HOSTING_OWN_PLATFORM)));
  $help['summary'][] = t('Each platform must be verified at least once, but if you find your platform is not being verified, look at the error log on the <a href="@verify_link">Verify platform task of this platform</a>', array("@verify_link" => url("node/" . $task->nid)));

   return $help;

}

/**
 * TODO: replace. this is OLD OLD OLD
 */
function _hosting_introduction() {
  $create_site_link = l(t('Create a site now?'), 'node/add/site');
  if ($_GET['q'] != variable_get('site_frontpage', 'node')) {
    return t("No sites have been created yet. !link", array(
      '!link' => $create_site_link));
  }
  else {
    drupal_set_title('Welcome to your new Hosting website!');
    $default_message = t('<p>Please follow these steps to set up and start using your website:</p>');
    $default_message .= '<ol>';

    if (!$admin) {
      $default_message .= '<li>'. t('<strong>Create your administrator account</strong> To begin, <a href="@register">create the first account</a>. This account will have full administration rights and will allow you to configure your website.', array('@register' => url('user/register'))) .'</li>';
    }
    $default_message .= '<li>'. t('<strong>Configure your website</strong>
                    Once logged in, visit the <a href="@admin">administration section</a>,
                    where you can <a href="@config">customize and configure</a> all aspects of your website.', 
                    array('@admin' => url('admin'), '@config' => url('admin/settings'))) .'</li>';
    $default_message .= '<li>'. t('<p><strong>Configure the Provisioning framework.</strong>
                    Before you configure the Provisioning framework, it is highly recommended that you first read the <a href="@requirements">Provision Requirement documentation</a>.
                    It provides a step by step guide for configuring your server to use the framework.</p>
                    <p>You will then need to complete the <a href="@provision">provisioning section</a> to configure the framework.</p>', 
                    array('@provision' => url('admin/settings/provision'), 
                    '@requirements' => url('admin/help/provision/requirements'))) .'</li>';
    $default_message .= '<li>'. t('<strong>Configure the Hosting framework.</strong>
      Most of the configuration for the system is configured for provisioning, but there are a number of small configuration changes that need to set.
      For all the information you will need, please follow the instructions in the <a href="@hosting_help">Hosting help section</a>.', array('@hosting_help' => url('admin/help/hosting/requirements'))) .'</li>';
    $default_message .= '<li>'. t('<strong>Create your first hosted site.</strong> This system uses special site posts to store information about your sites, so you can simple <a href="@create_site">create a site post</a> to get your first hosted site running.', array('@create_site' => url('node/add/site'))) .'</li>';
    $default_message .= '</ol>';
    $default_message .= '<p>'. t('For more information, please refer to the <a href="@help">help section</a>, or the <a href="@handbook">online Drupal handbooks</a>. You may also post at the <a href="@forum">Drupal forum</a>, or view the wide range of <a href="@support">other support options</a> available.', array('@help' => url('admin/help'), '@handbook' => 'http://drupal.org/handbooks', '@forum' => 'http://drupal.org/forum', '@support' => 'http://drupal.org/support')) .'</p>';

    $output = '<div id="first-time">'. $default_message .'</div>';

    return $output;

  }
  
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
                upgrades to be managed, as this is accomplished by moving the site a platform
                hosting an updated release.
                Platforms are most commonly built for specific releases of Drupal.");
      break;
    case 'client' :
      return t("<strong>The person or group that runs the site.</strong> 
                This information is usually required for billing and access purposes, to assure
                that only certain people are able to view the information for sites they run. 
                If you do not intend on having more than one client access the system, 
                you will not need to create any additional clients for your purposes.");
      break;
    case 'web_server' :
      return t("<strong>The physical machine where files will be stored for publication.</strong>
                Each web server hosts one or more platforms, which act as publishing points for the hosted sites.
                If you are not intending to use Hostmaster in a distributed fashion, you will not need to create
                additional web servers for your purposes..");
      break;
    case 'db_server' :
      return t("<strong>The database server on which sites will host their date.</strong>
                Most web servers and database servers are on the same machine, but for performance reasons 
                external database servers might be required. It is not uncommon for one database server
                to be shared amongst all site instances.
                If you are not intending to use an external database server, or multiple database servers, you
                will not need to create any additional database servers for your purposes.");
      break;
    case 'task' :
      return t("<strong>The mechanism whereby Hostmaster keeps track of all changes that occurr to the system.</strong>
                Each task acts as a command for the back end, and contains a full log of all changes that have occurred.
                If an task should fail, the administrator will be notified with an explanation of exactly what went wrong,
                and how to fix it.");
      break;      
  }
}

