<?php


function hosting_map_values_site($node) {
  $values['site_id'] = $node->nid;
  $values['site_url'] = $node->title;

  $values = array_merge($values, hosting_map_values($node->platform));
  $values = array_merge($values, hosting_map_values($node->client));
  $values = array_merge($values, hosting_map_values($node->db_server));

  # @todo : load profile / release details
  return $values;
}

function hosting_site_form($node) {
  $type = node_get_types('type', $node);
  
  // We need to define form elements for the node's title and body.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => check_plain($type->title_label),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#weight' => -5
  );
  $clients = _hosting_get_clients();
  if (sizeof($clients) > 1) {
    $form['client'] = array(
      '#type' => 'radios',
      '#title' => t('Client'),
      '#description' => t('The client who this site belongs to.'),
      '#options' => $clients,
      '#default_value' => $node->client,
    );
  }
  else {
    $form['client'] = array('#type' => 'value', '#value' => key($clients));
  }

  $platforms = _hosting_get_platforms();
  if (sizeof($platforms) > 1) {
    $form['platform'] = array(
      '#type' => 'radios',
      '#title' => t('Platform'),
      '#description' => t('The platform you want the site to be hosted on.'),
      '#options' => $platforms,
      '#default_value' => $node->platform,
    );
  }
  else {
    $form['platform'] = array('#type' => 'value', '#value' => key($platforms));
  }

  $db_servers = _hosting_get_db_servers();
  if (sizeof($db_servers) > 1) {
    $form['db_server'] = array(
      '#type' => 'radios',
      '#title' => t('Database server'),
      '#description' => t('The database server the site will use to host it\'s content.'),
      '#options' => $db_servers,
      '#default_value' => $node->db_server,
    );
  }
  else {
    $form['db_server'] = array('#type' => 'value', '#value' => key($db_servers));
  } 
  
  return $form;
}

function hosting_site_validate(&$node) {
  if (!preg_match("/^([\w-]+\.)+[a-z]{2,4}$/i", $node->title ) ) {
    form_set_error('title', t("You have not specified a valid url for this site."));
  }
  if (db_result(db_query("SELECT COUNT(nid) FROM {node} WHERE type='site' AND title='%s' AND  nid <> %d", $node->title, $node->nid))) {
    form_set_error('title', t("The url you specified is not unique."));
  }
}


function hosting_site_insert(&$node) {
  db_query("INSERT INTO {hosting_site} (vid, nid, client, db_server, platform, last_cron) VALUES (%d, %d, %d, %d, %d, %d)",
    $node->vid, $node->nid, $node->client, $node->db_server, $node->platform, $node->last_cron);
  if (!$node->old_vid) {
    hosting_add_action($node->nid, 'install');    
  }
}
 
function hosting_site_update(&$node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_site_insert($node);
  }
  else {
    db_query("UPDATE {hosting_site} SET client = %d, db_server = %d, platform = %d, last_cron =%d WHERE vid=%d",
              $node->publish_path, $node->web_server, $node->vid, $node->vid);
  }
  hosting_add_action($node->nid, 'synch');
}

function hosting_site_load($node) {
  $additions = db_fetch_object(db_query('SELECT  client, db_server, platform, last_cron FROM {hosting_site} WHERE vid = %d', $node->vid));
  return $additions;
}

