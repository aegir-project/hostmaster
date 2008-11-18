<?php
// $Id$
/**
 * @file Hosting features
 *
 * Contains the logic used to generate the features list
 */
define('HOSTING_FEATURE_DISABLED', 0);
define('HOSTING_FEATURE_ENABLED', 1);
define('HOSTING_FEATURE_REQUIRED', 2);

function hosting_feature($feature) {
  static $features = array();

  if (!sizeof($features)) {
    $features = hosting_get_features();
  }
  if ($features[$feature]['module']) {
    $return = (module_exists($features[$feature]['module'])) ? HOSTING_FEATURE_ENABLED : HOSTING_FEATURE_DISABLED;
  }
  else {
    $return = variable_get('hosting_feature_' . $feature, $features[$feature]['status']);
  }
  return $return;
}

function hosting_features_form() {
  $form['features'] = array(
    '#type' => 'item',
    '#title' => t('Optional system features'),
    '#value' => t('You may choose any of the additional system features from the list below.'),
      );

  $experimental = array(
    '#type' => 'fieldset',
    '#title' => t('Experimental'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
     '#description' => t('Features marked experimental have not been completed to a satisfactory level to be considered production ready, so use at your own risk.'),

    );
  $features = hosting_get_features(TRUE);
  foreach ($features as $feature => $info) {
    $element = array(
      '#type' => 'checkbox',
      '#title' => $info['title'],
      '#description' => $info['description'],
      '#default_value' => hosting_feature($feature),
      '#required' => hosting_feature($feature) == HOSTING_FEATURE_REQUIRED,
    );
    if ($info['group'] == 'experimental') {
      $experimental[$feature] = $element;
    }
    else {
      $form[$feature] = $element;
    }

  }
  $form['experimental'] = $experimental;

  return system_settings_form($form);
}

function hosting_features_form_submit($form_id, $values) {
  $features = hosting_get_features(TRUE);
  foreach ($features  as $feature => $info) {
    $value = $values[$feature];
    $current = hosting_feature($feature);
    variable_set('hosting_feature_' . $feature, $value);
    if ($module = $features[$feature]['module']) {
      if ((!$current) && $value) {
        include_once('includes/install.inc');
        // turn on module
        drupal_set_message(t("Enabling %module module", array('%module' => $module)));
        drupal_install_modules(array($module));
        module_enable(array($module));
      }
      if ($current && !$value) {
        drupal_set_message(t("Disabling %module module", array('%module' => $module)));
        // turn off module
        module_disable(array($module));
      }
    }
    #print("$feature $current $value $module <br />");
  }
  menu_rebuild();
}

function hosting_get_features($refresh = FALSE) {
  $cache = cache_get('hosting_features');

  if (!$cache->data || $refresh) {
    ## include any optional hosting.feature.*.inc files
    $files = drupal_system_listing("hosting\.feature\.[a-zA-Z_]*\.inc$", "modules");
    if (sizeof($files)) {
      foreach ($files as $name => $info) {
        include_once($info->filename);
      }
    }
    $functions = get_defined_functions();
    foreach ($functions['user'] as $function) {
      if (preg_match('/_hosting_feature$/', $function)) {
        $hooks[] = $function;
      }
    }
    $features = array();
    foreach ($hooks as $func) {
      $features = array_merge($features, $func());
    }
    cache_set('hosting_features', 'cache', serialize($features));
    return $features;
  }
  else {
    return unserialize($cache->data);
  }
}
