<?php


class views_handler_field_hosting_site_status extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return _hosting_site_status($value);
  }
}
