Install instructions
--------------------

 1. Deploy the code and activate the feature in the wizard.
   * this currently implies manually activating the provision_dns modules (see bug #366327)
 2. create a DNS server node, and setup appropriate defaults for it
 3. edit your platform node, to set the DNS server as your default for the platform 
 4. add a line to your sudoers file similar to the apache2ctl one, except for the rndc utility which the aegir user will use to restart BIND
