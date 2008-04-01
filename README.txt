=== The hostmaster system ===
This install profile is part of the front end of a system that also consists of the hosting framework (front end) and the provisioning framework (backend).

The front end and back end are designed to be run separately, and each front end will also be able to drive multiple back ends (in the simplest case, each drupal release will be a back end implementation of it's own)

Once installed go to admin -> help -> hosting for more elaborate and translatable help.

The most up to date information regarding the project and it's goal can be found in the hostmaster wiki page - http://groups.drupal.org/hm2/overview .

=== Hostmaster Layout ===
The installation profile should reside under the profiles directory under the drupal root and is named hostmaster.

Modules Location:
Hostmaster requires modules to be installed in -
  profiles/hostmaster/modules to avoid conflicts with different sites versions.

When installed your filesystem should look like this..

  profiles/hostmaster/hostmaster.profile
                     /modules/hosting
                     /modules/provision
                     /modules/views
                     /modules/drush

Note: Don't be alarmed by the existence of views both in your profiles and sites/all directory. Hostmaster uses it's own views so it does not rely on a specific site to work.

=== Pre Requirements ===
Required Modules:
  * hosting
  * provision
  * views
  * drush
