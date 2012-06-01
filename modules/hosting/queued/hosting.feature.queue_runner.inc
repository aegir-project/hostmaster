<?php

/**
 * @file
 *   The hosting feature definition for the hosting queue runner.
 */

/**
 * Register a hosting feature with Aegir.
 *
 * This will be used to generate the 'admin/hosting' page.
 *
 * @return
 *  associative array indexed by feature key.
 */
function hosting_queue_runner_hosting_feature() {
  $features['queue_runner'] = array(
    // title to display in form
    'title' => t('Hosting queue runner'),
    // description
    'description' => t('Runs the hosting tasks queue as a daemonized process.'),
    // initial status ( HOSTING_FEATURE_DISABLED, HOSTING_FEATURE_ENABLED, HOSTING_FEATURE_REQUIRED )
    'status' => HOSTING_FEATURE_DISABLED,
    // module to enable/disable alongside feature
    'module' => 'hosting_queue_runner',
    // associate with a specific node type.
    //  'node' => 'nodetype',
    // which group to display in ( null , experimental )
    'group' => 'experimental'
  );
  return $features;
}
