<?php

require_once(drupal_get_path('module', 'fb') . '/fb.admin.inc');

/**
 * Form callback.
 */
function fb_connect_admin_form() {

  $form['app'] = array(
    '#type' => 'fieldset',
    '#title' => t('Application'),
    '#description' => t('Facebook Connect uses the <a href=!app_url>site-wide default application</a>.', array(
                          '!app_url' => url(FB_PATH_ADMIN_CONFIG . '/settings/app'),
                        )),
  );

  $app = fb_get_app();

  $form['app']['info'] = fb_admin_app_info($app);

  // Display markup to indicate whether facebook connect will work properly.
  // Javascript should show/hide the right portion of the following.
  if ($app) {
    $form['fb_connect_status_test'] = array(
      '#type' => 'fieldset',
      '#title' => t('Connect Status Test'),
      '#description' => t('Facebook Connect requires an application specifically configured to work with this drupal server.'),
    );

    $markup = array('#type' => 'markup', '#prefix' => '<p>', '#suffix' => '</p>');

    // Markup to be displayed depending on getLoginStatus result.
    // Takes advantage of the fact that getLoginStatus will not call its callback when app cannot support facebook connect.
    $form['fb_connect_status_test']['status_no_fb'] = array(
      '#prefix' => '<div class="fb_connect_admin_status fb_connect_admin_status_no_fb fb_new_token_hide">', '#suffix' => '</div>',
    );
    $form['fb_connect_status_test']['status_no_fb'][] = array(
      '#markup' => t('TODO'),
    ) + $markup;

    $form['fb_connect_status_test']['status_no_fb'][] = array(
      '#markup' => 'test: ' . theme('fb_login_button'),
    ) + $markup;

    $form['fb_connect_status_test']['status_new_token'] = array(
      '#prefix' => '<div class="fb_connect_admin_status fb_new_token_show fb_hidden">', '#suffix' => '</div>',
    );
    $form['fb_connect_status_test']['status_new_token'][] = array(
      '#markup' => t('Received token for !application.  Facebook Connect appears to be working.', array(
                       '!application' => '<span class="fb_new_token_app_name">UNKNOWN APP</span>',
                     )),
    ) + $markup;

    $form['fb_connect_status_test']['status_none'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="fb_connect_admin_status fb_connect_admin_status_none">', '#suffix' => '</div>',
      '#markup' => "<p>Connect status: <strong>error</strong>.  Your application cannot support Facebook Connect on this server.</p>",
    );
    $form['fb_connect_status_test']['status_unknown'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="fb_connect_admin_status fb_connect_admin_status_unknown">', '#suffix' => '</div>',
      '#markup' => "<p>Connect status: <strong>not connected</strong>.  Either you are not logged into facebook, or have not authorized the application.</p><p><fb:login-button>Connect to Facebook</fb:lgin-button></p>",
    );
    $form['fb_connect_status_test']['status_connected'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="fb_connect_admin_status fb_connect_admin_status_connected">', '#suffix' => '</div>',
      '#markup' => "<p>Connect status: <strong> OK, connected to Facebook</strong>.</p><p><fb:name uid=loggedinuser useyou=false></fb:name> <fb:profile-pic uid=loggedinuser></fb:profile-pic></p>",
    );


    drupal_add_js(drupal_get_path('module', 'fb_connect') . '/fb_connect.admin.js', array(
                    'type' => 'file',
                    'scope' => 'header',
                    'group' => JS_LIBRARY,
                  ));

  }




  $form[] = array(
    '#type' => 'markup',
    '#markup' => t('There are no settings to save here (yet).'),
  );
  return $form;

  // TODO: full fledge system_settings_form here.




  try {
    $app = fb_get_app();
    $app_data = fb_graph($app['client_id']); // . '?metadata=1');

    $form['fb_application'] = array(
      '#type' => 'fieldset',
      '#title' => t('Application'),
      '#description' => t('Using <a target=_blank href=!app_url>%name</a>.  See the <a href=!admin_url>application tab</a> to change this.', array(
                            '!admin_url' => url(FB_PATH_ADMIN_APP),
                            '!app_url' => $app_data['link'],
                            '%name' => $app_data['name'],
                          )),
    );

  /*
  $form['fb_connect_test'] = array(
    '#type' => 'fieldset',
    '#title' => t('Connect Test'),
    '#description' => t('Facebook Connect requires Facebook\'s javascript to be initialized, and also an <em>application specifically configured to work with this drupal server</em>.'),
  );

  $form['fb_connect_test']['button'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class=fb_not_connected>', '#suffix' => '</div>',
    '#markup' => '<p>Test button: <fb:login-button>Click here to test Facebook Connect</fb:login-button></p><ul><li>If you don\'t see a button, javascript is not initialized.</li><li>Click the button.  If you see "an error occurred..." instead of an authorization dialog, then the application is not configured to work with this server. <br/>(Note, it is not necessary to log into facebook or accept the authorization.)</li></ul>',
  );
  */


    return system_settings_form($form);
  }
  catch (exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    return array();
  }
}