function hosting_site_view(&$node, $teaser = false) {
  $node->content['client'] = array(
    '#type' => 'item',
    '#title' => t('Client'),
    '#value' => _hosting_node_link($node->client),
  );
  
  $node->content['platform'] = array(
    '#type' => 'item',
    '#title' => t('Platform'),
    '#value' => _hosting_node_link($node->platform),
  );
  $node->content['db_server'] = array(
    '#type' => 'item',
    '#title' => t('Database server'),
    '#value' => _hosting_node_link($node->db_server),
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



function hosting_site_views_tables() {
  $tables['hosting_site'] = array(
    'name' => 'hosting_site',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid',
      ),
      'right' => array(
        'field' => 'vid',
      ),
    ),
    'fields' => array(
      'client' => array(
          'name' => t('Site: Client'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The client the site belongs to.'),
        ),
      'platform' => array(
          'name' => t('Site: Platform'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The platform the site is hosted on.'),
        ),
      'db_server' => array(
          'name' => t('Site: Database Server'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The database server the site is hosted on.'),
        ),
      ),
    );
  return $tables;
}

/**
 * Views 1.x argument handling for site nodes.
 */
function hosting_site_views_arguments() {
  $arguments['hosting_site_client'] = array(
    'name' => t('Site: Client'),
    'handler' => 'hosting_site_client_argument',
  );
  $arguments['hosting_site_platform'] = array(
    'name' => t('Site: Platform'),
    'help' => t('Filter sites by the platform they are hosted on.'),
    'handler' => 'hosting_site_platform_argument',
  );
  $arguments['hosting_site_db_server'] = array(
    'name' => t('Site: Database Server'),
    'help' => t('Filter sites by the database server they are hosted on.'),
    'handler' => 'hosting_site_db_server_argument',
  );

  return $arguments;
}


/**
 * Views 1.x argument handler for filtering site nodes by client.
 */
function hosting_site_client_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_site", "client", $op, $query, $a1, $a2);
}

/**
 * Views 1.x argument handler for filtering site nodes by client.
 */
function hosting_site_platform_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_site", "platform", $op, $query, $a1, $a2);
}

/**
 * Views 1.x argument handler for filtering site nodes by client.
 */
function hosting_site_db_server_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_site", "db_server", $op, $query, $a1, $a2);
}


/**
 * Add a site backup record.
 *
 * Builds a list of backups of the site that have been generated.
 */
function hosting_site_add_backup($site, $web_server, $filename, $description = '') {
  $bid = db_next_id("{hosting_site_backups}_bid");
  db_query("INSERT INTO {hosting_site_backups} (bid, site, web_server, filename, description, timestamp) VALUES (%d, %d, %d, '%s', '%s', %d)",
    $bid, $site, $web_server, $filename, $description, mktime());
  return $bid;
}

/**
 * Delete a site backup record
 */
function hosting_site_delete_backup($bid) {
  db_query("DELETE FROM {hosting_site_backups} WHERE bid=%d", $bid);
}

/**
 * Get a site backup record
 */
function hosting_site_get_backup($bid) {
  return db_fetch_array(db_query("SELECT bid, site, web_server, filename, description, timestamp FROM {hosting_site_backups} WHERE bid = %d", $bid));
}

/**
 * Retrieve a list of backup generated for a site.
 *
 * @param site
 *   The node if of the site backups are being retrieved for
 * @return
 *   An associative array of backups existing for the site, indexed by bid and sorted reverse chronologically.
 */
function hosting_site_backup_list($site) {
  $result = db_query("SELECT bid, description, timestamp FROM {hosting_site_backups} WHERE site=%d ORDER BY timestamp DESC", $site);
  while ($object = db_fetch_object($result)) {
    #needs to be cleaned up. but i am NOT generating a theme func for this right now.
    $backups[$object->bid] = '<strong>' . format_date($object->timestamp) . '</strong> - ' . $object->description;
  }
  return $backups;
}

/**
 * Views handler to display links instead of node id's
 */
function hosting_relation_link_handler($fieldinfo, $fielddata, $value, $data) {
  return _hosting_node_link($value);
}

function hosting_site_backups($node) {
  drupal_set_title($node->title);
  # ADD CONFIRM TO THIS.
  $form['site'] = array('#type' => 'value', '#value' => $node->nid);
  $form['bid'] = array(
    '#type' => 'radios',
    '#title' => t('Backups'),
    '#options' => hosting_site_backup_list($node->nid),
    '#required' => TRUE
  );

  $form['submit'] = array('#type' => 'submit', '#value' => "Restore this version");
  return $form;
}

function hosting_site_backups_submit($form_id, $values) {
  hosting_add_action($values['site'], 'restore', array('backup' => $values['bid']));
  return 'node/' . $values['site'];
}
