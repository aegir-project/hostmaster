core = 6.x
api = 2

; Contrib modules
projects[admin_menu][version] = "1.8"
projects[openidadmin][version] = "1.2"
projects[install_profile_api][version] = "2.2"
projects[jquery_ui][version] = "1.5"
projects[jquery_update][version] = "2.0-alpha1"
projects[modalframe][version] = "1.7"
projects[views][version] = "3.0"
projects[views_bulk_operations][version] = "1.15"
; See: https://drupal.org/node/2025215
projects[views_bulk_operations][patch][] = "http://drupal.org/files/view_bulk_operation-add_missing_argument-2025215-1.patch"

; These are contrib modules, but come under the Aegir 'umbrella' of control.
projects[hosting_platform_pathauto][version] = "2.0-beta2"
projects[eldir][version] = "2.0-beta1"
projects[hosting][version] = "2.0-beta2"

; Libraries
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][destination] = "modules/jquery_ui"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery-ui-1.7.3.zip"
libraries[jquery_ui][directory_name] = "jquery.ui"
