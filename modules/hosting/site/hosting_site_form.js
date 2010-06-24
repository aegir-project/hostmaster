if (Drupal.jsEnabled) {
  $(document).ready(function() {
    /**
    * initialize the form
    */
    hostingSitePopulate('profile');
    $('div.hosting-site-form-site-language-options').parent().hide();
    $('input[@name=platform]').change(function() {
      hostingSitePopulate('profile');
      $('div.hosting-site-form-site-language-options').parent().hide();
    });
    $('input[@name=profile]').change(function() {
      hostingSitePopulate('site_language');
    }); 
  });
}

/**
 * populate the form elements through ajax
 */
function hostingSitePopulate(option) {
  // initialize variables
  parent_field = (option == 'profile') ? 'platform' : 'profile';
  obj = _hostingSiteField(parent_field); 
  value = $('input[@name=' + option + ']:checked').val();
  
  // add loading animation gif pic
  $(obj).parent().eq(0).append("<div id='hm-processing'></div>");

  // ajax function
  var resultOptions = function (data) {
    var result = Drupal.parseJson(data);
    if (result['status'] == 'TRUE') {
      resultID = 'hosting-site-form-' + option.replace('_', '-') + '-options'; 
      if (result['data']) {
        // replace form element options
        if ($('div.' + resultID).html()) {
          $('div.' + resultID).parent().after(result['data']).remove(); 
          $('div.' + resultID).parent().show();
        }
        else {
          $('input.' + resultID).after(result['data']).remove();
          $('div.' + resultID).parent().show();
        }
        // restore choice of last time
        $('input[@name=' + option + '][@value=' + value + ']').attr("checked", "checked");
        // bind on-change event
        if (option == 'profile') {
          if (($('input[@name=profile]:checked').val()) || (result['type'] != 'radios')) {
            hostingSitePopulate('site_language');
          }
          if (result['type'] == 'radios') {
            $('input[@name=profile]').change(function() {
              hostingSitePopulate('site_language');
            }); 
          }
        }
      }
      else {
        $('div.' + resultID).parent().hide();
      }
    }
    // remove the gif animation
    $('div#hm-processing').remove();
  }
  // prepare url params
  params = (option == 'profile') ? $(obj).val() : ($(obj).val() + '/' + $(_hostingSiteField('platform')).val());
  $.get('/hosting/hosting_site_form_populate/' + option + '/' + params, null, resultOptions);
}

function _hostingSiteField(name) {
  obj = 'input[@name=' + name + ']';
  if ($(obj).attr('type') != 'hidden') {
    obj = 'input[@name=' + name + ']:checked';
  }
  return obj;
}
