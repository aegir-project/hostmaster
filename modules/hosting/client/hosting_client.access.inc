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
    if (hosting_feature('client')) {
      return hosting_client_user_view($user);
    }
    break;

  case 'form':
    if (hosting_feature('client')) {
      return hosting_client_user_form($edit, $user, $category);
    }
    break;

  case 'validate':
    return hosting_client_user_form_validate($edit);

  case 'insert':
  case 'update': 
    hosting_client_user_form_submit($edit, $user);
    $edit['hosting_client'] = NULL;
    break;
  case 'submit':
    break;

  case 'delete':
    db_query('DELETE FROM {hosting_client_user} WHERE user = %d', $user->uid);
  }
}


function hosting_client_user_view($user) {
  if ($user->client_id) {

    foreach($user->client_id as $client => $type) {
      $rows[] = array(_hosting_node_link($client));
    }

    // this is a table because we'll have types for clients eventually
    $header = array(t('Hosting client'));
    $items['client_list'] = array(
        'value' => theme('table', $header, $rows),
        'class' => 'client',
        );

    return array(t('Clients') => $items);
  }
}

function hosting_client_user_form($edit, $user, $category) {

  $clients = array();
  if ($user->client_id) {
    foreach($user->client_id as $client => $type) {
      $clients[$client] = '';
      $fields[$category]['name'][$client] = array('#type' => 'markup', '#value' => _hosting_node_link($client));
    }
  }
  if (user_access('edit client users')) {
    $fields[$category]['clients'] = array('#type' => 'checkboxes', '#options' => $clients);
  }
  
  $fields[$category]['header'] = array('#type' => 'value', '#value' => array(array('data' => t('Accessible clients')), array('data' => t('Remove'))));
  
  if (user_access('edit client users')) {
    $fields[$category]['hosting_client'] = array(
        '#type' => 'textfield',
        '#title' => t('Associate a client to this user'),
        '#weight' => 2,
        '#autocomplete_path' => 'hosting_client/autocomplete',
        '#description' => t('This field allows you to associate an existing client to a user. 
                             It does not create a new client, but allows this user
                             to manage sites belonging to the given client.'),
        );
  }
  $fields[$category]['#theme'] = 'hosting_client_user_form';
  return $fields;
}

function theme_hosting_client_user_form($form) {
  if (array_key_exists('hosting_client', $form)) {
    $edit_name = drupal_render($form['hosting_client']);
  }
  else {
    $edit_name = '';
  }
  foreach (element_children($form['name']) as $client) {
    $row = array();
    $row['data'][] = drupal_render($form['name'][$client]);
    if (user_access('edit client users')) {
      $row['data'][] = drupal_render($form['clients'][$client]);
    }
    $rows[] = $row;
  }
  $output = drupal_render($form);
  $output .= theme('table', $form['header']['#value'], $rows);
  $output .= $edit_name;
  return $output;
}

function hosting_client_user_form_validate($edit) {
  if (array_key_exists('hosting_client', $edit) && $edit['hosting_client'] && (!$client = hosting_get_client($edit['hosting_client']))) {
    form_set_error('hosting_client', 'Please fill in a valid client');
  }
}

function hosting_client_user_form_submit($edit, $user) {
  if (array_key_exists('clients', $edit)) {
    foreach ($edit['clients'] as $client) {
      $query = db_query('DELETE FROM {hosting_client_user} WHERE user = %d AND client = %d', $user->uid, $client);
    }
  }
  if (array_key_exists('hosting_client', $edit) && $edit['hosting_client']) {
    $client = hosting_get_client($edit['hosting_client']);
    $query = db_query('INSERT INTO {hosting_client_user} (client, user, contact_type) VALUES (%d, %d, "%s")', $client->nid, $user->uid, '');
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
  $types = array_merge(hosting_feature_node_types(), array('site', 'task', 'package', 'client')); 
  foreach ($types as $type) {
    if (user_access("$op $type")) {
      // TODO: restrict access to certain op-type based on the user relationship to this client - see content of $client_relations
      $grants['hosting ' . $type] = array_keys($account->client_id); 
    }
  }
  return $grants;
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
  $grants = array();
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
      $base_grant['grant_update'] = FALSE;
      break;
    case 'task' :
    case 'server' :
      // The rest of the node types are configuration, so only admin should see them. 
      $base_grant['gid'] = HOSTING_ADMIN_CLIENT;
      break;
    case 'platform' :
      $base_grant['gid'] = HOSTING_ADMIN_CLIENT;
      /*
      if (sizeof($node->clients)) {
        foreach ($node->clients as $cid => $client) {
          if ($cid != HOSTING_ADMIN_CLIENT) {
            $grants[] = array_merge($base_grant, array('gid' => $cid, 'grant_update' => FALSE));
          }
        }
      }
       */
      break;
    default :
      //Not hosting node, don't change access.
      return;
  }

  if ($base_grant['gid']) {
    $grants[] = $base_grant;

    // this should _always_ happen.
    if ($base_grant['gid'] != HOSTING_ADMIN_CLIENT) {
      // Also give full access to the administrator user.
      $base_grant['gid'] = HOSTING_ADMIN_CLIENT;
      $grants[] = $base_grant;
    }
    return $grants;
  }
}

/**
 * Returns an associative array that provides the relationships a user has with different clients
 */
function hosting_get_client_from_user($uid) {
  $clients = array();
  if ($results = db_query("SELECT client, contact_type FROM {hosting_client_user} WHERE user=%d", $uid)) {
    while ($result = db_fetch_array($results)) {
      $clients[$result['client']] = explode(',', $result['contact_type']);
    }
  }
  return $clients;
}
