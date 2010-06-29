<?php

/**
 * Implement hook_hosting_quota_resource
 */
function hosting_site_hosting_quota_resource () {
  $resources = array();

  $resources['sites'] = array(
    'title' => t('Sites'),
    'description' => t('The number of sites this client can have provisioned under them at any one time. Set to 0 for unlimited.'),
    'module' => 'hosting_site',
  );

  return $resources;
}

/**
 * Implement hook_hosting_quota_get_usage
 */
function hosting_site_hosting_quota_get_usage ($client, $resource, $start = NULL, $end = NULL) {
  if (hosting_get_client($client) && $resource = 'sites') {
    return db_result(db_query("SELECT count(*) FROM {hosting_site} WHERE client='%d'", $client));
  }
}

/**
 * Implement hook_hosting_quota_resource_render
 */
function hosting_site_hosting_quota_resource_render ($resource, $usage) {
  return t('@usage sites', '@usage' => $usage);
}
