<?php


function hosting_map_values_site($node) {
  $values['site_id'] = $node->nid;
  $values['site_url'] = $node->title;

  # load platform details - takes care of path information
  $values['site_platform'] = $node->field_platform[0]['nid'];
  $values = array_merge($values, hosting_map_values($node->field_platform[0]['nid']));
  $values['site_document_root'] = $values['publish_path'];
    
  # @todo : Load all client details. Probably for initial email
  $values['site_client_id'] = $node->field_client[0]['nid'];
  $values = array_merge($values, hosting_map_values($node->field_client[0]['nid']));
    
  # load database server details
  $values['site_db_server'] = $node->field_db_server[0]['nid'];
  $values = array_merge($values, hosting_map_values($node->field_db_server[0]['nid']));

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
  #TODO : only ask when there are multiple clients/dbservers/platforms
  $form['client'] = array(
    '#type' => 'radios',
    '#title' => t('Client'),
    '#description' => t('The client who this site belongs to.'),
    '#options' => _hosting_get_clients(),
    '#default_value' => $node->client,
  );
  
  $form['platform'] = array(
    '#type' => 'radios',
    '#title' => t('Platform'),
    '#description' => t('The platform you want the site to be hosted on.'),
    '#options' => _hosting_get_platforms(),
    '#default_value' => $node->platform,
  );
  $form['db_server'] = array(
    '#type' => 'radios',
    '#title' => t('Database server'),
    '#description' => t('The database server the site will use to host it\'s content.'),
    '#options' => _hosting_get_db_servers(),
    '#default_value' => $node->db_server,
  );
  
  
  return $form;
}

function hosting_site_validate(&$node) {
  if ($node->title) {
    $existing = db_result(db_query("SELECT nid FROM {node} WHERE title='%s' LIMIT 1", $node->title));
    if ($existing && $existing != $node->nid) {
      form_set_error('title', t('This domain has already been taken'));
    }
  }
  
  
}

function hosting_site_insert(&$node) {
  hosting_add_action($node->nid, 'install');
}

function hosting_site_update(&$node) {
  hosting_add_action($node->nid, 'synch');
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
}



function hosting_site_views_tables() {
  return array(
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
}