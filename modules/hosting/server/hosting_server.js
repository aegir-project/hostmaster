(function () {
  /**
   * Update visible settings to correspond to enabled service.
   */
  function hostingServerUpdateService() {
    var $this = $(this),
        name = $this.attr('name').replace(/services\[([a-z]*)\]\[type\]/, '$1'),
        type = $this.attr('value');
    $('.provision-service-settings-' + name).hide();
    $('#provision-service-settings-' + name + '-' + type).show();
  }

  Drupal.behaviors.hostingServer = function () {
    $('input.form-radio[name^=services]')
      .change(hostingServerUpdateService) // When a service is selected
      .filter(':checked').each(hostingServerUpdateService); // On page load
  };
}());
