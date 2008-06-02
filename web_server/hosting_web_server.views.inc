<?php


function hosting_web_server_views_tables() {
  $tables['hosting_web_server'] = array(
    'name' => 'hosting_web_server',
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
      'ip_address' => array(
          'name' => t('Server: IP address'),
          'sortable' => TRUE,
          'option' => 'string',
          'help' => t('Display the ip address of the server.'),
        ),
      ),

    );
  return $tables;
}
