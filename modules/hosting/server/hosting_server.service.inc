<?php


class hostingService {

  public $server;

  function __construct($node, $values = null) {
    self::save($node);
    self::setValues($values);
  }

  public function load() {
    $this->mergeData("SELECT available FROM {hosting_service} WHERE vid = %d AND type = '%s'", $this->server->vid, $this->type);
  }

  protected function mergeData($query) {
    $args = func_get_args();
    $result = db_fetch_array(call_user_func_array('db_query', $args));
    $this->setValues($result);
  }


  public function setValues($values = null) {
    if (is_array($values)) {
      foreach ($values as $key => $value) {
        $this->{$key} = $value;
      }
    }
  }

  public function save($node) {
    $this->server = new stdClass();
    $this->server->nid = $node->nid;
    $this->server->vid = $node->vid;
    $this->server->title = $node->title;
  }

  
  public function insert() {
    db_query("INSERT INTO {hosting_service} (nid, vid, service, type, available) VALUES (%d, %d, '%s', '%s', %d)",
      $this->server->nid, $this->server->vid, $this->service, $this->type, $this->available);
  }

  public function update() {
    $this->delete_revision();
    $this->insert();
  }
  
  public function delete() {
    db_query("DELETE FROM {hosting_service} WHERE service = '%s' AND nid=%d", $this->service, $this->server->nid);
  }

  public function delete_revision() {
    db_query("DELETE FROM {hosting_service} WHERE service = '%s' AND vid=%d", $this->service, $this->server->vid);
  }

  public function form(&$form) { }
  public function view(&$render) { }

  public function validate(&$node, &$form) {}

  public function context_options($task_type, $ref_type, &$task) {
    $task->context_options[$this->service . '_service_type'] = $this->type;
  }
}

class hostingService_server_base extends hostingService {
  protected $service = 'server';
  public $type = 'base';

  function insert() {
    parent::insert();

  }

  function delete_revision() {
    parent::delete_revision();
  }

  function delete() {
    parent::delete();
    db_query('DELETE FROM {hosting_server} WHERE nid = %d', $this->server->nid);
  }

  /**
   * Override hostingService::options() because we don't want to initialize
   * this service in provision.
   */
  public function context_options($task_type, $ref_type, &$task) {
    $task->context_options['remote_host'] = $this->server->title;
  }
}
