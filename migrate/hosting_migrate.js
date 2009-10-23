$(document).ready( function() {
  $('#hosting-task-confirm-form').before($("<div id='hosting-migrate-comparison-inline'></div>").hide());
 // $("#hosting-migrate-comparison-inline")
  $('a.hosting-package-comparison-link').click( function() {
    hostingMigrateComparisonInline($(this));
    return false;
  });
});

function hostingMigrateComparisonInline(elem) {
  var hostingMigrateCallback = function(data, responseText) {
    $("#hosting-migrate-comparison-inline").html(data).show();
    $('#hosting-task-confirm-form').hide();
    hostingMigrateToggleSize();
    $('.hosting-migrate-comparison-return').click( function() {
      hostingMigrateComparisonClose();
      return false;
    }
    );
  }
 
  hostingTaskAddOverlay('#hosting-task-list');
  $.get('/hosting/js' + $(elem).attr('href'), null, hostingMigrateCallback );
}

function hostingMigrateToggleSize() {
  parent.Drupal.modalFrame.resize( {
      width: $(document).width() ,
      height: $("body").height() + 25 }
   );
}

function hostingMigrateComparisonClose() {
  $("#hosting-migrate-comparison-inline").hide();
  $('#hosting-task-confirm-form').show();
  hostingMigrateToggleSize();
}
