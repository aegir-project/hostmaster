<?php
/**
 * @file Platform node type definition
 */

function hosting_map_values_platform($node) {
  $values['publish_path'] = $node->field_publish_path[0]['value'];

  # @TODO: get web server details. for distributed operation.
  return $values;
}

/**
 * Implementation of hook_form().
 */
function hosting_platform_form(&$node) {
  $type = node_get_types('type', $node);

  $form['publish_path'] = array(
    '#type' => 'textfield',
    '#title' => t('publish_path'),
    '#required' => TRUE,
    '#description' => t('The path on the filesystem where the sites will be hosted.'),
    '#size' => 40,
    '#maxlength' => 255,
  );

  $form['web_server'] = array(
    '#type' => 'radios',
    '#title' => t('Web server'),
    '#description' => t('The web server the sites will be hosted on.'),
    '#options' => 
    '#default_value' => ($node->web_server) ? $node->web_server : HOSTING_DEFAULT_WEB_SERVER,
  );
  
  return $form;
}



/**
 * Implementation of hook_insert().
 */
function hosting_platform_insert($node) {
  db_query("INSERT INTO {hosting_platform} (vid, nid, publish_path, web_server) VALUES (%d, %d, '%s', %d)",
    $node->vid, $node->nid, $node->publish_path, $node->web_server);
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_platform_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_platform_insert($node);
  }
  else {
    db_query("UPDATE {hosting_platform} SET nid=%d, publish_path = '%s', web_server = %d)",
              $node->nid, $node->publish_path, $node->web_server, $node->vid);
  }
}

function hosting_nodeapi_platform_delete_revision(&$node) {
  db_query('DELETE FROM {hosting_platform} WHERE vid = %d', $node->vid);
}

/**
 * Implementation of hook_delete().
 */
function hosting_platform_delete($node) {
  db_query('DELETE FROM {hosting_platform} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 */
function hosting_platform_load($node) {
  $additions = db_fetch_object(db_query('SELECT publish_path, web_server FROM {hosting_platform} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_platform_view($node, $teaser = FALSE, $page = FALSE) {
  $web_server = node_load($node->web_server);
  $node->content['web_server'] = array(
    '#type' => 'item',
    '#title' => t('Web server'),
    '#value' => l($web_server->title, "node/" . $node->web_server),
  );
  $node->content['publish_path'] = array(
    '#type' => 'item',
    '#title' => t('Publish path'),
    '#value' => $node->publish_path,
  );
  
  $view = views_get_view('actions_embed');
  $node->content['actions_view'] = array(
    '#type' => 'item',
    '#title' => t("Actions"),
    '#value' => views_build_view('embed', $view, array($node->nid)), 
    '#weight' => 10
  );
  return $node;
}