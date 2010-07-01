<?php
// $Id$

/**
 * @file Site node form.
 */


/**
 * Helper function to generate form elements for the site form.
 */
function _hosting_site_field(&$form, $node, $item, $element, $filter_display ='filter_xss', $editable = FALSE) {
  if (!$node->nid || $editable) {
    // create it
    if (($element['#type'] == 'radios') && !sizeof($element['#options'])) {
      $form[$item] = array('#type' => 'hidden', '#value' => key($element['#options']));
    }
    else {
      $form[$item] = $element;
    }
  }
  else {
    // display it
    $form['info'][$item] = array(
      '#type' => 'item',
      '#title' => $element['#title'],
      '#value' => $filter_display($element['#default_value']),
      '#required' => FALSE,
    );

    if (isset($element['#weight'])) {
      $form['info'][$item]['#weight'] = $element['#weight'];
    }

    $form[$item] = array('#type' => 'hidden', '#value' => $element['#default_value']);
  }

  $css_id = str_replace("_", "-", $item);
  $form[$item]['#hosting_site_field'] = $item;
  $form[$item]['#hosting_site_field_value'] = $element['#default_value'];
  $form[$item]['#prefix'] = "<div class='hosting-site-field hosting-site-field-{$element['#type']}' id='hosting-site-field-{$css_id}'>";
  $form[$item]['#suffix'] = "</div>";
}

/**
 * pre_render form callback
 *
 * Add the needed .js and .css files to render the form correctly before printing it
 */
function _hosting_site_form_pre_render($elements) {
  static $count = 0;

  // TODO : I have absolutely _NO_ idea why this is being called twice =\
  $count++;
  if ($count == 1) {
    $node = new stdClass();
    
    foreach (element_children($elements) as $key) {
      if (isset($elements[$key]['#hosting_site_field'])) {

        $node->$key = $elements[$key]['#hosting_site_field_value'];    
      }
    }

    $settings['hostingSiteAvailableOptions'] = hosting_site_available_options($node);
    drupal_add_js($settings, 'setting');
  }

  drupal_add_js(drupal_get_path('module', 'hosting_site') . '/hosting_site.form.js');
  return $elements;
}

function hosting_site_available_options($node) {
  // cast to object if it's an array.
  $node = (is_array($node)) ? (object) $node : $node;

  $return = array();

  $return['profile'] = array();
  $return['site_language'] = array();

  if (isset($node->platform)) {
    // Install profiles 
    $profiles = hosting_get_profiles($node->platform);
    foreach($profiles as $id => $name) {
      // Don't allow a site to be provisioned with hostslave or hostmaster profile
      if (in_array($name, array('hostslave', 'hostmaster'))) {
        unset($profiles[$id]);
      }
    }
    reset($profiles);
    $return['profile'] = array_keys($profiles);

    if (isset($node->profile) && in_array($node->profile, $return['profile'])) {
      $return['site_language'] = array_keys(hosting_get_profile_languages($node->profile, $node->platform));
    }
  }

  return $return;
}

/**
 * Implementation of hook_form
 */
