if (Drupal.jsEnabled) {
  $(document).ready(function() {
      hostingSiteToggleOptions();
     $('div.hosting-site-field input').change( hostingSiteCheck );
  });
}

hostingSiteToggleOptions  = function() {
  // iterate through the visible options
  
  settings = Drupal.settings.hostingSiteAvailableOptions;

  for (var key in settings) {
    css_key = key.replace('_', '-');
    if (settings[key].length) {
      $('div#hosting-site-field-' + css_key).show();
      $('div#hosting-site-field-' + css_key + ' div.form-radios div.form-item').hide();
      for (var option in settings[key]) {
        $('div#hosting-site-field-' + css_key + ' div.form-radios div#edit-' + css_key + '-' + settings[key][option] +'-wrapper').show();
      }
    }
    else {
      $('div#hosting-site-field-' + css_key).hide();
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



