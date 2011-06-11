<?php

/**
 * @file
 * Hooks provided by the hosting quota module.
 */

/**
 * @addtogroup hostinghooks
 * @{
 */

/**
 * Definition of hook_hosting_quota_resource
 */
function hook_hosting_quota_resource() {
  $resources = array();

  $resources['foo'] = array(
    'title' => t('Foo'),
    'description' => t('Limit for foo. Enter in 23rds of foo units.'),
    'module' => 'hook',
  );

  return $resources;
}

/**
 * Definition of hook_hosting_quota_get_usage
 *
 * @param $client int
 *   The nid of the client node
 * @param $resource string
 *   The machine name of the resource
 * @param $start string
 *   A MySQL format date
 * @param $end string
 *   Another MySQL format date
 * @return int
 *   Return an integer that can be compared to what the quota is set to
 */
function hook_hosting_quota_get_usage($client, $resource, $start, $end) {

  if (hosting_get_client($client)) {
    switch ($resource) {
      case 'foo':
        // Do some things
        return $usage;
    }
  }
}

/**
 * Definition of hook_hosting_quota_resource_render
 *
 * @param $resource string
 *   Machine name of the resource
 * @param $usage int
 *   Usage as returned by hosting_quota_get_usage
 */
function hook_hosting_quota_resource_render($resource, $usage) {
  switch ($resource) {
    case 'foo':
      $bar = 23;
      return $usage * $bar . ' units';
  }
}

/**
 * @} End of "addtogroup hooks".
 */
