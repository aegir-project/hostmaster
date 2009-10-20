hostingTaskRefreshList = function() {

  var hostingTaskListRefreshCallback = function(data, responseText) {
    // If the node has been modified, reload the whole page.
    if (Drupal.settings.hostingTaskRefresh.changed < data.changed) {
      document.location.reload();
    }
    else {
      $("#hosting-task-list").html(data.markup);
      setTimeout("hostingTaskRefreshList()", 30000);
    }
  }
  
 
  hostingTaskAddOverlay('#hosting-task-list');
  $.get('/hosting/tasks/' + Drupal.settings.hostingTaskRefresh.nid + '/list', null, hostingTaskListRefreshCallback , 'json' );
}


function hostingTaskAddOverlay(elem) {
  $(elem).prepend('<div class="hosting-overlay"><div class="hosting-throbber"></div></div>');
}


function hostingTaskRefreshQueueBlock() {
  var hostingTaskQueueRefreshCallback = function(data, responseText) {
    // If the node has been modified, reload the whole page.
    $("#hosting-task-queue-block").html(data.markup);
    setTimeout("hostingTaskRefreshQueueBlock()", 30000);
  }
 
  hostingTaskAddOverlay('#hosting-task-queue-block');
  $.get('/hosting/tasks/queue', null, hostingTaskQueueRefreshCallback , 'json' );
}

$(document).ready(function() {

//  hostingTaskAddOverlay('#hosting-task-queue-block');
  if (Drupal.settings.hostingTaskRefresh.nid) { 
    setTimeout("hostingTaskRefreshList()", 30000);
  }

  if (Drupal.settings.hostingTaskRefresh.queueBlock == 1) {
    setTimeout("hostingTaskRefreshQueueBlock()", 30000);
  }
  // hostingTaskBindButtons($(this));
/* */
});


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

