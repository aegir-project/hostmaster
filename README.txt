=== The hostmaster system ===
This install profile is part of the front end of a system that also consists of the hosting framework (front end) and the provisioning framework (backend).

The front end and back end are designed to be run separately, and each front end will also be able to drive multiple back ends (in the simplest case, each drupal release will be a back end implementation of it's own)
=== Hostmaster Layout ===
The installation profile should reside under the profiles directory and named hostmaster.

Modules Location:
Hostmaster requires modules to be installed in -
  profiles/hostmaster/modules to avoid conflicts with different sites versions.

When installed your filesystem should look like this..

  profiles/hostmaster/hostmaster.profile
                     /modules/hosting
                     /modules/provision
                     /modules/views
                     /modules/drush

=== Requirements ===
Required Modules:
  * hosting
  * provision
  * views
  * drush



 - 
