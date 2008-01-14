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
    'auto_nodetitle',
    'block',
    'cck_extras',
    'cck_field_perms',
    'cck_fullname',
    'color',
    'comment',
    'content', 
    'content_copy',
    'cvs_deploy',
    'date',
    'date_api',
    'drush',
    'drush_pm', 
    'drush_tools',
    'email', 
    'filter',
    'help', 
    'menu',
    'node',
    'nodequeue',
    'nodereference',
    'number', 
    'optionwidgets',
    'provision',
    'provision_apache',
    'provision_mysql',
    'system',
    'taxonomy',
    'text',
    'token',
    'update_status',
    'user',
    'userreference', 
    'views',
    'views_ui',
    'watchdog', 
    'hosting',
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
    'name' => 'Hosting',
    'description' => 'Select this profile to manage the installation and maintenance of hosted Drupal sites.'
  );
}

/**
 * Perform any final installation tasks for this profile.
 *
 * @return
 *   An optional HTML string to display to the user on the final installation
 *   screen.
 */
function hostmaster_profile_final() {

  /* Default node types and default node */
  node_types_rebuild();

  /* Default client */
  $node = array('type' => 'client');
  $values = array();
  $values['title'] = 'Default client';
  $values['field_organization'][0]['value'] = 'Default';
  $values['field_email'][0]['value'] = 'changeme@example.com';
  $values['field_name'][0]['first'] = 'Default';
  $values['field_name'][0]['last'] = 'Client';
  $values['status'] = 1;
  $messages = drupal_execute('client_node_form', $values, $node);
  variable_set('hosting_default_client', 1);  

  /* Default database server */
  $node = array('type' => 'db_server');
  $values = array();
  $values['title'] = 'localhost';
  $values['field_ip'][0]['value'] = '127.0.0.1';
#  $values['field_db_type']['key'] = 'mysql';
  $values['field_master_username'][0]['value'] = 'root';
  $values['field_master_password'][0]['value'] = 'password';
  $values['status'] = 1;
  $messages = drupal_execute('db_server_node_form', $values, $node);
  variable_set('hosting_default_db_server', 2);

  $node = array('type' => 'web_server');
  $values = array();
  $values['title'] = 'localhost';
  $values['field_user'][0]['value'] = 'hosting';
  $values['field_group'][0]['value'] = 'apache';
  $values['status'] = 1;
  drupal_execute('web_server_node_form', $values, $node);
  variable_set('hosting_default_web_server', 3);

  /* Default release */
  $node = array('type' => 'release');
  $values = array();
  $values['title'] = 'Default release';
  $docroot = ($_SERVER['DOCUMENT_ROOT']) ? $_SERVER['DOCUMENT_ROOT'] : $_SERVER['PWD'];
  $values['field_path'][0]['value'] = ereg_replace("/webroot$", "", $docroot);
  $values['field_web_server'][0]['nid'] = variable_get('hosting_default_web_server', 3);
  $values['status'] = 1;
  drupal_execute('release_node_form', $values, $node);
  variable_set('hosting_default_release', 4);  
  
  # Action queue
  $queue = (object) array(
     'qid' => '1',
     'title' => 'Hosting queue',
     'size' => '0',
     'link' => '',
     'link_remove' => '',
     'owner' => 'nodequeue',
     'show_in_ui' => '1',
     'show_in_tab' => '1',
     'show_in_links' => '0',
     'reference' => '0',
     'subqueue_title' => '',
     'reverse' => '0',
     'subqueues' => '1',
     'types' => 
        array (
          0 => 'action',
        ),
    );
   $queue->add_subqueue = array($queue->title);
   nodequeue_save($queue);
   variable_set('hosting_action_queue', $queue->qid);
   $subqueue = nodequeue_load_subqueues_by_queue($queue->qid);
   variable_set('hosting_action_subqueue', $subqueue->qid);
   
   #initial configuration of hostmaster - todo
  variable_set('site_front_page', 'hosting/help'); 
}


function hostmaster_install_help() {
/**
*@TODO documentation
*/  
  $default_message = t('<p>Please follow these steps to set up and start using your website:</p>');
  $default_message .= '<ol>';

  if (!$admin) {
    $default_message .= '<li>'. t('<strong>Create your administrator account</strong> To begin, <a href="@register">create the first account</a>. This account will have full administration rights and will allow you to configure your website.', array('@register' => url('user/register'))) .'</li>';
  }
  $default_message .= '<li>'. t('<strong>Configure your website</strong> Once logged in, visit the <a href="@admin">administration section</a>, where you can <a href="@config">customize and configure</a> all aspects of your website.', array('@admin' => url('admin'), '@config' => url('admin/settings'))) .'</li>';
  $default_message .= '<li>'. t('<strong>Enable additional functionality</strong> Next, visit the <a href="@modules">module list</a> and enable features which suit your specific needs. You can find additional modules in the <a href="@download_modules">Drupal modules download section</a>.', array('@modules' => url('admin/build/modules'), '@download_modules' => 'http://drupal.org/project/Modules')) .'</li>';
  $default_message .= '<li>'. t('<strong>Customize your website design</strong> To change the "look and feel" of your website, visit the <a href="@themes">themes section</a>. You may choose from one of the included themes or download additional themes from the <a href="@download_themes">Drupal themes download section</a>.', array('@themes' => url('admin/build/themes'), '@download_themes' => 'http://drupal.org/project/Themes')) .'</li>';
  $default_message .= '<li>'. t('<strong>Start posting content</strong> Finally, you can <a href="@content">create content</a> for your website. This message will disappear once you have promoted a post to the front page.', array('@content' => url('node/add'))) .'</li>';
  $default_message .= '</ol>';
  $default_message .= '<p>'. t('For more information, please refer to the <a href="@help">help section</a>, or the <a href="@handbook">online Drupal handbooks</a>. You may also post at the <a href="@forum">Drupal forum</a>, or view the wide range of <a href="@support">other support options</a> available.', array('@help' => url('admin/help'), '@handbook' => 'http://drupal.org/handbooks', '@forum' => 'http://drupal.org/forum', '@support' => 'http://drupal.org/support')) .'</p>';

  return $default_message;
}