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
  $ip_list = isset($node->ip_addresses) ? _hosting_ip_list($node->ip_addresses) : array();

  if (sizeof($ip_list)) {
    $node->content['info']['ip_addresses'] = array(
      '#type' => 'item',
      '#title' => t('IP addresses'),
      '#markup' => implode('<br />', $ip_list),
    );
  }
}

/**
 * IP Utility function for hook_update/hook_insert.
 */
function hosting_ip_save($node, $update = FALSE) {
  $ip_list = _hosting_ip_list($node->ip_addresses);

  if ($update) {
    hosting_ip_delete($node);
  }

  foreach ($ip_list as $ip) {
    $id = db_insert('hosting_ip_addresses')
      ->fields(array(
        'nid' => $node->nid,
        'ip_address' => $ip,
      ))
      ->execute();
  }
}

/**
 * Validate that IP list is valid. IP utility function for hook_validate.
 */
function hosting_ip_validate($node) {
  foreach (_hosting_ip_list($node->ip_addresses) as $ip) {
    if (!_hosting_valid_ip($ip)) {
      form_set_error('ip_addresses', t('Invalid IP address: %ip.', array(
        '%ip' => $ip,
      )));
    }
  }
}


/**
 * IP Utility function for hook_load.
 */
function hosting_ip_load($node) {
  $ip_list = array();
  $result = db_query("SELECT ip_address FROM {hosting_ip_addresses} WHERE nid = :nid", array(':nid' => $node->nid));
  while ($obj = $result->fetch()) {
    $ip_list[] = $obj->ip_address;
  }

  return $ip_list;
}

/**
 * IP Utility function for hook_delete.
 */
function hosting_ip_delete($node) {
  db_delete('hosting_ip_addresses')
    ->condition('nid', $node->nid)
    ->execute();
}

/**
 * IP Utility function for hook_delete_revision.
 *
 * @deprecated
 * @see hosting_ip_delete()
 */
function hosting_ip_delete_revision($node) {
  return hosting_ip_delete($node);
}

/**
 * Allocate an IP for a given site on a given server.
 */
function hosting_ip_allocate($cert, $site) {
  // make sure the IP is not allocated while we pick ours
  db_query("LOCK TABLES {hosting_ssl_cert_ips} WRITE, {hosting_ip_addresses} WRITE");
  $platform = node_load($site->platform);
  $server = node_load($platform->web_server);
  // guess the next available IP
  $ip = db_query("SELECT hosting_ip_addresses.id FROM {hosting_ip_addresses}
                             LEFT JOIN {hosting_ssl_cert_ips} ON hosting_ip_addresses.id = hosting_ssl_cert_ips.ip_address
                             WHERE hosting_ssl_cert_ips.ip_address IS NULL AND nid = :nid LIMIT 1;", array(':nid' => $server->nid))->fetchField();
  if ($ip) {
    $id = db_insert('hosting_ssl_cert_ips')
      ->fields(array(
        'cid' => $cert->cid,
        'ip_address' => $ip,
      ))
      ->execute();
  }
  db_query("UNLOCK TABLES");
  return $ip;
}
