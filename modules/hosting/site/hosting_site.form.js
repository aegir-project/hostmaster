if (Drupal.jsEnabled) {
  $(document).ready(function() {
     $('div.hosting-site-field-description').hide();
     hostingSiteToggleOptions();
     $('div.hosting-site-field input').change( hostingSiteCheck );
  });
}

hostingSiteToggleOptions  = function() {
  // iterate through the visible options
  settings = Drupal.settings.hostingSiteAvailableOptions;

  css_regex = /_/g;
  console.log(settings)
  for (var key in settings) {
    css_key = key.replace(css_regex, '-');
    
    // generate an css id to retrieve the value, based on the field type.
    var id = 'div#hosting-site-field-' + css_key;
    if ($(id).hasClass('hosting-site-field-radios')) { 
      // show and hide the visible radio options.
      if (settings[key].length) {
        $(id).show();
        $(id + ' div.form-radios div.form-item').hide();
        for (var option in settings[key]) {
          // modify the definition to get the right css id
          option_css_key = settings[key][option].toString().replace(/[\]\[\ _]/g, '-')

            $(id + ' div.form-radios div#edit-' + css_key + '-' + option_css_key +'-wrapper').show();
          if (settings[key].length == 1) {
            //it's the only option, select it.
            $('input[@name=' + key + '][@value=' + settings[key][option] + ']').attr("checked", "checked");
            //$('div#hosting-site-field-' + css_key + '-description').show();

          }
          else {

          }
        }

      }
      else {
        $(id).hide();
      }
    }
    else if ($(id).hasClass('hosting-site-field-textfield')) {
      // show and hide the visible radio options.
      if (settings[key].length || (settings[key] == true)) {
        $(id).show();

        // if the field does not have a value yet
        if (!$('input[@name=' + key + ']').val().length) {
          // we were given a default value by the server
          if (settings[key].length) {
            // set the textfield to the provided default
            $('input[@name=' + key + ']').val(settings[key])
          }
        }
      }
      else {
        $(id).hide();
      }
    }
  }
}

hostingSiteCheck = function() {
  var record = {}

   $('div.hosting-site-field').each( function() {
     // get the field name for this field.
     var field = $(this).attr('id').replace('hosting-site-field-', '').replace('-','_');

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



