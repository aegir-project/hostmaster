<?php

class hostingService_DB extends hostingService {
  protected $service = 'db';
  protected $type = 'mysql';

  function form(&$form) {
    $node = $form['#node'];

    $form['db_type'] = array(
      '#type' => 'value',
      '#value' => ($this->db_type) ? $this->db_type : 'mysqli'
    );
    $form['db_user'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('Username'),
      '#description' => t('The user that will be used to create users and databases for new sites.'),
      '#size' => 40,
      '#default_value' => $this->db_user,
      '#maxlength' => 64,
      '#weight' => 5,
    );
    if ($this->db_passwd) {
      $passwd_description = t('<strong>You have already set a password for this database server.</strong><br />');
    }
    $form['db_passwd'] = array(
      '#type' => 'password_confirm',
      '#required' => ($this->db_passwd) ? FALSE : TRUE,
      '#description' => $passwd_description . t('The user account that will be used to create new mysql users and databases for new sites'),
      '#size' => 30,
      '#weight' => 10
    );
  }

  function insert() {
    parent::insert();
    db_query("INSERT INTO {hosting_db_server} (vid, nid, db_type, db_user, db_passwd) 
      VALUES (%d, %d, '%s', '%s', '%s')", 
        $this->server->vid, $this->server->nid, $this->db_type, $this->db_user, $this->db_passwd);
  }

  function update() {
    parent::update();
    db_query("UPDATE {hosting_db_server} SET 
                  db_type = '%s', db_user = '%s', db_passwd = '%s'
              WHERE 
                  vid = %d", 
                  $this->db_type, $this->db_user, $this->db_passwd, $this->server->vid);

  }

  function delete_revision() {
    db_query('DELETE FROM {hosting_db_server} WHERE vid = %d', $this->server->vid);
  }

  function delete() {
    db_query('DELETE FROM {hosting_db_server} WHERE nid = %d', $this->server->nid);

  }


  function load() {
    parent::load();
    $this->mergeData('SELECT db_type, db_user, db_passwd FROM {hosting_db_server} WHERE vid = %d', $this->server->vid);
  }

  function view(&$render) {
    $render['db_type'] = array(
      '#type' => 'item',
      '#title' => t('Database type'),
      '#value' => filter_xss($this->db_type),
    );
    $render['db_user'] = array(
      '#type' => 'item',
      '#title' => t('Database user'),
      '#value' => filter_xss($this->db_user),
    );

  }

  public function db_url() {
    return sprintf("%s://%s:%s@%s", $this->db_type, urlencode($this->db_user), urlencode($this->db_passwd), urlencode($this->server->title));
  }


  public function options($task_type, $ref_type, &$task) {
    if ($ref_type == 'platform' && $task_type == 'verify') {
      $task->options['master_db'] = $this->db_url();
    }

    if ($ref_type == 'site') {
      $task->options['db_id'] = $this->node->nid;
    }

  }
}



