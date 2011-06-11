<?php

/**
 * @file
 * Hooks provided by the hosting site module.
 */

/**
 * @addtogroup hostinghooks
 * @{
 */

/**
 *
 * @param $return
 *  An array of arrays, keys are fields on the $node and values are valid
 *  options for those fields.
 * @param $node
 *   The node object that represents the site.
 *
 * @see hosting_site_available_options()
 */
function hook_hosting_site_options_alter(&$return, $node) {
  // From: hosting_ssl_hosting_site_options_alter().

  // Disable the ssl key fields by default.
  if (!sizeof(hosting_ssl_get_servers())) {
    $return['ssl_enabled'] = FALSE;
  }

  $return['ssl_key'] = false;
  $return['ssl_key_new'] = false;

  // Test if ssl has been enabled.
  if ($node->ssl_enabled != 0) {

    $keys = hosting_ssl_get_keys($node->client, TRUE);

    // return the list of valid keys, including the special 'new key' option.
    $return['ssl_key'] = array_keys($keys);

    // properly default this value so things dont fall apart later.
    if (sizeof($return['ssl_key']) == 1) {
      $node->ssl_key = HOSTING_SSL_CUSTOM_KEY;
    }

    // the user has chosen to enter a new key
    if ($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) {
      // default the new key to the site's domain name, after filtering.
      $default = hosting_ssl_filter_key($node->title);
      $return['ssl_key_new'] = (!empty($default)) ? $default : true;
    }

    // we need to ensure that the return value is properly indexed, otherwise it
    // gets interpreted as an object by jquery.
    $return['profile'] = array_values(array_intersect($return['profile'], hosting_ssl_get_profiles()));

    $return['platform'] = array_values(array_intersect($return['platform'], hosting_ssl_get_platforms()));
  }
}

/**
 * @see hosting_task_confirm_form()
 * @see hosting_site_list_form()
 */
function hosting_task_TASK_TYPE_form($node) {

}

/**
 * @see hosting_task_confirm_form()
 */
function hosting_task_TASK_TYPE_form_validate() {

}

/**
 * @} End of "addtogroup hooks".
 */
