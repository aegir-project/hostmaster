<?php
// $Id$


/**
 * Form callback for signup form
 *
 * This function connects to the xmlrpc method on the master
 * server to retrieve the form to be generated.
 *
 * Replaces the remote form's validation and submission callbacks with local
 * callbacks, that in turn contact the remote form to validate / process the form.
 */
function hosting_signup_form() {
  drupal_add_js(drupal_get_path('module', 'hosting_signup') . '/hosting_signup_form.js');
  $form = xmlrpc(_hosting_signup_get_url(), 'hosting_signup.getForm', _hosting_signup_get_key(), $_POST);
  if (!$form) {
    drupal_set_message(t("XMLRPC request failed: %error", array('%error' => xmlrpc_error_msg())), 'error');
  }
  $form['#action'] = '/hosting/signup';
  $form['#submit'][] = 'hosting_signup_form_submit';
  $form['#validate'][] = 'hosting_signup_form_validate';
  return $form;
}


/**
 * Local validate function for hosting_signup form.
 *
 * Passes through the values to the remote validation function
 * through the xmlrpc method.
 *
 * Sets any errors that are returned from the remote site on the local site.
 */
function hosting_signup_form_validate($form, &$form_state) {
  $key = variable_get('hosting_signup_server_key', '');
  $values = $form_state['values'];
  $values['form_id'] =  '_hosting_signup_form';

  $return = xmlrpc(_hosting_signup_get_url(), 'hosting_signup.validateForm', $key, $values);
  if (is_array($return)  && sizeof($return)) {
    foreach($return as $field => $message) {
      form_set_error($field, $message);
    }
  }
}



/**
 * Local submit function for hosting_signup_form.
 *
 * Passes through the values to the remote for submission
 * through the xmlrpc method.
 *
 * Redirects to the thank you page on success.
 */ 
function hosting_signup_form_submit($form, &$form_state) {
  //@TODO must be broken
  $key = variable_get('hosting_signup_server_key', '');

  $values = $form_state['values'];
  $form_state['values']['form_id'] =  '_hosting_signup_form';

  $return = xmlrpc(_hosting_signup_get_url(), 'hosting_signup.submitForm', $key, $form_state['values']);

  if (isset($return['site']['nid']) && isset($return['site']['nid'])) {
    $path = sprintf("hosting/signup/thanks/%s/%s/%s", $return['site']['title'], $return['client']['email'], $return['client']['client_name']);
    $form_state['redirect'] = $path;
  }
}


/**
 * Populate hosting site node form element with specified arguments
 *
 * This function does a drupal_http_request to the existing form_populate callbacks and
 * passes through the information from there.
 */
function _hosting_signup_form_populate($element, $value, $value2 = null) {
  $path = "/hosting/hosting_site_form_populate/$element/$value" . (($value2) ? "/$value2" : "");
  $request = drupal_http_request(_hosting_signup_get_url($path));
  print $request->data;
  exit();
}

/**
 * Thank you page callback
 */
function hosting_signup_thanks($url = '', $email = '', $name = '') {
  return t("<h3>Thank you @name</h3>
    Your site (@url) has been requested, and the moment it is ready you will receive a mail at @email
 with instructions on how to log into it.", array("@name" => $name, "@url" => $url, "@email" => $email));
} 
