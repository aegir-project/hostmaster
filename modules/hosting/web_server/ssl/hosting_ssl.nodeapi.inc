<?php

/**
 * This SSL key is a custom, Aegir generated one.
 */
define('HOSTING_SSL_CUSTOM_KEY', 'null');

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
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
    '#weight' => -9,
  ), 'hosting_ssl_status_options', $ssl_available);

  _hosting_site_field($form, $node, 'ssl_key', array(
    '#type' => 'radios',
    '#title' => t('Encryption key'),
    '#description' => t("Choose an existing SSL certificate. If you do not wish to use any of your existing certificates, you may choose to generate a new one."),
    '#options' => hosting_ssl_get_keys(null, TRUE),
    '#required' => TRUE,
    '#default_value' => ($node->ssl_key) ? $node->ssl_key : HOSTING_SSL_CUSTOM_KEY,
    '#access' => user_access('administer ssl'),
    '#weight' => -8,
  ), 'hosting_ssl_output_key', $ssl_available, !$node->nid);

  _hosting_site_field($form, $node, 'ssl_key_new', array(
    '#type' => 'textfield',
    '#title' => t('New encryption key'),
    '#description' => t("This field should only contain lower case alpha-numeric and '_', '-' or '.' characters. If the SSL certificate is not found, Aegir will automatically generate a self-signed certificate for you."),
    '#default_value' => '',
    '#access' => user_access('administer ssl'),
    '#weight' => -7,
  ), 'filter_xss', $ssl_available, FALSE);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
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


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_view(&$node, $teaser = false) {
  $node->content['info']['ssl_enabled'] = array(
    '#type' => 'item',
    '#title' => t('Encryption'),
    '#value' => hosting_ssl_status_options((!is_null($node->ssl_enabled)) ? $node->ssl_enabled : HOSTING_SSL_DISABLED),
    '#weight' => 6,
  );

  if ($node->ssl_enabled != 0) {
    $node->content['info']['ssl_key'] = array(
      '#type' => 'item',
      '#title' => t('Encryption key'),
      '#value' => hosting_ssl_output_key($node->ssl_key),
      '#description' => hosting_ssl_get_key($node->ssl_key) ? t("This site is using SSL certificates located at %path.", array('%path' => sprintf("config/ssl.d/%s/", hosting_ssl_output_key($node->ssl_key)))) : t("(key deleted)"),
      '#weight' => 7,
    );
  }
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_get_key($cid) {
  static $cache = array();

  if (!isset($cache[$cid])) {
    $cache[$cid] = db_query("SELECT ssl_key FROM {hosting_ssl_cert} WHERE cid = :cid", array(':cid' => $cid))->fetchField();
  }
  return $cache[$cid];
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_get_ip($cid) {
  return db_query("SELECT ips.ip_address FROM {hosting_ssl_cert_ips} cert INNER JOIN {hosting_ip_addresses} ips ON cert.ip_address = ips.id WHERE cid = :cid", array(':cid' => $cid))->fetchField();
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
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
    HOSTING_SSL_REQUIRED => t('Required'),
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

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
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

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_presave(&$node) {
  if (!isset($node->ssl_key)) {
    $node->ssl_key = HOSTING_SSL_CUSTOM_KEY;
  }
  if (!isset($node->ssl_enabled)) {
    $node->ssl_enabled = HOSTING_SSL_DISABLED;
  }
  // If the user has specified a new SSL key to use, sanitise the name.
  if (($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) && isset($node->ssl_key_new)) {
    $node->ssl_key = hosting_ssl_filter_key($node->ssl_key_new);
  }
  // this creates the SSL key for the site, if required
  $node->ssl_key = hosting_ssl_save_key($node);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_insert($node) {
  // TODO Please review the conversion of this statement to the D7 database API syntax.
  /* db_query("INSERT INTO {hosting_ssl_site} (vid, nid, ssl_enabled, ssl_key) VALUES (%d, %d, %d, %d)", $node->vid, $node->nid, $node->ssl_enabled, $node->ssl_key) */
  $id = db_insert('hosting_ssl_site')
  ->fields(array(
    'vid' => $node->vid,
    'nid' => $node->nid,
    'ssl_enabled' => $node->ssl_enabled,
    'ssl_key' => $node->ssl_key,
  ))
  ->execute();
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_update($node) {
  // check if an existing record is there
  $result = db_query("SELECT ssl_enabled FROM {hosting_ssl_site} WHERE vid = :vid", array(':vid' => $node->vid));
  if (!($obj = $result->fetch())) {
    hosting_ssl_nodeapi_site_insert($node);
  }
  else {
    // TODO Please review the conversion of this statement to the D7 database API syntax.
    /* db_query("UPDATE {hosting_ssl_site} SET ssl_enabled=%d, ssl_key=%d WHERE vid=%d", $node->ssl_enabled, $node->ssl_key, $node->vid) */
    db_update('hosting_ssl_site')
  ->fields(array(
    'ssl_enabled' => $node->ssl_enabled,
    'ssl_key' => $node->ssl_key,
  ))
  ->condition('vid', $node->vid)
  ->execute();
  }
  if ($node->site_status == HOSTING_SITE_DELETED) {
    hosting_ssl_clean_keys($node);
  }
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_load($node) {
  $result = db_query("SELECT ssl_enabled, ssl_key FROM {hosting_ssl_site} WHERE vid = :vid", array(':vid' => $node->vid));
  return $result->fetchAssoc();
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_delete($node) {
  // TODO Please review the conversion of this statement to the D7 database API syntax.
  /* db_query("DELETE FROM {hosting_ssl_site} WHERE nid=%d", $node->nid) */
  db_delete('hosting_ssl_site')
  ->condition('nid', $node->nid)
  ->execute();
  hosting_ssl_clean_keys($node);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_ssl_nodeapi_site_delete_revision($node) {
  // TODO Please review the conversion of this statement to the D7 database API syntax.
  /* db_query("DELETE FROM {hosting_ssl_site} WHERE vid=%d", $node->vid) */
  db_delete('hosting_ssl_site')
  ->condition('vid', $node->vid)
  ->execute();
}


/**
 * Store the SSL Cert key in the database.
 */
function hosting_ssl_save_key($node) {
  if (empty($node->ssl_enabled)) {
    return 0;
  }

  $client = hosting_get_client($node->client);

  $ssl_key = $node->ssl_key_new;
  if ($ssl_key) {
    $result = db_query("SELECT * FROM {hosting_ssl_cert} WHERE ssl_key = :ssl_key", array(':ssl_key' => $ssl_key));
    if ($obj = $result->fetch()) {
      // update
      if ($node->client != null) {
        $obj->client = $client->nid;
      }
      drupal_write_record("hosting_ssl_cert", $obj, 'cid');
      $node->ssl_key = $obj->cid;
    }
    else {
      // insert
      $obj = new stdClass();
      $obj->ssl_key = $ssl_key;
      $obj->client = $client->nid;
      $obj->status = 0;
      drupal_write_record("hosting_ssl_cert", $obj);
      if (!hosting_ip_allocate($obj, $node)) {
        form_set_error('ssl_key_new', t("Unable to allocate IP address for certificate, disabling SSL. Allocate more IP addresses to this server then try to enable SSL again."));
        // TODO Please review the conversion of this statement to the D7 database API syntax.
        /* db_query("DELETE FROM {hosting_ssl_cert} WHERE cid = %d", $obj->cid) */
        db_delete('hosting_ssl_cert')
  ->condition('cid', $obj->cid)
  ->execute();
        $obj->cid = FALSE;
        $node->ssl_enabled = HOSTING_SSL_DISABLED;
      }
      $node->ssl_key = $obj->cid;
    }

    return $obj->cid;
  }

  return $node->ssl_key;
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

  while ($obj = $result->fetch()) {
    if (sizeof($obj->ssl_key)) {
      $keys[$obj->cid] = $obj->ssl_key;
    }
  }

  return $keys;
}

/**
 * Remove unused SSL keys from the system (but not from the backend).
 *
 * This is designed to be ran on a site's delete task or the site node's deletion.
 */
function hosting_ssl_clean_keys($node) {
  // check if there are still sites using this site's certificate
  if (!db_query("SELECT * FROM hosting_ssl_site siteA INNER JOIN hosting_ssl_site siteB ON siteA.ssl_key = siteB.ssl_key INNER JOIN hosting_site s ON s.nid = siteA.nid WHERE siteA.ssl_enabled = :siteA.ssl_enabled AND s.status <> :s.status AND siteB.nid = :siteB.nid;", array(':siteA.ssl_enabled' => 1, ':s.status' => HOSTING_SITE_DELETED, ':siteB.nid' => $node->nid))->fetchField()) {
    drupal_set_message(t("cleaning up unused certificate associated with site"));
    // TODO Please review the conversion of this statement to the D7 database API syntax.
    /* db_query("DELETE FROM {hosting_ssl_cert} WHERE cid = %d", $node->ssl_key) */
    db_delete('hosting_ssl_cert')
  ->condition('cid', $node->ssl_key)
  ->execute();
    // TODO Please review the conversion of this statement to the D7 database API syntax.
    /* db_query("DELETE FROM {hosting_ssl_cert_ips} WHERE cid = %d", $node->ssl_key) */
    db_delete('hosting_ssl_cert_ips')
  ->condition('cid', $node->ssl_key)
  ->execute();
  }
}
