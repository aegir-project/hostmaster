<?php
// $Id$


/**
 * Implementation of hook_xmlrpc
 */
function hosting_signup_xmlrpc() {
  return array(
    array('hosting_signup.getForm',
          'hosting_signup_get_form',
    array('struct', 'string', 'struct'),
          t("Retrieve the form to build")
    ),
     array(
      'hosting_signup.submitForm',
      'hosting_signup_submit_form',
      array('struct', 'string', 'struct'),
      t('Submit your form'),
    ),
     array(
      'hosting_signup.validateForm',
      'hosting_signup_validate_form',
      array('struct', 'string', 'struct'),
      t('Validate your form'),
    ),  );
}


/**
 * XMLRPC function to retrieve the form
 *
 * A simple function that builds the form on the remote site and 
 * sends the structured array back over the line to the local site
 */
function hosting_signup_get_form($server_key, &$form_state, $post = array()) {
  if ($server_key != variable_get('hosting_signup_server_key', '')) {
    $form['error'] = array( '#type' => 'item',
      '#value' => t("The signup form has not been correctly configured"),
     );
  }
  else {
    $form_state['storage'] = null;
    $form_state['submitted'] = (bool) sizeof($post);
    $form_state['values'] = $post;
/* TODO drupal_retrieve_form() now accepts a form_state parameter. */
    $form = drupal_retrieve_form('_hosting_signup_form', $form_state);
    unset($form['#token']);
  }
  return $form; 
}

/**
 * Remote form definition
 *
 * This is a mixture of the client form and node form, 
 * and in the future we will be able to configure which
 * forms we want to be present.
 * 
 * This form is only indirectly access via the XMLRPC methods,
 * and not displayed directly
 */
function _hosting_signup_form() {
  $node = new stdClass();
  $form['site'] = hosting_site_form($node);
  unset($form['site']['client']);
  $form['site']['platform'] = array(
    '#type' => 'hidden',
    '#value' => HOSTING_DEFAULT_PLATFORM
  );
  $state = array(); 
  foreach (module_implements('form_alter') as $module) {
    $function = $module .'_form_alter';
    $function($form['site'], $state, 'site_node_form');
  }
  unset($form['site']['info']['client']);

  $form['client'] = hosting_client_form($node);
  
  foreach (module_implements('form_alter') as $module) {
    $function = $module .'_form_alter';

    $function($form['client'], $state, 'client_node_form');
  }

  $form['new_client'] = array('#type' => 'value', '#value' => TRUE);
  unset($form['client']['title']);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Sign up"),
  );
  return $form;
}


/**
 * XMLRPC method for validating the form
 *
 * This prepares and validates the form with the values
 * passed to the xmlrpc method (which are actually $_POST on the local site)
 *
 * @return
 *   The result of form_get_errors()
 */

function hosting_signup_validate_form($server_key, $values = array()) {
  $form_state = array();
  $form = hosting_signup_get_form($server_key, $form_state, $values);
  drupal_prepare_form('_hosting_signup_form', $form, $form_state);
  drupal_validate_form('_hosting_signup_form', $form, $form_state);
  return hosting_signup_singleton();
}


/**
 * XMLRPC method for submitting a form
 *
 * This function runs on the remote site, pulls the form definition
 * and processes the submitted form
 *
 * @return 
 *   An associative array containing both the site and client records created as arrays
 */
function hosting_signup_submit_form($server_key, $values = array()) {
  $form_state = array();
  $form = hosting_signup_get_form($server_key, $form_state, $values);

  $form['#post'] = $values;
  drupal_prepare_form('_hosting_signup_form', $form, $form_state);
  drupal_process_form('_hosting_process_form', $form, $form_state);
  return hosting_signup_singleton();
}


/**
 * Remote form validation
 *
 * This is the actual validation that the remote form
 * does. It sets any form errors into the hosting_signup_singleton
 * so they can be propogated to the local site.
 */
function _hosting_signup_form_validate($form, &$form_state) {
  $client = (object) $form_state['values'];
  $client->type = 'client';
  $client->title = '';
  node_validate($client);

  $site = (object) $form_state['values'];
  $site->type = 'site';
  node_validate($site);
  hosting_signup_singleton(form_get_errors());
} 

/**
 * Remote form submission
 *
 * Generates the nodes and saves them into hosting_signup_singleton,
 * so the local submission function can re-act to them.
 */
function _hosting_signup_form_submit($form, &$form_state) {
  $client = (object) $form_state['values'];
  $client->type = 'client';
  $client->title = '';
  $client->status = 1;
  node_save($client, 'submit');

  $site = (object) $form_state['values'];
  $site->type = 'site';
  $site->status = 1;
  $site->client = $client->nid;
  $site->uid = $client->uid;
  node_save($site);
  hosting_signup_singleton(array('client' => $client, 'site' => $site));
}


/**
 * Provide a simple getter/setter for the remote form
 * 
 * This allows us to return the results from submission
 * and validation, where the return values of the function
 * are not returned in any way to the caller.
 *
 * So instead we set what we want to return with this, 
 * and return the output of this from the xmlrpc methods.
 */
function hosting_signup_singleton($data = null) {
  static $static = null;
  if ($data) {
    $static = $data;
  }
  return $static;
}

