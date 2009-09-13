<?php

function hosting_ssl_hosting_feature() {
  $features['ssl'] = array(
    'title' => t('SSL support'),
    'description' => t('Provide SSL security to hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_ssl',
    'group' => 'experimental',
    );
  return $features;
}
