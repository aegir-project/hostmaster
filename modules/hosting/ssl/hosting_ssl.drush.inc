<?php

function hosting_ssl_context_options(&$task) {
  $task->context_options['ssl'] = $task->ref->ssl;
  $task->context_options['ssl_redirect'] = $task->ref->ssl_redirect;
}
