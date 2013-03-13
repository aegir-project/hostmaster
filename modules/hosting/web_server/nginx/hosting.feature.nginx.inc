<?php
/**
 * @file Register the NGINX Hosting feature.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_nginx_hosting_feature() {
  $features['nginx'] = array(
    'title' => t('Nginx web servers'),
    'description' => t('Provide support for the NGINX web server.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_nginx',
  );
  return $features;
}
