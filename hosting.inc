<?php
/**
 * @file Some api functions that don't really feature in the main code flow / integration
 */

function hosting_map_values($node) {
  if (is_numeric($node)) {
    $node = node_load($node);
  }
  
  $values = array();

  $func = 'hosting_map_values_' . $node->type;
  if (function_exists($func)) {
    $values = $func($node);      
  }

  return $values;
}

function hosting_map_values_action($node) {
  $values['action_id'] = $node->nid;
  $values['action_type'] = $node->field_action_type[0]['value'];

  $reference = node_load($node->field_reference[0]['nid']);
  $values['action_reference_type'] = $reference->type;
  $values['action_reference'] = $reference->nid;
  $values = array_merge($values, hosting_map_values($reference));    

  return $values;
}

function hosting_map_values_site($node) {
  $values['site_id'] = $node->nid;
  $values['site_url'] = $node->title;

  # load platform details - takes care of path information
  $values['site_platform'] = $node->field_platform[0]['nid'];
  $values = array_merge($values, hosting_map_values($node->field_platform[0]['nid']));
  $values['site_document_root'] = $values['publish_path'];
    
  # @todo : Load all client details. Probably for initial email
  $values['site_client_id'] = $node->field_client[0]['nid'];
  $values = array_merge($values, hosting_map_values($node->field_client[0]['nid']));
    
  # load database server details
  $values['site_db_server'] = $node->field_db_server[0]['nid'];
  $values = array_merge($values, hosting_map_values($node->field_db_server[0]['nid']));

  # @todo : load profile / release details
  return $values;
}

function hosting_map_values_platform($node) {
  $values['publish_path'] = $node->field_publish_path[0]['value'];

  # @TODO: get web server details. for distributed operation.
  return $values;
}

function hosting_map_values_db_server($node) {
  $values['site_db_host'] = $node->title;
  if (isset($node->field_ip_address[0]['value'])) {
    $values['site_db_ip_address'] = $node->field_ip_address[0]['value'];
  }
  $values['provision_db_username'] = $node->field_master_username[0]['value'];
  $values['provision_db_password'] = $node->field_master_password[0]['value'];
  
  return $values;
}

function hosting_add_action($nid, $type, $args = null) {
 global $user;

 $node = node_load($nid);
 $action = new stdClass();
 $action->type = 'action';
 $action->uid = $user->uid;
 $action->status = 1;
 $action->title = t("!type !title", array('!type' => $type, '!title' => $node->title) );
 $action->field_action_type[0]['value'] = $type;
 $action->field_reference[0]['nid'] = $node->nid;
 node_save($action);

 $queue = _hosting_get_queue();
 $subqueue = _hosting_get_subqueue();
 nodequeue_subqueue_add($queue, $subqueue, $action->nid);
}



function hosting_action_log($vid, $type, $message, $severity = 0, $timestamp = null ) {
 $timestamp = ($timestamp) ? $timestamp : mktime();
 db_query("INSERT INTO {action_log} (vid, type, message, severity, timestamp) VALUES (%d, '%s', '%s', %d, %d)", $vid, $type, $message, $severity, $timestamp);
}


/**
* Moved from inline in the field settings, to allow for simpler maintenance
*/
function _hosting_default_path() {
 return array(0 => array('nid' => ereg_replace("/webroot$", "", $_SERVER['DOCUMENT_ROOT'])));
}

function _hosting_default_client() {
 return array( 0 => array('nid' => variable_get('hosting_default_client', 1)),);
}

function _hosting_default_db_server() {
 return array( 0 => array( 'nid' => variable_get('hosting_default_db_server', 2) ),);
}

function _hosting_default_web_server() {
 return array( 0 => array( 'nid' => variable_get('hosting_default_web_server', 3) ),);
}

function _hosting_default_platform() {
 return array( 0 => array( 'nid' => variable_get('hosting_default_platform', 4) ),);
}

function _hosting_get_queue() {
 return nodequeue_load( variable_get('hosting_action_queue', 1));
}

function _hosting_get_subqueue() {
 return nodequeue_load_subqueue(variable_get('hosting_action_subqueue', 1));
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

function _hosting_parse_error_code($code) {
  global $provision_errors;
  if ($code) {
    foreach ($provision_errors as $error => $message) {
      if ($error != PROVISION_SUCCESS) {
        if ($code & $error) {
          $return[$error] = $message;
        }        
      }
    }    
  }
  else {
    $return[PROVISION_SUCCESS] = $provision_errors[PROVISION_SUCCESS];
  }
  return $return;
}