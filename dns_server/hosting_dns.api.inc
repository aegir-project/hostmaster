<?php
// $Id$

/**
 * Query status of a DNS entry (zone or RR)
 *
 * Get info about a domain's SOA record or a particular RR as an object.
 *
 * @param $op
 *   What kind of DNS entry to return. Valid $ops are 'zone' and 'rr'.
 * @param $args
 *   An array of fields to identify the object to return.  This must always contain
 *   either a 'zid' or 'domain' key to identify the zone being queried. For RR queries,
 *   $args must also contain either a 'rid' key OR a 'name' + 'type' key to identify the RR.
 *
 * @return
 *   An object representing the zone or Resource Record, depending on $op.
 **/
function hosting_dns_status($op, $args) {

  if (!isset($args['zid']) && isset($args['domain'])) {
    # TODO: error-check this!
    $args['zid'] = _hosting_dns_get_zid($args['domain']);
  }

  switch ($op) {
    case 'zone':
      // return zone SOA info if it exists
      $result = db_query("SELECT * FROM {hosting_dns_soa} WHERE zid = %d", $args['zid']);
      $object = db_fetch_object($result);
      break;
    case 'rr':
      // return RR details, if it exists
      if (isset($args['rid'])) {
        $result = db_query("SELECT * FROM {hosting_dns_rr} WHERE zid = %d AND rid = %d", $args['zid'], $args['rid']);
      } else {
        $result = db_query("SELECT * FROM {hosting_dns_rr} WHERE zid = %d AND name = '%s' AND type = '%s'", $args['zid'], $args['name'], $args['type']);
      }
      $object = db_fetch_object($result);
      break;
  }

  return ($object === false)?FALSE:$object;
}

/**
 * Get all records for a particular zone.
 *
 * @param $zid
 *   The internal zone ID for which records are being requested.
 * @param $num
 *   If TRUE, return only a count of how many records exist in the given zone.
 *   Otherwise, return an array of the records themselves.
 *
 * @return
 *   An array of record objects, or a count of how many records the zone contains (if $num == TRUE).
 **/
function hosting_dns_getrecords($zid, $num = FALSE) {
  $records = array();
  $result = db_query("SELECT * FROM {hosting_dns_rr} WHERE zid = %d", $zid);
  while ($rec = db_fetch_object($result)) {
    $records[$rec->rid] = $rec;
  }
  if ($num) {
    return count($records);
  } else {
    return $records;
  }
}

/**
 * Helper function for creating a new zone via the hosting_dns_zone API function.
 * Given the domain name and dns server, create a zone record in the db
 *
 * @param $domain
 *   The domain name/origin for the zone to be created.
 * @param $data
 *   An array of data to populate the new zone. Most importantly, the 'dns_id' 
 *   field must contain the nid of the DNS server which on which to privision this zone.
 *
 * @return
 *   The zone object, whether newly created, or already existing.
 **/
function hosting_dns_create_zone($domain, $dns) {
  # call _hosting_dns_zone and _hosting_dns_rr as needed
drupal_set_message("called hosting_dns_create_zone: ".$domain);
  if ($zone = hosting_dns_status('zone', array('domain' => $domain))) {
drupal_set_message("zone exists: ");
ob_start(); print_r($zone);
drupal_set_message(ob_get_contents());
ob_end_clean();
    # log "Notice, t("Loading zone for url @url", array('@url' => $domain)));
    return $zone;
  } else {
drupal_set_message("creating zone: ");
    # log "notice, t("Creating zone for url @url on dns server @id", array('@url' => $domain, '@id' => $data['dns_id'])
    $zone_fields['dns_id'] = $dns->nid;
    $zone_fields['origin'] = $domain;
    $zone_fields['serial'] = date('Ymd') . '01';
    $ns_recs = explode("\n", $dns->ns);
    $zone_fields['ns1'] = $ns_recs[0];
    $zone_fields['ns2'] = $ns_recs[1];
    $zone_fields['mbox'] = $dns->mbox;
ob_start(); print_r($zone_fields);
drupal_set_message(ob_get_contents());
ob_end_clean();
    return hosting_dns_zone('add', $zone_fields);
  }
}


/**
 * Helper function for creating a new A record via the hosting_dns_rr API function.
 * Given the host record array and a zone object, create a RR record in the db
 *
 * @param $host
 *   The name part of the record to be created.
 * @param $zone
 *   The zone object in which to create the record.
 * @param $data
 *   An array of data used to populate the record. Most importantly, the 'dns_ip' field should
 *   contain the IP address which the host record should point to.
 *
 * @return
 *   The record object, or FALSE if the record could not be created.
 **/
function hosting_dns_create_record($host, $zone, $data) {
  # provision_log("notice", t("Creating %type record for url @url", array('%type' => $data['type'], '@url' => $host)));
  $rec['name'] = $host;
  $rec['type'] = $data['type']?$data['type']:'A';
  $rec['data'] = $data['dns_ip'];
  return hosting_dns_rr('add', $zone->zid, $rec);
}

