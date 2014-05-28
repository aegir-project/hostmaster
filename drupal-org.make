core = 7.x
api = 2

; this makefile will fetch the Aegir distribution from drupal.org
;
; core aegir modules
; do *not* pin version number for Aegir core modules to make sure
; we fetch the latest release for those
projects[hosting][type] = "module"
projects[eldir][type] = "theme"

; contrib aegir modules
;projects[hosting_platform_pathauto][version] = "2.1"

; Contrib modules
projects[admin_menu][version] = "3.0-rc4"
projects[openidadmin][version] = "1.0"
projects[overlay_paths][version] = "1.3"
projects[views][version] = "3.8"
projects[ctools][version] = "1.4"
projects[views_bulk_operations][version] = "3.2"
projects[entity][version] = "1.5"

; These are contrib modules, but come under the Aegir 'umbrella' of control.
projects[hosting_platform_pathauto][version] = "2.1"
projects[eldir][version] = "3.x"
projects[hosting][version] = "3.x"
