<?php

function hosting_alias_hosting_feature() {
  $features['alias'] = array(
    'title' => t('Site aliasing'),
    'description' => t('Provide multiple domain names for sites to be accessible by.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_alias',
    );
  return $features;
}
