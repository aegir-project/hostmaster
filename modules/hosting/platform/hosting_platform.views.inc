<?php
/**
 * @file Hosting platform views integration.
 */

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function hosting_platform_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_platform'),
      ),
    'handlers' => array(
      'hosting_platform_handler_filter_hosting_platform_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/** 
 * Implements hook_views_data().
 */
function hosting_platform_views_data() {
  $data['hosting_platform'] = array(
    'table' => array(
      'group' => 'Hosting Platform',
      'title' => 'Platform',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'publish_path' => array(
      'title' => t('Publish Path'),
      'help' => t('The path on the server where this platform is installed.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'web_server' => array(
      'title' => t('Web Server'),
      'help' => t('The webserver this platform is installed on.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'release_id' => array(
      'title' => t('Release ID'),
      'help' => t('The release ID.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'verified' => array(
      'title' => t('Verfied Date'),
      'help' => t('The description information for this platform.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
    ),
    'status' => array(
      'title' => t('Status'),
      'help' => t('The descri.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
  );

  return $data;
}


