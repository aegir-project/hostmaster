(function($) {
  Drupal.behaviors.aegir_make_working_copy = function (context) {
    $('.hosting-platform-working-copy-source:not(.hosting-platform-working-copy-processed)', context)
      .addClass('hosting-platform-working-copy-processed')
      .bind('change', function() {
        Drupal.hosting_platform_working_copy.update_visibility($(this));
      })
      .bind('keyup', function() {
        Drupal.hosting_platform_working_copy.update_visibility($(this));
      })
      .each(function() {
        Drupal.hosting_platform_working_copy.update_visibility($(this));
      });
  }
  
  Drupal.hosting_platform_working_copy = Drupal.hosting_platform_working_copy || {};
  
  Drupal.hosting_platform_working_copy.update_visibility = function($elem) {
    if ($elem.val()) {
      $('.hosting-platform-working-copy-target').parents('.form-item')
        .show();
    }
    else {
      $('.hosting-platform-working-copy-target').parents('.form-item')
        .hide();
    }
  }
  
})(jQuery);
