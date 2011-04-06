<?php

include_once('resume.hostmaster.inc');

/**
 * backward compatibility for pre 1.0
 *
 * @deprecated use hosting-resume now, will be removed after 1.0
 */
function drush_hostmaster_resume() {
  return drush_hosting_resume();
}
