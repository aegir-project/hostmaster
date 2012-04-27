<?php

/**
 * @file
 *   Drush include for the Hosting module.
 */

/**
 * Implementation of hook_drush_command().
 */
function hosting_drush_command() {
  $items['hosting-dispatch'] = array(
    'description' => dt('Centralized command for dispatching the various queue processors (hosting, cron, backup etc.)'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL
  );

  $items['hosting-setup'] = array(
    'description' => dt('Set up initial configuration settings such as the cron entry for the queue dispatcher and more.'),
  );

  // If we're trying to get help, then try to bootstrap as much as possible.
  $current_command = drush_get_command();
  if (isset($current_command['command']) && ($current_command['command'] == 'help')) {
    drush_bootstrap_max();
  }

  // If we've not bootstrapped fully, then this function may not be around.
  if (function_exists('hosting_get_queues')) {
    $queues = hosting_get_queues();
    foreach ($queues as $queue => $info) {
      $dispatch = dt("Dispatched: @items items every @frequency minutes", array('@items' => $info['items'], '@frequency' => round($info['frequency'] / 60)));
      $items['hosting-' . $queue] = array(
        'callback' => 'hosting_run_queue',
        'description' => $info['description'] . " " . $dispatch,
        'queue' => $queue,
      );
    }
  }

  $items['hosting-task'] = array(
    'description' => 'execute a specific queue item',
    'arguments' => array(
      '@context_name' => 'Context to work on',
      'command' => 'provision-[command] to invoke',
    ),
    'options' => array(
      'force' => "Force the specified task to execute even if it's not queued to run.",
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL
  );

  $items['hosting-import'] = array(
    'description' => 'Import an existing backend context name into the front end.',
    'arguments' => array(
      '@context_name' => 'Context to import',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL
  );

  $items['hosting-pause'] = array(
    'description' => dt('Prepare the hostmaster site to be migrated to a new platform.'),
    'arguments' => array(
      'example.com' => dt('The url of the site being migrated.')),
  );

  /**
   * stub resume command
   *
   * @deprecated will just call hosting-resume, use hosting-resume
   * now, will be removed after 1.0
   */
  $items['hostmaster-resume'] = array(
    'description' => dt('Complete the migration of the hostmaster site to a new platform.'),
    'arguments' => array(
      'example.com' => dt('The url of the site being migrated.')),
  );

  $items['hosting-resume'] = array(
    'description' => dt('Complete the migration of the hostmaster site to a new platform.'),
    'arguments' => array(
      'example.com' => dt('The url of the site being migrated.')),
  );

  return $items;
}

/**
 * Command to import an existing provision named context and
 * generate nodes for it.
 *
 * @param $alias
 *   The name of the provision context to import.
 */
function drush_hosting_import($alias) {
  if (sizeof($alias)) {
    if (d($alias)->name) {
      drush_log("Importing $alias");
      hosting_drush_import($alias);
    }
  }
}

/**
 * Imports a drush named context / "site alias" into the hostmaster front
 * end, by creating nodes and translating the value.
 *
 * This is bascally the reverse of the context_options hook.
 * 
 * @param $alias
 *   The name of the provision context to import.
 */
function hosting_drush_import($alias) {
  $name = (is_object($alias)) ? $alias->name : $alias;

  static $known_contexts = array();

  // avoid re-importing the same object twice in one execution
  // of the script.
  if (isset($known_contexts[$name])) {
    drush_log("Already re-imported {$name} in this process.");
    return $known_contexts[$name];
  }

  $context = d($name);

  if ($node = hosting_context_load($name)) {
    drush_log("Context {$name} already has an associated node. Updating {$node->type} node {$node->nid} instead.");
    $node->no_verify = TRUE;
    $known_contexts[$name] = $node->nid;
  }
  else {
    // First time we've seen this context.
    $node = new stdClass;
    $node->type = $context->type;
    // set the hosting name too
    $node->hosting_name = trim($context->name, '@');
    $node->status = 1;
  }

  // iterate through all the objects which may want to save this node.
  $modules = drush_command_implements('drush_context_import');
  foreach ($modules as $name) {
    $func = "{$name}_drush_context_import";
    $func($context, $node);
  }

  node_save($node);
  if ($node->nid) {
    drush_log("Context {$name} has been imported. Updated {$node->type} node {$node->nid}.");
    $known_contexts[$name] = $node->nid;
  }

  return $node->nid;
}


