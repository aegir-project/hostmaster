<?php
// $Id$

/**
 * @file Site nodeapi implementations.
 */

/**
 * Implements hook_node_info().
 */
function hosting_site_node_info() {
  $types["site"] =  array(
    "type" => 'site',
    "name" => 'Site',
    'base' => 'hosting_site',
    "has_title" => TRUE,
    "title_label" => 'Domain name',
    "description" => hosting_node_help("site"),
    "has_body" => 0,
    "body_label" => '',
    "min_word_count" => 0,
  );
  return $types;
}

/**
 * Implements hook_view().
 */
function hosting_site_view(&$node, $teaser = FALSE) {
  hosting_set_breadcrumb($node);
  $node->content['info']['#prefix'] = '<div id="hosting-site-info" class="hosting-info-list">';
  if ($node->site_status == HOSTING_SITE_ENABLED) {
    $node->content['info']['link'] = array(
      '#markup' => _hosting_site_goto_link($node),
      '#weight' => -10,
    );
  }

  if (is_numeric($node->client)) {
    $node->content['info']['client'] = array(
      '#type' => 'item',
      '#title' => t('Client'),
      '#markup' => _hosting_node_link($node->client),
      '#weight' => 5,
    );
  }

  $node->content['info']['verified'] = array(
    '#type' => 'item',
    '#title' => t('Verified'),
    '#markup' => hosting_format_interval($node->verified),
  );

  $node->content['info']['platform'] = array(
    '#type' => 'item',
    '#title' => t('Platform'),
    '#markup' => _hosting_node_link($node->platform),
  );

  if ($node->profile) {
    $node->content['info']['profile'] = array(
      '#type' => 'item',
      '#title' => t('Install profile'),
      '#markup' => _hosting_node_link($node->profile),
    );
  }
  if ($node->site_language) {
    $node->content['info']['site_language'] = array(
      '#type' => 'item',
      '#title' => t('Language'),
      '#markup' => _hosting_language_name($node->site_language),
    );
  }

  if ($node->nid) {
    $node->content['info']['status'] = array(
      '#type' => 'item',
      '#title' => t('Status'),
      '#markup' => _hosting_site_status($node),
    );
  }

  if ($node->db_server) {
    $node->content['info']['db_server'] = array(
      '#type' => 'item',
      '#title' => t('Database server'),
      '#markup' => _hosting_node_link($node->db_server),
    );
  }

  if ($node->db_name) {
    $node->content['info']['db_name'] = array(
      '#type' => 'item',
      '#title' => t('Database name'),
      '#markup' => check_plain($node->db_name),
    );
  }

  $node->content['info']['#suffix'] = '</div>';

  if ($node->nid) {
    $node->content['tasks_view'] = array(
      '#type' => 'item',
      '#markup' => hosting_task_table($node),
      '#prefix' => '<div id="hosting-task-list">',
      '#suffix' => '</div>',
      '#weight' => 10,
    );
    $settings['hostingTaskRefresh'] = array(
      'nid' => $node->nid,
      'changed' => $node->changed,
    );
    drupal_add_js($settings, array('type' => 'setting', 'scope' => JS_DEFAULT));

    drupal_add_js(drupal_get_path('module', 'hosting_task') . '/hosting_task.js');
  }
  return $node;
}

/**
 * Implementation of hook_nodeapi_delete_revision().
 */
function hosting_nodeapi_site_delete_revision(&$node) {
  db_delete('hosting_site')
    ->condition('vid', $node->vid)
    ->execute();
}

/**
 * Implements hook_delete().
 */
function hosting_site_delete($node) {
  db_delete('hosting_site')
    ->condition('nid', $node->nid)
    ->execute();
  db_delete('hosting_package_instance')
    ->condition('rid', $node->nid)
    ->execute();
  hosting_context_delete($node->nid);
  $result = db_query("SELECT distinct nid FROM {hosting_task} WHERE rid = :rid", array(':rid' => $node->nid));
  while ($nid = $result->fetch()) {
    node_delete($nid->nid);
  }
}

/**
 * Implements hook_nodeapi().
 */
function hosting_site_nodeapi_site_presave(&$node) {
  $node->title = strtolower(trim($node->title)); // domain names are case-insensitive
}

/**
 * Implements hook_insert().
 */
function hosting_site_insert(&$node) {
  $client = hosting_get_client($node->client);
  $node->client = $client->nid;
  $node->site_language = isset($node->site_language) ? $node->site_language : 'en';
  // If the cron_key is set use it, otherwise generate a new one.
  $node->cron_key = isset($node->cron_key) ? $node->cron_key : '';
  // Ensure that the last_cron value is set.
  $node->last_cron = isset($node->last_cron) ? $node->last_cron : 0;

  $id = db_insert('hosting_site')
    ->fields(array(
      'vid' => $node->vid,
      'nid' => $node->nid,
      'client' => $node->client,
      'db_server' => $node->db_server,
      'db_name' => isset($node->db_name) ? $node->db_name : '',
      'platform' => $node->platform,
      'profile' => $node->profile,
      'language' => $node->site_language,
      'last_cron' => $node->last_cron,
      'cron_key' => $node->cron_key,
      'status' => isset($node->site_status) ? $node->site_status : HOSTING_SITE_QUEUED,
      'verified' => isset($node->verified) ? $node->verified : 0,
    ))
    ->execute();
  if ((empty($node->old_vid))) {
    hosting_context_register($node->nid, isset($node->hosting_name) ? $node->hosting_name : $node->title);
    if (isset($node->import)) {
      hosting_add_task($node->nid, 'import');
    }
    else {
      hosting_add_task($node->nid, 'install');
    }
  }
}

/**
 * Implements hook_update().
 */
function hosting_site_update(&$node) {
  // if this is a new node or we're adding a new revision,
  if (!empty($node->revision)) {
    hosting_site_insert($node);
  }
  else {
    $client = hosting_get_client($node->client);
    $node->client = $client->nid;
    if ($node->site_status == HOSTING_SITE_DELETED) {
      $node->no_verify = TRUE;
    }

    db_update('hosting_site')
      ->fields(array(
        'client' => $node->client,
        'db_server' => $node->db_server,
        'db_name' => $node->db_name,
        'platform' => $node->platform,
        'last_cron' => $node->last_cron,
        'cron_key' => $node->cron_key,
        'status' => $node->site_status,
        'profile' => $node->profile,
        'language' => $node->site_language,
        'verified' => $node->verified,
      ))
      ->condition('vid', $node->vid)
      ->execute();
  }
  if (!$node->no_verify) {
    hosting_add_task($node->nid, 'verify');
  }
}

/**
 * Implements hook_load().
 *
 * @param node
 *    Node object
 */
function hosting_site_load($nodes) {
  foreach ($nodes as $nid => &$node) {
    $additions = db_query('SELECT  client, db_server, db_name, platform, profile, language as site_language, last_cron, cron_key, status AS site_status, verified FROM {hosting_site} WHERE vid = :vid', array(':vid' => $node->vid))->fetch();
    foreach ($additions as $property => &$value) {
      $node->$property = $value;
    }
  }
}
