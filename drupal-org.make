core = 7.x
api = 2

; this makefile will fetch the Aegir distribution from drupal.org
;
; Aegir core modules
; Do *NOT* pin version number for Aegir core modules to make sure
; we fetch the latest release for those
projects[hosting][type] = "module"
projects[hosting][version] = "3.x"
projects[eldir][type] = "theme"
projects[eldir][version] = "3.x"

; Aegir "golden" contrib modules
projects[hosting_platform_pathauto][version] = "2.x"
projects[hosting_git][version] = "3.x"
;projects[aegir_tour][version] = "3.x"
;projects[hosting_site_backup_manager][version] = "3.x"
;projects[hosting_remote_import][version] = "3.x"
;projects[hosting_tasks_extra][version] = "3.x"
;projects[hosting_advanced_cron][version] = "3.x"


; Contrib modules
projects[admin_menu][version] = "3.0-rc4"
projects[betterlogin][version] = "1.2"
projects[openidadmin][version] = "1.0"
projects[overlay_paths][version] = "1.3"
projects[views][version] = "3.8"
projects[ctools][version] = "1.4"
projects[views_bulk_operations][version] = "3.2"
projects[entity][version] = "1.5"
projects[bootstrap_tour][version] = "1.0-beta8"
