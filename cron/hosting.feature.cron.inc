<?php

function hosting_cron_hosting_feature() {
  $features['cron'] = array(
    'title' => t('Cron queue'),
    'description' => t('Keeps track of running the cron process on all your sites.'),
    'status' => HOSTING_FEATURE_ENABLED,
    'module' => 'hosting_cron',
    );
  return $features;
}

