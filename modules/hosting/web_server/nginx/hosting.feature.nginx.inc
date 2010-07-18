<?php

function hosting_nginx_hosting_feature() {
  $features['nginx'] = array(
    'title' => t('Nginx web servers'),
    'description' => t('Provide support for the NGINX web server.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_nginx',
    'group' => 'experimental'
    );
  return $features;
}

