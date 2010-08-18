<?php
/**
 * @file Hosting site views integration.
 */



/** 
 * Implements hook_views_data().
 */
function hosting_site_views_data() {
  $data['hosting_site']['table'] = array(
      'group' => 'Hosting Site',
      'title' => 'Site',
      'join' => array(
         'node' => array(
           'left_field' => 'vid',
           'field' => 'vid',
        ),
      ),
    );

  $data['hosting_site']['client'] = array(
   'title' => t('Client'),
   'help' => t('Relate a node revision to the user who created the revision.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('client'),
    ),
  );

  $data['hosting_site']['db_server'] = array(
   'title' => t('Database Server'),
   'help' => t('Database where the site is installed.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('db server'),
    ),
  );

 $data['hosting_site']['profile'] = array(
   'title' => t('Install Profile'),
   'help' => t('Type of drupal site.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('profile'),
    ),
  );
 $data['hosting_site']['platform'] = array(
   'title' => t('Platform'),
   'help' => t('Platform'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('platform'),
    ),
  );

  $data['hosting_site']['verified'] = array(
      'title' => t('Verified'),
      'help' => t('The last date verified task run on this site.'),
      'field' => array(
        'handler' => 'hosting_views_field_handler_interval',
        'click sortable' => TRUE,
      ),
    );

  $data['hosting_site']['last_cron'] = array(
      'title' => t('Last Cron Run'),
      'help' => t('The time the last cron run was executed on this site.'),
      'field' => array(
        'handler' => 'hosting_views_field_handler_interval',
        'click sortable' => TRUE,
      ),
    );
  $data['hosting_site']['language'] = array(
      'title' => t('Language'),
      'help' => t('The default language of this site.'),
      'field' => array(
        'handler' => 'views_handler_field_hosting_language',
        'click sortable' => TRUE,
      ),
    );

  $data['hosting_site']['status'] = array(
      'title' => t('Status'),
      'help' => t('The current state of this site.'),
      'field' => array(
        'handler' => 'views_handler_field_hosting_site_status',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
    );

  return $data;
}

function hosting_site_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'hosting_site'),
    ),
    'handlers' => array(
      // field handlers
      'views_handler_field_hosting_language' => array(
        'parent' => 'views_handler_field',
      ),
      // field handlers
      'views_handler_field_hosting_site_status' => array(
        'parent' => 'views_handler_field',
      ),

    ),
  );
}

function hosting_site_views_plugins() {
  return array(
    'style' => array(
      'hosting_site_list' => array(
        'title' => t('Site listing'),
        'help' => t('Displays sites in a table.'),
        'handler' => 'views_plugin_style_site_table',
        'theme' => 'views_view_table',
        'parent' => 'table',
        'path' => drupal_get_path('module', 'hosting_site'),
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    )
  );

}


function hosting_site_views_default_views() {
  $view = new view;
  $view->name = 'hosting_site_list';
  $view->description = 'Display a list of sites hosted in aegir';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'client' => array(
      'id' => 'client',
      'table' => 'hosting_site',
      'field' => 'client',
    ),
    'db_server' => array(
      'id' => 'db_server',
      'table' => 'hosting_site',
      'field' => 'db_server',
    ),
    'profile' => array(
      'id' => 'profile',
      'table' => 'hosting_site',
      'field' => 'profile',
    ),
    'platform' => array(
      'id' => 'platform',
      'table' => 'hosting_site',
      'field' => 'platform',
    ),
    'web_server' => array(
      'label' => 'web server',
      'required' => 0,
      'id' => 'web_server',
      'table' => 'hosting_platform',
      'field' => 'web_server',
      'relationship' => 'platform',
    ),
  ));
  $handler->override_option('fields', array(
    'status' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'status_icon' => '1',
      'exclude' => 0,
      'id' => 'status',
      'table' => 'hosting_site',
      'field' => 'status',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'title' => array(
      'label' => 'Domain',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'title_3' => array(
      'label' => 'Install Profile',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title_3',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'profile',
    ),
    'language' => array(
      'label' => 'Language',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'exclude' => 0,
      'id' => 'language',
      'table' => 'hosting_site',
      'field' => 'language',
      'relationship' => 'none',
    ),
    'title_1' => array(
      'label' => 'Client',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 1,
        'max_length' => '10',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title_1',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'client',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'verified' => array(
      'label' => 'Verified',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'verified',
      'table' => 'hosting_site',
      'field' => 'verified',
      'relationship' => 'none',
    ),
    'title_2' => array(
      'label' => 'Platform',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 1,
        'max_length' => '20',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title_2',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'platform',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'title_5' => array(
      'label' => 'Web Server',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title_5',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'web_server',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'title_4' => array(
      'label' => 'Database Server',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title_4',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'db_server',
      'override' => array(
        'button' => 'Override',
      ),
    ),
  ));
  $handler->override_option('sorts', array(
    'title' => array(
      'order' => 'ASC',
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'site' => 'site',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '!=',
      'value' => array(
        'value' => '-2',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'hosting_site',
      'field' => 'status',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('items_per_page', 30);
  $handler->override_option('use_pager', 'mini');
  $handler->override_option('style_plugin', 'hosting_site_list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'title' => 'title',
      'status' => 'status',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'status' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));

  $views[$view->name] = $view;

  return $views;
}