/**
 * Helper function for removing a given URL's record as well as zonefile (if it's the last
 * record in the zone) via the hosting_dns_zone and hosting_dns_rr API functions.
 *
 * @param $url
 *   The URL to remove. The zone and RR will be extracted from this and removed via the API.
 **/
function hosting_dns_site_delete($url) {
  $url_parts = _hosting_dns_split_url($url);

  if ($zone = hosting_dns_status('zone', array('domain' => $url_parts['domain']))) {
    if ($rec = hosting_dns_status('rr', array('zid' => $zone->zid, 'name' => $url_parts['host'], 'type' => 'A'))) {

      # delete the A record for this site..
      hosting_dns_rr('delete', $zone, array('rid' => $rec->rid));

      # and (as long as it's the only host on this domain) delete the zone as well
      if (hosting_dns_getrecords($zone->zid, TRUE) <= 1) { 
	hosting_dns_zone('delete', array('zid' => $zone->zid));
      }
    }
  }
}

/** 
 * Manage zones
 *
 * @param $op
 *   The operation to be done on the zone. Valid $ops are 'add', 'delete' and 'update'.
 * @param $args
 *   An array which identifies the zone (using the 'origin' or 'zid' keys) plus data for 
 *   use in the operation itself. Valid data keys for $args are:
 *   'dns_id', 'zid', 'origin', 'ns1', 'ns2', 'mbox', 'serial', 'refresh', 'retry', 'expire', 'minimum', 'ttl'
 *   The special 'dns_id' key causes the function to load a dns_server node with this ID for use in populating 
 *   defaults for the other fields.
 *
 * @return
 *   For the 'add' and 'update' $ops, returns the zone object.
 **/
function hosting_dns_zone($op, $args) {
  $fields = array('origin', 'ns1', 'ns2', 'mbox', 'serial', 'refresh', 'retry', 'expire', 'minimum', 'ttl', 'active', 'xfer');
  if (isset($args['dns_id'])) { $dns = node_load($args['dns_id']); }
  // do whatever DB setup is necessary, then call hook_dns_zone($op, $args) to let the engine do the work..
  switch ($op) {
    case 'add':
      if ($zone = hosting_dns_status('zone', array('domain' => $args['origin']))) {
        // domain already exists, just return the zid
        return $zone;
      } else {
        if (!isset($args['origin'])) {
          // error, need at least this arg
# log this..
          return FALSE;
        }
        // build query, and INSERT and set the $zone object
        foreach ($fields as $field) {
          if (isset($args[$field])) {
            $f[] = $field;
            $v[] = $args[$field];
          } else {
            # Use default from dns_server node
            if (isset($dns)) {
	      $f[] = $field;
	      $v[] = $dns->$field;
            }
          }
        }
	$sql = sprintf("INSERT INTO {hosting_dns_soa} (%s) VALUES ('%s')",
			implode(",", $f),
			implode("','", $v));
        db_query($sql);
        $zone = hosting_dns_status('zone', array('domain' => $args['origin']));
      }
      module_invoke_all("hosting_dns_zone", $op, $zone);
      if ($dns) {  # Add a default A record for the origin
        hosting_dns_rr('add', $zone, array('name' => '@', 'type' => 'A', 'data' => $dns->default_ip));
      }
      return $zone;
      break;
    case 'update':
      if (!isset($args['zid'])) {
        $zone = hosting_dns_status('zone', array('domain' => $args['origin']));
      } else {
        $zone = hosting_dns_status('zone', array('zid' => $args['zid']));
      }

      if ($zone || $zone = hosting_dns_status('zone', $args)) {
        // build update query 
        $keys = array();
        if (!isset($args['serial']) || $zone->serial == $args['serial']) {
          $args['serial'] = _hosting_dns_increment_serial($zone->serial);
        }
        foreach ($fields as $field) {
          if (isset($args[$field]) && $args[$field] != $zone->$field) {
            if ($field == 'origin') {
              $zone->old_origin = $zone->origin; # Save this so engines can modify name
            }
            $keys[] = $field . "= '%s'";
            $values[] = $args[$field];
            $zone->$field = $args[$field];
          }
        }
        $values[] = $zone->zid;

        if (!empty($keys)) {
	  $sql = "UPDATE {hosting_dns_soa} SET ". implode(',', $keys) ." WHERE zid=%d";
	  db_query($sql, $values);
        }
      } else {
        // zone doesn't exist, return error? 
        return FALSE;
      }
      module_invoke_all("hosting_dns_zone", $op, $zone);
      return $zone;
      break;
    case 'delete':
      $zone = hosting_dns_status('zone', $args);
      module_invoke_all('hosting_dns_zone', 'delete', $zone);  # This will delete all the zone's records too

      db_query('DELETE FROM {hosting_dns_rr} WHERE zid = %d', $zone->zid);
      db_query('DELETE FROM {hosting_dns_soa} WHERE zid = %d', $zone->zid);
      return TRUE;
      break;
  }
}

