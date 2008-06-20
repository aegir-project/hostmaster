<?php

function hosting_wizard_init() {
  if ($step = hosting_wizard_current_step()) {
  if ($_GET['skip_wizard']) {
    variable_set('hosting_wizard_completed', TRUE);
  }
  elseif (($step['path'] != $_GET['q']) && (arg(0) .'/' . arg(1) != 'admin/help')){
    drupal_goto($step['path']);
  }
}
}

function hosting_wizard_form_alter($form_id, &$form) {
  // Add redirection to next step in wizard
  if ($step = hosting_wizard_current_step()) {
    if ($step['form_id'] == $form_id) {
      $form['#submit']['hosting_wizard_continue_submit'] = array();
    }
  }
}

/**
 * Progress to the next part of the installation wizard
 */
function hosting_wizard_continue_submit() {
  if ($step = hosting_wizard_current_step(TRUE)) {
      return $step['path'];
  }
  else {
      return variable_get('site_frontpage', 'node');      
  }
}

/**
 * Return information about the current step of the installation wizard
 */
function hosting_wizard_current_step($next = false) {
  if (variable_get('hosting_wizard_completed', false)) {
    return false;
  }
  $steps = hosting_wizard_steps();
  
  // set step, initializaing to first element of #steps
  $step = variable_get('hosting_wizard_current_step', $steps['#steps'][0]);
  if ($step == $steps['#steps'][sizeof($steps['#steps']) - 1] && $next) {
    variable_set('hosting_wizard_completed', true);
    variable_del('hosting_wizard_current_step');
    return false;
  }
  if ($next) {
    $key = array_search($step, $steps['#steps']);
    variable_set('hosting_wizard_current_step', $steps['#steps'][$key + 1]);
  }

  return $steps[$step];
}


/**
 * Control structure for stepping through the installation wizard.
 */
function hosting_wizard_steps() {
  return array(
    '#steps' => array('user_edit', 'provision_configure'),
    'user_edit' => array("path" => "user/1/edit", 'title' => t("Change your account password."), "form_id" => "user_edit", 
        'message' => t('An administrator account has been registered for you. 
          Please change your password to continue with the configuration process.')),
    'provision_configure' => array("path" => "admin/settings/provision", 'title' => t("Configure the provisioning framework."), "form_id" => "provision_configure",
      'message' => t('To be able to create sites using Hostmaster, we need some information about your server. Please complete this configuration form.')),
  );
}


