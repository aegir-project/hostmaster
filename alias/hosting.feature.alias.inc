<?php

function hosting_alias_hosting_feature() {
  $features['alias'] = array(
    'title' => t('Site aliasing (experimental)'),
    'description' => t('Provide multiple domain names for sites to be accessible by.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'callbacks' => array(
      'enable' => 'hosting_alias_enable',
      'disable' => 'hosting_alias_disable')
    );
  return $features;
}

function hosting_alias_enable() {
  include_once('includes/install.inc');
  drupal_install_modules(array('hosting_alias'));
}

function hosting_alias_disable() {
  module_disable(array('hosting_alias'));
}
