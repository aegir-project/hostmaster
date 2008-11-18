<?php

function hosting_stats_hosting_feature() {
  $features['stats'] = array(
    'title' => t('Generate statistics'),
    'description' => t('Keeps track of frequency of posts and usage of your hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'group' => 'experimental',
    'module' => 'hosting_stats',
    );
  return $features;
}

