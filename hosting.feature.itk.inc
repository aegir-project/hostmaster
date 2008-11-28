<?php

/**
 * Implementation of hook_hosting_feature()
 */
function hosting_itk_hosting_feature() {
  return array('itk' => array(
    'title' => t('ITK site security'),
    'description' => t('Provides security between sites using <a href="http://mpm-itk.sesse.net/">Apache-ITK</a>. This will create files with gids matching the client id, which means that sites from a client can read database settings and private files from the sites owned by that client, but not other clients.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_itk',
    'group' => 'experimental',
    ));
}

