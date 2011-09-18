<?php

/**
 * Filter by task type
 */
class hosting_task_handler_filter_hosting_task_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Task Type');

      $options = array();

      foreach (array('site', 'platform') as $type) {
        $tasks = hosting_available_tasks($type);
        if (!empty($tasks)) {
          foreach ($tasks as $identifier => $task) {
            $options[$identifier] = $task['title'];
          }
        }
      }

      $this->value_options = $options;
    }
  }
}

