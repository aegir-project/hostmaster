<?php
/**
 * @file
 *   Control client node access
 */

/**
 * Implementation of hook_user().
 */
function hosting_client_user($op, &$edit, &$user, $category = NULL) {
  switch ($op) {

  case 'load':
    $user->client_id = hosting_get_client_from_user($user->uid);
    break;

  case 'view':
    return hosting_client_user_view($user);

  case 'form':
    return hosting_client_user_form($edit, $user, $category);

  case 'validate':
    return hosting_client_user_form_validate($edit);
 
  case 'submit':
    return hosting_client_user_form_submit($edit, $user);

  case 'delete':
    db_query('DELETE FROM {hosting_client_user} WHERE user = %d', $user->uid);
  }
}


function hosting_client_user_view($user) {
  if ($user->client_id) {
    $client = hosting_get_client($user->client_id);

    $items['client'] = array(
      'title' => t('Hosting client'),
      'value' => $client->title,
      'class' => 'client',
    );

    return array(t('Client') => $items);
  }
}

function hosting_client_user_form($edit, $user, $category) {
  if (user_access('administer client users')) {
    $client = hosting_get_client($user->client_id);
    $fields[$category]['hosting_client'] = array(
      '#type' => 'textfield',
      '#title' => t('Client'),
      '#autocomplete_path' => 'hosting_client/autocomplete/client',
      '#default_value' => $client->title,
      '#weight' => 1,
    );

    return $fields;
  }
}

function hosting_client_user_form_validate($edit) {
  if (array_key_exists('hosting_client', $edit)) {
    if (!$client = hosting_get_client($edit['hosting_client'])) {
      form_set_error('hosting_client', 'Please fill in a valid client');
    }
  }
}

function hosting_client_user_form_submit($edit, $user) {
  if (array_key_exists('hosting_client', $edit)) {
    $client = hosting_get_client($edit['hosting_client']);
    $query = db_query('UPDATE {hosting_client_user} SET client = %d WHERE user = %d', $client->nid, $user->uid);
    if (!db_affected_rows()) {
      $query = db_query('INSERT INTO {hosting_client_user} (client, user) VALUES (%d, %d)', $client->nid, $user->uid);
    }
  }
}



/**
 * Simple function to make sure we don't respond with grants when disabling
 * ourselves.
 */
function hosting_client_disabling($set = NULL) {
  static $disabling = false;
  if ($set !== NULL) {
    $disabling = $set;
  }
  return $disabling;
}


/**
 * Implementation of hook_node_grants().
 *
 * Tell the node access system what GIDs the user belongs to for each realm.
 * In this example, we are providing two realms: the example realm, which
 * has just one group id (1) and the user is either a member or not depending
 * upon the operation and the access permission set.
 *
 * We are also setting up a realm for the node author, though, to give it
 * special privileges. That has 1 GID for every UID, and each user is
 * automatically a member of the group where GID == UID.
 *
 */
function hosting_node_grants($account, $op) {
  $account->client_id = hosting_get_client_from_user($account->uid);
  if ($account->client_id) {
    $types = array_merge(hosting_feature_node_types(), array('site', 'task', 'package', 'package_release', 'client')); 
    foreach ($types as $type) {
      if (user_access("$op $type")) {
        $grants['hosting ' . $type] = array($account->client_id);
      }
    }
    return $grants;
  }
}

/**
 * Implementation of hook_node_access_records().
 *
 * All node access modules must implement this hook. If the module is
 * interested in the privacy of the node passed in, return a list
 * of node access values for each grant ID we offer. Since this
 * example module only offers 1 grant ID, we will only ever be
 * returning one record.
 */
function hosting_client_node_access_records($node) {
  if (hosting_client_disabling()) {
    return;
  }

  $base_grant = array(
      'realm' => 'hosting ' . $node->type, 
      'grant_view' => TRUE, 
      'grant_update' => TRUE, 
      'grant_delete' => FALSE,
      'priority' => 1,
    );

  // tasks inherit from their parent
  if ($node->type == 'task') {
    $node = node_load($node->rid);
    $base_grant['grant_update'] = FALSE;
  }

  switch ($node->type) {
    case 'site' :
      $base_grant['gid'] = $node->client;
      break;
    case 'client' :
      $base_grant['gid'] = $node->nid;
      break;
    case 'package' :
    case 'package_release' : 
      $base_grant['grant_update'] = FALSE;
    case 'task' :
    case 'platform' :
    case 'web_server' :
    case 'db_server' :
      // The rest of the node types are configuration, so only admin should see them. 
      $base_grant['gid'] = HOSTING_DEFAULT_CLIENT;
      break;
    default :
      //Not hosting node, don't change access.
      return;
  }

  if ($base_grant['gid']) {
    $grants[] = $base_grant;
    if ($base_grant['gid'] != HOSTING_DEFAULT_CLIENT) {
      // Also give full access to the administrator user.
      $base_grant['gid'] = 1;
      $grants[] = $base_grant;
    }
    return $grants;
  }
}

function hosting_get_client_from_user($uid) {
  return db_result(db_query("SELECT client FROM {hosting_client_user} WHERE user=%d", $uid));
}
