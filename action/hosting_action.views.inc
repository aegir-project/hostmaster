<?php

function hosting_action_views_tables() {
  $tables['hosting_action'] = array(
    'name' => 'hosting_action',
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
        'name' => t('Action: Executed Time'),
        'sortable' => TRUE,
        'handler' => views_handler_field_dates(),
        'option' => 'string',
        'help' => t('Display the time the action was executed.'),
      ),
      'action_type' => array(
        'name' => t('Action: Action Type'),
        'sortable' => TRUE,
        'help' => t('Display the type of action.'),
      ),
      'rid' => array(
          'name' => t('Action: Reference'),
          'handler' => 'hosting_relation_link_handler',
          'help' => t('The object this action is referring to.'),
        ),
      'action_status' => array(
        'name' => t('Action: Action Status'),
        'sortable' => TRUE,
        'handler' => "hosting_action_status_handler",
        'option' => 'string',
        'help' => t('Whether or not the action was succesful.'),
      ),        
      ),
    );
  return $tables;
}

/**
 * Views 1.x display handler for the BITMASK status field.
 */
function hosting_action_status_handler($fieldinfo, $fielddata, $value, $data) {
  return implode(", ", _hosting_parse_error_code($data->hosting_action_action_status));
}

/**
 * Views 1.x argument handling for action nodes.
 */
function hosting_action_views_arguments() {
  $arguments['hosting_action_reference'] = array(
    'name' => t('Action: Reference'),
    'help' => t('Filter actions by the item the actions reference'),
    'handler' => 'hosting_action_views_reference_argument',
  );
  return $arguments;
}

/**
 * Views 1.x argument handler for filtering action nodes by reference.
 */
function hosting_action_views_reference_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_action", "rid", $op, $query, $a1, $a2);
}


