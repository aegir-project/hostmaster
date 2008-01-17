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
    'provision_drupal',
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
   
   
   views_invalidate_cache();
   #initial configuration of hostmaster - todo

}
