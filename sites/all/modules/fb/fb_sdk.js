/**
 * Initialize Facebook's Javascript SDK.
 */
FB_SDK = function(){};
FB_SDK.fbu = null;
FB_SDK.status = 'none'; // login status: none, unknown (not connected), or connected
FB_SDK.authResponse = null;


(function ($) {
  Drupal.behaviors.fb_sdk = {
    attach : function(context, settings) {
      if (typeof(FB) == 'undefined' && typeof(window.fbAsyncInit) == 'undefined') {
        // No other modules have initialed Facebook's javascript, so we can.

        jQuery('body').append('<div id="fb-root"></div>'); // Facebook recommends this tag.

        window.fbAsyncInit = FB_SDK.fbAsyncInit;

        // http://developers.facebook.com/docs/reference/javascript/
        var e = document.createElement('script'); e.async = true;
        e.src = document.location.protocol + '//connect.facebook.net/en_US/all.js';
        document.getElementById('fb-root').appendChild(e);
        Drupal.settings.fb.fb_initialized = 1;
      }
      else {
        // Because Facebook SDK initializes asynchronously, we must check that FB is defined.  Also that we were the module than initialized it, otherwise some other module is in charge and we shouldn't do anything.
        if (Drupal.settings.fb && Drupal.settings.fb.fb_initialized && typeof(FB) != 'undefined') {
          // Render any XFBML markup that may have been added by AJAX,
          // if we are the module that initialized the facebook
          // javascript.
          jQuery(context).each(function() {
            var elem = jQuery(this).get(0);
            FB.XFBML.parse(elem);
          });
          FB_SDK.handleLoginStatus(FB_SDK.fbu, context);
        }
      }
    }
  };
})(jQuery);

FB_SDK.fbAsyncInit = function() {
  FB.init({
    appId: Drupal.settings.fb.client_id,
    status: true,
    xfbml: true
  });
  // Notify third parties that global FB is now initialized.
  jQuery.event.trigger('fb_init');


  if (Drupal.settings.fb.client_id) {
    // Use FB.Event to detect Connect login/logout.
    FB.Event.subscribe('auth.authResponseChange', FB_SDK.authResponseChange);

    FB.getLoginStatus(function(response) {
      FB_SDK.authResponse = response.authResponse;
      FB_SDK.status = response.status;
      if (response.status == 'connected') {
        FB_SDK.handleLoginStatus(response.authResponse.userID);
      }
      else {
        FB_SDK.handleLoginStatus(0);
      }
      // Use FB.Event to detect Connect login/logout.
      //FB.Event.subscribe('auth.authResponseChange', FB_SDK.authResponseChange);

      // Notify third-parties.
      jQuery.event.trigger('fb_login_status', response);

      if (response.status != Drupal.settings.fb.status) {
        // Spoof a session change event.
        FB_SDK.authResponseChange(response);
      }
    });
  }
  else {
    FB_SDK.handleLoginStatus(0);
    // Use FB.Event to detect Connect login/logout.
    //FB.Event.subscribe('auth.authResponseChange', FB_SDK.authResponseChange);
  }

};


// Facebook pseudo-event handlers.
FB_SDK.authResponseChange = function(response) {
  FB_SDK.authResponse = response.authResponse;
  FB_SDK.status = response.status;
  if (response.status == 'connected') {
    FB_SDK.handleLoginStatus(response.authResponse.userID);
  }
  else {
    FB_SDK.handleLoginStatus(0);
  }
  if (response.status != Drupal.settings.fb.status) {
    FB_JS.ajaxEvent('session_change', {});
  }
};

/**
 * Called when we first learn the currently logged in user's Facebook ID.
 *
 * Responsible for showing/hiding markup not intended for the current
 * user.  Some sites will choose to render pages with fb_connected and
 * fb_not_connected classes, rather than reload pages when users
 * connect/disconnect.
 *
 * Also supports fb_status_none (getLoginStatus does not return),
 * fb_status_unknown (not connected), and fb_status_connected
 * (connected).
 */
FB_SDK.handleLoginStatus = function(fbu, context) {
  if (context || fbu != FB_SDK.fbu) {
    // User has changed.
    delete FB_JS.session[Drupal.settings.fb.client_id];
    FB_JS.setCookie('fb_js_session', JSON.stringify(FB_JS.session));

    if (fbu) {
      FB_SDK.fbu = fbu;
      // Show markup intended only for connected users.
      jQuery('.fb_not_connected', context).hide();
      jQuery('.fb_connected', context).show();
    }
    else {
      FB_SDK.fbu = null;
      // Show markup intended only for not connected users.
      jQuery('.fb_connected', context).hide();
      jQuery('.fb_not_connected', context).show();
    }
  }

  // Show or hide content based on status.
  var states = ['none', 'unknown', 'connected'];
  for (var i = states.length-1; i >= 0; --i){
    if (FB_SDK.status != states[i]) {
      jQuery('.fb_status_' + states[i]).hide();
    }
  }
  jQuery('.fb_status_' + FB_SDK.status).show();
};
