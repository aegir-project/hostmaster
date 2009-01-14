<?php

/**
 * Implementation of hook_hosting_feature
 * Defines the DNS server feature
 **/
function hosting_dns_hosting_feature() {
  $features['dns'] = array(
    'title' => t('DNS servers'),
    'description' => t('Configure and maintain multiple DNS servers.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'node' => 'dns_server',
    'module' => 'hosting_dns',
    'group' => 'experimental');
  return $features;
}

