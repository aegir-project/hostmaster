<?php

/**
 * Configuration form for backup schedules
 */
function hosting_queue_runner_settings_form() {

  $form['description'] = array(
    '#type' => 'markup',
    '#value' => t('Note that the settings on this form will only apply to the daemon once it has been restarted, which by default happens as least once an hour.'),
    '#weight' => -100,
  );

  $last_seen = variable_get('hosting_queue_runner_process_started', NULL);
  $form['hosting_queue_runner_process_started'] = array(
    '#type' => 'item',
    '#title' => t('Runner status'),
    '#value' => !empty($last_seen) ? t('Last started: @interval ago.', array('@interval' => format_interval(time() - $last_seen))) : t('Never started.'),
  );

  $form['hosting_queue_runner_post_task_delay'] = array(
    '#type' => 'select',
    '#title' => t('Post task delay'),
    '#description' => t('Tasks are executed as fast as possible, so you may wish to add a delay after the execution of each task. After this delay, new tasks will still start executing almost instantly.'),
    '#default_value' => variable_get('hosting_queue_runner_post_task_delay', 0),
    '#options' => array(
      0 => t('No delay'),
    ) + drupal_map_assoc(range(1, 60), '_hosting_queue_runner_settings_form_delay_callback'),
  );

  $form['hosting_queue_runner_autorestart'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically restart'),
    '#description' => t('If this is enabled, the queue runner will automatically restart after the timeout. Do *NOT* enable if you are running the dispatcher under "supervisord" or a similar daemon.'),
    '#default_value' => variable_get('hosting_queue_runner_autorestart', FALSE),
  );

  return system_settings_form($form);
}

function _hosting_queue_runner_settings_form_delay_callback($value) {
  return format_plural($value, '1 second', '@count seconds');
}
