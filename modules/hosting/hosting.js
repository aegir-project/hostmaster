(function($) {

Drupal.behaviors.hostingTaskLogAttach = {
  attach : function() {
    $('.hosting-summary-expand').click(
      function() {
        $(this).parent().toggle();
        $('.hosting-task-full', $(this).parent().parent()).toggle();
      }
    );
  }
}


})(jQuery);