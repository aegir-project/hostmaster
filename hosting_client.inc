<?php
/**
 * @file Web server node type is defined here.
 */

/**
 * Implementation of hook_form().
 */
function hosting_client_form(&$node) {
  $type = node_get_types('type', $node);

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full name'),
    '#size' => 40,
    '#maxlength' => 255,
  );

  $form['organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization'),
    '#size' => 40,
    '#maxlength' => 255,
  );


  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email address'),
    '#size' => 40,
    '#maxlength' => 255,
  );
  
  return $form;
}



/**
 * Implementation of hook_insert().
 */
function hosting_client_insert($node) {
  db_query("INSERT INTO {hosting_client} (vid, nid, action_type, rid, executed, action_status) VALUES (%d, %d, '%s', %d, %d, %d)",
    $node->vid, $node->nid, $node->action_type, $node->rid, $node->executed, $node->action_status);
  
  $queue = _hosting_get_queue();
  $subqueue = _hosting_get_subqueue();
  nodequeue_subqueue_add($queue, $subqueue, $node->nid);    
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_client_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_client_insert($node);
  }
  else {
    db_query("UPDATE {hosting_client} SET nid=%d, action_type = '%s', rid = %d, executed=%d, action_status=%d)",
              $node->nid, $node->action_type, $node->rid, $node->executed, $node->action_status, $node->vid);
  }
}

function hosting_nodeapi_web_server_delete_revision(&$node) {
  db_query('DELETE FROM {hosting_client} WHERE vid = %d', $node->vid);
}

/**
 * Implementation of hook_delete().
 */
function hosting_client_delete($node) {
  db_query('DELETE FROM {hosting_client} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 */
function hosting_client_load($node) {
  $additions = db_fetch_object(db_query('SELECT ip_address, script_user, web_group, config_path, backup_path FROM {hosting_client} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_client_view($node, $teaser = FALSE, $page = FALSE) {
  $view = views_get_view('sites_embed');
  $node->content['sites_view'] = array(
    '#type' => 'item',
    '#title' => t("Sites"),
    '#value' => views_build_view('embed', $view, array($node->nid)), 
    '#weight' => 10
  );
  return $node;
}
