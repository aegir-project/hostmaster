<?php

/**
 * this defines a few known site states, through bitmasks
 *
 * this is designed to be extensible. New states can be defined exactly like this in contrib modules.
 */
define(HOSTING_SITE_UNKNOWN, 0);
define(HOSTING_SITE_INSTALLED, 1);
define(HOSTING_SITE_NOTINSTALLED, ~HOSTING_SITE_INSTALLED);
define(HOSTING_SITE_ENABLED, 2);
define(HOSTING_SITE_DISABLED, ~HOSTING_SITE_ENABLED);
define(HOSTING_SITE_DELETED, 4);

global $hosting_site_status;
/**
 * bitmask => string translation map. this will be iterated upon to figure out what flags are set in a status field
 */
$hosting_site_status = array(
  HOSTING_SITE_UNKNOWN => "Unknown status",
  HOSTING_SITE_INSTALLED => "Site installed",
  HOSTING_SITE_ENABLED => "Site enabled",
  HOSTING_SITE_DELETED => "Site deleted",
);

global $hosting_site_antistatus;
/**
 * bitmask => string translation for states that, when unset do have a specific meaning
 *
 * XXX: this is not really graceful, but it is essential to have a seperate
 * array in order to be able to loop through the "real" statuses. another
 * solution could be to have a "HOSTING_SITE_MAXSTATUS" that would be used to
 * iterate ($i*=2) through the bitmasks instead of going through the array
 */
$hosting_site_antistatus = array(
  HOSTING_SITE_NOTINSTALLED => "Site not installed yet",
  HOSTING_SITE_DISABLED => "Site disabled",
);

/**
 * take a status and output a human-readable representation of the flags set
 */
function hosting_site_status_output($bitfield) {
  global $hosting_site_status, $hosting_site_antistatus;
  if ($bitfield == HOSTING_SITE_UNKNOWN) {
     return $hosting_site_status[$bitfield];
  }
  $output = array();
  foreach ($hosting_site_status as $bitmask => $string) {
    if ($bitfield & $bitmask) {
      $output[] = $hosting_site_status[$bitmask];
    } elseif (array_key_exists(~$bitmask, $hosting_site_antistatus)) {
      $output[] = $hosting_site_antistatus[~$bitmask];
    }
  }
  return join(', ', $output);
}
