<?php
// $Id$


/**
 * Batch migration of sites between platforms
 */
function hosting_migrate_platform($form_state, $node) {
  $step = isset($form_state['storage']['step']) ? $form_state['storage']['step'] : 1;

  // Step 1 - choose target platform
  if ($step == 1) {
 
    $result = db_query("SELECT count(nid) as site_count FROM {hosting_site} WHERE status=%d AND verified > 0 AND platform=%d", HOSTING_SITE_ENABLED, $node->nid);
    if ($obj = db_fetch_object($result)) {
      if ($obj->site_count == 0) {
        $form['error'] = array(
          '#type' => 'markup',
          '#value' => t('This platform does not have any sites that can be migrated.'),
        );
        return $form;
      }
    }

    $platforms = _hosting_get_platforms();
    if (sizeof($platforms) > 1) {
      unset($platforms[$node->nid]);
      $form['#current_platform'] = $node;
      $form['target_platform'] = array(
        '#type' => 'radios',
        '#required' => TRUE,
        '#title' => t('Platform'),
        '#description' => t('Choose where you want to migrate the sites on this platform to'),
        '#options' => $platforms,
      );
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
      );
    else {
      $form['no_platforms'] = array(
        '#type' => 'item',
        '#description' => t('There are no alternate platforms to migrate to.'),
      );
    }
  }

  // Step 2 - review sites that pass or fail the requirements to be migrated
  if ($step == 2) {
    $title = array(
      'passed' => t("The following sites will be migrated"),
      'failed' => t("The following sites will not be migrated")
    );
    $header = array(t('Site'), t('Upgrades'), t('Warnings'), t('Errors'));

    foreach (array('passed', 'failed') as $type) {
      if (sizeof($form_state['storage'][$type])) {

        $rows = array();

        foreach ($form_state['storage'][$type] as $site_id => $url) {
          $form['output'][$type]['title'] = array(
            '#type' => 'markup',
            '#value' => '<h2>' . $title[$type] . '</h2>',
          );
          $status = $form_state['storage']['status'][$site_id];
          $row = array( array('data' => $url, 'class' => 'hosting-status'), $status['upgrade'], $status['missing'], $status['error']);

          $rows[] = array('data' => $row, 'class' => 'hosting-' . _hosting_migrate_site_list_class($status));
        }
        $form['output'][$type]['table'] = array(
          '#type' => 'markup',
          '#value' => theme('table',  $header, $rows, array('class' => 'hosting-table')),
        );
         
      }
    }
    if (sizeof($form_state['storage']['passed'])) {
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
      );
    }
        
  }
  return $form;
}

function _hosting_migrate_site_list_class($status) {
  if ($status['error']) {
    return 'error';
  }
  if ($status['missing']) {
    return 'warning';
  }
  if ($status['upgrade']) {
    return 'success';
  }
  return 'queued';
}

/**
 * Implementation of hook_submit()
 */
function hosting_migrate_platform_submit($form, &$form_state) {
  $step = isset($form_state['storage']['step']) ? $form_state['storage']['step'] : 1;
  switch ($step) {
    case 1:
      $form_state['storage']['target_platform'] = $form_state['values']['target_platform']; 
      $max_per_batch = 5;
      $result = db_query("SELECT n.nid, n.title FROM  {hosting_site} s LEFT JOIN {node} n ON n.nid=s.nid WHERE platform = %d AND s.status = %d and s.verified > 0 ORDER BY n.title", $form['#current_platform']->nid, HOSTING_SITE_ENABLED);
      $operations = array();
      while ($site = db_fetch_object($result)) {
        $operations[] = array('hosting_migrate_platform_batch', 
          array($site->nid, $form_state['values']['target_platform'], $form_state)); 
      }
      if (sizeof($operations)) {
        $batch = array(
          'operations' => $operations,
          'finished' => 'hosting_migrate_platform_finished',
          'title' => t('Checking for sites that can be migrated.'),
          'init_message' => t('Retrieving list of sites.'),
          'progress_message' => t('Evaluated @current out of @total sites.'),
          'error_message' => t('Bulk migration has encountered an error.'),
        );
        batch_set($batch);
      } 
      break;
    case 2:
      foreach ($form_state['storage']['passed'] as $nid => $url) {
        hosting_add_task($nid, 'migrate', array('target_platform' => $form_state['storage']['target_platform']));
      }
      $form_state['redirect'] = 'node/' . $form_state['storage']['target_platform'];
      unset($form_state['rebuild']);
      unset($form_state['storage']);
      drupal_set_message(t('The sites have been added to the task queue to be migrated'));
      return false; 
      // this does not seem to work ?
      break;
   }
  $form_state['storage']['step'] = $step + 1;
}

/**
 * Batch comparison of site packages between platforms to determine
 * if the site can be migrated to the target platform or not.
 */
function hosting_migrate_platform_batch($site_id, $target_platform, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }

  $site = node_load($site_id);
  $profile = node_load($site->profile);

  // The hostmaster site can not be upgraded via the interface.
  if ($profile->title != 'hostmaster') {
    $batch =& batch_get();
    // Determine whether the install profile is available on the target platform
    $profile_instance = hosting_package_instance_load(
      array('i.rid' => $target_platform, 
            'r.type' => 'platform',
            'n.nid' => $site->profile));

    if ($profile_instance) {
      $status = hosting_package_comparison($site->nid, $profile_instance->iid);
      $batch['form_state']['storage']['status'][$site->nid] = $status;
      // If there were no errors, this site passes and can be migrated
      if (!$status['error']) {
        $batch['form_state']['storage']['passed'][$site->nid] = $site->title;
        return true;
      }
    }
    else {
      $batch['form_state']['storage']['status'][$site->nid] = array('error' => 1, 'missing' => 0, 'upgrade' => 0);
    }
    $batch['form_state']['storage']['failed'][$site->nid] = $site->title;
  } 
}


