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


function hosting_site_insert(&$node) {
  hosting_add_action($node->nid, 'install');
}

function hosting_site_update(&$node) {
  hosting_add_action($node->nid, 'synch');
}


function hosting_site_view(&$node, $teaser = false) {
  $view = views_get_view('actions_embed');
  $node->content['actions_view'] = array(
    '#type' => 'item',
    '#title' => t("Actions"),
    '#value' => views_build_view('embed', $view, array($node->nid)), 
    '#weight' => 10
  );
}
