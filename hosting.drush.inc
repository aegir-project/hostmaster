<?php


function hosting_drush_command() {
  $items['hosting dispatch'] = array(
    'callback' => 'hosting_dispatch',
    'description' => dt('Centralized command for dispatching the various queue processors (hosting, cron, backup etc.)'),
  );

  $items['hosting setup'] = array(
    'callback' => 'hosting_setup',
    'description' => dt('Set up initial configuration settings such as the cron entry for the queue dispatcher and more.'),
  );

  $queues = hosting_get_queues();
  foreach ($queues as $queue => $info) {
    $dispatch = dt("Dispatched: @items items every @frequency minutes", array('@items' => $info['items'], '@frequency' => round($info['frequency'] / 60)));
    $items['hosting ' . $queue] = array(
      'callback' => 'hosting_run_queue', 
      'description' => $info['description'] . " " . $dispatch,
      'queue' => $queue,
    );
  }

  $items['hosting task'] = array(
    'description' => 'execute a specific queue item',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL
  );

  $items['hosting migrate'] = array('description' => 'migrate the frontend site', 'arguments' => array('example.com' => dt('The name of the site to migrate'), '/path/to/platform' => dt('The platform to migrate the site to.')));

  $items['hosting park'] = array('description' => 'disable the frontend site prior to migration', 'arguments' => array('example.com' => dt('The name of the site to park')));

  $items['hosting unpark'] = array('description' => 'enable the frontend site after a migration', 'arguments' => array('example.com' => dt('The name of the site to unpark')));

  return $items;
}


