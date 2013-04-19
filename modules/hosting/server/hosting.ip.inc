<?php
// $Id$ 
/**
 * @file General IP address CRUD routines.
 */

/**
 * Display the ip address on the node. IP utility function for hook_view.
 */
function hosting_ip_view(&$node) {
  $ip_list = isset($node->ip_addresses) ? $node->ip_addresses : array();

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
  $ips = is_array($node->ip_addresses) ? $node->ip_addresses : array();

  foreach ($ips as $id => $ip) {
    $ip = trim($ip);
    if (empty($ip)) {
      // empty fields are considered removed
      db_query("DELETE FROM {hosting_ip_addresses} WHERE id=%d", $id);
    } else {
      db_query("UPDATE {hosting_ip_addresses} SET ip_address = '%s' WHERE id=%d", $ip, $id);
    }
  }

  $ips = is_array($node->new_ip_addresses) ? $node->ip_addresses : array();
  foreach ($ips as $id => $ip) {
    if (!empty($ip)) {
      // new entries are prefixed with the string 'new', insert those
      db_query("INSERT INTO {hosting_ip_addresses} (nid, ip_address) VALUES (%d, '%s')", $node->nid, $ip);
    }
  }
}

/**
 * Validate that IP list is valid. IP utility function for hook_validate.
 */
function hosting_ip_validate($node) { 
  foreach ($node->ip_addresses as $id => $ip) {
    $ip = trim($ip);
    if (empty($ip)) {
      // deletion, look if the IP is in use
      if ($cid = db_result(db_query('SELECT ssl_key FROM {hosting_ssl_cert_ips} i INNER JOIN {hosting_ssl_cert} c ON c.cid = i.cid WHERE ip_address = %d', $id))) {
        form_set_error("ip_addresses][$id", t('Cannot remove IP associated with certificate %cert.', array('%cert' => $cid)));
      }
    } elseif (!_hosting_valid_ip($ip)) {
      form_set_error("ip_addresses][$id", t('Invalid IP address: %ip.', array(
        '%ip' => $ip )));
    }
  }
  foreach ($node->new_ip_addresses as $id => $ip) {
    if (!empty($ip) && !_hosting_valid_ip($ip)) {
      form_set_error("ip_addresses][$id", t('Invalid IP address: %ip.', array(
        '%ip' => $ip )));
    }
  }
}


/**
 * IP Utility function for hook_load.
 */
function hosting_ip_load($node) {
  $ip_list = array();
  $result = db_query("SELECT id,ip_address FROM {hosting_ip_addresses} WHERE nid=%d ORDER BY id", $node->nid);
  while ($obj = db_fetch_object($result)) {
    $ip_list[$obj->id] = $obj->ip_address;
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
  $ip = db_result(db_query("SELECT hosting_ip_addresses.id FROM {hosting_ip_addresses}
                             LEFT JOIN {hosting_ssl_cert_ips} ON hosting_ip_addresses.id = hosting_ssl_cert_ips.ip_address
                             WHERE hosting_ssl_cert_ips.ip_address IS NULL AND nid = %d LIMIT 1;", $server->nid));
  if ($ip) {
    db_query("INSERT INTO {hosting_ssl_cert_ips} (cid, ip_address) VALUES (%d, %d)", $cert->cid, $ip);
  }
  db_query("UNLOCK TABLES");
  return $ip;
}
