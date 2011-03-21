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
 * Define service implementations.
 *
 * Implementation class should go in {module name}.service.inc and be named
 * hostingService_{service type}_{type}, which should be a subclass of
 * hostingService.
 *
 * @return
 *   An associative array with the service implementation as key, and the
 *   service type implemented as value.
 */
function hook_hosting_service() {
  return array(
    'apache' => 'http',  // Service implementation => service type.
  );
}
