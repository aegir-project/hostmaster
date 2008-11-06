<?php

function hosting_signup_hosting_feature() {
  $features['signup'] = array(
    'title' => t('Sign up form (experimental)'),
    'description' => t('Provides a simpler signup form that can be opened to anonymous users.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'callbacks' => array(
      'enable' => 'hosting_signup_enable',
      'disable' => 'hosting_signup_disable')
    );
  return $features;
}

function hosting_signup_enable() {
  include_once('includes/install.inc');
  drupal_install_modules(array('hosting_signup'));
}

function hosting_signup_disable() {
  module_disable(array('hosting_signup'));
}
