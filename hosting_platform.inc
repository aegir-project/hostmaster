<?php


function hosting_map_values_platform($node) {
  $values['publish_path'] = $node->field_publish_path[0]['value'];

  # @TODO: get web server details. for distributed operation.
  return $values;
}

function hosting_platform_view(&$node, $op, $teaser) {
  $view = views_get_view('actions_embed');
  $node->content['actions_view'] = array(
    '#type' => 'item',
    '#title' => t("Actions"),
    '#value' => views_build_view('embed', $view, array($node->nid)), 
    '#weight' => 10
  );
}
