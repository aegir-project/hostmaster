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
 * Given the domain name and dns server, create a zone record in the db
 **/
function _hosting_dns_create_zone($domain, $dns) {
  # call _hosting_dns_zone and _hosting_dns_rr as needed
}


/**
 * Given the host record array and a zone object, create a RR record in the db
 **/
function _hosting_dns_create_record($host, $zone) {
  # call _hosting_dns_zone and _hosting_dns_rr as needed
}

/**
 * Delete a zone
 **/
function _hosting_dns_delete($url) {
}

/* replace provision_dns_zone() multi-op API call to manage soa table */
function _hosting_dns_zone($op, $args) {
  
}

/* replace provision_dns_rr() multi-op API call to manage rr table */
function _hosting_dns_rr($op, $zone, $args) {
}
