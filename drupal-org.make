core = 6.x
api = 2

; Contrib modules
projects[admin_menu][version] = "3.0-rc4"
projects[openidadmin][version] = "1.0"
projects[overlay_paths][version] = "1.2"

; These are contrib modules, but come under the Aegir 'umbrella' of control.
; projects[hosting_platform_pathauto][version] = "2.0-beta1"
projects[eldir][type] = "theme"
projects[eldir][download][type] = "git"
projects[eldir][download][url] = "http://git.drupal.org/project/eldir.git"
projects[eldir][download][branch] = "7.x-3.x"
projects[hosting][type] = "theme"
projects[hosting][download][type] = "git"
projects[hosting][download][url] = "http://git.drupal.org/project/hosting.git"
projects[hosting][download][branch] = "7.x-3.x"

; Libraries
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][destination] = "modules/jquery_ui"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery.ui-1.6.zip"
libraries[jquery_ui][directory_name] = "jquery.ui"
