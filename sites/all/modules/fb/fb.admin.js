
FB_Admin = function(){};

/**
 * Drupal behaviors hook.
 *
 * Called when page is loaded, or content added via javascript.
 */
(function ($) {
  Drupal.behaviors.fb_admin = {
    attach : function(context) {
      jQuery(document).bind('fb_new_token', FB_Admin.newTokenHandler);
    }
  };

})(jQuery);


// We learned of a new token, client-side.
FB_Admin.newTokenHandler = function(e, data) {
  // Put the new value into admin form.
  jQuery('.fb_new_token_value').val(data.access_token);

  if (data.me) {
    jQuery('.fb_new_token_name').text(data.me.name);
  }
  if (data.app) {
    jQuery('.fb_new_token_app_name').text(data.app.name);
  }

  jQuery('#edit-fb-token-index-0').attr('checked', 'checked');
  jQuery('.fb_new_token_hide').hide();
  jQuery('.fb_new_token_show').show();
};
