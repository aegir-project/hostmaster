<?php


/**
 * Small helper function to get web servers.
*/ 
function _hosting_get_db_servers() {
  $return = array();
  $result = db_query("SELECT nid, title FROM {node} WHERE type='db_server' AND status=1");
  while($server = db_fetch_object($result)) {
    $return[$server->nid] = $server->title;
  }
  return $return;
}


function hosting_map_values_db_server($node) {
  $values['site_db_host'] = $node->title;
  if (isset($node->ip_address)) {
    $values['site_db_ip_address'] = $node->ip_address;
  }
  
  $values['provision_db_username'] = $node->db_user;
  $values['provision_db_password'] = $node->db_passwd;
  
  return $values;
}


/**
 * Implementation of hook_form().
 */
function hosting_db_server_form(&$node) {
  $type = node_get_types('type', $node);

  // We need to define form elements for the node's title and body.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => check_plain($type->title_label),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#weight' => -5
  );
  // merge the web server configuration.
  $form += provision_apache_provision_configure($node);
  return $form;
}

/**
 * Implementation of hook_validate().
  */
function hosting_db_server_validate(&$node) {
  # TODO : check paths. kind of tricky actually, due to multiple servers etc.
}

/**
 * Implementation of hook_insert().
 */
function hosting_db_server_insert($node) {
  db_query("INSERT INTO {db_server} (vid, nid, ip_address, script_user, web_group, config_path, backup_path) 
      VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s')", 
        $node->vid, $node->nid, $node->ip_address, $node->script_user, $node->web_group, $node->config_path, $node->backup_path);
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_db_server_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_db_server_insert($node);
  }
  else {
    db_query("UPDATE {hosting_db_server} SET 
                  ip_address = '%s', script_user = '%s', web_group = '%s', 
                  config_path = '%s', backup_path = '%s' 
              WHERE 
                  vid = %d", 
                  $node->ip_address, $node->script_user, $node->web_group,
                  $node->config_path, $node->backup_path, 
                  $node->vid);
  }
}

function hosting_nodeapi_db_server_delete_revision(&$node) {
  db_query('DELETE FROM {hosting_db_server} WHERE vid = %d', $node->vid);
}

/**
 * Implementation of hook_delete().
 */
function hosting_db_server_delete($node) {
  db_query('DELETE FROM {hosting_db_server} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 */
function hosting_db_server_load($node) {
  $additions = db_fetch_object(db_query('SELECT ip_address, script_user, web_group, config_path, backup_path FROM {hosting_db_server} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_db_server_view($node, $teaser = FALSE, $page = FALSE) {
  $node = node_prepare($node, $teaser);
  if ($node->ip_address) {
    $node->content['ip_address'] = array(
      '#type' => 'item',
      '#title' => t('IP address'),
      '#value' => $node->ip_address,
    );    
  }
  $node->content['script_user'] = array(
    '#type' => 'item',
    '#title' => t('Script user'),
    '#value' => $node->script_user,
  );
  $node->content['web_group'] = array(
    '#type' => 'item',
    '#title' => t('Web server group'),
    '#value' => $node->web_group,
  );
  $node->content['config_path'] = array(
    '#type' => 'item',
    '#title' => t('Configuration path'),
    '#value' => $node->config_path,
  );
  $node->content['backup_path'] = array(
    '#type' => 'item',
    '#title' => t('Backup path'),
    '#value' => $node->backup_path,
  );
  
  return $node;
}

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
    'fields' => array(
      'ip_address' => array(
          'name' => t('Server: IP address'),
          'sortable' => TRUE,
          'option' => 'string',
          'help' => t('Display the ip address of the server.'),
        ),
      ),
    );
}






/**
 * Create the configuration file containing the master database credentials
 *
 * Outside of web root. not readable by web server. _important_
 */
function _hosting_generate_database_config_file($node) {
  if ($node->type != "db_server") {
    return null;
  }
  $path = _provision_config_path();
  
}