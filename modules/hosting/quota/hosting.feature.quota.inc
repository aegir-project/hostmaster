<?php

/**
 * @file
 *   Expose the quota feature to hostmaster.
 */

/**
 * Implementation of hook_hosting_feature().
 */
function hosting_quota_hosting_feature() {
  $features['quota'] = array(
    'title' => t('Quotas'),
    'description' => t('Provides the ability to define and allocate quotas for various resources to clients.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_quota',
    'group' => 'experimental',
  );
  return $features;
}

