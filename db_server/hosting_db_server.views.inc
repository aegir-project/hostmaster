<?php


function hosting_db_server_views_tables() {
  return array(
    'name' => 'hosting_db_server',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid',
      ),
      'right' => array(
        'field' => 'vid',
      ),
    ),
  );
}