function hosting_site_form($node) {
  $form['#pre_render'][] = '_hosting_site_form_pre_render';

  if ($node->nid) {
    $form['info']['#prefix'] = '<div class="clear-block" id="hosting-site-edit-info">';
    $form['info']['#suffix'] = '<br /></div>';
  }

  _hosting_site_field($form, $node, 'title', array(
      '#type' => 'textfield',
      '#title' => t('Domain name'),
      '#required' => TRUE,
      '#default_value' => $node->title,
      '#weight' => -5
    ));

  $editable = ((!$node->client || $node->nid) || user_access('administer site')) && hosting_feature('client');
  _hosting_site_field($form, $node, 'client', array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Client'),
    '#default_value' => _hosting_client_site_default($node),
    '#description' => t('The client who this site belongs to.'),
    '#autocomplete_path' => 'hosting_client/autocomplete/client',
  ), 'filter_xss', $editable);


 // TODO : move this someplace more appropriate 
  $platforms = _hosting_get_platforms();
  $user = user_load($GLOBALS['user']->uid);
  if (sizeof($platforms)) {
    foreach ($platforms as $nid => $title) {
      $platform = node_load($nid);

      if ($platform->platform_status != HOSTING_PLATFORM_LOCKED) {
        if (!isset($platform->clients) || sizeof(array_intersect(array_keys($user->client_id), $platform->clients)) || $user->uid == 1) {
          $options[$nid] = $title;
        }
      }
    }
  }

  if (sizeof($options) > 0) {
    _hosting_site_field($form, $node, 'platform', array(
       '#type' => 'radios',
       '#title' => t('Platform'),
       '#required' => TRUE,
       '#description' => t('The platform you want the site to be hosted on.'),
       '#options' => $options,
       '#default_value' => ($node->platform) ? $node->platform : HOSTING_DEFAULT_PLATFORM,
     ), '_hosting_node_link');
  }
  else {
    drupal_set_message(t('You have no enabled platforms! Please add or unlock a platform before attempting to create a site.'), 'error');
  }

  // Install profiles 
  $profiles = hosting_get_profiles();
  foreach($profiles as $id => $name) {
    // Don't allow a site to be provisioned with hostslave or hostmaster profile
    if (in_array($name, array('hostslave', 'hostmaster'))) {
      unset($profiles[$id]);
    }
  }
  reset($profiles);

  _hosting_site_field($form, $node, 'profile', array(
      '#type' => 'radios',
      '#title' => t('Install profile'),
      '#description' => t('The type of site to install.'),
      '#options' => $profiles,
      '#default_value' => $node->profile ? $node->profile : key($profiles),
      '#required' => TRUE,
      '#attributes' => array('class' => "hosting-site-form-profile-options"),
    ), '_hosting_node_link');

  _hosting_site_field($form, $node, 'site_language', array(
      '#type' => 'radios',
      '#title' => t('Language'),
      '#description' => t('The language of site being installed.'),
      '#options' => hosting_get_profile_languages(),
      '#required' => TRUE,
      '#default_value' => $node->site_language ? $node->site_language : 'en',
      '#attributes' => array('class' => "hosting-site-form-site-language-options"),
    ), '_hosting_language_name');


  _hosting_site_field($form, $node, 'db_server', array(
        '#type' => 'radios',
        '#title' => t('Database server'),
        '#required' => TRUE,
        '#description' => t('The database server the site will use to host it\'s content.'),
        '#options' => hosting_get_servers('db'),
        '#default_value' => $node->db_server,
      ), '_hosting_node_link');

  foreach(array('verified', 'last_cron', 'site_status') as $extra_attribute) {
    $form["$extra_attribute"] = array('#type' => 'value', '#value' => $node->$extra_attribute);
  }

  return $form;
}




/**
 * Implementation of hook_validate().
 */
function hosting_site_validate($node, &$form) {
  global $user;
  $url = strtolower(trim($node->title)); // domain names are case-insensitive
  if (!_hosting_valid_fqdn($url)) {
    form_set_error('title', t("You have not specified a valid url for this site."));
  }
   
  if (!$node->new_client) {
    $client = hosting_get_client($node->client);
    if (!$node->client || !$client) {
      form_set_error('client', t('Please fill in a valid client'));
    }
    if (!user_access('administer clients') && !array_key_exists($client->nid, hosting_get_client_from_user($user->uid))) {
      form_set_error('client', t('Access denied to client @client', array('@client' => $client->title)));
    }
    $node->client = $client->nid;
  }
  # TODO: maybe we should allow creation of sites that conflict with HOSTING_SITE_DISABLED (which would then need to be renamed before being re-enabled)
  if (!hosting_domain_allowed($url, (array) $node)) {
    form_set_error('title', t("The domain name you have specified is already in use."));
  }

  if (!array_key_exists($node->profile, hosting_get_profiles($node->platform))) {
    form_set_error('profile', t('Please fill in a valid profile'));
  }

  // Check that we are selecting a valid language for this profile, but only when a new site is created. 
  if (!array_key_exists($node->site_language, hosting_get_profile_languages($node->profile, $node->platform)) && !$node->nid) {
    form_set_error('site_language', t('Please fill in a valid language'));
  }

}

/**
 * Implementation of hook_form_alter().
 * Hide the delete button on site nodes
 */
function hosting_site_form_alter(&$form, &$form_state, $form_id) {
  // Remove delete button from site edit form, unless the site's already been deleted via the Delete task
  if ($form_id == 'site_node_form') {
    $node = $form['#node'];
    if ($node->site_status !== '-2') {
      $form['buttons']['delete']['#type'] = 'hidden';
    }
  }
}
