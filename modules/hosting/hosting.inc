<?php

/**
 * @file
 *
 * General purpose Hosting module functions.
 *
 * These can be used by both the frontend Hosting module and drush commands.
 */

/**
 * Check if a hostname provided is an ip address.
 *
 * @param $hostname
 *   The hostname to check.
 * @return
 *   TRUE is the $hostname is a valid IP address, FALSE otherwise.
 */
function _hosting_valid_ip($hostname) {
  return is_string(inet_pton($hostname));
}

/**
 * Check if the FQDN provided is valid.
 *
 * @param $fqdn
 *   The Fully Qualified Domain Name (FQDN) to validate.
 * @return
 *   An integer greater than 0 if the $fqdn is valid, or 0 or FALSE if it not
 *   valid.
 */
function _hosting_valid_fqdn($fqdn) {
  # regex is an implementation of RFC1035
  # original: return preg_match("/^([a-z0-9]([a-z0-9-]*[a-z0-9])?\.?)+$/i", $fqdn);
  #
  # We don't allow IPs here.
  #
  if (preg_match("/host8\.biz$/", $fqdn)) {
    return (preg_match("/^[a-z0-9]+[a-z0-9-.]*[a-z0-9]+\.(?:a|b|v|o)[0-9]{1,10}\./", $fqdn));
  }
  else {
    return (preg_match("/^(\*\.)?([a-z0-9]([a-z0-9-]*[a-z0-9])?\.?)+[a-z0-9]+$/", $fqdn) && preg_match("/[a-z]+/", $fqdn));
  }
}

/**
 * Check if the FQDN or IP provided is valid.
 */
function _hosting_valid_fqdn_ip($fqdn) {
  # regex is an implementation of RFC1035
  #
  # We allow IPs here.
  #
  return (preg_match("/^([a-z0-9]([a-z0-9-]*[a-z0-9])?\.?)+[a-z0-9]+$/", $fqdn));
}

/**
 * Format a timestamp as a string in a friendly way.
 *
 * @param $ts
 *   The timestamp to format as a an interval.
 * @return
 *    Returns a string representing the given timestamp:
 *    - If the timestamp is the current time: 'Now'.
 *    - If the timestamp is 0 or FALSE: 'Never'
 *    - Otherwise formatted as 'X ago' where 'X' is for example 1 year or 1
 *      minute etc.
 *
 * @see format_interval()
 */
function hosting_format_interval($ts) {
  if ($ts==time()) { 
    return t('Now');
  }
  if (!$ts) {
    //Treats the UNIX EPOCH as never.
    return t('Never');
  }

  return t("@interval ago", array("@interval" => format_interval(time() - $ts, 1)));
}

/**
 * Make a path canonical.
 *
 * This removes duplicate slashes, trailing slashes and /./ occurences. It does
 * not (yet?) resolve .. instances.
 */
function hosting_path_normalize($path) {
  return rtrim(preg_replace('/(\/\/*\.)?\/\/*/', '/', $path), '/');
}
