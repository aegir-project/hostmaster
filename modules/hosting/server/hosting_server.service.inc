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

  public function options($task_type, $ref_type, &$task) {
    if ($task_type === 'named-context-save' && $ref_type === 'server') {
      $task->options['provision-service-' . $this->service] = $this->type;
    }
  }
}

class hostingService_base extends hostingService {
  protected $service = 'server';
  protected $type = 'base';

  function form(&$form) {

    $form['ip_address'] = array(
      '#type' => 'textfield',
      '#title' => t('IP address'),
      '#default_value' => $this->ip_address,
      '#size' => 15,
      '#maxlength' => 15,
      '#description' => t("The IP address the server can be accessed by. This will be used to create database grants, amongst other things.<br />If this is empty, Hostmaster will attempt to resolve the hostname to get the address.<br />If that fails, the hostname (ie. the node title) will be used instead."),
      '#weight' => -9
    );

    $form['drush_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Drush path'),
      '#required' => $this->available,
      '#size' => 40,
      '#default_value' => ($this->drush_path) ? $this->drush_path : HOSTING_DEFAULT_PARENT_PATH.'/drush/drush.php',
      '#description' => t("The full path to the drush.php script on this server."),
      '#maxlength' => 255,
      '#weight' => -8,
    );

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
    
    $form['backup_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Backup path'),
      '#required' => $this->available,
      '#size' => 40,
      '#default_value' => ($this->backup_path) ? $this->backup_path : HOSTING_DEFAULT_BACKUP_PATH,
      '#description' => t("The path on the server where backups will be stored.<br />
          It is essential that this directory should not be accessible via a web browser."),
      '#maxlength' => 255,
      '#weight' => -3,
    );


  }


  function validate(&$node, &$form) {
    if ($this->script_user == 'root') {
      form_set_error('script_user', t('For security reasons, you should not run drush commands as the root user. Please choose a different system account name.'));
    }
    if ($this->ip_address) {
      if (!_hosting_valid_ip($this->ip_address)) {
        form_set_error('ip_address', t('Invalid IP address'));
      }
    }
  }

  function insert() {
    parent::insert();
    db_query("INSERT INTO {hosting_server} (vid, nid, ip_address, script_user, config_path, backup_path, drush_path) 
        VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s')", 
          $this->server->vid, $this->server->nid, $this->ip_address, $this->script_user, $this->config_path, $this->backup_path, $this->drush_path);
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
    $this->mergeData('SELECT ip_address, script_user, config_path, backup_path, drush_path FROM {hosting_server} WHERE vid = %d', $this->server->vid);
  }

  function view(&$render) {
    if ($this->ip_address) {
      $render['ip_address'] = array(
        '#type' => 'item',
        '#title' => t('IP address'),
        '#value' => filter_xss($this->ip_address),
      );    
    }
    $render['drush_path'] = array(
      '#type' => 'item',
      '#title' => t('Drush path'),
      '#value' => filter_xss($this->drush_path),
    );

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
    $render['backup_path'] = array(
      '#type' => 'item',
      '#title' => t('Backup path'),
      '#value' => filter_xss($this->backup_path),
    );
  }

  /**
   * Override hostingService::options() because we don't want to initialize
   * this service in provision.
   */
  public function options($task_type, $ref_type, &$task) {
    $task->options['web_host'] = $this->server->title;
    $task->options['drush_path'] = $this->drush_path;
    $task->options['web_ip'] = $this->ip_address;
    $task->options['remote-user'] = $this->script_user;
  }
}
