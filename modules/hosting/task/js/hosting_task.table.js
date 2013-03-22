(function($) {
  Drupal.behaviors.hosting_task_table = {
    attach: function(context, settings) {
      $(context).find('table.hosting-table').once('hosting_task_table', function() {
        var base = $(this).attr('id');
        if (base) {
          var element_settings = {};
          // Clicked links look better with the throbber than the progress bar.
          element_settings.progress = { 'type': 'throbber' };
          element_settings.url = settings.hosting_task.refresh_url + '/' + base;
          element_settings.event = 'hosting_table_ajax';
          Drupal.ajax[base] = new Drupal.ajax(base, this, element_settings);
          $(this).trigger(element_settings.event);
          Drupal.ajax[base].error = function (response, uri) {
            // Remove the nasty alert for this call, if it fails, oh well.
            //alert(Drupal.ajaxError(response, uri));
            // Remove the progress element.
            if (this.progress.element) {
              $(this.progress.element).remove();
            }
            if (this.progress.object) {
              this.progress.object.stopMonitoring();
            }
            // Undo hide.
            $(this.wrapper).show();
            // Re-enable the element.
            $(this.element).removeClass('progress-disabled').removeAttr('disabled');
            // Reattach behaviors, if they were detached in beforeSerialize().
            if (this.form) {
              var settings = response.settings || this.settings || Drupal.settings;
              Drupal.attachBehaviors(this.form, settings);
            }
          };
        }

      });
    }
  }

  Drupal.ajax.prototype.commands.hosting_table_append = function(ajax, response, status) {
    // Get information from the response. If it is not there, default to
    // our presets.
    var wrapper = response.selector ? $(response.selector) : $(ajax.wrapper);
    var effect = ajax.getEffect(response);

    // We don't know what response.data contains: it might be a string of text
    // without HTML, so don't rely on jQuery correctly iterpreting
    // $(response.data) as new HTML rather than a CSS selector. Also, if
    // response.data contains top-level text nodes, they get lost with either
    // $(response.data) or $('<div></div>').replaceWith(response.data).
    var new_content = $(response.data);

    // Work out if the user is scrolled to the bottom.
    var at_bottom = false;
    if ($(window).scrollTop() + $(window).height() == $(document).height()) {
      at_bottom = true;
    }

    // Add the new content to the page.
    new_content.find('tr').each(function() {
      wrapper.find('tbody').append(this);
    });

    if (at_bottom) {
      $(window).scrollTop($(document).height());
    }

      // Apply any settings from the returned JSON if available.
      var settings = response.settings || ajax.settings || Drupal.settings;
      Drupal.attachBehaviors(wrapper, settings);
  }

  Drupal.ajax.prototype.commands.hosting_table_check = function(ajax, response, status) {
    // Get information from the response. If it is not there, default to
    // our presets.
    var wrapper = response.selector ? $(response.selector) : $(ajax.wrapper);
    var base = $(wrapper).attr('id');
    Drupal.ajax[base].url = response.url + '/' + base;
    Drupal.ajax[base].options.url = response.url + '/' + base;
    setTimeout(function() {
      // Work out if the user is scrolled to the bottom.
      var at_bottom = false;
      if ($(window).scrollTop() + $(window).height() == $(document).height()) {
        at_bottom = true;
      }
      $(wrapper).trigger('hosting_table_ajax');

      if (at_bottom) {
        $(window).scrollTop($(document).height());
      }
    }, 200);

  }


})(jQuery);