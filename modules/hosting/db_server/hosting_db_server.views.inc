<?php
/**
 * @file Views integration for the DB Servers.
 */

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function hosting_db_server_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_db_server'),
      ),
    'handlers' => array(
      'hosting_db_server_handler_filter_db_server_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/** 
 * Implements hook_views_data().
 */

function hosting_db_server_views_data() {
  $data['hosting_db_server'] = array(
    'table' => array(
      'group' => 'Hosting DB Sever',
      'title' => 'Database Sever',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'db_type' => array(
      'title' => t('Type'),
      'help' => t('The name of the database server.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'hosting_db_server_handler_filter_db_server_type',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'db_user' => array(
      'title' => t('Username'),
      'help' => t('The username for the database.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'db_passwd' => array(
      'title' => t('Password'),
      'help' => t('The password for the database.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
  );

  return $data;
}
