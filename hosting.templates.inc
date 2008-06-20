<?php

/**
 * Page callback
 * Configure the templates used to generate config files for the framework.
 */
function hosting_config_templates($node) {
  $result = db_query("SELECT * FROM {config_template} WHERE vid=%d AND type='drupal_settings' limit 1", $node->vid);

  if (db_num_rows($result)) {
    $drupal_settings = db_fetch_array($result);
  }
  else {
    $drupal_settings = array(
      'template' => variable_get('provision_drupal_settings_template', _provision_drupal_default_template()),
      'status' => 0
    );
  }

  $form['#tree'] = true;
  $form['drupal_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Drupal configuration files'),
    '#description' => t('These settings control the generation of the Drupal settings.php file for this host.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['drupal_settings']['template'] = array(
    '#type' => 'textarea',
    '#title' => t('Drupal settings file template'),
    '#description' => t('The text you enter here will be used by the Provisioning framework to generate your Drupal settings.php file. This field contains php, and should start with &lt;?php.'),
    '#default_value' => $drupal_settings['template'],
    '#cols' => 60,
    '#rows' => 5,
  );
  $form['drupal_settings']['status'] = array(
    '#type' => 'checkbox',
    '#default_value' => $drupal_settings['status'],
    '#title' => t('Use this configuration file for this platform.'),
  );
  
  $result = db_query("SELECT * FROM {config_template} WHERE vid=%d AND type='apache_vhost' limit 1", $node->vid);
  if (db_num_rows($result)) {
    $apache_vhost = db_fetch_array($result);
  }
  else {
    $apache_vhost = array(
      'template' => variable_get('provision_apache_vhost_template', _provision_apache_default_template()),
      'status' => 0
    );
  }

  $form['#tree'] = true;
  $form['apache_vhost'] = array(
    '#type' => 'fieldset',
    '#title' => t('Apache Virtual Host configuration files'),
    '#description' => t('These settings control the generation of the Virtual Host directives for this platform.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['apache_vhost']['template'] = array(
    '#type' => 'textarea',
    '#title' => t('Drupal settings file template'),
    '#description' => t('The text you enter here will be used by the Provisioning framework to generate your Apache Virtual Host file. This field contains php, and should start with &lt;?php.'),
    '#default_value' => $apache_vhost['template'],
    '#cols' => 60,
    '#rows' => 5,
  );
  $form['apache_vhost']['status'] = array(
    '#type' => 'checkbox',
    '#default_value' => $apache_vhost['status'],
    '#title' => t('Use this configuration file for this platform.'),
  );
  return $form;
}

