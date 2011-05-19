<?php
/**
 *   $Id$
 */

/**
 * @file
 *   Example service implementation for the hosting front end.
 *
 * @addtogroup hostingserviceapi
 * @{
 */

/**
 * The base service type class , registered with hook_hosting_service_type.
 */
class hostingService_example extends hostingService {
 /**
  * the value stored in the service column of hosting_service table.
  */
  public $service = 'example';
}


/**
 * An implementation of the example service type, registered with hook_hosting_service.
 */

class hostingService_example_basic extends hostingService_example {
 /**
  * the value stored in the type column of the hosting_service table.
  */
  public $type = 'basic';


 /**
  * this service needs to have a port specified for it.
  */
  public $has_port = TRUE;

 /**
  * the default value for the port input.
  */
  function default_port() {
    return 12345;
  }

 /**
  * this service needs to be restarted with a shell command.
  */
  public $has_restart_cmd = TRUE;

 /**
  * The default value for the restart command input.
  */
  function default_restart_cmd() {
    return "/usr/bin/true";
  }



 /**
  * node operations
  */

  /**
   * Load associated values for the service.
   *
   * In this example we will use the variable system to retrieve values.
   */
  function load() {
    // REMEMBER TO CALL THE PARENT!
    parent::load();
    $this->example_field = variable_get('hosting_example_field_' . $this->server->nid, 'default value');

    /**
     * Although this example does not have it's own tables, we provide some utitilty functions
     * for use in this method.
     * 
     * If this example used it's own tables, we could use the mergeData method below to merge in the
     * results automatically, instead of iterating through the results ourself.
     */

     // $this->mergeData("SELECT example_field FROM {hosting_example} WHERE vid = %d", $this->server->vid);
  }

  /**
   * Display settings on the server node page.
   *
   * Modify the reference passed to the method to add additional implementation
   * specific fields to be displayed.
   *
   * @param 
   *   A reference to the associative array of the subsection of the page
   *   reserved for this service implementation.
   */
  function view(&$render) {
    // REMEMBER TO CALL THE PARENT!
    parent::view($render);
    $render['example_field'] = array(
      '#type' => 'item',
      '#title' => t('Example field'),

      // Remember to pass the display through filter_xss!
      '#value' => filter_xss($this->example_field),
    );
  }

  /**
   * Extend the server node form.
   *
   * Modify the reference passed to the method to add additional implementation
   * specific fields to be stored and managed.
   *
   * @param 
   *   A reference to the associative array of the subsection of the form
   *   reserved for this service implementation.
   */
  function form(&$form) {
    // REMEMBER TO CALL THE PARENT!
    parent::form($form);

    $form['example_field'] = array(
      '#type' => 'textfield',
      '#title' => t('Example field'),
      '#description' => t('An example field for the user to fill in.'),
      '#size' => 40,
      '#default_value' => ($this->example_field) ? $this->example_field : 'default value',
      '#maxlength' => 64,
      '#weight' => 5,
    );
  }

  /**
   * Validate a form submission.
   */
  function validate(&$node, &$form) {
    // REMEMBER TO CALL THE PARENT!
    parent::validate($node, $form);

    if (sizeof($this->example_field) > 30) {
      form_set_error('example_field', t("The example string must not be longer than 30 characters"));
    }
  }

 /**
  * Insert a record into the database.
  *
  * Called by hosting_server_hook_insert().
  *
  * The values associated with this implementation have already
  * been set as properties of $this object, so we now need to 
  * save them.
  *
  * For this example we will use the variables table, but you should
  * create your own tables with an install file and hook_schema.
  */
  function insert() {
    // REMEMBER TO CALL THE PARENT!
    parent::insert();

    variable_set('hosting_example_field_' . $this->server->nid, $this->example_field);
  }

 /**
  * Update a record in the database.
  *
  * Called by hosting_server_hook_update().
  *
  * For this example we will use the variables table, but you should
  * create your own tables with an install file and hook_schema.
  */
  function update() {
    // REMEMBER TO CALL THE PARENT!
    parent::update();

    variable_set('hosting_example_field_' . $this->server->nid, $this->example_field);
  }

  /**
   * Delete a record from the database, based on server node.
   */
   function delete() {
     // REMEMBER TO CALL THE PARENT!
     parent::delete();

     variable_del('hosting_example_field_' . $this->server->nid);
   }

  /**
   * Delete a specific reivision from the database.
   *
   * Not relevant in our example but shown anyway.
   */
   function delete_revision() {
      // REMEMBER TO CALL THE PARENT!
     parent::delete_revision();
   }

   /**
    * Pass values to the provision backend when we call provision-save.
    *
    * By selecting this type we already pass the '--example_service_type=basic' option
    * to the command, which will load the matching provisionService class in the backend.
    *
    * This backend class will be responsible for receiving and reacting to the options
    * passed here.
    *
    * @ingroup backend-frontend-IPC
    */
   public function context_options($task_type, $ref_type, &$task) {
     // REMEMBER TO CALL THE PARENT!
     parent::context_options($task_type, $ref_type, $task);
     
     $task->context_options['example_field'] = $this->example_field;
   }
}

/**
 * @} End of "addtogroup hostingserviceapi".
 */