<?php
// $Id$

/**
 * @file Site node form.
 */


/**
 * Helper function to generate form elements for the site form.
 */
function _hosting_site_field(&$form, $node, $item, $element, $filter_display ='filter_xss', $editable = FALSE, $show_desc = TRUE) {
  $css_id = str_replace("_", "-", $item);

  $type = $element['#type'];

  if (!$node->nid || $editable) {
    // create it
    if (($element['#type'] == 'radios') && !sizeof($element['#options'])) {
      $form[$item] = array('#type' => 'hidden', '#value' => key($element['#options']));
    }
    else {
      $form[$item] = $element;
    }

    if ($show_desc) {
      // the text to display when there are no valid options to select
      $form[$item . '_description'] = array(
        '#prefix' => "<div class='hosting-site-field-description' id='hosting-site-field-{$css_id}-description'>",
        '#suffix' => '</div>',
        '#type' => 'item',
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#value' => "<div class='placeholder'>" . $filter_display($element['#default_value']) . "</div>",
      );
      if (isset($element['#weight'])) {
        $form[$item . '_description']['#weight'] = $element['#weight'];
      }
    }
  }
  else {
    $type = 'display';

    if ($show_desc) {
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
    }

    $form[$item] = array('#type' => 'hidden', '#value' => $element['#default_value']);
  }

  $form[$item]['#hosting_site_field'] = $item;
  $form[$item]['#hosting_site_field_value'] = $element['#default_value'];
  $form[$item]['#prefix'] = "<div class='hosting-site-field hosting-site-field-{$type}' id='hosting-site-field-{$css_id}'>";
  $form[$item]['#suffix'] = "</div>";
}

/**
 * pre_render form callback
 *
 * Add the needed .js and .css files to render the form correctly before printing it
 */
function _hosting_site_form_pre_render($elements) {

  drupal_add_js(drupal_get_path('module', 'hosting_site') . '/hosting_site.form.js');
  return $elements;
}


/**
 * Pass in a site node and return an array of valid options for it's fields.
 *
 * Modules can define the hook_hosting_site_options_alter function to modify which
 * fields are available for selection.
 *
 */
