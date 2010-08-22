<?php

module_load_include('service.inc', 'hosting_web_server');
module_load_include('service.inc', 'hosting_ssl');


class hostingService_http_nginx_ssl extends hostingService_http_ssl {
  
  public $type = 'nginx_ssl';
  protected $has_restart_cmd = TRUE;
  
  function default_restart_cmd() {
    return "sudo /etc/init.d/nginx reload";
  }

}
