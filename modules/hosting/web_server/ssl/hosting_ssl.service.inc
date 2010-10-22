<?php

class hostingService_http_ssl extends hostingService_http_public {

  function load() {
    parent::load();
    $this->mergeData("SELECT ssl_port FROM {hosting_ssl_server} WHERE vid=%d", $this->server->vid);
    $this->ssl_enabled = TRUE;
  }

  function insert() {
    parent::insert();
    db_query("INSERT INTO {hosting_ssl_server} (vid, nid, ssl_port) VALUES (%d, %d, %d)", $this->server->vid, $this->server->nid, $this->ssl_port);
  }

  function delete() {
    parent::delete();
    db_query("DELETE FROM {hosting_ssl_server} WHERE nid=%d", $this->server->nid);
  }

  function delete_revision() {
    parent::delete_revision();
    db_query("DELETE FROM {hosting_ssl_server} WHERE vid=%d", $this->server->vid);
  }

  function form(&$form) {
    parent::form($form);

    $form['ssl_port'] = array(
      '#type' => 'textfield',
      '#title' => t('SSL Port'),
      '#required' => $this->available,
      '#size' => 40,
      '#default_value' => ($this->ssl_port) ? $this->ssl_port : '443',
      '#description' => t("The port that this service will use for encrypted traffic."),
      '#maxlength' => 255,
      '#weight' => -7);
  }

  function view(&$render) {
    parent::view($render);
    $render['ssl_port'] = array(
      '#type' => 'item',
      '#title' => t('SSL Port'),
      '#value' => filter_xss($this->ssl_port),
    );
  }
  
  public function validate(&$node, &$form) {
    parent::validate($node, $form);
    if ((int) $this->ssl_port <= 0) {
      form_set_error('ssl_port', t("The port you specify must be a number."));
    }
  }

  public function context_options($task_type, $ref_type, &$task) {
    parent::context_options($task_type, $ref_type, $task);
    $task->context_options[$this->service . '_ssl_port'] = $this->ssl_port;
  }

  public function context_import($context) {
    parent::context_import($context);

    $this->ssl_port = $context->http_ssl_port;
  }

}

class hostingService_http_apache_ssl extends hostingService_http_ssl {
  public $type = 'apache_ssl';

  
  protected $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    $command = '/usr/sbin/apachectl'; # a proper default for most of the world
    foreach (explode(':', $_SERVER['PATH']) as $path) {
      $options[] = "$path/apache2ctl";
      $options[] = "$path/apachectl";
    }
    # try to detect the apache restart command
    $options[] = '/usr/local/sbin/apachectl'; # freebsd
    $options[] = '/usr/sbin/apache2ctl'; # debian + apache2
    $options[] = $command;

    foreach ($options as $test) {
      if (is_executable($test)) {
        $command = $test;
        break;
      }
    }

    return "sudo $command graceful";
  }

}


