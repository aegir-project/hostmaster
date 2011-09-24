<?php

/**
 * General settings form.
 */
function hosting_settings($form, &$form_state) {
  $form['settings'] = array(
    '#type' => 'item',
    '#title' => t('General Hosting settings'),
    '#value' => t('Here you may set various general settings that apply to site management or to the frontend system.'),
    '#weight' => 0,
  );

  $form['hosting_ignore_default_profiles'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide platforms with non-default profiles'),
    '#description' => t('When selecting a Drupal core profile, hide platforms that also contain other profiles (i.e intended as distributions). Warning: requires a platform other than the Hostmaster platform.'),
    '#default_value' => variable_get('hosting_ignore_default_profiles', FALSE),
  );
  $form['hosting_require_disable_before_delete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require site to be disabled before deletion'),
    '#description' => t('Check if you wish for users to run the Disable task on a site prior to running the Delete task.'),
    '#default_value' => variable_get('hosting_require_disable_before_delete', TRUE),
  );
  
  if (hosting_feature('cron')) {
    $form['hosting_cron_use_backend'] = array(
      '#type' => 'radios',
      '#title' => t('Cron method'),
      '#description' => t('For running cron on a site. You can use the drush cron implementation or the web-based cron.php method using builtin HTTP requests. The drush implementation is more reliable but will be slower than the web-based approach if the webserver has an opcode cache (like APC) configured.'),
      '#options' => array('Web-based', 'Drush'),
      '#default_value' => variable_get('hosting_cron_use_backend', TRUE),
    );
  }
  return system_settings_form($form);
}
