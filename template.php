<?php
// $Id$

/**
 * Preprocessor for theme_page().
 */
function eldir_preprocess_page(&$variables, $hook) {

  $variables['logo'] = l($variables['site_name'], '<front>');

  $variables['tabs2'] = $variables['tabs'];
  unset($variables['tabs']['#secondary']);
  unset($variables['tabs2']['#primary']);

  // Move the local actions into place of the secondary tabs, for now.
  if (is_array($variables['action_links'])) {
    foreach ($variables['action_links'] as $link) {
      $variables['tabs2']['#secondary'][] = $link;
      $variables['tabs2']['#theme'] = 'menu_local_tasks';
    }
    $variables['action_links']['#access'] = FALSE;
  }

}

function eldir_preprocess_html(&$variables, $hook) {
  if (!empty($variables['node'])) {
    // Add a node type label on node pages to help users.
    $types = node_get_types();
    $type = $variables['node']->type;
    if (!empty($types[$type])) {
      $variables['title'] = "<span class='label'>{$types[$type]->name}</span> {$variables['title']}";
    }


    $variables['classes_array'][] = " node-page";
    $variables['classes_array'][] = " ntype-{$type}";
  }

  // Add path-based class for a last line of defense
  $current_path = current_path();
  if (!empty($current_path)) {
    $variables['classes_array'][] = ' path-'. str_replace('/', '-', current_path());
  }

  // Add special body class for error pages
#  if (menu_get_active_item() === 0) {
#    $variables['body_classes'] .= ' error-page';
#  }

}

/**
 * Preprocessor for theme_node().
 */
function eldir_preprocess_node(&$variables, $hook) {
  if (!empty($variables['node'])) {
    // Add a node type label on node pages to help users.
    $types = node_get_types();
    $type = $variables['node']->type;
    if (!empty($types[$type])) {
      $variables['title'] .= "<span class='label'>{$types[$type]->name}</span> {$variables['title']}";
    }
  }
}
