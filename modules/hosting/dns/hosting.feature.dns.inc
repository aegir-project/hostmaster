<?php
/**
 * @file
 *   Expose the dns feature to hostmaster.
 */

/**
 * Implementation of hook_hosting_feature().
 */
function hosting_dns_hosting_feature() {
  $features['dns'] = array(
    'title' => t('DNS support'),
    'description' => t('Manage DNS records for your hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_dns',
    'group' => 'experimental'
    );
  return $features;
}
