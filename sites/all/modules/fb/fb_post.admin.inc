<?php

require_once(drupal_get_path('module', 'fb') . '/fb.admin.inc');

/**
 * Post to Facebook settings form.
 *
 * TODO
 */
function fb_post_admin_form($form, &$form_state) {

  $form['token'] = array(
    '#type' => 'fieldset',
    '#title' => t('Access Token'),
    '#description' => t('Post to Facebook uses the <a href=!token_url>site-wide default access token</a>.', array(
                          '!token_url' => url(FB_PATH_ADMIN_CONFIG . '/settings/token'),
                        )),
  );

  $token = fb_get_admin_token();
  $form['token']['info'] = fb_admin_token_info($token);

  $form[] = array(
    '#type' => 'markup',
    '#markup' => t('There are no settings to save here (yet).'),
  );
  return $form;

  // TODO: full fledge system_settings_form here.
}
