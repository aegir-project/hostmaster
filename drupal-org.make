core = 7.x
api = 2



; Aegir core

; Do *NOT* pin version number for Aegir core modules to make sure
; we fetch the latest release for those.

projects[eldir][type] = theme
projects[eldir][subdir] = contrib
projects[eldir][version] = 3.x

projects[hosting][type] = module
projects[hosting][subdir] = contrib
projects[hosting][version] = 3.x



; Modules - Aegir "golden"

;projects[aegir_tour][type] = module
;projects[aegir_tour][subdir] = contrib
;projects[aegir_tour][version] = 3.x

;projects[hosting_advanced_cron][type] = module
;projects[hosting_advanced_cron][subdir] = contrib
;projects[hosting_advanced_cron][version] = 3.x

projects[hosting_git][type] = module
projects[hosting_git][subdir] = contrib
projects[hosting_git][version] = 3.x

projects[hosting_platform_pathauto][type] = module
projects[hosting_platform_pathauto][subdir] = contrib
projects[hosting_platform_pathauto][version] = 2.x

;projects[hosting_remote_import][type] = module
;projects[hosting_remote_import][subdir] = contrib
;projects[hosting_remote_import][version] = 3.x

;projects[hosting_site_backup_manager][type] = module
;projects[hosting_site_backup_manager][subdir] = contrib
;projects[hosting_site_backup_manager][version] = 3.x

;projects[hosting_tasks_extra][type] = module
;projects[hosting_tasks_extra][subdir] = contrib
;projects[hosting_tasks_extra][version] = 3.x



; Modules - contrib

projects[admin_menu][type] = module
projects[admin_menu][subdir] = contrib
projects[admin_menu][version] = 3.0-rc4

projects[betterlogin][type] = module
projects[betterlogin][subdir] = contrib
projects[betterlogin][version] = 1.2

projects[bootstrap_tour][type] = module
projects[bootstrap_tour][subdir] = contrib
projects[bootstrap_tour][version] = 1.0-beta8

projects[ctools][type] = module
projects[ctools][subdir] = contrib
projects[ctools][version] = 1.4

projects[entity][type] = module
projects[entity][subdir] = contrib
projects[entity][version] = 1.5

projects[openidadmin][type] = module
projects[openidadmin][subdir] = contrib
projects[openidadmin][version] = 1.0

projects[overlay_paths][type] = module
projects[overlay_paths][subdir] = contrib
projects[overlay_paths][version] = 1.3

projects[views][type] = module
projects[views][subdir] = contrib
projects[views][version] = 3.8

projects[views_bulk_operations][type] = module
projects[views_bulk_operations][subdir] = contrib
projects[views_bulk_operations][version] = 3.2
