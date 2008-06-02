<?php


function hosting_site_views_tables() {
  $tables['hosting_site'] = array(
    'name' => 'hosting_site',
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
      'client' => array(
          'name' => t('Site: Client'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The client the site belongs to.'),
        ),
      'platform' => array(
          'name' => t('Site: Platform'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The platform the site is hosted on.'),
        ),
      'db_server' => array(
          'name' => t('Site: Database Server'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The database server the site is hosted on.'),
        ),
      ),
    );
  return $tables;
}

/**
 * Views 1.x argument handling for site nodes.
 */
function hosting_site_views_arguments() {
  $arguments['hosting_site_client'] = array(
    'name' => t('Site: Client'),
    'handler' => 'hosting_site_client_argument',
  );
  $arguments['hosting_site_platform'] = array(
    'name' => t('Site: Platform'),
    'help' => t('Filter sites by the platform they are hosted on.'),
    'handler' => 'hosting_site_platform_argument',
  );
  $arguments['hosting_site_db_server'] = array(
    'name' => t('Site: Database Server'),
    'help' => t('Filter sites by the database server they are hosted on.'),
    'handler' => 'hosting_site_db_server_argument',
  );

  return $arguments;
}

/**
 * Views handler to display links instead of node id's
 */
function hosting_relation_link_handler($fieldinfo, $fielddata, $value, $data) {
  return _hosting_node_link($value);
}

