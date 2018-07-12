api = 2
core = 7.x

; MAKE file for Drupal core.  Used by the Drupal.org packager.

; Pin a core version, as long as we have a core patch below.
projects[drupal][type] = core
projects[drupal][version] = 7.59

; Function each() is deprecated since PHP 7.2; https://www.drupal.org/project/drupal/issues/2925449
projects[drupal][patch][2925449] = "https://www.drupal.org/files/issues/2018-04-08/deprecated_each2925449-106.patch"

; [PHP 7.2] Avoid count() calls on uncountable variables; https://www.drupal.org/project/drupal/issues/2885610
projects[drupal][patch][2885610] = "https://www.drupal.org/files/issues/2018-04-21/drupal-7-count-function-deprecation-fixes-2885610-19.patch"
