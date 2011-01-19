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
  if (hosting_get_client($client) && $resource == 'sites') {
    return db_result(db_query("SELECT count(*) FROM {hosting_site} WHERE client='%d' AND status <> '%d'", $client, HOSTING_SITE_DELETED));
  }
}

/**
 * Implement hook_hosting_quota_resource_render
 */
function hosting_site_hosting_quota_resource_render ($resource, $usage) {
  return t('@usage sites', array('@usage' => $usage));
}

/**
 * Check for quotas and return an appropriate error message to the site creation form
 */
function hosting_site_quota_exceeded($node = array()) {
  $client = node_load($node['client']);
  $usage = hosting_site_hosting_quota_get_usage($node['client'], 'sites');
  $limit = $client->quota['sites']['limit'];
  if ($usage >= $limit && !empty($limit)) {
    return t('@client has reached their site quota of @sites. No new sites can be added unless an existing site is deleted or the site quota is increased.', array('@client' => $client->title, '@sites' => $limit));
  }
  return FALSE;
}
