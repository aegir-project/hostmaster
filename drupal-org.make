core = 7.x
api = 2

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"



; Aegir core

; Do *NOT* pin version number for Aegir core modules to make sure
; we fetch the latest release for those.

projects[eldir][type] = theme
projects[eldir][subdir] = aegir

projects[hosting][subdir] = aegir



; Modules - Aegir "golden"

projects[hosting_git][subdir] = aegir

projects[hosting_remote_import][subdir] = aegir

projects[hosting_site_backup_manager][subdir] = aegir

projects[hosting_tasks_extra][subdir] = aegir

projects[hosting_civicrm][subdir] = aegir


; Modules - contrib

projects[admin_menu][version] = 3.0-rc5
projects[betterlogin][version] = 1.4
projects[ctools][version] = 1.9
projects[entity][version] = 1.7
projects[openidadmin][version] = 1.0
projects[overlay_paths][version] = 1.3
projects[r4032login][version] = 1.8
projects[views][version] = 3.13
projects[views_bulk_operations][version] = 3.3
