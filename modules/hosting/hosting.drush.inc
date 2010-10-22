<?php


function hosting_drush_command() {
  $items['hosting-dispatch'] = array(
    'description' => dt('Centralized command for dispatching the various queue processors (hosting, cron, backup etc.)'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL
  );

  $items['hosting-setup'] = array(
    'description' => dt('Set up initial configuration settings such as the cron entry for the queue dispatcher and more.'),
  );



  $queues = hosting_get_queues();
  foreach ($queues as $queue => $info) {
    $dispatch = dt("Dispatched: @items items every @frequency minutes", array('@items' => $info['items'], '@frequency' => round($info['frequency'] / 60)));
    $items['hosting-' . $queue] = array(
      'callback' => 'hosting_run_queue', 
      'description' => $info['description'] . " " . $dispatch,
      'queue' => $queue,
    );
  }

  $items['hosting-task'] = array(
    'description' => 'execute a specific queue item',
    'arguments' => array(
      '@context_name' => 'Context to work on',
      'command' => 'provision-[command] to invoke',
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

  return $items;
}

/**
 * Command to import an existing provision named context and
 * generate nodes for it.
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
 * this is bascally the reverse of the context_options hook.
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


