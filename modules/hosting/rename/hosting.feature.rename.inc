<?php

function hosting_rename_hosting_feature() {
  $features['rename'] = array(
    'title' => t('Site rename'),
    'description' => t('Rename a site both in the frontend and in the backend filesystem.'),
    'status' => HOSTING_FEATURE_ENABLED,
    'module' => 'hosting_rename',
    );
  return $features;
}

