<?php

module_load_include('service.inc', 'hosting_web_server');

class hostingService_http_nginx extends hostingService_http_public {
  public $type = 'nginx';
  public $has_restart_cmd = TRUE;

  function default_restart_cmd() {
    return 'sudo /etc/init.d/nginx reload';
  }
}
