<?php
/**
 * @file
 * Hooks provided by the hosting module, and some other random ones.
 */

/** @defgroup hostinghooks Frontend hooks
 * @{
 *
 * Those hooks are hooks usable within contrib Drupal modules running
 * in the Aegir frontend site. 
 */

/**
 * Determine if a site can be created using the specified domain.
 *
 * The frontend will only create a specified domains if all implementations of
 * this hook return TRUE, so in most cases you will be looking for domains that
 * you don't allow, and fallback to a default position of allowing the domain.
 *
 * @param $url
 *   The URL of the site that hosting wishes to create.
 * @param $params
 *   An array of paramters that may contain information about the site. None of
 *   the keys are required however, so you should not depend on the value of any
 *   particular key in this array. If the array is not empty it will usually
 *   contain at least a 'nid' key whose value is the nid of the site being
 *   created.
 * @return
 *   Return TRUE/FALSE if you allow or deny the domain respectively.
 *
 * @see hosting_domain_allowed()
 */
function hook_allow_domain($url, $params) {
  // Don't allow another drupal.org, it's special.
  if ($url == 'drupal.org') {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Import a backend context into the corresponding frontend node.
 *
 * This hook will be invoked when an object is being imported from the backend
 * into the frontend, for example a site that has just been cloned. You should
 * inspect the context coming from the backend and store anything the frontend
 * that you need to.
 *
 * A node to represent the object will have already been created and is
 * available to store things in, this node will be automatically saved after all
 * implementations of this hook are called. You should not call node_save()
 * manually on this node.
 *
 * If you implement hook_hosting_TASK_OBJECT_context_options() then you will
 * probably want to implement this hook also, as they mirror each other.
 *
 * @param $context
 *   The backend context that is being imported.
 * @param $node
 *   The node object that is being built up from the $context. You should modify
 *   the fields and properties so that they reflect the contents of the
 *   $context.
 *
 * @see hosting_drush_import()
 * @see hook_hosting_TASK_OBJECT_context_options()
 */
function hook_drush_context_import($context, &$node) {
  // From hosting_alias_drush_context_import().
  if ($context->type == 'site') {
    $node->aliases = $context->aliases;
    $node->redirection = $context->redirection;
  }
}

/**
 * Register a hosting feature with Aegir.
 *
 * The frontend provides a UI for enabling and disabling features, which usually
 * corresponds to enabling and disabling a module providing the feature.
 *
 * This hook can be implemented in a file named:
 * hosting.feature.FEATURE_KEY.inc
 *
 * Note that the module providing this hook does not need to be enabled for it
 * to be called. The frontend will use details in this hook to enable a module
 * if the feature is enabled.
 *
 * @return
 *   An array of hosting features, keyed by the machine name of the feature.
 *   Inner arrays may contain the following keys:
 *   - 'title': The localised title of the feature.
 *   - 'description': The localised description of the feature.
 *   - 'status': The inital status of the feature, either
 *      HOSTING_FEATURE_DISABLED, HOSTING_FEATURE_ENABLED or
 *      HOSTING_FEATURE_REQUIRED.
 *   - 'module': A module to enable or disable whenever the feature is enabled
 *      or disabled.
 *   - 'node': A node type that is associated with this feature.
 *   - 'callback': A function name to call when this feature is enabled or
 *      disabled.
 *   - 'group': The group that this feature belongs to, should be either NULL or
 *     'experimental'.
 *
 * @see hosting_get_features()
 */
function hook_hosting_feature() {
  // From hosting_example_hosting_feature().
  $features['example'] = array(
    // title to display in form
    'title' => t('Example feature'),
    // description
    'description' => t('Example feature documenting how to create your own extensions.'),
    // initial status ( HOSTING_FEATURE_DISABLED, HOSTING_FEATURE_ENABLED, HOSTING_FEATURE_REQUIRED )
    'status' => HOSTING_FEATURE_DISABLED,
    // module to enable/disable alongside feature
    'module' => 'hosting_example',
    // associate with a specific node type.
    //  'node' => 'nodetype',
    // which group to display in ( null , experimental )
    'group' => 'experimental'
    );
  return $features;
}

/**
 * Define hosting queues.
 *
 * @see hosting_get_queues()
 */
function hook_hosting_queues() {

}

/**
 * Add or change context options before a hosting task runs.
 *
 * This hook is invoked just before any task that has the 'provision_save' flag
 * equal to TRUE. These include the 'install', 'verify' and 'import' tasks.
 *
 * The TASK_OBJECT will be either: 'server', 'platform' or 'site'.
 *
 * This gives other modules the chance to send data to the backend to be
 * persisted by services there. The entire task is sent so that you have access
 * to it, but you should avoid changing things outside of the
 * $task->content_options collection.
 *
 * If you are sending extra context options to the backend based on properties
 * in the object's node, then you should also implement the
 * hook_drush_context_import() hook to re-create those properties on the node
 * when a context is imported.
 *
 * @param $task
 *   The hosting task that is about to be executed, the task is passed by
 *   reference. The context_options property of this object is about to be saved
 *   to the backend, so you can make any changes before that happens. Note that
 *   these changes won't persist in the backend unless you have a service that
 *   will store them.
 *
 *   The node representing the object of the task, e.g. the site that is being
 *   verified is available in the $task->ref property.
 *
 * @see drush_hosting_task()
 * @see hook_drush_context_import()
 * @see hook_hosting_tasks()
 */
function hook_hosting_TASK_OBJECT_context_options(&$task) {
  // From hosting_hosting_platform_context_options().
  $task->context_options['server'] = '@server_master';
  $task->context_options['web_server'] = hosting_context_name($task->ref->web_server);
}

/**
 * Perform actions when a task has failed and has been rolled back.
 *
 * Replace TASK_TYPE with the type of task that if rolled back you will be
 * notified of.
 *
 * @param $task
 *   The hosting task that has failed and has been rolled back.
 * @param $data
 *   An associative array of the drush output of the backend task from
 *   drush_backend_output(). The array should contain at least the following:
 *   - "output": The raw output from the drush command executed.
 *   - "error_status": The error status of the command run on the backend.
 *   - "log": The drush log messages.
 *   - "error_log": The list of errors that occurred when running the command.
 *   - "context": The drush options for the backend command, this may contain
 *     options that were set when the command ran, or options that were set by
 *     the command itself.
 *
 * @see drush_hosting_hosting_task_rollback()
 * @see drush_backend_output()
 */
function hook_hosting_TASK_TYPE_task_rollback($task, $data) {
  // From hosting_site_hosting_install_task_rollback().

  // @TODO : we need to check the returned list of errors, not the code.
  if (drush_cmp_error('PROVISION_DRUPAL_SITE_INSTALLED')) {
    // Site has already been installed. Try to import instead.
    drush_log(dt("This site appears to be installed already. Generating an import task."));
    hosting_add_task($task->rid, 'import');
  }
  else {
    $task->ref->no_verify = TRUE;
    $task->ref->site_status = HOSTING_SITE_DISABLED;
    node_save($task->ref);
  }
}

/**
 * Act on nodes defined by other modules.
 *
 * This is a more specific version of hook_nodeapi() that includes a node type
 * and operation being performed, in the function name. When implementing this
 * hook you should replace TYPE with the node type and OP with the node
 * operation you would like to be notified for. A list of possible values for OP
 * can be found in the documentation for hook_nodeapi().
 *
 * This hook may help you write code that is easier to follow, but this is a
 * hosting specific hook so you may confuse other developers that are not so
 * familar with the internals of hosting.
 *
 * @param &$node
 *   The node the action is being performed on.
 * @param $a3
 *   - When OP is "view", passes in the $teaser parameter from node_view().
 *   - When OP is "validate", passes in the $form parameter from node_validate().
 * @param $a4
 *   - When OP is "view", passes in the $page parameter from node_view().
 * @return
 *   This varies depending on the operation (OP).
 *   - The "presave", "insert", "update", "delete", "print" and "view"
 *     operations have no return value.
 *   - The "load" operation should return an array containing pairs
 *     of fields => values to be merged into the node object.
 *
 * @see hook_nodeapi()
 * @see hosting_nodeapi()
 */
function hook_nodeapi_TYPE_OP(&$node, $a3, $a4) {
  // From hosting_nodeapi_client_delete_revision().
  db_query('DELETE FROM {hosting_client} WHERE vid = %d', $node->vid);
}

/**
 * TODO: Remove this hook documentation?
 *
 * @param $task
 *   The task.
 * @param $task_type
 *   The type of the task, e.g. 'verify'.
 *
 * @see hosting_queues_get_arguments()
 */
function hook_provision_args($task, $task_type) {

}

/**
 * Perform actions when a task has completed succesfully.
 *
 * Replace TASK_TYPE with the type of task that if completed you will be
 * notified of. This is a good place to hook in and record changes in the
 * frontend as a result of the task executing in the backend. If you just want
 * to hook into the backend then you probably want to consider using the
 * 'standard' Drush hooks there, i.e. host_post_provision_verify(),
 * hook_post_provision_install() etc.
 *
 * @param $task
 *   The hosting task that has completed.
 * @param $data
 *   An associative array of the drush output of the completed backend task from
 *   drush_backend_output(). The array should contain at least the following:
 *   - "output": The raw output from the drush command executed.
 *   - "error_status": The error status of the command run on the backend,
 *     should be DRUSH_SUCCESS normally.
 *   - "log": The drush log messages.
 *   - "error_log": The list of errors that occurred when running the command.
 *   - "context": The drush options for the backend command, this may contain
 *     options that were set when the command ran, or options that were set by
 *     the command itself.
 *
 * @see drush_hosting_post_hosting_task()
 * @see drush_backend_output()
 */
function hook_post_hosting_TASK_TYPE_task($task, $data) {
  // From hosting_site_post_hosting_backup_task().
  if ($data['context']['backup_file'] && $task->ref->type == 'site') {
    $platform = node_load($task->ref->platform);

    $desc = $task->task_args['description'];
    $desc = ($desc) ? $desc : t('Generated on request');
    hosting_site_add_backup($task->ref->nid, $platform->web_server, $data['context']['backup_file'], $desc, $data['context']['backup_file_size']);
  }
}

/**
 * Process the specified queue.
 *
 * Modules providing a queue should implement this function an process the
 * number of items from the queue that are specified. It is up the to the
 * module to determine which items it wishes to process.
 *
 * If you wish to process multiple items at the same time you will need to fork
 * the process by calling drush_invoke_process() with the 'fork' option,
 * specifying a drush command with the arguments required to process your task.
 * Otherwise you can do all your processing in this function, or similarly call
 * drush_invoke_process() without the 'fork' option.
 *
 * @param $count
 *   The maximum number of items to process.
 *
 * @see hosting_run_queue()
 * @see hosting_get_queues()
 */
function hosting_QUEUE_TYPE_queue($count = 5) {
  // From hosting_tasks_queue().
  global $provision_errors;

  drush_log(dt("Running tasks queue"));
  $tasks = _hosting_get_new_tasks($count);
  foreach ($tasks as $task) {
    drush_invoke_process('@self', "hosting-task", array($task->nid), array(), array('fork' => TRUE));
  }
}

/**
 * @see hosting_queues()
 */
function hosting_TASK_SINGULAR_list() {

}

/**
 * @see hosting_queue_block()
 */
function hosting_TASK_SINGULAR_summary() {

}

/**
 * @} End of "addtogroup hostinghooks".
 */
