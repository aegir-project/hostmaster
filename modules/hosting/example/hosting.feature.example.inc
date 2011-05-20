<?php
// $Id$
/**
 * @file
 *   an example hosting feature definition.
 * 
 */

/**
 * Register a hosting feature with Aegir.
 *
 * The frontend provides a UI for enabling and disabling features, which usually
 * corresponds to enabling and disabling a module providing the
 * feature.
 *
 * This hook should be implemented in a file named:
 * hosting.feature.FEATURE_KEY.inc
 *
 * @return
 *   An array of hosting features, keyed by the name of the feature.
 *
 * @see hosting_get_features()
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
