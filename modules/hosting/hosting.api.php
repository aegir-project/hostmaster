<?php
/**
 * @file
 * Hooks provided by the hosting module, and some other random ones.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Determine if a site can be created using the specified domain.
 *
 * @param $url
 *   The URL of the site that hosting wishes to create.
 * @param $params
 *   An array of paramters that may contain information about the site. None of
 *   the keys are required however, so you should not depend on the value of any
 *   particular key in this array. If the array is not empty it will usually
 *   contain at least a 'nid' key whose value is the nid of the site being
 *   created.
 * @return
 *   Return TRUE/FALSE if you allow or deny the domain respectively.
 */
function hook_allow_domain($url, $params) {
  // Don't allow another drupal.org, it's special.
  if ($url == 'drupal.org') {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Define hosting queues.
 */
function hook_hosting_queues() {

}

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

/**
 * Act on nodes defined by other modules.
 *
 * This is a more specific version of hook_nodeapi() that includes a node type
 * and operation being performed, in the function name. When implementing this
 * hook you should replace TYPE with the node type and OP with the node
 * operation you would like to be notified for. A list of possible values for OP
 * can be found in the documentation for hook_nodeapi().
 *
 * This hook may help you write code that is easier to follow, but this is an
 * hosting specific hook so you may confuse other developers that are not so
 * familar with the internals of hosting.
 *
 * @param &$node
 *   The node the action is being performed on.
 * @param $a3
 *   - When OP is "view", passes in the $teaser parameter from node_view().
 *   - When OP is "validate", passes in the $form parameter from node_validate().
 * @param $a4
 *   - When OP is "view", passes in the $page parameter from node_view().
 * @return
 *   This varies depending on the operation (OP).
 *   - The "presave", "insert", "update", "delete", "print" and "view"
 *     operations have no return value.
 *   - The "load" operation should return an array containing pairs
 *     of fields => values to be merged into the node object.
 *
 * @see hook_nodeapi()
 * @see hosting_nodeapi()
 */
function hook_nodeapi_TYPE_OP(&$node, $a3, $a4) {
  // This is an example from hosting_nodeapi_client_delete_revision().
  db_query('DELETE FROM {hosting_client} WHERE vid = %d', $node->vid);
}

/**
 * @} End of "addtogroup hooks".
 */
