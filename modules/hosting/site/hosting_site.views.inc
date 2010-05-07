<?php
/**
 * @file Hosting site views integration.
 */

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function hosting_site_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_site'),
      ),
    'handlers' => array(
      /* add handlers */
    ),
  );
}

/** 
 * Implements hook_views_data().
 */
function hosting_site_views_data() {
  $data['hosting_site'] = array(
    'table' => array(
      'group' => 'Hosting Site',
      'title' => 'Site',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'client' => array(
      'title' => t('Client ID'),
      'help' => t('The ID of the client for this site.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'db_server' => array(
      'title' => t('DB Server ID'),
      'help' => t('The database server id.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'platform' => array(
      'title' => t('Platform ID'),
      'help' => t('The ID of the platform this site is installed on.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'profile' => array(
      'title' => t('Profile ID'),
      'help' => t('The ID of the profile package installed on this site.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'language' => array(
      'title' => t('Language'),
      'help' => t('The default language of this site.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'last_cron' => array(
      'title' => t('Last Cron Run'),
      'help' => t('The time the last cron run was executed on this site.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
    ),
    'verified' => array(
      'title' => t('Verified'),
      'help' => t('The last date verified task run on this site.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
    ),
    'status' => array(
      'title' => t('Status'),
      'help' => t('The current state of this site.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
    ),
    'port' => array(
      'title' => t('Port'),
      'help' => t('The HTTP port the site is running on.'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
    ),
    'ssl' => array(
      'title' => t('SSL'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'ssl_redirect' => array(
      'title' => t('SSL Redirect'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
  );

  return $data;
}


