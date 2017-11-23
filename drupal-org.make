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
projects[ctools][version] = 1.11
projects[entity][version] = 1.8
projects[openidadmin][version] = 1.0
projects[overlay_paths][version] = 1.3
projects[r4032login][version] = 1.8
projects[views][version] = 3.14
projects[views_bulk_operations][version] = 3.3

; Two factor authentication
projects[libraries][version] = 2.3
projects[tfa][version] = 2.0
projects[tfa_basic][version] = 1.0-beta3
projects[tfa_basic][patch][] = "https://www.drupal.org/files/issues/use_libraries_module-2807953-8.patch"

libraries[qrcodejs][download][type] = git
libraries[qrcodejs][download][url] = https://github.com/davidshimjs/qrcodejs.git
libraries[qrcodejs][download][revision] = 04f46c6a0708418cb7b96fc563eacae0fbf77674

; JQuery TimeAgo plugin
projects[timeago][version] = 2.3
libraries[timeago][download][type] = get
libraries[timeago][download][url] = https://raw.githubusercontent.com/rmm5t/jquery-timeago/v1.5.3/jquery.timeago.js
libraries[timeago][destination] = libraries

; Vue.js
libraries[vuejs][download][type] = get
libraries[vuejs][download][url] = https://github.com/vuejs/vue/raw/v2.4.4/dist/vue.min.js
libraries[vuejs][destination] = libraries