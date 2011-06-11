<?php

/**
 * @file
 *   Provides a views field handler for formatting timestamps.
 */

class hosting_views_field_handler_interval extends views_handler_field {
  
  function render($values) {
    $value = $values->{$this->field_alias};
    return hosting_format_interval($value);

  }

}

