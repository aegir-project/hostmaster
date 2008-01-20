<?php
// $Id$

include_once('profiles/hostmaster/modules/install_profile_api/crud.inc');

/**
 * Return an array of the modules to be enabled when this profile is installed.
 *
 * @return
 *  An array of modules to be enabled.
 */
function hostmaster_profile_modules() {
  return array(
    /* core */ 'block', 'color', 'filter', 'help', 'menu', 'node', 'system', 'user', 'watchdog',
    /* contrib */ 'auto_nodetitle', 'drush', 'nodequeue', 'token', 'update_status', 'views', 'views_ui',
    /* cck */ 'content', 'cck_fullname', 'date', 'date_api', 'email', 'nodereference', 'number', 'optionwidgets', 'password', 'text',
    /* custom */ 'provision', 'provision_apache', 'provision_mysql', 'provision_drupal', 'hosting');
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
  $values['title'] = $_SERVER['HTTP_HOST'];
  $values['field_user'][0]['value'] = provision_get_script_owner();
  $values['field_group'][0]['value'] = provision_get_group_name();
  $values['status'] = 1;
  drupal_execute('web_server_node_form', $values, $node);
  variable_set('hosting_default_web_server', 3);

  /* Default platform */
  $node = array('type' => 'platform');
  $values = array();
  $values['title'] = "Drupal " . VERSION . ' on '. $_SERVER['HTTP_HOST'];
  $values['field_publish_path'][0]['value'] = $_SERVER['DOCUMENT_ROOT'];
  $values['field_web_server'][0]['nid'] = variable_get('hosting_default_web_server', 3);
  $values['status'] = 1;
  drupal_execute('platform_node_form', $values, $node);
  variable_set('hosting_default_platform', 4);  
  
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
   
   
   install_add_block("views", "platforms", "garland", 1, 0, "right");
   install_add_block("views", "servers", "garland", 1, 0, "right");
   
   
   #initial configuration of hostmaster - todo
   
   variable_set('site_frontpage', 'sites');
   
   // @todo create proper roles, and set up views to be role based
   install_set_permissions(install_get_rid('anonymous user'), array('access content', 'access all views'));
   install_set_permissions(install_get_rid('authenticated user'), array('access content', 'access all views'));
   views_invalidate_cache();
   menu_rebuild();
}
