<?php

/**
 * Define service types.
 */
function hook_hosting_service_type() {
  return array(
    'http' => array(       // Machine name
      'title' => t('Web'), // Human-readable name
      'weight' => 0,       // Optional, defaults to 0
    ),
  );
}

/**
 * Define service implementation.
 *
 * Implementation class should go in {module name}.service.inc and be named
 * hostingService_{service type}_{type}, which should be a subclass of
 * hostingService.
 */
function hook_hosting_service() {
  return array(
    'http' => 'apache',  // Service type => type
  );
}
