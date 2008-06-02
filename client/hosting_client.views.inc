<?php

function hosting_client_views_tables() {
  return array(
    'name' => 'hosting_client',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid',
      ),
      'right' => array(
        'field' => 'vid',
      ),
    ),
    'fields' => array(
      'name' => array(
        'name' => t('Client: Contact name'),
        'sortable' => TRUE,
        'option' => 'string',
      ),
      'organization' => array(
        'name' => t('Client: Organization'),
        'sortable' => TRUE,
        'option' => 'string',
      ),
      'email' => array(
        'name' => t('Client: Email'),
        'sortable' => TRUE,
        'option' => 'string',
      ),   
    ),
  );
}
