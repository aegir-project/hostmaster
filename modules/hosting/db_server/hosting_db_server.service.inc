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
      '#required' => $this->available,
      '#title' => t('Username'),
      '#description' => t('The user that will be used to create new mysql users and databases for new sites.'),
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
      '#required' => ($this->db_passwd) ? FALSE : $this->available,
      '#description' => $passwd_description . t('The password for the user that will be used to create new mysql users and databases for new sites'),
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
    if (!empty($this->db_passwd)) {
      parent::update();
    }
    else {
      // only do the parent's update routine.
      parent::delete_revision();
      parent::insert();
    }
  }

  function delete_revision() {
    parent::delete_revision();
    db_query('DELETE FROM {hosting_db_server} WHERE vid = %d', $this->server->vid);
  }

  function delete() {
    parent::delete();
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

  public function options($task_type, $ref_type, &$task) {
    parent::options($task_type, $ref_type, $task);

    $web_server = _hosting_get_web_server($task);

    // If the web server nid is the same as the db server nid, assume the db_host to be localhost
    if ($web_server->nid == $this->server->nid) {
      $db_host = 'localhost';
    }
    else {
      $db_host = $this->server->title;
    }

    if (($ref_type === 'server' || $ref_type === 'platform') && $task_type === 'verify') {
      $task->options['master_db'] = sprintf("%s://%s:%s@%s", $this->db_type, urlencode($this->db_user), urlencode($this->db_passwd), urlencode($db_host));
    }

    if ($ref_type == 'site') {
      $task->options['db_id'] = $this->server->nid;
      $task->options['db_host'] = $db_host;
      $task->options['master_db_host'] = $db_host;
      $task->options['master_db_user'] = $this->db_user;
      $task->options['master_db_passwd'] = $this->db_passwd;
    }

  }
}



