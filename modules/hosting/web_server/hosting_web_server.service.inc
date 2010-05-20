<?php

class hostingService_http_Apache extends hostingService {
  protected $service = 'http';
  public $type = 'apache';

  public function load() {
    parent::load();
    $this->mergeData('SELECT web_group, restart_cmd, ports FROM {hosting_web_server} WHERE vid = %d', $this->server->vid);
  }

  function insert() {
    parent::insert();
    db_query("INSERT INTO {hosting_web_server} (vid, nid, web_group, restart_cmd, ports) 
        VALUES (%d, %d, '%s', '%s', '%s')", 
          $this->server->vid, $this->server->nid, $this->web_group, $this->restart_cmd, $this->ports);
  }


  /**
   * Implementation of hook_validate().
    */
  function validate(&$node, &$form) {

    $ports = explode(",", $this->ports);
    if (count($ports) < 1) {
      form_set_error('ports', t('At least one port must be provided.'));
    }
    foreach($ports as $port) {
      if ($port < 1) {
        form_set_error('ports', t('Invalid port specified'));
      }
    }
  }

  public function form(&$form) {
    parent::form($form);
    $node = $form['#node'];

    if (hosting_feature('ports') == HOSTING_FEATURE_ENABLED) {
      $form['ports'] = array(
        '#type' => 'textfield',
        '#title' => t('Ports'),
        '#required' => $this->available,
        '#size' => 40,
        '#default_value' => ($this->ports) ? $this->ports : '80',
        '#description' => t("All the ports enabled on this server, comma separated. Port 80 is the default http, 443 is https."),
        '#maxlength' => 255,
        '#weight' => -8,
      );
    } else {
      $form['ports'] = array(
        '#type' => 'value',
        '#value' => '80',
      );
    }
    
    $form['restart_cmd'] = array(
        '#type' => 'textfield',
        '#title' => t('Restart command'),
        '#required' => $this->available,
        '#description' => t('The command to run to restart the web server for new changes to take effect. This is required for the new site to become live'),
        '#default_value' => ($this->restart_cmd) ? $this->restart_cmd : HOSTING_DEFAULT_RESTART_CMD,
        '#size' => 40,
        '#maxlength' => 255,
        '#weight' => -7,
    );
    
    $form['web_group'] = array(
      '#type' => 'textfield',
      '#title' => t('Web server group'),
      '#required' => $this->available,
      '#description' => t('The group that the hosted files will belong to.<br />This should be the group the web server is running as.'),
      '#default_value' => ($this->web_group) ? $this->web_group : HOSTING_DEFAULT_WEB_GROUP,
      '#size' => 20,
      '#maxlength' => 75,
      '#weight' => -5,
    );

  }

  function view(&$render) {
    parent::view($render);
    if (hosting_feature('ports') == HOSTING_FEATURE_ENABLED) {
      $render['ports'] = array(
        '#type' => 'item',
        '#title' => t('Ports'),
        '#value' => filter_xss($this->ports),
      );
    }
    $render['web_group'] = array(
      '#type' => 'item',
      '#title' => t('Web server group'),
      '#value' => filter_xss($this->web_group),
    );
    $render['restart_cmd'] = array(
      '#type' => 'item',
      '#title' => t('Restart command'),
      '#value' => filter_xss($this->restart_cmd),
    );
  }

  function delete() {
    parent::delete();
    db_query('DELETE FROM {hosting_web_server} WHERE nid = %d', $this->server->nid);
  }

  function delete_revision() {
    parent::delete_revision();
    db_query('DELETE FROM {hosting_web_server} WHERE vid = %d', $this->server->vid);
  }

  function update() {
    parent::update();
    $query = db_query("SELECT nid FROM {hosting_platform} WHERE web_server = %d", $this->server->nid);
    while($nid = db_fetch_object($query)) {
      hosting_add_task($nid->nid, 'verify');
    }
  }

  public function context_options($task_type, $ref_type, &$task) {
    parent::context_options($task_type, $ref_type, $task);

    $task->context_options['web_ports'] = explode(",", $this->ports);
    foreach($task->context_options['web_ports'] as $key => $val) {
      $val = trim($val);
      $task->context_options['web_ports'][$key] = $val;
      if(!is_numeric($val)) {
        unset($task->context_options['web_ports'][$key]);
      }
    }

    $task->context_options['web_group'] = $this->web_group;
    $task->context_options['restart_cmd'] = $this->restart_cmd;
  }
}
