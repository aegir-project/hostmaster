<?php
/**
 * @file Views handler for Hosting module.
 */

class views_handler_field_hosting_language extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return _hosting_language_name($value);
  }
}

