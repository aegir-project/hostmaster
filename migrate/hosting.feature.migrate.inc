<?php

function hosting_migrate_hosting_feature() {
  $features['migrate'] = array(
    'title' => t('Site migration'),
    'description' => t('Migrate sites to platforms of the same or later version.'),
    'status' => HOSTING_FEATURE_ENABLED,
    'module' => 'hosting_migrate',
    );
  return $features;
}

