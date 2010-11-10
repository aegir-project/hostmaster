if (Drupal.jsEnabled) {
  $(document).ready(function() {
     $('div.hosting-site-field-description').hide();
     hostingSiteCheck();
     $('div.hosting-site-field input').change( hostingSiteCheck );
  });

  hostingSiteToggleOptions  = function() {
    // iterate through the visible options
    settings = Drupal.settings.hostingSiteAvailableOptions;

    // Set the form to not be submittable
    $('#edit-submit').attr('disabled', 'disabled');

    var can_submit = true;

    for (var key in settings) {
      css_key = key.replace(/_/g, '-');
      desc_id = 'div#hosting-site-field-' + css_key + '-description';

      // generate an css id to retrieve the value, based on the field type.
      var id = 'div#hosting-site-field-' + css_key;
      if ($(id).hasClass('hosting-site-field-radios')) {
        // show and hide the visible radio options.
        if (typeof(settings[key]) != 'object') {
          $(id).hide();
          $(desc_id).hide(); 
        }
        else if (settings[key].length > 1) {
          // There is more than one possible option, so we display the radio dialogs.
          $(desc_id).hide()
          $(id).show();
          $(id + ' div.form-radios div.form-item').hide();
          checked = false
          for (var option in settings[key]) {
            // modify the definition to get the right css id
            option_css_key = settings[key][option].toString().replace(/[\]\[\ _]/g, '-')
            input_id = 'input[@name=' + key + '][@value=' + settings[key][option] + ']'
            $(id + ' div.form-radios div#edit-' + css_key + '-' + option_css_key +'-wrapper').show();

            // one of the visible radio options has already been checked
            if ($(input_id).attr('checked')) {
              checked = true;
            }
          }
          if (!checked) {
            $('input[@name=' + key + ']:visible:first').attr('checked', 'checked');
          }
        }
        else if (settings[key].length == 1) {
          // There is only one valid option, so we select it and display it as text.
          input_id = 'input[@name=' + key + '][@value=' + settings[key][0] + ']'
          $(input_id).attr("checked", "checked");
          $(id).hide();


          // we have a special case for radios that do not want their description
          // shown. These options have the index value 'null'.
          if (settings[key][0] != 'null') {
            $(desc_id).show()
              .find('div.placeholder')
              .removeClass('error')
              .contents()
              .replaceWith($(input_id).parent().text().trim())
          }
        }
        else {
          // If we have any errors, for the form submit not to be possible.
          can_submit = false;

          $(id).hide();
          $(desc_id).show()
            .find('div.placeholder')
            .addClass('error')
            .contents()
            .replaceWith('No valid choices')
        }
      }
      else if ($(id).hasClass('hosting-site-field-textfield') || $(id).hasClass('hosting-site-field-textarea')) {
        input_id = 'input[@name=' + key + ']'

        if (settings[key] == null) {
          // we do not want the user to be able to manipulate this value,
          // but we need to display the default value to the user.
          $(id).hide();
          if ($(input_id).val().length) {
            $(desc_id).show()
              .find('div.placeholder')
              .contents().
              replaceWith($(input_id).val().trim());
          }
        }
        else if ((settings[key].toString().length || (settings[key] == true)) && (settings[key] != false)) {
          $(id).show();
          $(desc_id).hide();

          // if the field does not have a value yet
          if (!$(input_id).val().length) {
            // we were given a default value by the server
            if (settings[key].length) {
              // set the textfield to the provided default
              $(input_id).val(settings[key])
            }
          }
        }
        else {
          // hide the whole field and description
          $(desc_id).hide();
          $(id).hide();
        }
      }
    }

    if (can_submit) {
      $('#edit-submit').removeAttr('disabled');
    }
  }

  hostingSiteCheck = function() {
    var record = {}

     $('div.hosting-site-field').each( function() {
       // get the field name for this field.
       var field = $(this).attr('id').replace('hosting-site-field-', '').replace(/-/g,'_');

       // generate an css id to retrieve the value, based on the field type.
       var id = 'input[@name=' + field + ']';
       if ($(this).hasClass('hosting-site-field-radios')) { 
         id = id + ':checked' 
       }
        
       // Update the record with the right values.
       record[field] = $(id, this).val();
    });
    var resultOptions = function(data) {
      Drupal.settings.hostingSiteAvailableOptions = data
      hostingSiteToggleOptions();
    }
    $.post('/hosting/hosting_site_form_check', record, resultOptions, 'json');
  }

}

