core = 6.x

projects[hosting][type] = "module"
projects[hosting][download][type] = "cvs"
; projects[hosting][download][revision] = "DRUPAL-6--0-4-ALPHA2"
projects[hosting][download][revision] = "HEAD"
projects[hosting][download][root] = "pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal-contrib"
projects[hosting][download][module] = "contributions/modules/hosting"

projects[eldir][type] = "theme"
projects[eldir][download][type] = "cvs"
; projects[eldir][download][revision] = "DRUPAL-6--0-4-ALPHA2"
projects[eldir][download][revision] = "HEAD"
projects[eldir][download][root] = "pserver:anonymous:anonymous@cvs.drupal.org:/cvs/drupal-contrib"
projects[eldir][download][module] = "contributions/themes/eldir"


; Contrib modules
projects[admin_menu][version] = "1.5"
projects[install_profile_api][version] = "2.1"
projects[jquery_ui][version] = "1.3"
projects[modalframe][version] = "1.4"

; Libraries
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery.ui-1.6.zip"
libraries[jquery_ui][destination] = "modules/jquery_ui"
libraries[jquery_ui][directory_name] = "jquery.ui"

