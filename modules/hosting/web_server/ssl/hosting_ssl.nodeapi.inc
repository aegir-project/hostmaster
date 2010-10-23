<?php

define('HOSTING_SSL_CUSTOM_KEY', 'null');

function hosting_ssl_site_form(&$form, &$form_state, $form_id) {
  $node = $form['#node'];

  $ssl_available = FALSE;
  // Only allow the user to modify these values when the platform is SSL enabled.
  if ($node->nid) {
    $platform = node_load($node->platform);
    $server = node_load($platform->web_server);

    if ($server->services['http']->ssl_enabled) {
      $ssl_available = TRUE;
    }
  }

  _hosting_site_field($form, $node, 'ssl_enabled', array(
      '#type' => 'radios',
      '#title' => t('Encryption'),
      '#options' => hosting_ssl_status_options(),
      '#description' => t('Enabling encryption will publish your site on both HTTP and HTTPS ports, allowing you to redirect users to the more secure version for certain pages that require the additional security. Requiring encryption will automatically redirect all unencrypted traffic to your HTTPS site.'),
      '#required' => TRUE,
      '#default_value' => (isset($node->ssl_enabled)) ? $node->ssl_enabled : HOSTING_SSL_DISABLED,
      '#access' => user_access('administer ssl'),
      '#weight' => -9
    ), 'hosting_ssl_status_options', $ssl_available);

  _hosting_site_field($form, $node, 'ssl_key', array(
      '#type' => 'radios',
      '#title' => t('Encryption key'),
      '#description' => t("Choose an existing SSL certificate. If you do not wish to use any of your existing certificates, you may choose to generate a new one."),
      '#options' => hosting_ssl_get_keys(null, TRUE),
      '#required' => TRUE,
      '#default_value' => ($node->ssl_key) ? $node->ssl_key : HOSTING_SSL_CUSTOM_KEY,
      '#access' => user_access('administer ssl'),
      '#weight' => -8
    ), 'hosting_ssl_output_key', $ssl_available, !$node->nid);

  _hosting_site_field($form, $node, 'ssl_key_new', array(
      '#type' => 'textfield',
      '#title' => t('New encryption key'),
      '#description' => t("This field should only contain lower case alpha-numeric and '_', '-' or '.' characters. If the SSL certificate is not found, Aegir will automatically generate a self-signed certificate for you."),
      '#default_value' => '',
      '#access' => user_access('administer ssl'),
      '#weight' => -7
    ), 'filter_xss', $ssl_available, FALSE);
}

function hosting_ssl_hosting_site_options_alter(&$return, $node) {
  // Disable the ssl key fields by default.
  if (!sizeof(hosting_ssl_get_servers())) {
    $return['ssl_enabled'] = FALSE;
  }

  $return['ssl_key'] = false;
  $return['ssl_key_new'] = false;

  // Test if ssl has been enabled.
  if ($node->ssl_enabled != 0) {

    $keys = hosting_ssl_get_keys($node->client, TRUE);

    // return the list of valid keys, including the special 'new key' option.
    $return['ssl_key'] = array_keys($keys);

    // properly default this value so things dont fall apart later.
    if (sizeof($return['ssl_key']) == 1) {
      $node->ssl_key = HOSTING_SSL_CUSTOM_KEY;
    }
    
    // the user has chosen to enter a new key
    if ($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) {
      // default the new key to the site's domain name, after filtering.
      $default = hosting_ssl_filter_key($node->title);
      $return['ssl_key_new'] = (!empty($default)) ? $default : true;
    }

    // we need to ensure that the return value is properly indexed, otherwise it
    // gets interpreted as an object by jquery.
    $return['profile'] = array_values(array_intersect($return['profile'], hosting_ssl_get_profiles()));

    $return['platform'] = array_values(array_intersect($return['platform'], hosting_ssl_get_platforms()));
  }
}


function hosting_ssl_nodeapi_site_view(&$node, $teaser = false) {
  $node->content['info']['ssl_enabled'] = array(
    '#type' => 'item',
    '#title' => t('Encryption'),
    '#value' => hosting_ssl_status_options((!is_null($node->ssl_enabled)) ? $node->ssl_enabled : HOSTING_SSL_DISABLED),
    '#weight' => 6
  );

  if ($node->ssl_enabled != 0) {
    $path = sprintf("config/ssl.d/%s/", hosting_ssl_get_key($node->ssl_key));
    $node->content['info']['ssl_key'] = array(
      '#type' => 'item',
      '#title' => t('Encryption key'),
      '#value' => hosting_ssl_output_key($node->ssl_key),
      '#description' => t("This site is using the SSL certificate and SSL key located at %crt_file and %key_file.", array('%crt_file' => $path . 'openssl.crt', '%key_file' => $path . 'openssl.key')),
      '#weight' => 7
    );
  }
}

function hosting_ssl_get_key($cid) {
  static $cache = array();

  if (!isset($cache[$cid])) {
    $cache[$cid] = db_result(db_query("SELECT ssl_key FROM {hosting_ssl_cert} WHERE cid=%d", $cid));
  }
  return $cache[$cid];
}

function hosting_ssl_output_key($cid) {
  return filter_xss(hosting_ssl_get_key($cid));
}

/**
 * Output filter for SSL enabled field.
 */
