Drupal.hostingHelpAttach = function() {
  $('.hosting-help-toggle').click(
    function() {
      $('.hosting-help', $(this).parent()).toggle('slow')
    }
  );
}
if (Drupal.jsEnabled) {
  $(document).ready(Drupal.hostingHelpAttach);
}


