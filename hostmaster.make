core = 6.x
api = 2

; this makefile will make sure we get the development code from the
; aegir modules instead of the tagged releases
includes[hostmaster] = "drupal-org.make"
projects[hosting][download][type] = 'git'
projects[hosting][download][url] = 'http://git.drupal.org/project/hosting.git'
projects[hosting][download][branch] = '6.x-2.x'
projects[eldir][download][type] = 'git'
projects[eldir][download][url] = 'http://git.drupal.org/project/eldir.git'
projects[eldir][download][branch] = '6.x-2.x'
