<?php



function hosting_map_values_db_server($node) {
  $values['site_db_host'] = $node->title;
  if (isset($node->field_ip_address[0]['value'])) {
    $values['site_db_ip_address'] = $node->field_ip_address[0]['value'];
  }
  $values['provision_db_username'] = $node->field_master_username[0]['value'];
  $values['provision_db_password'] = $node->field_master_password[0]['value'];
  
  return $values;
}


/**
 * Create the configuration file containing the master database credentials
 *
 * Outside of web root. not readable by web server. _important_
 */
function _hosting_generate_database_config_file($node) {
  if ($node->type != "web_server") {
    return null;
  }
  $path = _provision_config_path();
  
}