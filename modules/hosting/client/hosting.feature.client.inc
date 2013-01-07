<?php

/**
 * Implementation of hook_hosting_feature().
 */
function hosting_client_hosting_feature() {
  $features['client'] = array(
      'title' => t('Clients'),
      'description' => t('Track and manage ownership of hosted sites.'),
      'status' => HOSTING_FEATURE_ENABLED,
      'module' => 'hosting_client',
      'node' => 'client',
    );
  return $features;
}
