<?php
// $Id$

/**
 * Preprocessor for theme_page().
 */
function eldir_preprocess_page(&$variables) {
  $variables['logo'] = l($variables['site_name'], '<front>');

  if ($primary = menu_primary_local_tasks()) {
    $variables['tabs'] = "<ul class='links tabs clear-block'>{$primary}</ul>";
  }

  if ($secondary = menu_secondary_local_tasks()) {
    $variables['tabs2'] = "<ul class='links tabs clear-block'>{$secondary}</ul>";
  }

}

function eldir_preprocess_html(&$variables) {
  if (!empty($variables['node'])) {
    // Add a node type label on node pages to help users.
    $types = node_get_types();
    $type = $variables['node']->type;
    if (!empty($types[$type])) {
      $variables['title'] = "<span class='label'>{$types[$type]->name}</span> {$variables['title']}";
    }

    $variables['body_classes'] .= " node-page";
    $variables['body_classes'] .= " ntype-{$type}";
  }

  // Add path-based class for a last line of defense
  if (!empty($_GET['q'])) {
    $variables['body_classes'] .= ' path-'. str_replace('/', '-', $_GET['q']);
  }

  // Add special body class for error pages
#  if (menu_get_active_item() === 0) {
#    $variables['body_classes'] .= ' error-page';
#  }

  // Add IE 6 compatibility stylesheet
  $variables['styles_ie6'] = base_path() . path_to_theme() . '/ie6.css';
}

/**
 * Preprocessor for theme_node().
 */
function eldir_preprocess_node(&$variables) {
  if (!empty($variables['node'])) {
    // Add a node type label on node pages to help users.
    $types = node_get_types();
    $type = $variables['node']->type;
    if (!empty($types[$type])) {
      $variables['title'] = "<span class='label'>{$types[$type]->name}</span> {$variables['title']}";
    }
  }
}
