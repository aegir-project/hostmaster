<?php


class hostingService {

  public $server;

  protected $has_restart_cmd = FALSE;
  protected $has_port = FALSE;

  function __construct($node, $values = null) {
    self::save($node);
    self::setValues($values);
  }

  public function has_port() {
    return $this->has_port;
  }

  public function load() {
    $this->mergeData("SELECT port, restart_cmd, available FROM {hosting_service} WHERE vid = %d AND type = '%s'", $this->server->vid, $this->type);
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



  public function default_restart_cmd() {
    return '';
  }

  public function default_port() {
    return 0;
  }

  public function insert() {
    db_query("INSERT INTO {hosting_service} (nid, vid, service, type, port, restart_cmd, available) VALUES (%d, %d, '%s', '%s', %d, '%s', %d)",
      $this->server->nid, $this->server->vid, $this->service, $this->type, $this->port, $this->restart_cmd, $this->available);
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

  public function form(&$form) {
    if ($this->has_restart_cmd) {
      $form['restart_cmd'] = array(
          '#type' => 'textfield',
          '#title' => t('Restart command'),
          '#required' => $this->available,
          '#description' => t('The command to run to restart this service.'),
          '#default_value' => (!is_null($this->restart_cmd)) ? $this->restart_cmd : $this->default_restart_cmd(),
          '#size' => 40,
          '#maxlength' => 255,
          '#weight' => -7,
      );
    }
    else {
      $form['restart_cmd'] = array('#type' => 'value', '#value' => null);
    }
 

    if ($this->has_port) {
     $form['port'] = array(
        '#type' => 'textfield',
        '#title' => t('Port'),
        '#required' => $this->available,
        '#size' => 40,
        '#default_value' => ($this->port) ? $this->port : $this->default_port(),
        '#description' => t("The port that this service is listening on."),
        '#maxlength' => 255,
        '#weight' => -8,
      );
    }
    else {
      $form['port'] = array(
        '#type' => 'value',
        '#value' => '0',
      );
    } 
 
  }

  public function view(&$render) {
    if ($this->has_restart_cmd) {
      $render['restart_cmd'] = array(
        '#type' => 'item',
        '#title' => t('Restart command'),
        '#value' => filter_xss($this->restart_cmd),
      );

    } 
   
    if ($this->has_port) {
      $render['port'] = array(
        '#type' => 'item',
        '#title' => t('Port'),
        '#value' => filter_xss($this->port),
      );
    } 
  }



  public function validate(&$node, &$form) {
    if ($this->has_port) {
      if ((int) $this->port <= 0) {
        form_set_error('port', t("The port you specify must be a number."));
      }
    }
  }

  public function context_options($task_type, $ref_type, &$task) {
    $task->context_options[$this->service . '_service_type'] = $this->type;

    if ($this->has_restart_cmd) {
      $task->context_options[$this->service . '_restart_cmd'] = $this->restart_cmd;
    }

    if ($this->has_port) {
      $task->context_options[$this->service . '_port'] = $this->port;
    }
  }

  public function context_import($context) {
    $this->available = 1;

    if ($this->has_port) {
      $this->port = $context->{$this->service . '_port'};
    }

    if ($this->has_restart_cmd) {
      $this->restart_cmd = $context->{$this->service . '_restart_cmd'};
    }
  }

}

