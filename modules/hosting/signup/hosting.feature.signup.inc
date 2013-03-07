<?php

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_signup_hosting_feature() {
  $features['signup'] = array(
    'title' => t('Sign up form'),
    'description' => t('Provides a simpler signup form that can be opened to anonymous users.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_signup',
  );
  return $features;
}

