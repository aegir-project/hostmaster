<?php
/**
 * DNS service base class.
 */
class hostingService_dns extends hostingService {
  public $service = 'dns';
}


/**
 * An implementation of the dns service type, registered with hook_hosting_service.
 */
class hostingService_dns_dnsmasq extends hostingService_dns {
  public $type = 'dnsmasq';

  public $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    return "sudo /etc/init.d/dnsmasq restart";
  }
}

/**
 * An implementation of the dns service type, registered with hook_hosting_service.
 */
class hostingService_dns_bind extends hostingService_dns {
  public $type = 'bind';

  public $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    return "sudo /etc/init.d/bind restart";
  }
}