function hosting_site_available_options($node) {
  // cast to object if it's an array.
  $node = (is_array($node)) ? (object) $node : clone $node;

  $return = array();

  $return['profile'] = array();
  $return['platform'] = array();
  $return['site_language'] = array();

  if (!hosting_feature('client')) {
    // Setting the return value of a text field to null, 
    // will signal to the front end that the field needs to
    // be displayed, but is not editable.
    $return['client'] = null;
  }

  // Load up the user we'll use to check platform and profile access
  $user = user_load($GLOBALS['user']->uid);

  // Install profiles 
  $profiles = hosting_get_profiles();
  foreach($profiles as $id => $name) {

    // Don't allow a site to be provisioned with hostslave or hostmaster profile
    if (in_array($name, array('Hostslave', 'Hostmaster'))) {
      unset($profiles[$id]);
    }

    // Trim down the list of profiles to those that are available and the user has access to
    // XXX This hack (next 22 lines) hides profiles that can't be accessed
    // Eventually we should lighten up the content of this callback
    $result = db_query("SELECT l.nid FROM hosting_package_instance i 
                        JOIN hosting_package p ON p.nid = i.package_id 
                        JOIN hosting_platform l ON l.nid = i.rid WHERE i.package_id = %d 
                        AND p.package_type = 'profile' AND l.status = %d;", $id, HOSTING_PLATFORM_ENABLED);
    $allowed_plats = _hosting_get_allowed_platforms($user->uid);
    $access_check = FALSE;

    while ($row = db_fetch_array($result)) {
      if (array_key_exists($row['nid'], $allowed_plats)) {
        $access_check = TRUE;
      }
      // open access if no platform access has been set
      // @todo move this into _hosting_get_allowed_platforms
      else if (!$unrestricted = db_result(db_query("SELECT cid FROM {hosting_platform_client_access} WHERE pid = '%d' LIMIT 1", $row['nid']))) {
        $access_check = TRUE;
      }
    }
    if (!$access_check) {
      unset($profiles[$id]);
    }
  }
  reset($profiles);
  $return['profile'] = array_keys($profiles);

  if (!isset($node->profile)) {
    $node->profile = hosting_get_default_profile($return['profile'][0]);
  }

  // filter the available platforms based on which clients the user has access to.
  $options = array();
  $platforms = hosting_get_profile_platforms($node->profile, isset($node->check_profile_migrations) ? $node->check_profile_migrations : FALSE);
  if (sizeof($platforms)) {
    foreach ($platforms as $nid => $title) {
      $platform = node_load($nid);

      if ($platform->platform_status != HOSTING_PLATFORM_LOCKED) {
        if (!isset($platform->clients) || sizeof(array_intersect(array_keys($user->client_id), $platform->clients)) || $user->uid == 1) {
          $options[] = $nid;
        }
      }
    }

    $return['platform'] = $options;
  }

  if (!isset($node->platform) || !in_array($node->platform, $return['platform'])) {
    $node->platform = $return['platform'][0];
  }

  $return['site_language'] = array_keys((array) hosting_get_profile_languages($node->profile, $node->platform));

  drupal_alter('hosting_site_options', $return, $node);

  return $return;
}



/**
 * Implementation of hook_form
 */
function hosting_site_form($node) {
  $form['#pre_render'][] = '_hosting_site_form_pre_render';
  $form['#node'] = $node;

  if ($node->nid) {
    $form['info'] = array(
      '#prefix' => '<div class="clear-block" id="hosting-site-edit-info">',
      '#suffix' => '<br /></div>',
      '#weight' => -10,
    );
  }

  _hosting_site_field($form, $node, 'title', array(
      '#type' => 'textfield',
      '#title' => t('Domain name'),
      '#required' => TRUE,
      '#default_value' => $node->title,
      '#weight' => -10
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

  // Install profiles 
  $profiles = hosting_get_profiles();
  foreach($profiles as $id => $name) {
    // Don't allow a site to be provisioned with hostslave or hostmaster profile
    if (in_array($name, array('Hostslave', 'Hostmaster'))) {
      unset($profiles[$id]);
    }
  }
  reset($profiles);

  _hosting_site_field($form, $node, 'profile', array(
      '#type' => 'radios',
      '#title' => t('Install profile'),
      '#description' => t('The type of site to install.<br />
                           The profile selected here determines the list of supported platforms below.'),
      '#options' => $profiles,
      '#default_value' => $node->profile ? $node->profile : hosting_get_default_profile(key($profiles)),
      '#required' => TRUE,
      '#attributes' => array('class' => "hosting-site-form-profile-options"),
    ), '_hosting_node_link');



  _hosting_site_field($form, $node, 'platform', array(
     '#type' => 'radios',
     '#title' => t('Platform'),
     '#required' => TRUE,
     '#description' => t('The platform you want the site to be hosted on.<br />
                          Not seeing a certain platform? Platforms shown are those that support the profile above.
                          If a different profile is selected, this list may change automatically.'),
     '#options' => _hosting_get_platforms(),
     '#default_value' => ($node->platform) ? $node->platform : null,
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
        '#description' => t('The database server the site will use to host its content.'),
        '#options' => hosting_get_servers('db'),
        '#default_value' => $node->db_server ? $node->db_server : HOSTING_DEFAULT_DB_SERVER,
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

  $valid_options = hosting_site_available_options($node);
 
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

  // If the quota module is loaded, check the site quota
  if (function_exists('hosting_site_quota_exceeded')) {
    $quota_error = hosting_site_quota_exceeded((array) $node);
    if ($quota_error) {
      form_set_error('title', $quota_error);
    }
  }

  if (!in_array($node->profile, $valid_options['profile']) && !$node->nid) {
    form_set_error('profile', t('Please choose a valid profile'));
  }

  if (!in_array($node->platform, $valid_options['platform'])) {
    form_set_error('platform', t('Please choose a valid platform'));
  }

  // Check that we are selecting a valid language for this profile, but only when a new site is created. 
  if (!in_array($node->site_language, $valid_options['site_language']) && !$node->nid) {
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
