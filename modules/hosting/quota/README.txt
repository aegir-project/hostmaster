API Documentation
~~~~~~~~~~~~~~~~~

Functions
---------

These are all defined in hosting_quota.module

hosting_quota_get($resource)
    Get info about a particular resource.
hosting_quota_get_usage($client, $resource, $start = NULL, $end = NULL)
    Get usage info about a resource for a given client, by calling that resources usage hook.
hosting_quota_resource_render($resource, $usage)
    Render a given resource, by calling its rendering hook.
hosting_quota_set_limit($client, $resource, $value)
    Sets the limit for a given clients quota.
hosting_quota_check($client, $resource, $start = NULL, $end = NULL)
    Convenient function to return a boolean, true if the client has not reached their limit, false if they have.
hosting_quota_get_all_info($client, $start = NULL, $end = NULL)
    Retrieve all of the info for a given clients quotas, within a declared period (defaults to the last month).

Hooks
-----

Hook definitions are documented in hosting_quota.hooks.php. A partial usage example was created in hosting_site.quota.inc

hook_hosting_quota_resource()
    Returns an array containing info about each resource provided by a module.
hook_hosting_quota_get_usage($client, $resource, $start, $end)
    This hook should return an integer that can be compared to the set limit.
hook_hosting_quota_resource_render($resource, $usage)
    This hook should return a human readable version of the usage for this resource.
