<?php
/**
 * @file Web server node type is defined here.
 */


function hosting_action_log($vid, $type, $message, $severity = 0, $timestamp = null ) {
 $timestamp = ($timestamp) ? $timestamp : mktime();
 db_query("INSERT INTO {action_log} (vid, type, message, severity, timestamp) VALUES (%d, '%s', '%s', %d, %d)", $vid, $type, $message, $severity, $timestamp);
}


function hosting_map_values_action($node) {
  $values['action_id'] = $node->nid;
  $values['action_type'] = $node->action_type;

  $reference = node_load($node->rid);
  $values['action_reference_type'] = $reference->type;
  $values['action_reference'] = $reference->nid;
  
  $values = array_merge($values, hosting_map_values($reference));    

  return $values;
}

/**
 * Helper function to generate new action node
 */
function hosting_add_action($nid, $type, $args = null) {
 global $user;

 $node = node_load($nid);
 $action = new stdClass();
 $action->type = 'action';
 $action->uid = $user->uid;
 $action->status = 1;
 #todo: make this pretty
 $action->title = t("!type !title", array('!type' => $type, '!title' => $node->title) );
 $action->action_type = $type;
 $action->rid = $node->nid;
 node_save($action);
}

/**
 * Implementation of hook_form().
 */
function hosting_action_form(&$node, $rid = null) {
  $type = node_get_types('type', $node);
  $rid = ($node->rid) ? $node->rid : $rid;
  if (!is_numeric($rid)) {
    drupal_goto('node/add');
  }
  # This node can _only_ be created on another object. hook_menu manages the permissions for this.
  $ref = node_load($rid);
  $form['rid'] = array('#type' => 'value', '#value' => $rid);
  $form['action_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Type of action'),
    '#description' => t('The type of action you want to perform on this %type', array('%type' => $ref->type)),
    '#options' => hosting_available_actions($ref),
    '#default_value' => $node->action_type,
  );
  
  #TODO action parameters

  return $form;
}


function hosting_available_actions($ref = null) {
  if ($ref->type == 'site') {
    $options['backup'] = t('Backup site');
    $options['synch'] = t('Regenerate configuration files');

    if ($node->site_status) {
        $options['disable'] =  t('Disable the enabled site.');      
    }
    else {
        $options['enable'] = t('Enable the disabled site.');      
    }
  }
  elseif ($ref->type == 'platform') {
    $options['verify'] = t('Verify that this platform is correctly installed.');
  }
  return $options;
}


/**
 * Implementation of hook_insert().
 */
function hosting_action_insert($node) {
  db_query("INSERT INTO {hosting_action} (vid, nid, action_type, rid, executed, action_status) VALUES (%d, %d, '%s', %d, %d, %d)",
    $node->vid, $node->nid, $node->action_type, $node->rid, $node->executed, $node->action_status);
  
  db_query("INSERT INTO {hosting_action_queue} (nid, timestamp, status) VALUES (%d, %d, 0)", $node->nid, mktime());
}

/**
 * Implementation of hook_update().
 *
 * As an existing node is being updated in the database, we need to do our own
 * database updates.
 */
function hosting_action_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    hosting_action_insert($node);
  }
  else {
    db_query("UPDATE {hosting_action} SET nid=%d, action_type = '%s', rid = %d, executed=%d, action_status=%d)",
              $node->nid, $node->action_type, $node->rid, $node->executed, $node->action_status, $node->vid);
  }
}

function hosting_nodeapi_action_delete_revision(&$node) {
  db_query('DELETE FROM {hosting_action} WHERE vid = %d', $node->vid);
}

/**
 * Implementation of hook_delete().
 */
function hosting_action_delete($node) {
  db_query('DELETE FROM {hosting_action} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 */
function hosting_action_load($node) {
  $additions = db_fetch_object(db_query('SELECT action_type, executed, action_status, rid FROM {hosting_action} WHERE vid = %d', $node->vid));
  return $additions;
}

/**
 * Implementation of hook_view().
 */
function hosting_action_view($node, $teaser = FALSE, $page = FALSE) {
  $node = node_prepare($node, $teaser);
  
  $ref = node_load($node->rid);
  $node->content['reference'] = array(
    '#type' => 'item',
    '#title' => $ref->type,
    '#value' => 'value',
  );
  
  if ($node->executed) {
    $node->content['executed'] = array(
      '#type' => 'item',
      '#title' => t('Executed'),
      '#value' => format_date($node->executed),
    );
  }
  if ($node->action_status) {
    $node->content['status'] = array(
      '#type' => 'item',
      '#title' => t('Status'),
      '#value' => implode(", ", _hosting_parse_error_code($node->action_status)),
    );
  }
    
  # Display table containing the logged information for this action
  $result = db_query("SELECT * FROM {action_log} WHERE vid=%d", $node->vid);
  if ($result) {
    $header = array("type", "message", "date");
    while ($entry = db_fetch_object($result)) {
      $rows[] = array($entry->type, $entry->message, format_date($entry->timestamp, "custom", "Y-m-d m:s"));
    }

    
    $node->content['hosting_log'] = array(
      '#weight' => 1,
      '#value' => theme_table($header, (array) $rows)
    );
    
  }
  
  return $node;
}

/**
 * Turn bitmask integer error code into associative array
 */
function _hosting_parse_error_code($code) {
  global $provision_errors;
  if ($code) {
    foreach ($provision_errors as $error => $message) {
      if ($error != PROVISION_SUCCESS) {
        if ($code & $error) {
          $return[$error] = $message;
        }        
      }
    }    
  }
  else {
    $return[PROVISION_SUCCESS] = $provision_errors[PROVISION_SUCCESS];
  }
  return $return;
}

function hosting_action_views_tables() {
  return array(
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
          'sortable' => TRUE,
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
}

function hosting_action_status_handler($fieldinfo, $fielddata, $value, $data) {
  return ($data->hosting_action_action_status) ? implode(", ", _hosting_parse_error_code($data->hosting_action_action_status)) : t('In progress');
}
