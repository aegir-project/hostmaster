core = 7.x
api = 2

; this makefile is designed to be able to bootstrap hostmaster without
; having to use provosion's makefile
projects[drupal][type] = "core"
includes[hostmaster] = "drupal-org.make"
