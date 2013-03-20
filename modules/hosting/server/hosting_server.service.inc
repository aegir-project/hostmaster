<?php
/**
 * @file Define the base Hosting service class.
 */

class hostingService {

  public $server;
  public $available = 0;

  protected $has_restart_cmd = FALSE;
  protected $has_port = FALSE;

  function __construct($node, $values = NULL) {
    self::save($node);
    self::setValues($values);
  }

  public function has_port() {
    return $this->has_port;
  }

  public function load() {
    $this->mergeData("SELECT port, restart_cmd, available
                      FROM {hosting_service}
                      WHERE vid = :vid
                      AND type = :type",
                      array(
                        ':vid' => $this->server->vid,
                        ':type' => $this->type
                      )
                    );
  }

  protected function mergeData($query, $args = array()) {
    $result = db_query($query, $args)->fetchAssoc();
    $this->setValues($result);
  }

  public function setValues($record = NULL) {
    if (is_array($record)) {
      foreach ($record as $key => $value) {
        $this->{$key} = $value;
      }
    }
  }

  public function save($node) {
    if (isset($node->nid)) {
      $this->server = new stdClass();
      $this->server->nid = $node->nid;
      $this->server->vid = $node->vid;
      $this->server->title = $node->title;
    }
  }

  public function default_restart_cmd() {
    return '';
  }

  public function default_port() {
    return 0;
  }

  public function insert() {
    $id = db_insert('hosting_service')
      ->fields(array(
        'nid' => $this->server->nid,
        'vid' => $this->server->vid,
        'service' => $this->service,
        'type' => $this->type,
        'port' => $this->port,
        'restart_cmd' => $this->restart_cmd,
        'available' => $this->available,
      ))
      ->execute();
  }

  public function update() {
    $this->delete_revision();
    $this->insert();
  }

  public function delete() {
    db_delete('hosting_service')
      ->condition('service', $this->service)
      ->condition('nid', $this->server->nid)
      ->execute();
  }

  public function delete_revision() {
    db_delete('hosting_service')
      ->condition('service', $this->service)
      ->condition('vid', $this->server->vid)
      ->execute();
  }

  public function form(&$form) {
    if ($this->has_restart_cmd) {
      $form['restart_cmd'] = array(
        '#type' => 'textfield',
        '#title' => t('Restart command'),
        '#required' => !empty($this->available),
        '#description' => t('The command to run to restart this service.'),
        '#default_value' => (isset($this->restart_cmd)) ? $this->restart_cmd : $this->default_restart_cmd(),
        '#size' => 40,
        '#maxlength' => 255,
        '#weight' => -7,
      );
    }
    else {
      $form['restart_cmd'] = array(
        '#type' => 'value',
        '#value' => NULL,
      );
    }


    if ($this->has_port) {
      $form['port'] = array(
        '#type' => 'textfield',
        '#title' => t('Port'),
        '#required' => !empty($this->available),
        '#size' => 40,
        '#default_value' => isset($this->port) ? $this->port : $this->default_port(),
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
        '#markup' => filter_xss($this->restart_cmd),
      );

    }

    if ($this->has_port) {
      $render['port'] = array(
        '#type' => 'item',
        '#title' => t('Port'),
        '#markup' => filter_xss($this->port),
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

