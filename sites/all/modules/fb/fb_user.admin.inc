<?php

/**
 * @file
 * Admin pages and forms for user settings.
 *
 */

/**
 * Form builder; Configure settings for this site.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function fb_user_admin_settings() {

  $form[FB_USER_VAR_MAP_POLICY] = array(
    '#type' => 'checkboxes',
    '#title' => t('Link Existing Accounts'),
    '#description' => t('Linking an account means a user can log into this website by connecting to Facebook.<br/>Matching based on email requires the email extended permission on Facebook.'),
    '#options' => array(
      FB_USER_OPTION_MAP_LOGGED_IN => t('Link account when user is simultaneously logged in locally and connected to facebook'),
      FB_USER_OPTION_MAP_EMAIL => t('Link account when Facebook email exactly matches local account'),
    ),
    '#default_value' => variable_get(FB_USER_VAR_MAP_POLICY, array(
                                       FB_USER_OPTION_MAP_LOGGED_IN,
                                       FB_USER_OPTION_MAP_EMAIL,
                                     )),
  );

  $form[FB_USER_VAR_CREATE_POLICY] = array(
    '#type' => 'radios',
    '#title' => t('Create Local Accounts'),
    '#description' => t('This option will create a new local account when a user connects to Facebook.  The user will be logged into Drupal immediately upon connecting.'),
    '#options' => array(
      FB_USER_OPTION_CREATE_NEVER => t('Do not create accounts automatically'),
      FB_USER_OPTION_CREATE_LOGIN => t('When user is connected to Facebook'),
    ),
    '#default_value' => variable_get(FB_USER_VAR_CREATE_POLICY, FB_USER_OPTION_CREATE_NEVER),
    '#required' => TRUE,
  );


  // Name options for automatically created user accounts.
  $default = variable_get(FB_USER_VAR_USERNAME_STYLE, FB_USER_OPTION_USERNAME_FULL);

  $form[FB_USER_VAR_USERNAME_STYLE] = array(
    '#type' => 'radios',
    '#title' => t('Username Style for Automatically Created Accounts'),
    '#description' => t('Machine-friendly names include Facebook user ids to ensure uniqueness. <br/>Human-friendly names are like "John Smith".  Because Drupal requires unique names, you may see "John Smith_2", "John Smith_3" and so on.'),
    '#options' => array(
      FB_USER_OPTION_USERNAME_FBU => t('Machine-friendly, i.e. "1234565789@facebook"'),
      FB_USER_OPTION_USERNAME_FULL => t('Human-friendly, i.e. "John Smith"'),
    ),
    '#default_value' => $default,
  );
  // TODO: better support for machine-friendly names.
  $form[FB_USER_VAR_USERNAME_STYLE]['#description'] .= "<br/>" . t("Note, machine-friendly names not fully supported in this .dev release of modules/fb.  Human-friendly names are recommended.");

  return system_settings_form($form);
}
