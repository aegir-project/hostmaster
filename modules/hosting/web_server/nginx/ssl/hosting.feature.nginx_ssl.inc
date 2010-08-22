<?php

function hosting_nginx_ssl_hosting_feature() {
  $features['nginx_ssl'] = array(
    'title' => t('Nginx SSL support'),
    'description' => t('Provide support for the NGINX web server with SSL functionality.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_nginx_ssl',
    'group' => 'experimental'
    );
  return $features;
}

