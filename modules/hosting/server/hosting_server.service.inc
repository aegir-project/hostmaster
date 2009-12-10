<?php

function hosting_server_init_services(&$node) {
  $node = (is_int($node)) ? node_load($node) : $node;

  $services = module_invoke_all('hosting_service');

  $result = db_query("SELECT service, type FROM {hosting_service} WHERE vid=%d", $node->vid);
  while ($record = db_fetch_object($result)) {
    $name = $record->service;
    $class = $services[$name]['class'];
    if ($services[$name]['file']) {
      include_once(drupal_get_path('module', $services[$name]['module']) . '/' . $services[$name]['file']);
    }
    $node->services[$name] = new $class($node);
  }

  hosting_server_invoke_services($node, 'load');
  return $node;
}

function hosting_server_invoke_services(&$node, $method, &$arg1 = null, &$arg2 = null, &$arg3 = null, &$arg4 = null) {
  $return = array();
  foreach ($node->services as $name => $service) {
    $result = $service->$method($arg1, $arg2, $arg3, $arg4);
    if (isset($result) && is_array($result)) {
      $return = array_merge_recursive($return, $result);
    }
    else if (isset($result)) {
      $return[] = $result;
    }
  }
  return $return;
}

class hostingService {

  public $server;
  public $service_title = null;
  public $type = null;

  function __construct($node) {
    $this->server = (is_int($node)) ? node_load($node) : $node;
  }

  public function load() { }

  protected function mergeData($query) {
    $args = func_get_args();
    $result = db_fetch_array(call_user_func_array('db_query', $args));

    foreach ($result as $key => $value) {
      $this->{$key} = $value;
    }
  }

  public function save($node) {
    $this->server = $node;

    // if this is a new node or we're adding a new revision,
    if ($this->server->revision) {
      $this->insert();
    }
    else {
      $this->update();
    }
  }

  public function insert() { }
  public function update() { }
  
  public function delete() { }
  public function delete_revision() { }

  public function form(&$form) { }
  public function view(&$render) { }

}

