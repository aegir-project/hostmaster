<?php


class views_handler_field_hosting_site_status extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();

    $options['status_icon'] = array('default' => '0');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['status_icon'] = array(
      '#type' => 'radios',
      '#title' => t('Display icon'),
      '#options' => array('Text', 'Image', 'Both'),
      '#default_value' => isset($this->options['status_icon']) ? $this->options['status_icon'] : '0',
      '#description' => t("Display icons instead of status values.")
    );
  }

  function render($values) {
    $value = $values->{$this->field_alias};
    $output = _hosting_site_status($value);

    if ($this->options['status_icon'] == 1) {
      $output = "<span class='hosting-status hosting-status-icon'></span>";
    }
    elseif ($this->options['status_icon'] == 2) {
      $output = "<span class='hosting-status'>{$output}</span>";
    }
    return $output;
  }
}
