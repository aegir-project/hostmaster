<?php

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_hosting_feature() {
  $features['ssl'] = array(
    'title' => t('SSL support'),
    'description' => t('Provides a service to expose a site encrypted by SSL certificates.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_ssl',
  );
  return $features;
}
