<?php

/**
 * Main queue processing command for hostmaster.
 *
 * This is a single command, which will (based on configuration) run all the other
 * queue commands (cron, backup, tasks, stats). This is done so that there
 * is only one cron job to configure, and allow the frequency of calls to be configured
 * from the interface.
 */
function drush_hosting_dispatch() {
  $now = mktime();
  variable_set("hosting_dispatch_last_run", $now);
  drush_log(t("dispatching queues"));
  $platform = node_load(HOSTING_OWN_PLATFORM);
  $root = $platform->publish_path;

  if (variable_get('hosting_dispatch_enabled', false)) {
    $queues = hosting_get_queues();
    foreach ($queues as $queue => $info) {
      if ($info['running']) {
        drush_log(dt("queue @queue already running", array('@queue' => $queue)));
      } else {
        if ($info['enabled']) {
          if (($now - $info["last"]) >= $info["calc_frequency"] || drush_get_option('force', false)) {
            $count = $info['calc_items'] - $info['running_items'];
            if ($count <= 0) {
              drush_log(dt("maximum number of tasks (@count) already running", array('@count' => $info['running_items'])));
            } else {
              drush_log(dt("found @running running tasks, starting @count out of @max items", array('@running' => $info['running_items'], '@count' => $count, '@max' => $info['calc_items'])));
              drush_backend_fork("hosting-" . $queue, array('items' => $count));
            }
          } else {
            drush_log(dt("too early for queue @queue", array('@queue' => $queue)));
          }
        } else {
          drush_log(dt("queue @queue disabled", array('@queue' => $queue)));
        }
      }
    }
  } else {
    drush_log(dt("dispatching disabled"));
  }
}
