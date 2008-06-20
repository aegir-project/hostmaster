<?php

/**
 * A very basic views 1.x argument handler, to be able to filter by node fields that refer to other nodes.
 */
function hosting_reference_arg_handler($table, $column, $op, &$query, $a1, $a2 = null) {
  switch ($op) {
  case 'summary' :
      $query->ensure_table($table);
      $query->add_field($column, $table);
      return array('field' => $table . '.' . $column);
      break;
    
    case 'link' :
      return _hosting_node_link($query->$column);
      break;

    case 'filter' : 
      $query->ensure_table($table);
      $where = db_escape_string($a2);
      $query->add_where($table . '.' . $column . " = %d", $where);
      break;
      
    case 'title' :
      return db_result(db_query("SELECT title FROM {node} WHERE nid = %d", $query));
      break;

  }
}


/**
 * Format a date as "X time after".
 */
function views_handler_field_offset($fieldinfo, $fielddata, $value, $data) {
  return $value ? format_interval($value - $data->node_created, 1) : '-';
}


function hosting_views_default_views() {
  $view = new stdClass();
  $view->name = 'site_embed';
  $view->description = 'A list of sites related to the specified node';
  $view->access = array (
);
  $view->view_args_php = '$args[0] = arg(1);';
  $view->page = TRUE;
  $view->page_title = '';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = 'No sites available.';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = '';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->sort = array (
    );
  $view->argument = array();
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Domain Name',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
    array (
      'tablename' => 'hosting_site',
      'field' => 'platform',
      'label' => 'Platform',
    ),
    array (
      'tablename' => 'hosting_site',
      'field' => 'db_server',
      'label' => 'Database',
    ),
    array (
      'tablename' => 'node',
      'field' => 'created',
      'label' => 'Created',
      'handler' => 'views_handler_field_since',
      'options' => '1',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'site',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, hosting_site);
  $views[$view->name] = $view;

  /**
   * Clone the view for each of the different embedded site views.
   *
   * This is a bit nasty, but is better than maintaining 3 separate views with only small differences
   * Views 2 makes this uneccesary.
   *
   * Drupal_clone is used here as the view object is a reference, and making changes to it modifies
   * the value of the view in the $views array.
   */
  $view = drupal_clone($view);
  $view->name = 'site_embed_client';
  $view->argument = array (
    array (
      'type' => 'hosting_site_client',
      'argdefault' => '1',
      'title' => '',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $views[$view->name] = $view;

  $view = drupal_clone($view);
  $view->name = 'site_embed_platform';
  $view->argument = array (
    array (
      'type' => 'hosting_site_platform',
      'argdefault' => '1',
      'title' => '',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $views[$view->name] = $view;

  $view = drupal_clone($view);
  $view->name = 'site_embed_db_server';
  $view->argument = array (
    array (
      'type' => 'hosting_site_db_server',
      'argdefault' => '1',
      'title' => '',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $views[$view->name] = $view;

  
  $view = new stdClass();
  $view->name = 'tasks_embed';
  $view->description = 'A list of tasks related to the item in question';
  $view->access = array (
);
  $view->view_args_php = 'if (!$args[0]) {
  $args[0] = arg(1);
}
return $args;';
  $view->page = TRUE;
  $view->page_title = 'task';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = 'No tasks';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = '';
  $view->use_pager = FALSE;
  $view->nodes_per_page = '10';
  $view->sort = array (
  );
  $view->argument = array (
    array (
      'type' => 'hosting_task_reference',
      'argdefault' => '1',
      'title' => 'Reference',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Task',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
    array (
      'tablename' => 'hosting_task',
      'field' => 'task_status',
      'label' => 'Status',
    ),
    array (
      'tablename' => 'node',
      'field' => 'created',
      'label' => 'Created',
      'handler' => 'views_handler_field_since',
      'options' => '1',
      
    ),
    array (
      'tablename' => 'hosting_task',
      'field' => 'executed',
      'label' => 'Completed in',
      'handler' => 'views_handler_field_offset',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'task',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, hosting_task);
  $views[$view->name] = $view;



  $view = new stdClass();
  $view->name = 'sites';
  $view->description = 'List of sites on the system';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Hosted sites';
    $view->access = array (
    0 => '1',
    1 => '2',
  );  
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '<?php return _hosting_introduction(); ?>';
  $view->page_empty_format = '2';
  $view->page_type = 'table';
  $view->url = 'hosting/sites';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '20';
  $view->sort = array (
    array (
      'tablename' => 'node',
      'field' => 'nid',
      'sortorder' => 'ASC',
      'options' => '',
    ),
  );
  $view->argument = array (
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Domain',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
 
    array (
      'tablename' => 'node',
      'field' => 'created',
      'label' => 'Created',
      'handler' => 'views_handler_field_since',
      'options' => '1',
    ),
    array (
      'tablename' => 'hosting_site',
      'field' => 'platform',
      'label' => 'Platform',
    ),
    array (
      'tablename' => 'hosting_site',
      'field' => 'db_server',
      'label' => 'Database Server',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'site',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, hosting_site);
  $views[$view->name] = $view;


  $view = new stdClass();
  $view->name = 'servers';
  $view->description = 'List of available servers which sites can be hosted on.';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Servers available';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = 'servers';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->block = TRUE;
  $view->block_title = 'Servers';
  $view->block_header = '';
  $view->block_header_format = '1';
  $view->block_footer = '';
  $view->block_footer_format = '1';
  $view->block_empty = '';
  $view->block_empty_format = '1';
  $view->block_type = 'table';
  $view->nodes_per_block = '5';
  $view->block_more = TRUE;
  $view->block_use_page_header = FALSE;
  $view->block_use_page_footer = FALSE;
  $view->block_use_page_empty = FALSE;
  $view->sort = array (
  );
  $view->argument = array (
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'label' => 'Type',
    ),
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Hostname',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'db_server',
  1 => 'web_server',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node);
  $views[$view->name] = $view;

  $view = new stdClass();
  $view->name = 'platforms';
  $view->description = 'List of available platforms which sites can be hosted on.';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Platforms available';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = 'platforms';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->block = TRUE;
  $view->block_title = 'Platforms';
  $view->block_header = '';
  $view->block_header_format = '1';
  $view->block_footer = '';
  $view->block_footer_format = '1';
  $view->block_empty = '';
  $view->block_empty_format = '1';
  $view->block_type = 'table';
  $view->nodes_per_block = '5';
  $view->block_more = TRUE;
  $view->block_use_page_header = FALSE;
  $view->block_use_page_footer = FALSE;
  $view->block_use_page_empty = FALSE;
  $view->sort = array (
  );
  $view->argument = array (
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Platform',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),

  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'platform',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node);
  $views[$view->name] = $view;

  $view = new stdClass();
  $view->name = 'package_release_embed';
  $view->description = '';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = '';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = '';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '20';
  $view->sort = array (
  );
  $view->argument = array (
    array (
      'type' => 'hosting_package_reference',
      'argdefault' => '1',
      'title' => 'Package',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => 'Release',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
    array (
      'tablename' => 'hosting_package_release',
      'field' => 'schema_version',
      'label' => 'Schema Version',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'package_release',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, hosting_package_release);
  $views[$view->name] = $view;


  return $views;
}
