<?php
/**
 * Page callback for admin/hosting/quotas
 */
function hosting_quota_admin_client_list() {
  $items_per_page = 25;
  $all_resources = module_invoke_all('hosting_quota_resource');
  $result = pager_query('SELECT q.*, n.title FROM {hosting_client_quota} q INNER JOIN {node} n ON n.nid = q.client', $items_per_page);
  $output = array();

  while ($row = db_fetch_object($result)) {

    $resource = array($row->resource => $all_resources[$row->resource]);

    // Pass in raw limit and usage
    $output[$row->client][$row->resource]['limit'] = $row->value;
    $output[$row->client][$row->resource]['usage'] = hosting_quota_get_usage($row->client, $resource);

    // Render limit and usage
    $output[$row->client][$row->resource]['rendered usage'] = hosting_quota_resource_render($resource, $output[$row->client][$row->resource]['usage']);
    $output[$row->client][$row->resource]['rendered limit'] = hosting_quota_resource_render($resource, $row->value);

    // Set client identifier
    $output[$row->client]['name'] = $row->title;
  }

  return theme('hosting_quota_admin_list', $output, $all_resources, $items_per_page);
}

/**
 * Callback for admin/hosting/quotas/defaults
 */
function hosting_quota_admin_defaults_form($form_state) {

  $form = array();
  $all_resources = module_invoke_all('hosting_quota_resource');

  $form['info'] = array(
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#value' => t('Set default limits for each resource. This value will be used as the default value when new clients are created and to set quota limits for all clients currently without limits'),
    '#suffix' => '</p>',
  );

  $form['quota'] = array(
    '#type' => 'fieldset',
    '#title' => t('Client quota settings'),
    '#access' => user_access('edit all quotas'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  foreach ($all_resources as $resource => $quota) {
    $form['quota'][$resource] = array(
      '#title' => $quota['title'],
      '#description' => $quota['description'],
      '#type' => 'textfield',
      '#default_value' => variable_get("hosting_quota_default_{$resource}", 0),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  $form['#tree'] = TRUE;

  return $form;
}

/**
 * Submit function for hosting_quota_admin_defaults_form
 */
function hosting_quota_admin_defaults_form_submit ($form, &$form_state) {
  foreach ($form_state['values']['quota'] as $resource => $quota) {
    // Set the default quota
    variable_set("hosting_quota_default_{$resource}", $quota);

    // Set quotas for clients that don't already have them
    $client_result = db_query('SELECT nid FROM {hosting_client} WHERE nid not in (SELECT client FROM {hosting_client_quota})');
    while ($row = db_fetch_object($client_result)) {
      hosting_quota_set_limit($row->nid, $resource, $quota);
    }
  }
}
