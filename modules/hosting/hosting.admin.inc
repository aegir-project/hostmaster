<?php

/**
 * General settings form.
 */
function hosting_settings($form, &$form_state) {
  $form['settings'] = array(
    '#type' => 'item',
    '#title' => t('General Hosting settings'),
    '#value' => t('Here you may set various general settings that apply to site management or to the frontend system.'),
    '#weight' => 0,
  );

  $form['hosting_ignore_default_profiles'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide platforms with non-default profiles'),
    '#description' => t('When selecting a Drupal core profile, hide platforms that also contain other profiles (i.e intended as distributions). Warning: requires a platform other than the Hostmaster platform.'),
    '#default_value' => variable_get('hosting_ignore_default_profiles', FALSE),
  );
  $form['hosting_require_disable_before_delete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require site to be disabled before deletion'),
    '#description' => t('Check if you wish for users to run the Disable task on a site prior to running the Delete task.'),
    '#default_value' => variable_get('hosting_require_disable_before_delete', TRUE),
  );

  if (hosting_feature('cron')) {
    $form['hosting_cron_use_backend'] = array(
      '#type' => 'radios',
      '#title' => t('Cron method'),
      '#description' => t('For running cron on a site. You can use the drush cron implementation or the web-based cron.php method using builtin HTTP requests. The drush implementation is more reliable but will be slower than the web-based approach if the webserver has an opcode cache (like APC) configured.'),
      '#options' => array('Web-based', 'Drush'),
      '#default_value' => variable_get('hosting_cron_use_backend', TRUE),
    );
  }
  return system_settings_form($form);
}

/**
 * Form to configure the frequency of queue execution.
 */
function hosting_queues_configure() {
  drupal_add_css(drupal_get_path('module', 'hosting') . '/hosting.css');
  $units = array(
    strtotime("1 second", 0) => t("Seconds"),
    strtotime("1 minute", 0) => t("Minutes"),
    strtotime("1 hour", 0) => t("Hours"),
    strtotime("1 day", 0) => t("Days"),
    strtotime("1 week", 0) => t("Weeks"),
  );

  $queues = hosting_get_queues();
  $form['#tree'] = TRUE;

  foreach ($queues as $queue => $info) {
    $form[$queue]['description'] = array(
      '#type' => 'item',
      '#value' => $info['name'],
      '#description' => $info['description']
    );

    $form[$queue]["enabled"] = array(
      '#type' => 'checkbox',
      '#default_value' => $info['enabled']
    );

    $form[$queue]["last_run"] = array(
      '#value' => hosting_format_interval(variable_get('hosting_queue_' . $queue . '_last_run', false))
    );
    $form[$queue]['frequency']['#prefix'] = "<div class='hosting-queue-frequency'>";
    $form[$queue]['frequency']['#suffix'] = "</div>";

    if ($info['type'] == 'batch') {
	  $form[$queue]['frequency']['items'] = array(
        '#value' => t('%count %items every', array("%count" => $info['total_items'],
		"%items" => format_plural($info['total_items'], $info['singular'], $info['plural']))),
		'#prefix' => "<div class='hosting-queue-frequency-items'>",
		'#suffix' => "</div>",
      );
    }
    else {
      $form[$queue]['frequency']['items'] = array(
        '#type' => 'textfield',
        '#size' => 3,
        '#maxlength' => 3,
        '#default_value' => $info['items'],
        '#suffix' => t(' %items every ', array('%items' => $info['plural'])),
      );
    }
    foreach (array_reverse(array_keys($units)) as $length) {
      $unit = $units[$length];

      if (!($info['frequency'] % $length)) {
        $frequency_ticks = $info['frequency'] / $length;
        $frequency_length = $length;
        break;
      }
    }
    $form[$queue]['frequency']["ticks"] = array(
      '#type' => 'textfield',
      '#default_value' => $frequency_ticks,
      '#maxlength' => 5,
      '#size' => 5
    );
    $form[$queue]['frequency']["unit"] = array(
      '#type' => 'select',
      '#options' => $units,
      '#default_value' => $frequency_length,
    );
  }
  $form['help'] = array(
    '#type' => 'item',
    '#description' => t('Increasing the queue frequency to every 1 second will not cause them to be dispatched every second, as the dispatcher can only run once per minute via cron. However, with such a short frequency, executing the hosting-dispatch command manually from the CLI will allow you to \'force\' the queues to be dispatched between cron runs. This may be useful for development purposes.'),
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));
  return $form;
}

/**
 * Theme function to render the queue configuration form.
 *
 * @see hosting_queues_configure()
 */
function theme_hosting_queues_configure($form) {
  $queues = hosting_get_queues();

  $rows = array();
  $header = array('', t('Description'),
    array('data' => t('Frequency'), 'class' => 'hosting-queue-frequency-head'),
    t('Last run'),);
  foreach ($queues as $key => $info) {
    $row = array();
    $row[] = drupal_render($form[$key]['enabled']);
    $row[] = drupal_render($form[$key]['description']);
    $row[] = drupal_render($form[$key]['frequency']);
    $row[] = drupal_render($form[$key]['last_run']);
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form['help']);
  $output .= drupal_render($form['submit']);
  $output .= drupal_render($form);
  return $output;
}

/**
 * Validation callback for the the queue execution frequency form.
 *
 * @see hosting_queues_configure()
 */
function hosting_queues_configure_validate($form, &$form_state) {
  foreach (hosting_get_queues() as $queue => $info) {
    if ($form_state['values'][$queue]) {
      if ($form_state['values'][$queue]['frequency']['ticks'] && !is_numeric($form_state['values'][$queue]['frequency']['ticks'])) {
        form_set_error($queue, t('Please enter a valid frequency.'));
      }
      if ($form_state['values'][$queue]['frequency']['items'] && !is_numeric($form_state['values'][$queue]['frequency']['items'])) {
        form_set_error($queue, t('Please enter a valid amount of items.'));
      }
    }
  }
}

/**
 * Submit callback for the the queue execution frequency form.
 *
 * @see hosting_queues_configure()
 */
function hosting_queues_configure_submit($form, &$form_state) {
  foreach (hosting_get_queues() as $queue => $info) {
    if ($form_state['values'][$queue]) {
      variable_set("hosting_queue_" . $queue . "_enabled", $form_state['values'][$queue]['enabled']);
      variable_set("hosting_queue_" . $queue . "_frequency", $form_state['values'][$queue]['frequency']['ticks'] * $form_state['values'][$queue]['frequency']['unit']);
      if ($info['type'] == 'serial') {
        variable_set("hosting_queue_" . $queue . "_items", $form_state['values'][$queue]['frequency']['items']);
      }
    }
  }
  drupal_set_message(t('The queue settings have been updated.'));
}
