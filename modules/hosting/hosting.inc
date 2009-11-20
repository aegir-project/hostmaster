<?php

/**
 * @file
 *
 * General purpose functions
 *
 * Those functions are destined to be used by the frontend Drupal
 * module as well as the drush hooks of the frontend
 */

/**
 * Check if a hostname provided is an ip address
 */
function _hosting_valid_ip($hostname) {
  //TODO : provide IPv6 support
  $parts = explode('.', $hostname);
  if (sizeof($parts) != 4) {
    return false;
  }
  foreach ($parts as $part) {
    if (((int) $part < 0) || ((int) $part > 255)) {
      return false;
    }
  }
  return true;
}

/**
 * Check if the FQDN provided is valid.
 */
function _hosting_valid_fqdn($fqdn) {
  # regex is an implementation of RFC1035
  return preg_match("/^([a-z0-9]([a-z0-9-]*[a-z0-9])?\.?)+$/i", $fqdn);
}

function hosting_format_interval($ts) {
  if ($ts==mktime()) { 
    return t('Now');
  }
  if (!$ts) {
    //Treats EPOCH as never
    return t('Never');
  }

  return t("@interval ago", array("@interval" => format_interval(mktime() - $ts, 1)));
}

/**
 * Make a path canonical
 *
 * This removes duplicate slashes, trailing slashes and /./ occurences. It does
 * not (yet?) resolve .. instances.
 */
function hosting_path_normalize($path) {
  return rtrim(preg_replace('/(\/\/*\.)?\/\/*/', '/', $path), '/');
}
