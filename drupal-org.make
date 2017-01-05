core = 7.x
api = 2

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"



; Aegir core

projects[eldir][type] = theme
projects[eldir][subdir] = aegir
projects[eldir][version] = 3.9

projects[hosting][subdir] = aegir
projects[hosting][version] = 3.9



; Modules - Aegir "golden"

projects[hosting_git][subdir] = aegir
projects[hosting_git][version] = 3.9

projects[hosting_remote_import][subdir] = aegir
projects[hosting_remote_import][version] = 3.9

projects[hosting_site_backup_manager][subdir] = aegir
projects[hosting_site_backup_manager][version] = 3.9

projects[hosting_tasks_extra][subdir] = aegir
projects[hosting_tasks_extra][version] = 3.9

projects[hosting_civicrm][subdir] = aegir
projects[hosting_civicrm][version] = 3.9

projects[hosting_dns][subdir] = aegir
projects[hosting_dns][version] = 3.x

; Modules - contrib

projects[admin_menu][version] = 3.0-rc5
projects[betterlogin][version] = 1.4
projects[ctools][version] = 1.12
projects[entity][version] = 1.8
projects[openidadmin][version] = 1.0
projects[overlay_paths][version] = 1.3
projects[r4032login][version] = 1.8
projects[views][version] = 3.14
projects[views_bulk_operations][version] = 3.3

; Two factor authentication
projects[libraries][version] = 2.3
projects[tfa][version] = 2.0
projects[tfa_basic][version] = 1.0
projects[tfa_basic][patch][] = "https://www.drupal.org/files/issues/use_libraries_module-2807953-8.patch"

libraries[qrcodejs][download][type] = git
libraries[qrcodejs][download][url] = https://github.com/davidshimjs/qrcodejs.git
libraries[qrcodejs][download][revision] = 04f46c6a0708418cb7b96fc563eacae0fbf77674
