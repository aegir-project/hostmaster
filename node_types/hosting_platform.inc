<?php
/**
 * @file Platform node type definition
 */

/**
 * Small helper function to get platforms.
*/ 
function _hosting_get_platforms() {
  $return = array();
  $result = db_query("SELECT nid, title FROM {node} WHERE type='platform' AND status=1");
  while($server = db_fetch_object($result)) {
    $return[$server->nid] = $server->title;
  }
  return $return;
}


function hosting_map_values_platform($node) {
  $values['platform'] = $node->nid;
  $values['publish_path'] = $node->publish_path;

  $reference = node_load($node->web_server);
  $values = array_merge($values, hosting_map_values($reference));

  return $values;
}

/**
 * Implementation of hook_form().
 */
function hosting_platform_form(&$node) {
  $type = node_get_types('type', $node);
  $form['publish_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Publish path'),
    '#required' => TRUE,
    '#description' => t('The path on the filesystem where the sites will be hosted.'),
    '#size' => 40,
    '#default_value' => $node->publish_path,
    '#maxlength' => 255,
  );

  $form['web_server'] = array(
    '#type' => 'radios',
    '#title' => t('Web server'),
    '#description' => t('The web server the sites will be hosted on.'),
    '#options' => _hosting_get_web_servers(),
    '#default_value' => ($node->web_server) ? $node->web_server : HOSTING_DEFAULT_WEB_SERVER,
  );
  
  return $form;
}



/**
 * Implementation of hook_insert().
 */
function hosting_platform_insert($node) {
  db_query("INSERT INTO {hosting_platform} (vid, nid, publish_path, release_id, verified, web_server) VALUES (%d, %d, '%s', %d, %d, %d)",
    $node->vid, $node->nid, $node->publish_path, $node->release_id, $node->verified, $node->web_server);
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_platform_update($node) {
  if ($node->nid == variable_get('hosting_own_platform', 4)) {
    variable_set('hosting_platform_path', $node->publish_path);
    #Switching web server here should change the web server.
  }
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_platform_insert($node);
  }
  else {
    db_query("UPDATE {hosting_platform} SET publish_path = '%s', web_server = %d, release_id = %d, verified = %d WHERE nid=%d",
              $node->publish_path, $node->web_server, $node->release_id, $node->verified, $node->nid);
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
  $additions = db_fetch_object(db_query('SELECT publish_path, release_id, verified, web_server FROM {hosting_platform} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_platform_view($node, $teaser = FALSE, $page = FALSE) {
  $node->content['release'] = array(
    '#type' => 'item',
    '#title' => t('Release'),
    '#value' => _hosting_node_link($node->release_id),
  );
 
  $node->content['web_server'] = array(
    '#type' => 'item',
    '#title' => t('Web server'),
    '#value' => _hosting_node_link($node->web_server),
  );

  $node->content['publish_path'] = array(
    '#type' => 'item',
    '#title' => t('Publish path'),
    '#value' => $node->publish_path,
  );
  
  if ($node->verified) {
    $node->content['verified'] = array(
      '#type' => 'item',
      '#title' => t('Verified on'),
      '#value' => format_date($node->verified),
    );
  }

  $view = views_get_view('site_embed_platform');
  $node->content['sites_view'] = array(
    '#type' => 'item',
    '#title' => t("Sites"),
    '#value' => views_build_view('embed', $view, array($node->nid)), 
    '#weight' => 10
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


/**
 * Create the configuration file containing the drush details.
 *
 * Outside of web root. not readable by web server. _important_
 */
function _hosting_generate_platform_config_file($node) {
  if ($node->type != "platform") {
    return null;
  }
  $path = _provision_config_path();
  $web_server = node_load($node->web_server);

  if (!provision_check_path($path . '/drush', "is_dir")) {
    provision_check_path($path, "chmod", "770");
    if (!provision_check_path($path . '/drush', "mkdir")) {
      #TODO Add error message.
      return false;
    }
  }
  if (provision_check_path($path, 'writeable')) {
    $file = fopen($path . '/drush/platform_' . $node->nid . '.php', 'w');
    fwrite($file, "<?php\n");
    $options['root'] = $node->publish_path;
    #todo - this needs to be unique...
    $options['uri'] = $web_server->title;
  
    fwrite($file, '$options = ' . var_export($options, true) . ";\n");
    fwrite($file, "if (file_exists('db_server.php')) { include_once('db_server.php'); }\n");
    fclose($file);
  }
  else {
    #TODO add error message
    return false;
  }
  
  

  
}
