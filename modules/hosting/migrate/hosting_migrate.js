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
 
  $.get('/hosting/js' + $(elem).attr('href'), null, hostingMigrateCallback );
}

function hostingMigrateToggleSize() {
  if (parent.Drupal.modalFrame.isOpen) {
    parent.Drupal.modalFrame.resize( {
        width: $(document).width() ,
        height: $("body").height() + 25 }
     );
  }
}

function hostingMigrateComparisonClose() {
  $("#hosting-migrate-comparison-inline").hide();
  $('#hosting-task-confirm-form, #hosting-migrate-platform').show();
  hostingMigrateToggleSize();
}