function hosting_ssl_status_options($status = null) {
  $options = array(
    HOSTING_SSL_DISABLED => t('Disabled'),
    HOSTING_SSL_ENABLED => t('Enabled'),
    HOSTING_SSL_REQUIRED => t('Required')
  );

  if (!is_null($status)) {
    return $options[$status];
  }
  return $options;
  
}


/**
 * Filter disallowed characters from a ssl certificate key.
 *
 * Only lowercase alphanumeric- and '.', '_' or '-' characters are allowed for ssl keys.
 */
function hosting_ssl_filter_key($key) {
  return strtolower(preg_replace("/[^\w\.\-]/", "", $key));
}

function hosting_ssl_nodeapi_site_validate($node, &$form) {
  if ($node->ssl_enabled) {
    // TODO: find a way to avoid calling this function multiple times in hook_validate
    $valid_options = hosting_site_available_options($node);

    if ($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) {
      if (!strlen($node->ssl_key_new)) {
        form_set_error('ssl_key_new', t("The encryption key field is required to enable us to generate a new SSL certificate for your site."));
      }
      else {
        $key = hosting_ssl_filter_key($node->ssl_key_new);

        if (($node->ssl_key_new != $key) || !strlen($key)) {
          form_set_error('ssl_key_new', t("The encryption key field should only contain lower case alpha-numeric and '_', '-' or '.' characters."));
        }

        if (!ctype_alnum($key[0])) {
          form_set_error('ssl_key_new', t("The encryption key field must start with an alpha-numeric character."));
        }

        if ($key == HOSTING_SSL_CUSTOM_KEY) {
          form_set_error('ssl_key_new', t("This encryption key value is reserved for internal use, please choose another"));
        }
      }
    }
    else {
      if (!in_array($node->ssl_key, $valid_options['ssl_key'])) {
        form_set_error('ssl_key', t("You have chosen an invalid SSL key"));
      }
    }
  }
}

function hosting_ssl_nodeapi_site_presave(&$node) {
  if ($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) {
    $node->ssl_key = hosting_ssl_filter_key($node->ssl_key_new);
  }
}

function hosting_ssl_nodeapi_site_insert($node) {
  db_query("INSERT INTO {hosting_ssl_site} (vid, nid, ssl_enabled, ssl_key) VALUES (%d, %d, %d, %d)", $node->vid, $node->nid, $node->ssl_enabled, hosting_ssl_save_key($node->ssl_key, $node->ssl_enabled, $node->client));
}

function hosting_ssl_nodeapi_site_update($node) {
  // check if an existing record is there
  $result = db_query("SELECT ssl_enabled FROM {hosting_ssl_site} WHERE vid=%d", $node->vid);
  if (!($obj = db_fetch_object($result))) {
    hosting_ssl_nodeapi_site_insert($node);
  }
  else {
    db_query("UPDATE {hosting_ssl_site} SET ssl_enabled=%d, ssl_key=%d WHERE vid=%d", $node->ssl_enabled, hosting_ssl_save_key($node->ssl_key, $node->ssl_enabled, $node->client), $node->vid);
  }
}

function hosting_ssl_nodeapi_site_load($node) {
  $result = db_query("SELECT ssl_enabled, ssl_key FROM {hosting_ssl_site} WHERE vid=%d", $node->vid);
  $additions = db_fetch_array($result); 
  return $additions;
}

function hosting_ssl_nodeapi_site_delete($node) {
  db_query("DELETE FROM {hosting_ssl_site} WHERE nid=%d", $node->nid);
}

function hosting_ssl_nodeapi_site_delete_revision($node) {
  db_query("DELETE FROM {hosting_ssl_site} WHERE vid=%d", $node->vid);
}


/**
 * Store the SSL Cert key in the database.
 */
function hosting_ssl_save_key($key, $ssl_enabled = TRUE, $client = null) {
  if (!$ssl_enabled) {
    return 0;
  }

  // we only save the new key if it's a string
  if (!is_numeric($key)) {
    $result = db_query("SELECT * FROM {hosting_ssl_cert} WHERE ssl_key = '%s'", $key);
    if ($obj = db_fetch_object($result)) {
      // update
      if ($client != null) {
        $obj->client = $client;
      }
      drupal_write_record("hosting_ssl_cert", $obj, 'cid');
    }
    else {
      // insert
      $obj = new stdClass();
      $obj->ssl_key = $key;
      $obj->client = $client;
      $obj->status = 0;
      drupal_write_record("hosting_ssl_cert", $obj);
    }

    return $obj->cid;
  }

  return $key;
}

/**
 * Retrieve an associated array of possible keys.
 *
 * @param $client
 *   The client to filter the keys by.
 * @param $ask_custom
 *   Include the special 'generate new key' value used by the site form.
 */
function hosting_ssl_get_keys($client = null, $ask_custom = FALSE) {
  $keys = array();

  if ($ask_custom == TRUE) {
    $keys[HOSTING_SSL_CUSTOM_KEY] = t("Generate a new encryption key.");
  }

  $args = array();
  $query = "SELECT cid, ssl_key FROM {hosting_ssl_cert}";

  if (!is_null($client)) {
    $client = hosting_get_client($client);
    if ($client) {
      $query .= " WHERE client = %d";
      $args[] = $client->nid;
    }
  }
  $result = db_query($query, $args);

  while ($obj = db_fetch_object($result)) {
    if (sizeof($obj->ssl_key)) {
      $keys[$obj->cid] = $obj->ssl_key;
    }
  }

  return $keys;
}
