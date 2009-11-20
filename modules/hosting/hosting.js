Drupal.hostingTaskLogAttach = function() {
  $('.hosting-summary-expand').click(
    function() {
      $(this).parent().toggle();
      $('.hosting-task-full', $(this).parent().parent()).toggle();
    }
  );
}
if (Drupal.jsEnabled) {
  $(document).ready(Drupal.hostingTaskLogAttach);
}

