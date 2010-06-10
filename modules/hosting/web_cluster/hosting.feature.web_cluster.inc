<?php

function hosting_web_cluster_hosting_feature() {
  $features['web_cluster'] = array(
    'title' => t('Web clusters'),
    'description' => t('Provides a service to expose a site across multiple web front ends.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_web_cluster',
    );
  return $features;
}

