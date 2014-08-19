core = 7.x
api = 2

; this makefile will make sure we get the development code from the
; aegir modules instead of the tagged releases
includes[hostmaster] = "drupal-org.make"

projects[eldir][type] = "theme"
projects[eldir][download][type] = "git"
projects[eldir][download][url] = "http://git.drupal.org/project/eldir.git"
projects[eldir][download][branch] = "7.x-3.x"
projects[hosting][download][type] = 'git'
projects[hosting][download][url] = 'http://git.drupal.org/project/hosting.git'
projects[hosting][download][branch] = '7.x-3.x'

;projects[aegir_tour][download][type] = 'git'
;projects[aegir_tour][download][url] = 'http://git.drupal.org/project/aegir_tour.git'
;projects[aegir_tour][download][branch] = '7.x-3.x'

projects[hosting_platform_pathauto][download][type] = 'git'
projects[hosting_platform_pathauto][download][url] = 'http://git.drupal.org/project/hosting_platform_pathauto.git'
projects[hosting_platform_pathauto][download][branch] = '7.x-2.x'


; Add devel while we're in the 7.x-3.x dev cycle;
; enabled in hostmaster.info.
projects[] = devel
