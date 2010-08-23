<?php
/**
 * @file Hosting task views integration.
 */

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function hosting_task_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_task'),
      ),
    'handlers' => array(
      'hosting_task_handler_filter_hosting_task_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/** 
 * Implements hook_views_data().
 */
function hosting_task_views_data() {
  $data['hosting_task'] = array(
    'table' => array(
      'group' => 'Hosting Task',
      'title' => 'Task',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    ),
    'task_type' => array(
      'title' => t('Type'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
       'handler' => 'hosting_task_handler_filter_hosting_task_type',
      ),
    ),
    'rid' => array(
      'title' => t('Reference'),
      'help' => t(''),
    'relationship' => array(
        'handler' => 'views_handler_relationship',
        'base' => 'node',
        'field' => 'nid',
        'label' => t('Reference'),
      ),
    ),
    'task_status' => array(
      'title' => t('Status'),
      'help' => t('Current status of the task.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    ),
    'executed' => array(
      'title' => t('Executed'),
      'help' => t('Date this task was executed.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
    ),
  );
  return $data;
}


