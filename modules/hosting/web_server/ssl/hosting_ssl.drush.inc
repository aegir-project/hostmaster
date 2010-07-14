<?php

function hosting_ssl_hosting_site_context_options(&$task) {
  $node = $task->ref;

  if (isset($node->ssl_enabled)) {
    // we pass null to disable the option.
    $task->context_options['ssl_enabled'] = ($node->ssl_enabled) ? $node->ssl_enabled : 'null';

    if ($node->ssl_enabled) {
      $task->context_options['ssl_key'] = hosting_ssl_get_key($node->ssl_key);
    }
    else {
      $task->context_options['ssl_key'] = 'null';
    }
  }
}
