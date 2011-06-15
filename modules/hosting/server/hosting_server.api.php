<?php

/**
 * @file
 * Hooks provided by the hosting server module.
 */

/**
 * @addtogroup hostinghooks
 * @{
 */

/**
 * Alter the servers available for selection in the frontend.
 *
 * You can use this hook to modify the servers a user can choose from in the
 * frontend when selecting things like which server to deploy a platform to, or
 * which server to use for a site's database.
 *
 * In most cases all you'll want to do is either modify the title of the server,
 * which is for the user's informational purposes only, or remove servers so
 * they can't choose it. Removing servers from this list may not be supported by
 * the code getting the list of servers, so be very careful if you do this.
 *
 * @param $servers
 *   An array of enabled servers, keys are the nid's of the nodes representing
 *   them, values are the titles of the servers.
 * @param $service
 *   Service type string, like 'http' or 'db'.
 *
 * @see hosting_get_servers()
 */
function hook_hosting_servers_titles_alter(&$servers, $service) {
  // Append the string 'SERVER' to all server titles.
  foreach ($servers as $nid => $title) {
    $servers[$nid] .= 'SERVER';
  }
  
  // Don't allow the user to use the server with $nid == 123, for the 'db' service
  if ($service == 'db') {
    unset($servers[123]);
  }
}

/**
 * @} End of "addtogroup hooks".
 */
