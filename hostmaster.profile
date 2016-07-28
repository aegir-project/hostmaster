<?php

// We need this (empty) file for Drupal to recognize our profile

/**
 * Implements hook_update_projects_alter().
 */
function hostmaster_update_projects_alter(&$projects) {
  // Enable update status for the Hostmaster profile.
  $modules = system_rebuild_module_data();

  // The module object is shared in the request, so we need to clone it here.
  $hostmaster = clone $modules['hostmaster'];
  $hostmaster->info['hidden'] = FALSE;
  _update_process_info_list($projects, array('hostmaster' => $hostmaster), 'module', TRUE);
}
