<?php

define(HOSTING_SSL_CUSTOM_KEY, '__custom_key');


function hosting_ssl_site_form(&$form, &$form_state, $form_id) {
  $node = $form['#node'];

  _hosting_site_field($form, $node, 'ssl_enabled', array(
      '#type' => 'radios',
      '#title' => t('Encryption'),
      '#options' => array(0 => t('Disabled'), 1 => t('Enabled'), 2 => t('Required')),
      '#description' => t('Choose wether your site will be encrypted using SSL. Should you choose to enable encryption, your site will still be available through an unencrypted port, allowing you to only redirect your users to the encrypted version of your site for certain pages that require the aditional security. Should you choose to require encryption, all requests to the unencrypted version of your site will be redirected to the more secure version.'),
      '#required' => TRUE,
      '#default_value' => ($node->ssl_enabled) ? $node->ssl_enabled : 0,
      '#weight' => -9
    ), 'filter_xss', TRUE);

  $keys = hosting_ssl_get_keys();
  $keys[HOSTING_SSL_CUSTOM_KEY] = t("Generate a new encryption key.");
  _hosting_site_field($form, $node, 'ssl_key', array(
      '#type' => 'radios',
      '#title' => t('Encryption key'),
      '#description' => t("Choose an identifier for your SSL certificate. This value will be used to determine the path to your key. If you do not wish to use any of your existing certificates, you may choose to generate a new one."),
      '#options' => $keys,
      '#required' => TRUE,
      '#default_value' => ($node->ssl_key) ? $node->ssl_key : HOSTING_SSL_CUSTOM_KEY,
      '#weight' => -8
    ), 'filter_xss', TRUE);

  _hosting_site_field($form, $node, 'ssl_key_new', array(
      '#type' => 'textfield',
      '#title' => t('New encryption key'),
      '#description' => t("This field should only contain lower case alpha-numeric and '_', '-' or '.' characters. If the SSL certificate is not found, Aegir will automatically generate a self-signed certificate for you."),
      '#default_value' => '',
      '#weight' => -7
    ), 'filter_xss', TRUE);
}

function hosting_ssl_hosting_site_options_alter(&$return, $node) {
  $return['ssl_key'] = array();
  $return['ssl_key_new'] = false;

  if ($node->ssl_enabled != 0) {
    $keys = hosting_ssl_get_keys($node->client);
    $keys[HOSTING_SSL_CUSTOM_KEY] = t("Generate a new encryption key.");

    // ssl has been enabled for this site
    $return['ssl_key'] = array_keys($keys);

    if (sizeof($return['ssl_key']) == 1) {
      $node->ssl_key = HOSTING_SSL_CUSTOM_KEY;
    }

    // the user has chosen to enter a new key
    if ($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) {
      // default the new key to the site's domain name. 
      $default = hosting_ssl_filter_key($node->title);
      $return['ssl_key_new'] = (!empty($default)) ? $default : true;
    }

    // Only platforms that are SSL enabled can be chosen now.
    $ssl_servers = hosting_ssl_get_servers();

    $return['ssl_servers'] = $ssl_servers;
    foreach ($return['platform'] as $key => $nid) {
      $platform = node_load($nid);
      
      if (!in_array($platform->web_server, $ssl_servers)) {
        // Remove the platform from the list because SSL is not available for it.
        unset($return['platform'][$key]);
      } 
    }
    // we need to ensure that the return value is properly indexed, otherwise it
    // gets interpreted as an object by jquery.
    $return['platform'] = array_values($return['platform']);
  }
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
  if ($node->ssl_key == HOSTING_SSL_CUSTOM_KEY) {
    if (!$node->ssl_key_new) {
      form_set_error('ssl_key_new', t("The encryption key field is required to enable us to generate a new SSL certificate for your site."));
    }
    else {
      $key = hosting_ssl_filter_key($node->ssl_key_new);
      if ($node->ssl_key_new != $key) {
        form_set_error('ssl_key_new', t("The encryption key field should only contain lower case alpha-numeric and '_', '-' or '.' characters."));
      }
      if ($key == HOSTING_SSL_CUSTOM_KEY) {
        form_set_error('ssl_key_new', t("This encryption key value is reserved for internal use, please choose another"));
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
  db_query("INSERT INTO {hosting_ssl_site} (vid, nid, ssl_enabled, ssl_key) VALUES (%d, %d, %d, '%s')", $node->vid, $node->nid, $node->ssl_enabled, hosting_ssl_save_key($node->ssl_key, $node->client));
}

function hosting_ssl_nodeapi_site_update($node) {
  db_query("UPDATE {hosting_ssl_site} SET ssl_enabled=%d, ssl_key='%s' WHERE vid=%d", $node->ssl_enabled, hosting_ssl_save_key($node->ssl_key, $node->client), $node->vid);
}

function hosting_ssl_nodeapi_site_load($node) {
  $result = db_query("SELECT ssl_enabled, ssl_key FROM {hosting_ssl_site} WHERE vid=%d", $node->vid);
 return db_fetch_object($result); 
}

function hosting_ssl_nodeapi_site_delete($node) {
  db_query("DELETE FROM {hosting_ssl_site} WHERE nid=%d", $node->nid);
}

function hosting_ssl_nodeapi_site_delete_revision($node) {
  db_query("DELETE FROM {hosting_ssl_site} WHERE vid=%d", $node->vid);
}

function hosting_ssl_save_key($key, $client = null) {
  if (sizeof(trim($key))) {
    $result = db_query("SELECT * FROM {hosting_ssl_cert} WHERE ssl_key = '%s'", $key);
    if ($obj = db_fetch_object($result)) {
      // update
      if ($client != null) {
        $obj->client = $client;
      }
      drupal_write_record("hosting_ssl_cert", $obj, array('ssl_key'));
    }
    else {
      // insert
      $obj = new stdClass();
      $obj->ssl_key = $key;
      $obj->client = $client;
      $obj->status = 0;
      drupal_write_record("hosting_ssl_cert", $obj);
    }
  }
}

function hosting_ssl_get_keys($client = null) {
  $keys = array();

  $args = array();
  $query = "SELECT ssl_key FROM {hosting_ssl_cert}";

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
      $keys[$obj->ssl_key] = $obj->ssl_key;
    }
  }
  return $keys;
}
