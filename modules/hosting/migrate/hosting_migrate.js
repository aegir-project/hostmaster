$(document).ready( function() {
  $('#hosting-task-confirm-form, #hosting-migrate-platform').before($("<div id='hosting-migrate-comparison-inline'></div>").hide());
 // $("#hosting-migrate-comparison-inline")
  $('a.hosting-package-comparison-link').click( function() {
    hostingMigrateComparisonInline($(this));
    return false;
  });
});

function hostingMigrateComparisonInline(elem) {
  var hostingMigrateCallback = function(data, responseText) {
    $("#hosting-migrate-comparison-inline").html(data).show();
    $('#hosting-task-confirm-form, #hosting-migrate-platform').hide();
    hostingMigrateToggleSize();
    $('.hosting-migrate-comparison-return').click( function() {
      hostingMigrateComparisonClose();
      return false;
    }
    );
  }
 
  $.get(Drupal.settings.basePath + 'hosting/js' + $(elem).attr('href'), null, hostingMigrateCallback );
}

function hostingMigrateToggleSize() {
  if (parent.Drupal.modalFrame.isOpen) {

    var self = Drupal.modalFrameChild;
    // Tell the parent window to resize the Modal Frame if the child window
    // size has changed more than a few pixels tall or wide.
    var newWindowSize = {width: $(window).width(), height: $('body').height() + 25};
    if (Math.abs(self.currentWindowSize.width - newWindowSize.width) > 5 || Math.abs(self.currentWindowSize.height - newWindowSize.height) > 5) {
      self.currentWindowSize = newWindowSize;
      self.triggerParentEvent('childResize');
    }

  }
}

function hostingMigrateComparisonClose() {
  $("#hosting-migrate-comparison-inline").hide();
  $('#hosting-task-confirm-form, #hosting-migrate-platform').show();
  hostingMigrateToggleSize();
}
