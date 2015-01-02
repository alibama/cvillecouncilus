// TODO: move all this to fb.admin.js
FbConnectAdmin = function(){};

FbConnectAdmin.loginStatusHandler = function(event, response) {
  jQuery('.fb_connect_admin_status').hide();
  jQuery('.fb_connect_admin_status_' + response.status).show();
};

FbConnectAdmin.initHandler = function(event, response) {
  jQuery('.fb_connect_admin_status').hide();

  // The status_none text will be hidded if getLoginStatus returns.
  jQuery('.fb_connect_admin_status_none').show();
};

/**
 * Drupal behaviors hook.
 *
 * Called when page is loaded, or content added via javascript.
 */
(function ($) {
  Drupal.behaviors.fb_connect_admin = {
    attach : function(context) {
      // Respond to our jquery pseudo-events
      jQuery(document).bind('fb_login_status', FbConnectAdmin.loginStatusHandler);
      jQuery(document).bind('fb_init', FbConnectAdmin.initHandler);

      jQuery('.fb_connect_admin_status').hide();
      jQuery('.fb_connect_admin_status_no_fb').show();

    }
  };

})(jQuery);
