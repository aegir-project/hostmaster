<?php

function hosting_cron_hosting_feature() {
  $features['cron'] = array(
    'title' => t('Cron queue (Recommended)'),
    'description' => t('Keeps track of running the cron process on all your sites.'),
    'status' => HOSTING_FEATURE_ENABLED,
    'callbacks' => array(
      'enable' => 'hosting_cron_enable',
      'disable' => 'hosting_cron_disable')
    );
  return $features;
}

function hosting_cron_enable() {
  include_once('includes/install.inc');
  drupal_install_modules(array('hosting_cron'));
}

function hosting_cron_disable() {
  module_disable(array('hosting_cron'));
}
