<?php

function hosting_task_views_tables() {
  $tables['hosting_task'] = array(
    'name' => 'hosting_task',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid',
      ),
      'right' => array(
        'field' => 'vid',
      ),
    ),
    'fields' => array(
      'executed' => array(
        'name' => t('Task: Executed Time'),
        'sortable' => TRUE,
        'handler' => views_handler_field_dates(),
        'option' => 'string',
        'help' => t('Display the time the task was executed.'),
      ),
      'task_type' => array(
        'name' => t('Task: Task Type'),
        'sortable' => TRUE,
        'help' => t('Display the type of task.'),
      ),
      'rid' => array(
          'name' => t('Task: Reference'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The object this task is referring to.'),
        ),
      'task_status' => array(
        'name' => t('Task: Task Status'),
        'sortable' => TRUE,
        'handler' => "hosting_task_status_handler",
        'option' => 'string',
        'help' => t('Whether or not the task was succesful.'),
      ),        
      ),
    );
  return $tables;
}

/**
 * Views 1.x display handler for the BITMASK status field.
 */
function hosting_task_status_handler($fieldinfo, $fielddata, $value, $data) {
  return implode(", ", _hosting_parse_error_code($data->hosting_task_task_status));
}

/**
 * Views 1.x argument handling for task nodes.
 */
function hosting_task_views_arguments() {
  $arguments['hosting_task_reference'] = array(
    'name' => t('Task: Reference'),
    'help' => t('Filter tasks by the item the tasks reference'),
    'handler' => 'hosting_task_views_reference_argument',
  );
  return $arguments;
}

/**
 * Views 1.x argument handler for filtering task nodes by reference.
 */
function hosting_task_views_reference_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_task", "rid", $op, $query, $a1, $a2);
}


