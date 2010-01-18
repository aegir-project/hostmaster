<?php

/**
 * Filter by task type
 */
class hosting_task_handler_filter_hosting_task_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Task Type');
      $tasks = array();
      foreach (array('site', 'platform') as $task) {
        $types = hosting_available_tasks('site');
        foreach ($types as $type => $task) {
          $options[$type] = $task['title'];
        }
      }
      $this->value_options = $options;
    }
  }
}

