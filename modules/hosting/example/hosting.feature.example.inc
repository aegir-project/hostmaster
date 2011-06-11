<?php

/**
 * @file
 *   Expose the example feature to hostmaster.
 */

/**
 * Implementation of hook_hosting_feature().
 * 
 * Register the example hosting feature with Aegir, initially this feature will
 * be disabled.
 */
function hosting_example_hosting_feature() {
  $features['example'] = array(
    // title to display in form
    'title' => t('Example feature'),
    // description
    'description' => t('Example feature documenting how to create your own extensions.'),
    // initial status ( HOSTING_FEATURE_DISABLED, HOSTING_FEATURE_ENABLED, HOSTING_FEATURE_REQUIRED )
    'status' => HOSTING_FEATURE_DISABLED,
    // module to enable/disable alongside feature
    'module' => 'hosting_example',
    // associate with a specific node type.
    //  'node' => 'nodetype',
    // which group to display in ( null , experimental )
    'group' => 'experimental'
    );
  return $features;
}
