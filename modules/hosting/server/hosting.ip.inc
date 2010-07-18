<?php
// $Id$ 
/**
 * @file General IP address CRUD routines.
 */

/**
 * Get an array from the text ip address list.
 */
function _hosting_ip_list($addresses) {
  $return = array();
  $ip_list = is_array($addresses) ? $addresses : explode("\n", $addresses);
  
  foreach ($ip_list as $key => $ip) {
    $ip = trim($ip);
    if (!empty($ip)) {
      $return[$key] = $ip;
    }
  }

  return $return;
}

/**
 * Display the ip address on the node. IP utility function for hook_view.
 */
function hosting_ip_view(&$node) {
  $ip_list = _hosting_ip_list($node->ip_addresses);

  if (sizeof($ip_list)) {
    $node->content['info']['ip_addresses'] = array(
      '#type' => 'item',
      '#title' => t('IP addresses'),
      '#value' => implode('<br />', $ip_list),
    );  
  }
}

/**
 * IP Utility function for hook_update/hook_insert.
 */
function hosting_ip_save($node, $update = FALSE) {
  $ip_list = _hosting_ip_list($node->ip_addresses);

  if ($update) {
    hosting_ip_delete_revision($node);
  }

  foreach ($ip_list as $ip) {
    db_query("INSERT INTO {hosting_ip_addresses} (vid, nid, ip_address) VALUES (%d, %d, '%s')", $node->vid, $node->nid, $ip); 
  }
}

/**
 * Validate that IP list is valid. IP utility function for hook_validate.
 */
function hosting_ip_validate($node) { 
  foreach (_hosting_ip_list($node->ip_addresses) as $ip) {
    if (!_hosting_valid_ip($ip)) {
      form_set_error('ip_addresses', t('Invalid IP address: %ip.', array(
        '%ip' => $ip )));
    }
  }
}


/**
 * IP Utility function for hook_load.
 */
function hosting_ip_load($node) {
  $ip_list = array();
  $result = db_query("SELECT ip_address FROM {hosting_ip_addresses} WHERE vid=%d", $node->vid);
  while ($obj = db_fetch_object($result)) {
    $ip_list[] = $obj->ip_address;
  }

  return $ip_list;
}

/**
 * IP Utility function for hook_delete.
 */
function hosting_ip_delete($node) {
  db_query("DELETE FROM {hosting_ip_addresses} WHERE nid=%d", $node->nid);
}

/**
 * IP Utility function for hook_delete_revision.
 */
function hosting_ip_delete_revision($node) {
  db_query("DELETE FROM {hosting_ip_addresses} WHERE vid=%d", $node->vid);
}

