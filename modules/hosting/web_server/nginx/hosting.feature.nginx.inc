<?php

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
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
