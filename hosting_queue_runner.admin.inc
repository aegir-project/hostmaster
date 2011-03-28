<?php

/**
 * Configuration form for backup schedules
 */
function hosting_queue_runner_settings_form() {

  $form['hosting_queue_runner_post_task_delay'] = array(
    '#type' => 'select',
    '#title' => t('Post task delay'),
    '#description' => t('Tasks are executed as fast as possible, so you may wish to add a delay after the execution of each task. New tasks will still start executing almost instantly.'),
    '#default_value' => variable_get('hosting_queue_runner_post_task_delay', 0),
    '#options' => array(
      0 => t('No delay'),
    ) + drupal_map_assoc(range(1, 60)),
  );

  return system_settings_form($form);
}
