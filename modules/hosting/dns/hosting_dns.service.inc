<?php
/**
 * @file
 *   Provide the hosting serivce classes for DNS integration.
 */

/**
 * DNS service base class.
 */
class hostingService_dns extends hostingService {
  public $service = 'dns';
}

/**
 * Helper class to factor together master functionality
 *
 * This mostly consists of master->slave mapping
 */
class hostingService_dns_master extends hostingService_dns {
  function view(&$render) {
    parent::view($render);

    if (sizeof($this->slave_servers)) {
      $render['slave_servers'] = array(
        '#type' => 'item',
        '#title' => t('Slave servers'),
        '#value' => theme('item_list', array_map('_hosting_node_link', array_intersect_key($this->slave_servers, hosting_get_servers('dns')))),
      );
    }
  }

  function form(&$form) {
    parent::form($form);

    $servers = hosting_get_servers('dns');
    unset($servers[$this->server->nid]);

    if (sizeof($servers)) {
      $form['slave_servers'] = array(
        '#title' => t('Slave DNS servers'),
        '#type' => 'checkboxes',
        '#options' => $servers,
        '#default_value' => isset($this->slave_servers) ? $this->slave_servers : array(),
      );
    }
    else {
      $form['slave_server'] = array(
        '#type' => 'value',
        '#value' => array(),
      );
    }
  }

  function load() {
    parent::load();

    $slave_servers = array();
    $result = db_query('SELECT slave_nid FROM {hosting_dns_slaves} WHERE master_vid = %d', $this->server->vid);
    while ($slave_server = db_fetch_object($result)) {
      $slave_servers[] = $slave_server->slave_nid;
    }

    $this->setValues(array('slave_servers' => drupal_map_assoc($slave_servers)));
  }

  function insert() {
    parent::insert();
    if (is_array($this->slave_servers)) {
      foreach (array_filter($this->slave_servers) as $slave_server) {
        $record = array(
          'master_nid' => $this->server->nid,
          'master_vid' => $this->server->vid,
          'slave_nid' => $slave_server,
        );
        drupal_write_record('hosting_dns_slaves', $record);
      }
    }
  }

  function delete() {
    parent::delete();
    db_query('DELETE FROM {hosting_dns_slaves} WHERE master_nid = %d', $this->server->nid);
  }

  function delete_revision() {
    parent::delete_revision();
    db_query('DELETE FROM {hosting_dns_slaves} WHERE master_vid = %d', $this->server->vid);
  }

  function context_options($task_type, $ref_type, &$task) {
    parent::context_options($task_type, $ref_type, $task);

    if (is_array($this->slave_servers) && sizeof($this->slave_servers)) {
      $task->context_options['slave_servers'] = implode(',', array_values(array_map('hosting_context_name', array_intersect_key($this->slave_servers, hosting_get_servers('dns')))));
    }
  }

}


/**
 * An implementation of the dns service type, registered with hook_hosting_service.
 */
class hostingService_dns_dnsmasq extends hostingService_dns_master {
  public $type = 'dnsmasq';

  public $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    return "sudo /etc/init.d/dnsmasq restart";
  }
}

/**
 * An implementation of the dns service type, registered with hook_hosting_service.
 */
class hostingService_dns_bind extends hostingService_dns_master {
  public $type = 'bind';

  public $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    return "sudo /etc/init.d/bind restart";
  }
}

/**
 * Slave-specific servers
 */
class hostingService_dns_bind_slave extends hostingService_dns {
  public $type = 'bind_slave';

  public $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    return "sudo /etc/init.d/bind restart";
  }
}
