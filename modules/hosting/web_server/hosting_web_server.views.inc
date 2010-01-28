<?php
/**
 * @file Hosting web_server views integration.
 */

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function hosting_web_server_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_web_server'),
      ),
    'handlers' => array(
      'hosting_web_server_handler_filter_hosting_web_server_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/** 
 * Implements hook_views_data().
 */
function hosting_web_server_views_data() {
  $data['hosting_web_server'] = array(
    'table' => array(
      'group' => 'Hosting Web Sever',
      'title' => 'Package',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'ip_address' => array(
      'title' => t('IP Address'),
      'help' => t('IP address of the webserver.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'script_user' => array(
      'title' => t('Script User'),
      'help' => t('User.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'web_group' => array(
      'title' => t('Web group'),
      'help' => t('The group.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'config_path' => array(
      'title' => t('Config Path'),
      'help' => t('The path where configuration files are saved.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'backup_path' => array(
      'title' => t('Backup Path'),
      'help' => t('The path where backup files are saved.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'restart_cmd' => array(
      'title' => t('Restart Command'),
      'help' => t('Command use to restart the webserver.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'drush_path' => array(
      'title' => t('Drush Path'),
      'help' => t('The path to drush on this server.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'ports' => array(
      'title' => t('Ports'),
      'help' => t('.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
  );
  return $data;
}


