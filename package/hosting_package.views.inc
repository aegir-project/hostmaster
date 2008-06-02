<?php

function hosting_package_views_tables() {
  $tables['hosting_package'] = array(
    'name' => 'hosting_package',
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
      'package_type' => array(
        'name' => t('Package: Package Type'),
        'help' => t('Display the type of package.'),
      ),
      'short_name' => array(
        'name' => t('Package: Project Name'),
        'sortable' => TRUE,
        'help' => t('Display the name of the project.'),
      ),
    ),
  );

  $tables['hosting_package_release'] = array(
    'name' => 'hosting_package_release',
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
      'package' => array(
        'name' => t('Package Release: Package'),
        'handler' => 'hosting_relation_link_handler',
        'help' => t('Display the name of the package the release is for.'),
      ),
      'version' => array(
        'name' => t('Package Release: Version'),
        'sortable' => TRUE,
        'help' => t('Display the version of the package.'),
      ),
      'schema_version' => array(
        'name' => t('Package Release: Schema Version'),
        'sortable' => TRUE,
        'help' => t('Display the database schema version of the module.'),
      ),
    ),
  );

  $tables['hosting_package_instance'] = array(
    'name' => 'hosting_package_instance',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'release_id',
      ),
    ),
    'fields' => array(
      'release_id' => array(
        'name' => t('Package Instance: Release'),
        'handler' => 'hosting_relation_link_handler',
        'help' => t('Display the name of the package linked to the platform/site.'),
      ),
      'rid' => array(
        'name' => t('Package Instance: Reference'),
        'sortable' => TRUE,
        'help' => t('Display the name of the package or site that this instance has been linked to.'),
      ),
      'path' => array(
        'name' => t('Package Instance: Path'),
        'sortable' => TRUE,
        'help' => t('Display where the package is in this instance.'),
      ),
    ),
  );
  return $tables;
}


/**
 * Views 1.x argument handling for package nodes.
 */
function hosting_package_views_arguments() {
  $arguments['hosting_package_reference'] = array(
    'name' => t('Package Release: Package'),
    'help' => t('Filter package releases by the package.'),
    'handler' => 'hosting_package_release_package_argument',
  );
  return $arguments;
}


/**
 * Views 1.x argument handler for filtering package nodes by reference.
 */
function hosting_package_release_package_argument($op, &$query, $a1, $a2 = null) {
  hosting_reference_arg_handler("hosting_package_release", "package", $op, $query, $a1, $a2);
}

