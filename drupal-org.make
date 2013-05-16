core = 6.x
api = 2

; Contrib modules
projects[admin_menu][version] = "1.8"
projects[openidadmin][version] = "1.2"
projects[install_profile_api][version] = "2.1"
projects[jquery_ui][version] = "1.4"
projects[modalframe][version] = "1.6"
projects[views][version] = "3.0"
projects[views_bulk_operations][version] = "1.13"
; Add support for row classes - http://drupal.org/node/1843166#comment-6743346
projects[views_bulk_operations][patch][] = "http://drupal.org/files/views_row_classes-6.x-1.13-1843166-2.patch"

; These are contrib modules, but come under the Aegir 'umbrella' of control.
projects[hosting_platform_pathauto][version] = "2.0-beta2"
projects[eldir][version] = "2.0-alpha1"
projects[hosting][version] = "2.0-alpha2"

; Libraries
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][destination] = "modules/jquery_ui"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery.ui-1.6.zip"
libraries[jquery_ui][directory_name] = "jquery.ui"
