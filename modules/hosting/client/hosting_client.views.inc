<?php
/**
 * @file
 *   Client module views integration.
 */

/**
 * Implementation of hook_views_data().
 */
function hosting_client_views_data() {
  $data['hosting_client'] = array(
    'table' => array(
      'group' => 'Hosting Client',
      'title' => 'Client',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'uname' => array(
      'title' => t('Internal name'),
      'help' => t('The internal name for this client.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
  );

  return $data;
}
