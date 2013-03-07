<?php
/**
 * @file
 *   Drush include for the hosting_alias module.
 */

function drush_hosting_alias_pre_hosting_task() {
  if (module_exists('hosting_alias')) {
    $task = &drush_get_context('HOSTING_TASK');
    if ($task->ref->type == 'site') {
      $aliases = implode(',', hosting_alias_get_aliases($task->ref));
      $task->context_options['aliases'] = ($aliases) ? $aliases : 'null';

      $redirection = db_query("SELECT redirection FROM {hosting_site_alias} WHERE vid = :vid", array(':vid' => $task->ref->vid))->fetchField();
      $task->context_options['redirection'] = ($redirection) ? $redirection : 'null';
    }
  }
}

/**
 * Implements hook_drush_context_import().
 *
 * If we're importing a site, then import the aliases and redirection values.
 */
function hosting_alias_drush_context_import($context, &$node) {
  if ($context->type == 'site') {
    $node->aliases = $context->aliases;
    $node->redirection = $context->redirection;
  }
}
