core = 7.x
api = 2

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"



; Aegir core

projects[eldir][type] = theme
projects[eldir][download][type] = git
projects[eldir][subdir] = aegir
projects[eldir][download][tag] = 7.x-3.140

projects[hosting][download][type] = git
projects[hosting][subdir] = aegir
projects[hosting][download][tag] = 7.x-3.142



; Modules - Aegir "golden"

projects[hosting_git][download][type] = git
projects[hosting_git][subdir] = aegir
projects[hosting_git][download][tag] = 7.x-3.140

projects[hosting_remote_import][download][type] = git
projects[hosting_remote_import][subdir] = aegir
projects[hosting_remote_import][download][tag] = 7.x-3.140

projects[hosting_site_backup_manager][download][type] = git
projects[hosting_site_backup_manager][subdir] = aegir
projects[hosting_site_backup_manager][download][tag] = 7.x-3.140

projects[hosting_tasks_extra][download][type] = git
projects[hosting_tasks_extra][subdir] = aegir
projects[hosting_tasks_extra][download][tag] = 7.x-3.140

projects[hosting_civicrm][download][type] = git
projects[hosting_civicrm][subdir] = aegir
projects[hosting_civicrm][download][tag] = 7.x-3.140

projects[hosting_logs][download][type] = git
projects[hosting_logs][subdir] = aegir
projects[hosting_logs][download][tag] = 7.x-3.140

projects[hosting_dns][download][type] = git
projects[hosting_dns][subdir] = aegir
projects[hosting_dns][download][branch] = 7.x-3.x

projects[hosting_https][download][type] = git
projects[hosting_https][subdir] = aegir
projects[hosting_https][download][tag] = 7.x-3.140

; Modules - contrib

projects[admin_menu][version] = 3.0-rc5
projects[betterlogin][version] = 1.4
projects[ctools][version] = 1.14
projects[entity][version] = 1.9
projects[module_filter][version] = 2.1
projects[openidadmin][version] = 1.0
projects[overlay_paths][version] = 1.3
projects[r4032login][version] = 1.8
projects[views][version] = 3.20
projects[views_bulk_operations][version] = 3.4

; Two factor authentication
projects[libraries][version] = 2.3
projects[tfa][version] = 2.0
projects[tfa_basic][version] = 1.0
projects[tfa_basic][patch][] = "https://www.drupal.org/files/issues/use_libraries_module-2807953-8.patch"

libraries[qrcodejs][download][type] = git
libraries[qrcodejs][download][url] = https://github.com/davidshimjs/qrcodejs.git
libraries[qrcodejs][download][revision] = 04f46c6a0708418cb7b96fc563eacae0fbf77674

; JQuery TimeAgo plugin
projects[timeago][version] = 2.3
libraries[timeago][download][type] = get
libraries[timeago][download][url] = https://raw.githubusercontent.com/rmm5t/jquery-timeago/v1.6.1/jquery.timeago.js
libraries[timeago][destination] = libraries

; Vue.js
libraries[vuejs][download][type] = get
libraries[vuejs][download][url] = https://github.com/vuejs/vue/raw/v2.4.4/dist/vue.min.js
libraries[vuejs][destination] = libraries
