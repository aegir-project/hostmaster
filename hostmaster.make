core = 7.x
api = 2



; Includes

; This makefile will make sure we get the development code from the Aegir
; modules instead of the tagged releases.
includes[hostmaster] = drupal-org.make



; Aegir core

projects[eldir][download][type] = git
projects[eldir][download][branch] = 7.x-3.x

projects[hosting][download][type] = git
projects[hosting][download][branch] = 7.x-3.x



; Modules - Aegir "golden"

;projects[aegir_tour][download][type] = git
;projects[aegir_tour][download][branch] = 7.x-3.x

projects[hosting_git][download][type] = git
projects[hosting_git][download][branch] = 7.x-3.x

projects[hosting_remote_import][download][type] = git
projects[hosting_remote_import][download][branch] = 7.x-3.x

; Modules - Temporary

; Add devel while we're in the 7.x-3.x dev cycle;
; enabled in hostmaster.info.
projects[devel][subdir] = developer
projects[devel_debug_log][subdir] = developer
