<?php


/**
 * Implementation of hook_help().
 *
 * @see _hosting_requirements()
 * @see hosting_help_requirements()
 */
function hosting_web_server_help($path, $arg) {
  switch ($path) {
    case 'admin/help/provision#requirements':
      $output .= _hosting_requirements('drush_path');
      $output .= _hosting_requirements('config_path');
      $output .= _hosting_requirements('httpd_conf');
      $output .= _hosting_requirements('visudo');
      $output .= _hosting_requirements('ports');
      return $output;
      break;
  }
}

function _hosting_config_path_requirements() {
  $username = HOSTING_DEFAULT_SCRIPT_USER;
  $group = HOSTING_DEFAULT_WEB_GROUP;
  $vhost_path = HOSTING_DEFAULT_CONFIG_PATH;
  $mkdir_cmd['@vhost_path'] = $vhost_path;
  $mkdir_cmd['@mkdir_cmd'] = <<<EOF
    mkdir -p $vhost_path
    chown $username:$username $vhost_path
    chmod 0711 $vhost_path
EOF;

  $help['title'] = t('Write access to a directory to store configuration information');
  $help['summary'] = t('The Provision framework takes special care to make sure that the file permissions of the 
                       hosted sites are always as safe as can be, especially to make sure that the web server does 
                       not have the ability to modify the code of the site, therefore this information is required 
                       to assure that safety while keeping the sites accessible. 
                       The recommended path is directly above your platform path, but it can be anywhere.');

  $help['configuration'][] = t('Based on your server configuration we have determined that your path should be <code>@vhost_path</code>.', $mkdir_cmd);
  $help['configuration'][] = t(' please enter the following commands : <pre>@mkdir_cmd</pre>', $mkdir_cmd);

  return $help;
}

function _hosting_drush_path_requirements() {
  $help['title'] = t('Full path to the Drush script on this server.');
  $help['summary'] = t('<a href="http://drupal.org/project/drush">Drush</a> is a command line script that is used to
                       interact with Drupal sites through the unix shell.<br />
                       To be able to use Aegir, you need to download Drush, and place it somewhere on your server
                       that is accessible by the script user (eg: <code>!parent_path/drush/drush.php</code>).<br />
                       Once Drush is installed, you are required to download <a href="http://drupal.org/project/provision">Provision</a>
                       and place it in the .drush folder of the script user\'s home directory.
                       (eg: <code>!parent_path/.drush/provision</code>).<br />', 
                       array('!parent_path' => HOSTING_DEFAULT_PARENT_PATH));

  return $help;
}


function _hosting_httpd_conf_requirements() {
  $vhost_path = HOSTING_DEFAULT_VHOST_PATH;
  $vhost_line = <<<EOF
    Include $vhost_path
EOF;
  $help['title'] = t('Modify the web server\'s configuration file');
  $help['summary'] = t('To allow the system to load additional virtual hosts that are generated,
			you are required to add a line to your Apache configuration.</p>
			If you followed the instructions in the hostmaster INSTALL.txt prior to
			starting this wizard, you will already have placed a file into
			<code>/etc/apache2/conf.d/</code>, in which case you can skip this step.
			If you have not yet done this, or this option is not available due to your 
			Apache configuration differing based on your distribution, put this configuration 
			change in the relevant configuration file.
			The location of this file differs between distributions, but is most commonly
			found in <code>/etc/httpd</code>, <code>/etc/apache</code> or <code>/etc/apache2</code>.
			It may be called <code>apache2.conf</code> or <code>httpd.conf</code>.');

  $help['configuration'] = t('Once you have determined the location of your configuration file, add the following line to it:
                            <pre>@vhost_line</pre>', array('@vhost_line' => $vhost_line));

  return $help;
}

function _hosting_visudo_requirements() {
  $username = HOSTING_DEFAULT_SCRIPT_USER;
  $cmd = trim(str_replace("sudo", '', HOSTING_DEFAULT_RESTART_CMD));
  $cmd = substr($cmd, 0, strpos($cmd, " "));
  $visudo_cmd['@visudo_cmd'] = <<<EOF
    sudo visudo
EOF;
  $visudo_cmd['@visudo_line'] = <<<EOF
    $username ALL=NOPASSWD: $cmd
EOF;

  $help['title'] = t('Permission to restart the web server');
  $help['summary'] = t('As the provisioning framework should not be run as root,
                         and the web server group should not be allowed access to the 
                         functionality to stop/start the web server, it is required that you provide access
                         to the Apache restart command for the user account the script will be running as. 
                         If this is not configured, every command will ask for a sudo password when restarting the server.');
  $help['configuration'] = t('Run the visudo command: <pre>@visudo_cmd</pre>
                             Then add the following line to the file: <pre>@visudo_line</pre>', 
                             $visudo_cmd);
  return $help;

}

function _hosting_ports_requirements() {
   $help['title'] = t('Web server ports');
   $help['summary'] = t('If the web server is to serve sites on ports other than just 80,
                         you may add those other ports here, separated by a comma. Later,
                         during the provisioning of sites, you may choose which port to
                         serve the site on.');
   return $help;
}

