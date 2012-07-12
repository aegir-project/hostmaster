<?php
/**
 * @file
 *   Provide the hosting serivce classes for database integration.
 */

/**
 * The base db service type class.
 */
class hostingService_db extends hostingService {
  public $service = 'db';
}

/**
 * A MySQL specific db service implementation class.
 */
class hostingService_db_mysql extends hostingService_db {
  public $type = 'mysql';
  public $has_port = TRUE;

  function default_port() {
    return 3306;
  }

  function form(&$form) {
    parent::form($form);
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
      '#required' => FALSE,
      '#description' => $passwd_description . t('The password for the user that will be used to create new mysql users and databases for new sites'),
      '#size' => 30,
      '#weight' => 10
    );
  }

  function insert() {
    parent::insert();
    db_query("INSERT INTO {hosting_db_server} (vid, nid, db_user, db_passwd) 
      VALUES (%d, %d, '%s', '%s')", 
      $this->server->vid, $this->server->nid, $this->db_user, $this->db_passwd);
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
    $this->mergeData('SELECT db_user, db_passwd FROM {hosting_db_server} WHERE vid = %d', $this->server->vid);
  }

  function view(&$render) {
    parent::view($render);

    $render['db_user'] = array(
      '#type' => 'item',
      '#title' => t('Database user'),
      '#value' => filter_xss($this->db_user),
    );

  }

  public function context_options($task_type, $ref_type, &$task) {
    parent::context_options($task_type, $ref_type, $task);

    // Provide context_options for verification and writing out to an alias
    $task->context_options['master_db'] = 'mysql' . '://' . urlencode($this->db_user) . ':' . urlencode($this->db_passwd) . '@' . $this->server->title;
  }

  public function context_import($context) {
    parent::context_import($context);

    $matches = array();
    preg_match("+^mysql://(.*):(.*)@.*$+", stripslashes($context->master_db), $matches);
    $this->db_user = urldecode($matches[1]);
    $this->db_passwd = urldecode($matches[2]);
  }
}
