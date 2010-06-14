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
    db_query("DELETE FROM {hosting_service} WHERE type = '%s' AND nid=%d", $this->type, $this->server->nid);
  }

  public function delete_revision() {
    db_query("DELETE FROM {hosting_service} WHERE type = '%s' AND vid=%d", $this->type, $this->server->vid);
  }

  public function form(&$form) { }
  public function view(&$render) { }

  public function validate(&$node, &$form) {}

  public function context_options($task_type, $ref_type, &$task) {
    if ($this->type !== 'base') {
      $task->context_options[$this->service . '_service_type'] = $this->type;
    }
  }
}

class hostingService_server_base extends hostingService {
  protected $service = 'server';
  public $type = 'base';

  function form(&$form) {
    $form['script_user'] = array(
      '#type' => 'textfield',
      '#title' => t('System account'),
      '#required' => $this->available,
      '#description' => t('The system account that the hosted files will belong to, for security reasons.<br />This should be a different account to the account the server is running as.'),
      '#default_value' => ($this->script_user) ? $this->script_user : HOSTING_DEFAULT_SCRIPT_USER,
      '#size' => 20,
      '#maxlength' => 255,
      '#weight' => -6,
    );
      
    $form['config_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Configuration path'),
      '#required' => $this->available,
      '#size' => 40,
      '#default_value' => ($this->config_path) ? $this->config_path : HOSTING_DEFAULT_CONFIG_PATH,
      '#description' => t("The path on the server where configuration files will be stored.<br />
          It is essential that this directory should not be accessible via a web browser."),
      '#maxlength' => 255,
      '#weight' => -4,
    
    );

  }


  function validate(&$node, &$form) {
    if ($this->script_user == 'root') {
      form_set_error('script_user', t('For security reasons, you should not run drush commands as the root user. Please choose a different system account name.'));
    }
  }

  function insert() {
    parent::insert();
    db_query("INSERT INTO {hosting_server} (vid, nid, script_user, config_path) 
        VALUES (%d, %d, '%s', '%s')", 
          $this->server->vid, $this->server->nid, $this->script_user, $this->config_path);
  }

  function delete_revision() {
    parent::delete_revision();
    db_query('DELETE FROM {hosting_server} WHERE vid = %d', $this->server->vid);
  }

  function delete() {
    parent::delete();
    db_query('DELETE FROM {hosting_server} WHERE nid = %d', $this->server->nid);
  }

  function load() {
    parent::load();
    $this->mergeData('SELECT script_user, config_path FROM {hosting_server} WHERE vid = %d', $this->server->vid);
  }

  function view(&$render) {
    $render['script_user'] = array(
      '#type' => 'item',
      '#title' => t('Script user'),
      '#value' => filter_xss($this->script_user),
    );

    $render['config_path'] = array(
      '#type' => 'item',
      '#title' => t('Configuration path'),
      '#value' => filter_xss($this->config_path),
    );
  }

  /**
   * Override hostingService::options() because we don't want to initialize
   * this service in provision.
   */
  public function context_options($task_type, $ref_type, &$task) {
    $task->context_options['remote_host'] = $this->server->title;
    $task->context_options['script_user'] = $this->script_user;
  }
}
