if (Drupal.jsEnabled) {
  $(document).ready(function() {
/**
 * initialize the form
 */
    hostingSitePopulate('input[@name=platform]', 'profile');
    $('div.hosting-site-form-language-options').parent().hide();

    $('input[@name=platform]').change(function() {
      hostingSitePopulate(this, 'profile');
      $('div.hosting-site-form-language-options').parent().hide();
    });
    $('input[@name=profile]').change(function() {
      hostingSitePopulate(this, 'language');
    }); 
  });
}

/**
 * populate the form elements through ajax
 */
function hostingSitePopulate(obj, option) {
  $(obj).parent().eq(0).append("<div id='hm-processing'>processing</div>");
  var resultOptions = function (data) {
    var result = Drupal.parseJson(data);
    if (result['status'] == 'TRUE') {
      resultID = 'hosting-site-form-' + option + '-options'; 
      if (result['data']) {
        if ($('div.' + resultID).html()) {
          $('div.' + resultID).parent().after(result['data']).remove(); 
          $('div.' + resultID).parent().show();
        }
        else {
          $('input.' + resultID).after(result['data']).remove();
          $('div.' + resultID).parent().show();
        }
        if (option == 'profile') {
          $('input[@name=profile]').change(function() {
            hostingSitePopulate(this, 'language');
          }); 
        }
      }
      else {
        $('div.' + resultID).parent().hide();
      }
    }
    $('div#hm-processing').remove();
  }
  $.get('/hosting/hosting_site_form_populate/' + option + '/' + $(obj).val(), null, resultOptions);
}
