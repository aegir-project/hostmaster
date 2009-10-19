hostingTaskRefreshList = function() {
  $.get('/hosting/tasks/' + Drupal.settings.hostingTaskRefresh.nid + '/list', null,  function(data, responseText) {
    $("#hosting-task-list").html(data.markup);
    setTimeout("hostingTaskRefreshList()", 30000);
   }, 'json' );
}


$(document).ready(function() {
    if (Drupal.settings.hostingTaskRefresh.nid) { 
      setTimeout("hostingTaskRefreshList()", 30000);
    }
/*     $('.hosting-button-enabled').filter('[href^="/node"]').click(function() {
       var options = {
          url : '/hosting/js' + $(this).attr('href'),
          draggable : false,
          width : 800,
          height : 450
        }
        Drupal.modalFrame.open(options);
        return false;
     });
*/
});
