<?php
/**
 * @file
 *   Expose the web pack feature to hostmaster.
 */

/**
 * Implementation of hook_hosting_feature().
 */
function hosting_web_pack_hosting_feature() {
  $features['web_pack'] = array(
    'title' => t('Web pack'),
    'description' => t('Provides a service to expose a site across multiple web front ends. This adds to the base cluster class the capability of having "light" servers which do not have files copied around.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_web_pack',
    'group' => 'experimental',
  );
  return $features;
}
