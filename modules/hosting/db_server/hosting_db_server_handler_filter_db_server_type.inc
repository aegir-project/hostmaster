<?php
/**
 * @file
 *   Define a views filter handler to filter by database type.
 */

/**
 * Filter handler to allowing filtering by database type.
 */
class hosting_db_server_handler_filter_db_server_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('DB Sever Type');
      $result = db_query("SELECT DISTINCT(db_type) FROM {hosting_db_server}");
      while ($db_server = db_fetch_object($result)) { 
        $options[$db_server->db_type] = $db_server->db_type;
      }
           
      $this->value_options = $options;
    }
  }
}

