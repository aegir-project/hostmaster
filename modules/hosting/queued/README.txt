Hosting queue daemon
====================

Simple Drupal module intended to make it easy to run the Aegir tasks
queue with near-instant execution times. The daemon is designed to run
standalone, and started through regular services: there's an init.d
script available, which is installed with the Debian package, but that
you will need to manually install in other platforms.

Note that before the service is setup and the daemon can be started,
it needs to be enabled as a module in the frontend.

Install this module in your main hostmaster (Aegir) site. You can
enable the feature from: `admin/hosting`. This will disable your
hosting tasks queue for you, ready for you to enable the daemon.

The daemon logs some of its activities to the Drupal watchdog.

Installing as a service
-----------------------

Those instructions will setup the daemon to run as a regular service
in /etc/init.d/ - instructions will vary according to platforms, the
following should work in Debian, running as root.

1. Install the init script in place

        cp init.d.example /etc/init.d/hosting-queued

2. Setup symlinks and runlevels

        update-rc.d hosting-queued defaults

3. Start the daemon

        /etc/init.d/hosting-queued

Supervisord configuration instructions
--------------------------------------

You can also use a daemon like supervisor to make sure the daemon is
restarted if it crashes, but this is optional. An example
configuration file (hosting_queued.conf) is included.

These instructions are for Debian based linux distributions, you may need to
adjust settings for other distributions.

1. Install supervisor

        sudo apt-get install supervisor

2. Copy the `hosting_queued.sh` script from the module directory to the
   root of the Aegir home directory (usually `/var/aegir`). You will want to
   ensure that the script is executable and owned by the Aegir user:

        chown aegir:aegir hosting_queued.sh
        chmod 700 hosting_queued.sh

3. Copy the supervisor example configuration file from the module directory
   to the conf.d directory of supervisor.

        cp hosting_queued.conf /etc/supervisor/conf.d/

    Adjust the settings in that file to match your environment. If you have a
    standard Aegir setup, and have followed the README so far, then you
    shouldn't need to change anything.

4. Restart supervisor and add a task to your hosting tasks queue in Aegir,
   re-verify a site and see if it executes then you're all set up!
   Supervisor keeps a log about the execution of the queue daemon that may be
   useful if you are trying to resolve an issue where your tasks are not being
   executed. The output from the queue daemon is also logged by supervisor to
   (by default) `/var/log/hosting_queued` it may be useful to view this
   log occasionally to ensure that there are no errors being logged. 'Duplicate
   task' errors, if you get them are nothing to worry about however.

Troubleshooting
---------------

Try to run the bash script from the command line as the Aegir user yourself, if
you can do this, then the issue is with Supervisor, otherwise it might be an
issue with this module/script.

Look into the Drupal watchdog to see when the daemon has been started
or was restarted. The settings page should also tell you the last time
the daemon was started.
