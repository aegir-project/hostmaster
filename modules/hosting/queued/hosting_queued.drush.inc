<?php

/**
 * @file
 * Dispatcher daemon
 *
 * This file is the heart of the dispatcher drush command. It
 * implements most of the backend functionality.
 */

// This is necessary for signal handling to work
declare(ticks=1);

/**
 * Implementation of hook_drush_command().
 */
function hosting_queued_drush_command() {
  $items = array();

  $items['hosting-queued'] = array(
    'description' => 'Runs the tasks queue',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'drupal dependencies' => array(
      'hosting_queued',
    ),
    // We need this get around a drush bug, see: http://drupal.org/node/704848
    // But we remove it below for Drush 4.5 and above.
    'callback' => 'drush_hosting_queued',
    'options' => array('onetime' => array('description' => dt('exit if there are no more tasks queued after the 1 second poll timeout.'))),
  );

  // We don't need the callback defined for Drush > 4.4
  if (defined('DRUSH_VERSION') && version_compare(DRUSH_VERSION, '4.4', '>')) {
    unset($items['hosting-queued']['callback']);
  }

  return $items;
}

/**
 * Drush command to execute hosting tasks.
 */
function drush_hosting_queued() {
  if (function_exists('pcntl_signal')) {
    // reload the server on SIGHUP
    pcntl_signal(SIGHUP, 'hosting_queued_restart');
    pcntl_signal(SIGINT, 'hosting_queued_stop');
    pcntl_signal(SIGTERM, 'hosting_queued_stop');
  }

  // Set a nice high time limit, if we can:
  if (function_exists('set_time_limit')) {
    @set_time_limit(0);
  }

  // in some environments (e.g. in "productin") ENV is not actually
  // set (!) so try to guess from $_SERVER
  if (strpos(ini_get('variables_order'), 'E') === FALSE) {
    if (strpos(ini_get('variables_order'), 'S') === FALSE) {
      drush_log(dt('Neither $_ENV nor $_SERVER are available to set up proper environment inheritance; ensure E and/or S is set in your php.ini\'s "variables_order" setting.'), 'warning');
    }
    else {
      $_ENV = $_SERVER;
    }
  }

  $end_time = variable_get('hosting_queued_process_lifetime', 3600)  + time();

  // Record the fact that we're running, so we can give some feedback in the
  // frontend.
  variable_set('hosting_queued_process_started', time());

  watchdog('hosting_queued', 'Started Hosting queue daemon, waiting for new tasks');

  while (TRUE) {
    // Should we terminate.
    if (time() > $end_time) {
      // Restart the daemon to recycle leaked memory
      hosting_queued_restart();
    }

    // Get some tasks to run
    $tasks = _hosting_get_new_tasks();
    foreach ($tasks as $task) {
      // We sleep for a second just in case others want to run the task first.
      // This guards against other processes that want to add a hosting task
      // with arguments and run it immediately, they should be able to do this
      // without us getting in there first.
      // This is a workaround for http://drupal.org/node/1003536
      drush_log(dt('Found task to execute. Pausing before execution.'));
      sleep(1);

      // Execute the task in the backend
      drush_invoke_process('@self', 'hosting-task', array($task->nid));
      drush_log(dt('Finished executing task.'));

      // Delay for a configurable amount of time.
      $delay = variable_get('hosting_queued_post_task_delay', 0);
      if (!empty($delay)) {
        drush_log(dt('Going to sleep for @count seconds after completing task.', array('@count' => $delay)));
        sleep($delay);
      }

      // We're done with this task, this unset might help reduce memory usage.
      unset($task);

      // Should we terminate.
      if (time() > $end_time) {
        // Restart the daemon to recycle leaked memory
        hosting_queued_restart();
      }
    }
    // wait here only if we didn't process tasks
    // this is to avoid an infinite loop if there are no tasks in the queue
    if (!$tasks) {
      sleep(1);
    }
    if (drush_get_option('onetime')) {
      drush_log(dt("exiting after processing all tasks, as requested by --onetime"));
      break;
    }
    unset($tasks);
  }
}

/**
 * Handle interruption signals gracefully
 *
 * We do not want to interrupt children tasks, so we wait for them
 * before stopping.
 */
function hosting_queued_stop($signal) {
  watchdog('hosting_queued', 'Received signal @signal, waiting for children to die.', array('@signal' => $signal));
  $status = NULL;
  pcntl_wait($status);
  watchdog('hosting_queued', 'Stopped daemon');
  exit($status);
}

/**
 * Restart the dispatcher to work around memory leaks
 */
function hosting_queued_restart($signal = NULL) {
  // If we received a singal, process it.
  if (!is_null($signal)) {
    watchdog('hosting_queued', 'Received signal @signal, waiting for children to die.', array('@signal' => $signal));
    $status = NULL;
    pcntl_wait($status);
  }

  // We need the PCNTL extension to be able to auto restart.
  if (function_exists('pcntl_exec')) {
    $drush_command = drush_get_command();
    // should be hosting-queued unless something is very wrong
    $command = $drush_command['command'];

    // Get the drush executable and any arguments it needs.
    $args = hosting_queued_drush_find_drush();
    $drush = array_shift($args);

    // Get the original options passed to this command.
    $options = drush_get_context('cli');
    // The 'php' option, if needed will be set in $args already.
    unset($options['php']);

    // We need to reimplement _drush_escape_option() because it adds
    // extra leading spaces.
    foreach ($options as $key => $value) {
      if ($value !== TRUE) {
        $args[] = "--$key=" . escapeshellarg($value);
      }
      else {
        $args[] = "--$key";
      }
    }

    array_push($args, '@hostmaster', $command);
    watchdog('hosting_queued', 'Restarting queue daemon with @drush @args.', array('@drush' => $drush, '@args' => implode(" ", $args)));
    pcntl_exec($drush, $args, $_ENV);
    watchdog('hosting_queued', 'Could not restart the queue daemon, aborting.', array(), WATCHDOG_ERROR);
    /* NOTREACHED */
  }
  else {
    watchdog('hosting_queued', 'PCNTL not installed, unable to auto-restart.', array(), WATCHDOG_WARNING);
  }
  
  // Explicit exit in case we're handling a signal
  exit(1); 
}

/**
 * Get the proper way to call drush again.
 *
 * Note that unlike drush_find_drush() we return an array of parts, and we trim
 * the 'php' option of extra single quotes.
 *
 * @see drush_find_drush()
 */
function hosting_queued_drush_find_drush() {
 $php = drush_get_option('php');
 if (isset($php)) {
    $php = trim($php, "'");
    $drush = array($php, realpath($_SERVER['argv'][0]) ,  "--php='$php'");
  } else {
    $drush = array(realpath($_SERVER['argv']['0']));
  }
  return $drush;
}
