<?php

/**
 * @file
 *   Views integration for the Hosting module.
 */

/**
 * Implementation of hook_views_handlers().
 */
function hosting_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting'),
    ),
    'handlers' => array(
      // field handlers
      'hosting_views_field_handler_interval' => array(

        'parent' => 'views_handler_field',
      ),
    ),
  );
}


