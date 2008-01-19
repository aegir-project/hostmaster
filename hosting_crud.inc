<?php
/**
 *@file Just some helper functions for installing cck types before cck2 is out
 */

function _hosting_node_field_add($field_name, $type, $global_settings = array(), $required = 0, $multiple = 0, $db_storage = 0) {
  db_query("INSERT INTO {node_field} VALUES ('%s','%s','%s', %d, %d, %d)", $field_name, $type, serialize($global_settings), $required, $multiple, $db_storage);
}

function _hosting_node_field_instance_add($field_name, $type_name, $weight, $label, $widget_type, $widget_settings = array(), $display_settings = array(), $description = null) {
  db_query("INSERT INTO {node_field_instance} VALUES ('%s','%s', %d,'%s','%s','%s','%s','%s')", $field_name, $type_name, $weight, $label, $widget_type, serialize($widget_settings), serialize($display_settings), $description);
}