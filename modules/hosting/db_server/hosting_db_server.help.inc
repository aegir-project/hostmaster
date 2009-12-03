<?php
/**
 * Implementation of hook_help().
 */
function hosting_db_server_help($section) {
  switch ($section) {
    case 'admin/help/provision#requirements':
      $output .= _hosting_requirements('mysql_user');
      return $output;
      break;
  }
}

/**
 * Display helpful information regarding the creation
 * of a MySQL superuser
 */
function _hosting_mysql_user_requirements() {
   $username = HOSTING_DEFAULT_SCRIPT_USER;
   $command = <<<EOF
     mysql -uroot -pXXXXXXXXX mysql

     mysql> GRANT ALL PRIVILEGES ON *.* TO 'aegir_root'@'localhost' \
            IDENTIFIED BY 'mypassword' WITH GRANT OPTION;

     Note: If your database server is not running on localhost alongside
     the webserver but on a remote host, you may wish to change the query
     to allow these privileges for the user from the specific IP address,
     or more generally, grant them access from the wildcard '%' per the
     following query:

     mysql> GRANT ALL PRIVILEGES ON *.* TO 'aegir_root'@'%' IDENTIFIED \
            BY 'mypassword' WITH GRANT OPTION;
EOF;
   $help['title'] = t('Mysql user account capable of creating new databases');
   $help['summary'] = t('To be able to create new sites, the provisioning framework needs a database
     superuser to create new databases and users. It is not recommended to use the mysql root password 
     for this, but any account with the correct permissions will do. As a standard, it is recommended
     to name this superuser "aegir_root".');
   $help['configuration'] = t('Log in to your mysql server as root, and type in the following command:
     <pre>@command_text</pre>', array('@command_text' => $command));     

  return $help;
}
