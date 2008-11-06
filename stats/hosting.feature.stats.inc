<?php

function hosting_stats_hosting_feature() {
  $features['stats'] = array(
    'title' => t('Generate statistics (experimental)'),
    'description' => t('Keeps track of frequency of posts and usage of your hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'callbacks' => array(
      'enable' => 'hosting_stats_enable',
      'disable' => 'hosting_stats_disable')
    );
  return $features;
}

function hosting_stats_enable() {
  include_once('includes/install.inc');
  drupal_install_modules(array('hosting_stats'));
}

function hosting_stats_disable() {
  module_disable(array('hosting_stats'));
}
