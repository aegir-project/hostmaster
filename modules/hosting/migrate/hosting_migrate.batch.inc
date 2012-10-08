<?php
/**
 * @file
 *   Implemented batch migration of sites.
 */

/**
 * Batch migration of sites between platforms.
 */
function hosting_migrate_platform($form_state, $node) {
  drupal_add_js(drupal_get_path('module', 'hosting_migrate') . '/hosting_migrate.js');

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
      $form['description'] = array(
        '#type' => 'item',
        '#description' => 'You may want to read our <a href="http://community.aegirproject.org/node/1381" target=_blank><strong>best recipes for disaster</strong></a> before going further. You can perform a batch migration of sites from this source platform to a different target platform using this form, but if you wish to do that securely, always <i>Clone</i> one of them in its <strong>current</strong> platform, then test <i>Migrate</i> of the cloned copy first and check if the cloned copy works fine, before you will try that with your live sites. This seems tedious, but it is much easier and faster than recovering from some disaster.',
      );
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
    }
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
    $header = array(t('Site'), t('Upgrades'), t('Warnings'), t('Errors'), t('Actions'));
    $options['attributes']['class'] = 'hosting-package-comparison-link';
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
          if (isset($form_state['storage']['instance'][$site_id])) {
            $link = l(t('Compare'), 'hosting/migrate/compare/' . $node->nid . '/' . $form_state['storage']['instance'][$site_id], $options);
          }
          else  {
            $link = t('Profile not found');
          }
          $row[] = $link;
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

  // Step 3 - All migrate tasks have been created, close the modal if needed.
  if ($step == 3) {
    if (!empty($GLOBALS['modalframe_page_template'])) {
      modalframe_close_dialog();
    }
    else {
      drupal_goto('node/' . $form_state['storage']['target_platform']);
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
 * Implements hook_submit().
 */
function hosting_migrate_platform_submit($form, &$form_state) {
  $step = isset($form_state['storage']['step']) ? $form_state['storage']['step'] : 1;
  switch ($step) {
    case 1:
      $form_state['storage']['current_platform'] = $form['#current_platform']->nid;
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
          'file' => drupal_get_path('module', 'hosting_migrate') . '/hosting_migrate.batch.inc',
        );
        batch_set($batch);

        if (!empty($GLOBALS['modalframe_page_template'])) {
          $batch =& batch_get();
          $batch['url'] = 'hosting/js/batch';
          $batch['source_page'] = 'hosting/js/' . $_GET['q'];
        }
      }
      break;
    case 2:
      // Create a task node for logging purposes.
      $current = $form_state['storage']['current_platform'];
      $target = $form_state['storage']['target_platform'];

      $task = hosting_add_task($current, 'migrate', array('target_platform' => $target), HOSTING_TASK_SUCCESS);

      $operations = array();
      foreach ($form_state['storage']['passed'] as $nid => $url) {
        $operations[] = array('hosting_migrate_platform_submit_batch',
          array($nid, $form_state['storage']['target_platform'], $url, $task->vid));
      }

      if (sizeof($operations)) {
        $batch = array(
          'operations' => $operations,
          'finished' => 'hosting_migrate_platform_submit_finished',
          'title' => t('Submitting sites for migration.'),
          'init_message' => t('Retrieving list of sites.'),
          'progress_message' => t('Submitted @current out of @total sites.'),
          'error_message' => t('Bulk migration has encountered an error.'),
          'file' => drupal_get_path('module', 'hosting_migrate') . '/hosting_migrate.batch.inc',
        );
        batch_set($batch);

        if (!empty($GLOBALS['modalframe_page_template'])) {
          $batch =& batch_get();
          $batch['url'] = 'hosting/js/batch';
          $batch['source_page'] = 'hosting/js/' . $_GET['q'];
        }
      }

      break;
  }
  $form_state['storage']['step'] = $step + 1;
}

/**
 * Batch callback for platform migration submission.
 */
function hosting_migrate_platform_submit_batch($site_id, $target_platform, $url, $platform_task_vid) {
  $site = node_load($site_id);
  hosting_add_task($site_id, 'migrate', array(
    'target_platform' => $target_platform,
    'new_uri' => $url,
    'new_db_server' => $site->db_server,
  ));
  hosting_task_log($platform_task_vid, 'success', t("Migrate task for !url created", array('!url' => $url)));
}

/**
 * Batch finished callback for platform migration submission.
 */
function hosting_migrate_platform_submit_finished() {
  drupal_set_message(t('The sites have been added to the task queue to be migrated'));
}

/**
 * Batch comparison of site packages between platforms to determine
 * if the site can be migrated to the target platform or not.
 */
function hosting_migrate_platform_batch($site_id, $target_platform, $context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }
  $site = node_load($site_id);
  $profile = node_load($site->profile);
  $batch =& batch_get();
  // Determine whether the installation profile is available on the target platform
  $profile_instance = hosting_package_instance_load(
    array('i.rid' => $target_platform,
          'r.type' => 'platform',
          'n.nid' => $site->profile));
  if (!$profile_instance) {
    // Check if there is a possibly renamed profile available
    $profile_instance = hosting_package_instance_load(
      array('i.rid' => $target_platform,
            'r.type' => 'platform',
            'p.old_short_name' => $profile->short_name));
  }
  if ($profile_instance) {
    $status = hosting_package_comparison($site->nid, $profile_instance->iid);
    $batch['form_state']['storage']['status'][$site->nid] = $status;
    $batch['form_state']['storage']['instance'][$site->nid] = $profile_instance->iid;
    // If there were no errors, this site passes and can be migrated
    if (!$status['error'] && $profile->short_name != 'hostmaster') {   // The hostmaster site can not be upgraded via the interface.
      $batch['form_state']['storage']['passed'][$site->nid] = strtolower(trim($site->title));
      return TRUE;
    }
  }
  else {
    $batch['form_state']['storage']['status'][$site->nid] = array('error' => 1, 'missing' => 0, 'upgrade' => 0);
  }
  $batch['form_state']['storage']['failed'][$site->nid] = strtolower(trim($site->title));
}
