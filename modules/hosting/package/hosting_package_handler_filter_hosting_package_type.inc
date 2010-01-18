<?php

/**
 * Filter by node type
 */
class hosting_package_handler_filter_hosting_package_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Package Type');
      $types = _hosting_package_types();
      foreach ($types as $type => $name) {
        $options[$type] = $name;
      }
      $this->value_options = $options;
    }
  }
}

