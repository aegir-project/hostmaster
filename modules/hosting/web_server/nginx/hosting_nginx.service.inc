<?php

module_load_include('service.inc', 'hosting_web_server');

class hostingService_http_nginx extends hostingService_http_public {
  public $service = 'http';
  public $type = 'nginx';

  function default_restart_cmd() {
    return 'sudo /etc/init.d/nginx restart';
  }
}
