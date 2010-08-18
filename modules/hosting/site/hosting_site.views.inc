<?php
/**
 * @file Hosting site views integration.
 */



/** 
 * Implements hook_views_data().
 */
function hosting_site_views_data() {
  $data['hosting_site']['table'] = array(
      'group' => 'Hosting Site',
      'title' => 'Site',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    );

  $data['hosting_site']['client'] = array(
   'title' => t('Client'),
   'help' => t('Relate a node revision to the user who created the revision.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('client'),
    ),
  );

  $data['hosting_site']['db_server'] = array(
   'title' => t('Database Server'),
   'help' => t('Database where the site is installed.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('db server'),
    ),
  );

 $data['hosting_site']['profile'] = array(
   'title' => t('Install Profile'),
   'help' => t('Type of drupal site.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('profile'),
    ),
  );
 $data['hosting_site']['platform'] = array(
   'title' => t('Platform'),
   'help' => t('Platform'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('platform'),
    ),
  );

  $data['hosting_site']['verified'] = array(
      'title' => t('Verified'),
      'help' => t('The last date verified task run on this site.'),
      'field' => array(
        'handler' => 'hosting_views_field_handler_interval',
        'click sortable' => TRUE,
      ),
    );

  $data['hosting_site']['last_cron'] = array(
      'title' => t('Last Cron Run'),
      'help' => t('The time the last cron run was executed on this site.'),
      'field' => array(
        'handler' => 'hosting_views_field_handler_interval',
        'click sortable' => TRUE,
      ),
    );
  $data['hosting_site']['language'] = array(
      'title' => t('Language'),
      'help' => t('The default language of this site.'),
      'field' => array(
        'handler' => 'views_handler_field_hosting_language',
        'click sortable' => TRUE,
      ),
    );

  $data['hosting_site']['status'] = array(
      'title' => t('Status'),
      'help' => t('The current state of this site.'),
      'field' => array(
        'handler' => 'views_handler_field_hosting_site_status',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
    );

  return $data;
}

function hosting_site_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_site'),
    ),
    'handlers' => array(
      // field handlers
      'views_handler_field_hosting_language' => array(
        'parent' => 'views_handler_field',
      ),
      // field handlers
      'views_handler_field_hosting_site_status' => array(
        'parent' => 'views_handler_field',
      ),

    ),
  );
}

function hosting_site_views_plugins() {
  return array(
    'style' => array(
      'hosting_site_list' => array(
        'title' => t('Site listing'),
        'help' => t('Displays sites in a table.'),
        'handler' => 'views_plugin_style_site_table',
        'theme' => 'views_view_table',
        'parent' => 'table',
        'path' => drupal_get_path('module', 'hosting_site'),
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    )
  );

}



