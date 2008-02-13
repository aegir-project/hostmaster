<?php
function hosting_views_tables() {
  $tables["hosting_web_server"] = hosting_web_server_views_tables();
  $tables["hosting_action"] = hosting_action_views_tables();

  return $tables;
}

function hosting_views_default_views() {
   $view = new stdClass();
  $view->name = 'sites_embed';
  $view->description = 'A list of sites related to the specified node';
  $view->access = array (
);
  $view->view_args_php = '$args[0] = arg(1);';
  $view->page = TRUE;
  $view->page_title = 'sites';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = t('No sites available.');
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = 'sites_embed';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->sort = array (
  );
  $view->argument = array (
    array (
      'type' => 'content: field_client',
      'argdefault' => '1',
      'title' => '',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => '',
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
        0 => 'site',
       ),
     ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array('node');
  $views[$view->name] = $view;
  

  $view = new stdClass();
  $view->name = 'actions_embed';
  $view->description = 'A list of related actions';
  $view->access = array (
);
  $view->view_args_php = '$args[0] = arg(1);';
  $view->page = TRUE;
  $view->page_title = 'action';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'table';
  $view->url = 'actions';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->sort = array (
    array (
      'tablename' => 'node',
      'field' => 'nid',
      'sortorder' => 'DESC',
      'options' => '',
    ),
  );
  $view->argument = array (
    array (
      'type' => 'content: field_reference',
      'argdefault' => '1',
      'title' => '',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array (
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => '',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
    array (
      'tablename' => 'node',
      'field' => 'created',
      'label' => '',
      'handler' => 'views_handler_field_since',
    ),
    array (
      'tablename' => 'node_data_field_action_type',
      'field' => 'field_action_type_value',
      'label' => '',
      'handler' => 'content_views_field_handler_group',
      'options' => 'default',
    ),
    array (
      'tablename' => 'action_data',
      'field' => 'executed',
      'label' => '',
      'handler' => 'views_handler_field_date_small',
    ),
    array (
      'tablename' => 'action_data',
      'field' => 'action_status',
      'label' => '',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'action',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, node_data_field_action_type, action_data);
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
  $view->url = 'sites';
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
      'tablename' => 'node_data_field_client',
      'field' => 'field_client_nid',
      'label' => 'Owner',
      'handler' => 'content_views_field_handler_group',
      'options' => 'default',
    ),
    array (
      'tablename' => 'node',
      'field' => 'created',
      'label' => 'Created',
      'handler' => 'views_handler_field_date',
    ),
    array (
      'tablename' => 'node_data_field_platform',
      'field' => 'field_platform_nid',
      'label' => 'Platform',
      'handler' => 'content_views_field_handler_group',
      'options' => 'default',
    ),
    array (
      'tablename' => 'node_data_field_db_server',
      'field' => 'field_db_server_nid',
      'label' => 'Database Server',
      'handler' => 'content_views_field_handler_group',
      'options' => 'default',
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
  $view->requires = array(node, node_data_field_client, node_data_field_platform, node_data_field_db_server);
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
  $view->name = 'nodequeue_1';
  $view->description = 'View node queue Hosting queue';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Hosting queue';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'teaser';
  $view->url = 'nodequeue/1';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->block = TRUE;
  $view->block_title = 'Hosting queue';
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
    array (
      'tablename' => 'nodequeue_nodes',
      'field' => 'position',
      'sortorder' => 'ASC',
      'options' => '',
    ),
  );
  $view->argument = array (
  );
  $view->field = array (
    array (
      'tablename' => 'node_data_field_action_type',
      'field' => 'field_action_type_value',
      'label' => 'action',
      'handler' => 'content_views_field_handler_group',
      'options' => 'default',
    ),
    array (
      'tablename' => 'node_data_field_reference',
      'field' => 'field_reference_nid',
      'label' => 'item',
      'handler' => 'content_views_field_handler_group',
      'options' => 'default',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'nodequeue_nodes',
      'field' => 'qid',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => '1',
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(nodequeue_nodes, node_data_field_action_type, node_data_field_reference);
  $views[$view->name] = $view;


  
  return $views;
}