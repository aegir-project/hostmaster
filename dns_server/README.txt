Install instructions
--------------------

 0. Install bind9 through usual mechanisms for your operating system
 1. Deploy the code and activate the feature in the wizard.
   * this currently implies manually activating the provision_dns modules (see
     bug #366327)
 2. create a DNS server node, and setup appropriate defaults for it
 3. edit your platform node, to set the DNS server as your default for the
    platform 
 4. add a line to your sudoers file for the rndc utility which the aegir user
    will use to restart BIND

    hostmaster ALL=(ALL) NOPASSWD: /usr/sbin/rndc

 5. add a line to your named.conf to include aegir-generated configurations:

    include "/var/hostmaster/conf/named.conf";

Note that slave DNS configuration is not yet automated, so if you want more
than one DNS server for your zones (which is obviously strongly encouraged,
generally), you will need to configure those zones manually.
