<?php

/**
 * @file
 *   Drush include for the hostmaster resume command.
 */

include_once('resume.hosting.inc');

/**
 * backward compatibility for pre 1.0
 *
 * @deprecated use hosting-resume now, will be removed after 1.0
 */
function drush_hosting_hostmaster_resume() {
  return drush_hosting_resume();
}
