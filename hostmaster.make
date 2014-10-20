core = 7.x
api = 2



; Includes

; This makefile will make sure we get the development code from the Aegir
; modules instead of the tagged releases.
includes[hostmaster] = drupal-org.make



; Aegir core

projects[eldir][type] = theme
projects[eldir][subdir] = contrib
projects[eldir][download][type] = git
projects[eldir][download][branch] = 7.x-3.x

projects[hosting][type] = module
projects[hosting][subdir] = contrib
projects[hosting][download][type] = git
projects[hosting][download][branch] = 7.x-3.x



; Modules - Aegir "golden"

;projects[aegir_tour][type] = module
;projects[aegir_tour][subdir] = contrib
;projects[aegir_tour][download][type] = git
;projects[aegir_tour][download][branch] = 7.x-3.x

projects[hosting_git][type] = module
projects[hosting_git][subdir] = contrib
projects[hosting_git][download][type] = git
projects[hosting_git][download][branch] = 7.x-3.x

projects[hosting_platform_pathauto][type] = module
projects[hosting_platform_pathauto][subdir] = contrib
projects[hosting_platform_pathauto][download][type] = git
projects[hosting_platform_pathauto][download][branch] = 7.x-2.x



; Modules - Temporary

; Add devel while we're in the 7.x-3.x dev cycle;
; enabled in hostmaster.info.
projects[] = devel
projects[] = devel_debug_log
