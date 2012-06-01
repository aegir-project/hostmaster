<?php

/**
 * Configuration form for backup schedules
 */
function hosting_queued_settings_form() {

  $form['description'] = array(
    '#type' => 'markup',
    '#value' => t('Note that the settings on this form will only apply to the daemon once it has been restarted, which by default happens as least once an hour.'),
    '#weight' => -100,
  );

  $last_seen = variable_get('hosting_queued_process_started', NULL);
  $form['hosting_queued_process_started'] = array(
    '#type' => 'item',
    '#title' => t('Runner status'),
    '#value' => !empty($last_seen) ? t('Last started: @interval ago.', array('@interval' => format_interval(time() - $last_seen))) : t('Never started.'),
  );

  $form['hosting_queued_post_task_delay'] = array(
    '#type' => 'select',
    '#title' => t('Post task delay'),
    '#description' => t('Tasks are executed as fast as possible, so you may wish to add a delay after the execution of each task. After this delay, new tasks will still start executing almost instantly.'),
    '#default_value' => variable_get('hosting_queued_post_task_delay', 0),
    '#options' => array(
      0 => t('No delay'),
    ) + drupal_map_assoc(range(1, 60), '_hosting_queued_settings_form_delay_callback'),
  );

  $form['hosting_queued_process_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Process lifetime timeout'),
    '#description' => t('Because of memory leaks and bugs in PHP, the daemon automatically stops after this delay, and is restarted. If you are running a lot of tasks, you may want to lower this so it gets restarted more often. In empirical tests, it was found that around 100KB are leaked for every task fired.'),
    '#default_value' => variable_get('hosting_queued_process_lifetime', 3600),
    '#options' => drupal_map_assoc(array(
      60,
      60 * 5,
      60 * 10,
      60 * 15,
      60 * 20,
      60 * 25,
      60 * 30,
      60 * 35,
      60 * 40,
      60 * 45,
      60 * 50,
      60 * 55,
      60 * 60,
    ), 'format_interval'),
  );

  return system_settings_form($form);
}

function _hosting_queued_settings_form_delay_callback($value) {
  return format_plural($value, '1 second', '@count seconds');
}
