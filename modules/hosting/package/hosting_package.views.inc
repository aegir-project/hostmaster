<?php
/**
 * @file Hosting package views integration.
 */

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function hosting_package_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_package'),
      ),
    'handlers' => array(
      'hosting_package_handler_filter_hosting_package_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/** 
 * Implements hook_views_data().
 */
function hosting_package_views_data() {
  $data['hosting_package'] = array(
    'table' => array(
      'group' => 'Hosting Package',
      'title' => 'Package',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'package_type' => array(
      'title' => t('Type'),
      'help' => t('The type of the package (e.g. module, theme, profile).'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'hosting_package_handler_filter_hosting_package_type',
      ),
    ),
    'short_name' => array(
      'title' => t('Name'),
      'help' => t('The name used .'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'description' => array(
      'title' => t('Description'),
      'help' => t('The description information for this package.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
  );

  return $data;
}