/** 
 * Add, delete, or update Resource Records (see http://www.dns.net/dnsrd/rr.html and http://www.bind9.net/dns-parameters)
 *
 * @param $op
 *   What type of operation to execute on the zone: add, delete, or update.
 * @param $zone
 *   The zone object in which the RR resides.
 * @param $args
 *   An array identifying the RR to operate on plus data for use in the operation itself.
 *   Valid keys for this array are:
 *   'name', 'type', 'data', 'aux', 'ttl'
 *
 * @return
 *   For the 'add' and 'update' $ops, return the record object.
 **/
function hosting_dns_rr($op, $zone, $args) {
  $fields = array('name', 'type', 'data', 'aux', 'ttl');
  if (is_numeric($zone)) {
    $zone = hosting_dns_status('zone', array('zid' => $zone));
  }
  if (!is_object($zone)) {
    return FALSE;  # Error, the zone doesn't seem to exist!
  }

  // do whatever DB setup is necessary, then call hook_dns_rr($op, $args)
  switch ($op) {
    case 'add':
      if ($rr = hosting_dns_status('rr', array_merge(array('zid' => $zone->zid), $args))) {
	// RR already exists
	return $rr;
      } 
      if (!isset($args['name']) || !isset($args['type']) || !isset($args['data'])) {  # Not enough info
	return FALSE;
      } 
      $sql = sprintf("INSERT INTO {hosting_dns_rr} (zid, name, type, data, aux, ttl) VALUES (%d, '%s', '%s', '%s', %d, %d)", $zone->zid, $args['name'], $args['type'], $args['data'], $args['aux'], $args['ttl']);
      db_query($sql);
      $record = hosting_dns_status('rr', array('zid' => $zone->zid, 'name' => $args['name'], 'type' => $args['type']));
      module_invoke_all('hosting_dns_rr', $op, $zone, $record);
      hosting_dns_zone('update', array('zid' => $zone->zid, 'serial' => _hosting_dns_increment_serial($zone->serial)));
      return $record;
      break;
    case 'update':
      $record = hosting_dns_status('rr', array('zid' => $zone->zid, 'rid' => $args['rid']));
      
      $keys = array();
      foreach ($fields as $field) {
        if (isset($args[$field]) && $args[$field] != $record->$field) {
          if ($field == 'name') { $record->old_name = $record->name; } # Save this so engines can modify it
          $values[] = $args[$field];
          $record->$field = $args[$field];
          if ($field == 'aux' || $field == 'ttl') { # numeric
            $keys[] = $field . "= %d";
          } else { # string
            $keys[] = $field . "= '%s'";
          }
        }
      }
      $values[] = $zone->zid;
      $values[] = $args['rid'];

      if (!empty($keys)) {
	$sql = "UPDATE {hosting_dns_rr} SET ". implode(',', $keys) ." WHERE zid=%d AND rid=%d";
	db_query($sql, $values);
      }
      module_invoke_all('hosting_dns_rr', $op, $zone, $record);
      hosting_dns_zone('update', array('zid' => $zone->zid, 'serial' => _hosting_dns_increment_serial($zone->serial)));
      return $record;
      break;
    case 'delete':
      $record = hosting_dns_status('rr', array('zid' => $zone->zid, 'rid' => $args['rid']));
      module_invoke_all('hosting_dns_rr', 'delete', $zone, $record);

      db_query('DELETE FROM {hosting_dns_rr} WHERE zid = %d AND rid = %d', $zone->zid, $record->rid);
      break;
  }

}

/**
 * Helper function to increment a zone's serial number.
 * 
 * @param $serial
 *    A serial in YYYYMMDDnn format
 * 
 * @return
 *    The serial, incremented based on current date and index
 **/
function _hosting_dns_increment_serial($serial) {
  $date = substr($serial, 0, 8); # Get the YYYYMMDD part
  $index = substr($serial, 8, 2); # Get the index part
  $today = date('Ymd');
  if ($date == $today) {
    return $date . sprintf('%02d', $index+1);
  } else {
    return $today . '01';
  }
}

/**
 * Need to modify this to use an admin-generated list of TLDs, and return the
 * domain as the TLD plus one other component
 * Examples:
 *  domain.tld
 *  www.domain.tld
 *  sub.domain.tld
 *  www.sub.domain.tld
 *  sub.multi.domain.tld (domain has multiple parts before TLD, but after host?)
 **/
function _hosting_dns_split_url($url) {
  $tlds = split("\n", variable_get('hosting_dns_tlds', ''));
  $first_dot = strpos($url, '.');

  # Strip off the first part
  $host = substr($url, 0, $first_dot);
  $zone = substr($url, $first_dot);

  # If the remaining hostname is a TLD
  if (in_array($zone, $tlds)) {
    # Add the first part back, and that's the zone (the host is @)
    $zone = $host . $zone;
    $host = '@';
  } else {
    # that's the zone (and the host is the first part)
    $zone = substr($zone, 1); # strip the leading .
  }

  return array('host' => $host, 'zone' => $zone);
}

/**
 * Helper function to get the zid for a given zone
 *
 * @param $origin
 *   The name (or origin) of the zone/domain **/
function _hosting_dns_get_zid($origin) {
  return db_result(db_query("SELECT zid FROM {hosting_dns_soa} WHERE origin LIKE '%%%s%%'", $origin));
}   

