<?php
/**
 * @file Client views integration.
 */

function hosting_client_views_data() {
  $data['hosting_client'] = array(
    'table' => array(
      'group' => 'Hosting Client',
      'title' => 'Client',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'name' => array(
      'title' => t('Name'),
      'help' => t('The name of the client.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'organization' => array(
      'title' => t('Organization'),
      'help' => t('The name of the client organization.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'email' => array(
      'title' => t('Email'),
      'help' => t('The email contact address for this client.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
  );

  return $data;
}
