hostingTaskRefreshList = function() {

  $('#hosting-task-list').prepend('<div class="hosting-overlay"><div class="hosting-throbber"></div></div>');
  $.get('/hosting/tasks/' + Drupal.settings.hostingTaskRefresh.nid + '/list', null,  function(data, responseText) {
    $("#hosting-task-list").html(data.markup);
    hostingTaskBindButtons($('#hosting-task-list'));
    setTimeout("hostingTaskRefreshList()", 30000);
   }, 'json' );
}

hostingTaskBindButtons = function(elem) {
  $('.hosting-button-enabled', elem).filter('[href^="/node"]').click(function() {
     var options = {
        url : '/hosting/js' + $(this).attr('href'),
        draggable : false,
        width : 800,
        height : 450
      }
      Drupal.modalFrame.open(options);
      return false;
   });
}

$(document).ready(function() {
    if (Drupal.settings.hostingTaskRefresh.nid) { 
      setTimeout("hostingTaskRefreshList()", 3000);
    }
   // hostingTaskBindButtons($(this));
/* */
});
