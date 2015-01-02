<?php

// Javascript effects for admin forms.
drupal_add_js(drupal_get_path('module', 'fb') . '/fb.admin.js', array(
    'type' => 'file',
    'scope' => 'header',
    'group' => JS_LIBRARY,
  ));

/**
 * Display detailed information about a token.
 *
 * TODO: more detail, including extended perms.
 */
function fb_admin_token_info($token) {

  $markup = array(
    '#type' => 'markup',
    '#prefix' => '<p>', '#suffix' => '</p>',
  );

  if (!$token) {
    return array(
      '#markup' => t('There is no token configured.'),
    ) + $markup;
  }

  $tdata = db_query("SELECT * FROM {fb_token} WHERE access_token=:token", array(
             ':token' => $token,
           ))->fetchAssoc();
  $me = NULL;
  $app = NULL;

  if ($tdata) {
    try {
      $app = fb_graph($tdata['fba']);
      $me = fb_graph($tdata['fbu']);
    }
    catch (Exception $e) {
    }
  }
  else {
    $tdata = array(
      'fba' => t('unknown'),
      'fbu' => t('unknown'),
    );
  }


  if (!$app || !$me) {
    try {
      $graph = fb_graph_batch(array('app', 'me'), $token);
      extract($graph);
    }
    catch (exception $e) {
    }
  }

  $args = array();
  if ($app) {
    $args['!app'] = l(fb_get_name($app), fb_get_link($app));
    $args['!app_logo'] = $app['logo_url'];
  }
  else {
    $args['!app'] = $tdata['fba']; // Show number.
    $args['!app_logo'] = '';
  }
  if ($me) {
    $args['!me'] = l(fb_get_name($me), fb_get_link($me));
    $args['!me_logo'] = "//graph.facebook.com/" . $me['id'] . '/picture';
  }
  else {
    $args['!me'] = $tdata['fbu'];
    $args['!me_logo'] = '';
  }

  $output[] = array(
    '#markup' => t('<img src=!me_logo /> <img src=!app_logo /> Token has the privileges of !me, via the application !app.', $args),
  ) + $markup;

  if (user_access(FB_PERM_ADMINISTER_TOKEN)) {
    $output[] = array(
      '#markup' => t('<a target=_blank href=!fb_url_token_debug>Debug this token on developers.facebook.com</a>.', array(
                   '!fb_url_token_debug' => url('https://developers.facebook.com/tools/debug/access_token', array(
                                            'query' => array(
                                              'q' => $token,
                                            ),
                                          )),
                 )),
    ) + $markup;
  }

  return $output;
}

/**
 * Display detailed information about an application.
 *
 * TODO: make this is theme function.
 */
function fb_admin_app_info($app) {

  $markup = array('#type' => 'markup', '#prefix' => '<p>', '#suffix' => '</p>');

  if (!$app) {
    return array(
      '#markup' => t('There is no application configured.'),
    ) + $markup;
  }

  try {
    // TODO: determine whether app token is known.
    // TODO: FQL queries if necessary for more data
    $app = fb_graph($app['client_id']);
    $args = array(
      '!app' => l($app['name'], $app['link']),
    );
    // Args to t() need a ! in front.
    foreach ($app as $key => $value) {
      if (!is_array($value)) {
        $args['!' . $key] = $value;
      }
    }

    $output[] = array(
      '#markup' => '<img src="' . $app['logo_url'] . '"/>',
    ) + $markup;

    $output[] = array(
      '#markup' => t('Using application !app.', $args),
    ) + $markup;
  }
  catch (exception $e) {
    fb_log_exception($e, t('Failed to query application data.'));
    $output[] = array(
      '#markup' => t('Could not find the application %name (%client_id) !', array(
                       '%name' => $app['name'],
                       '%client_id' => $app['client_id'],
                     )),
    ) + $markup;
  }

  return $output;
}

function fb_admin_default_token_form($form, &$form_state) {

  // Explanation of form.
  $markup = array(
    '#type' => 'markup',
    '#prefix' => '<p>', '#suffix' => '</p>',
  );
  $form['description']['#weight'] = -20;
  $form['description'][] = array(
    '#markup' => t('This form configures an <em>access token</em> for advanced integration between this website and Facebook.'),
  ) + $markup;
  $form['description'][] = array(
    '#markup' => t('Save a token if you want the website to interact with Facebook when the current user is not connected.  For example, to cross-post content to a Facebook page while the author is not logged into Facebook.'),
  ) + $markup;

  // The form to create a new token is mostly the same as the form to replace a token.
  $form = fb_admin_replace_token_form($form, $form_state, FB_VAR_ADMIN_ACCESS_TOKEN, array(
            'replace' => FALSE,
            'scope' => array(
              // Extended permissions.
              'manage_pages',
              'publish_actions',
            ),
          ));

  $form['submit']['#value'] = t('Save default token');

  drupal_set_title(t('Site-Wide Default Access Token'));

  return $form;
}

function fb_admin_replace_token_form($form, &$form_state, $variable, $options = array()) {
  // defaults
  $options = $options + array(
    'title' => t('Configure Access Token'),
    'save' => TRUE,
    'replace' => TRUE,
    'scope' => array(
    ),
  );

  drupal_set_title($options['title']);

  // Save values needed in submit handler.
  $form_state['fb_vars']['variable'] = $variable;
  $form_state['fb_vars']['options'] = $options;

  // Show details about the current token.
  if ($variable) {
    $current = variable_get($variable, FALSE);
    if ($current) {
      $form['current_token'] = array(
        '#type' => 'fieldset',
        '#title' => t('Current Token'),
        'info' => fb_admin_token_info($current),
        '#attributes' => array('class' => array('fb_new_token_hide')),
        '#weight' => -10,
      );
    }
  }


  // Allow selection of previously saved tokens.
  $form['fb_admin_token_select'] = array(
    '#type' => 'fb_admin_token_select',
    '#title' => t('Select from previously saved tokens'),
    '#description' => t('Select a token from the <a href=!url>token management</a> page.', array(
                      '!url' => url(FB_PATH_ADMIN_CONFIG . '/token'),
                    )),
    '#default_value' => $variable ? variable_get($variable, NULL) : NULL,
  );

  // Allow generation of new token.
  $form['fb_admin_token_generate'] = array(
    '#type' => 'fb_admin_token_generate',
    '#title' => t('Generate a new token'),
    '#description' => t('Click a link to create a token for your Facebook account.'),
    '#scope' => $options['scope'],
  );

  $form[FB_VAR_PREFER_LONG_TOKEN] = array(
    '#type' => 'checkbox',
    '#title' => t('Prefer longer-lived tokens.'),
    '#description' => t('Some Facebook access tokens expire in minutes, or when user logs out of facebook.com. Longer lived tokens can last weeks before expiring.  Facebook no longer offers access that never expire.'),
    '#default_value' => variable_get(FB_VAR_PREFER_LONG_TOKEN, TRUE),
  );

  $form['#validate'] = array('fb_admin_replace_token_form_validate');
  $form['#submit'] = array('fb_admin_replace_token_form_submit');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save token'),
  );

  return $form;
}

function fb_admin_replace_token_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  if ($values['fb_admin_token_select'] && $values['fb_admin_token_select'] != -1 &&
    $values['fb_admin_token_generate'] && $values['fb_admin_token_generate'] != -1) {
    form_error($form[fb_admin_token_select], t('Select either a saved token or a new token.  Not both.'));
  }

}

function fb_admin_replace_token_form_submit($form, &$form_state) {
  extract($form_state['fb_vars']); // $variable, $options

  $values = $form_state['values'];
  $long_token = NULL;
  $token = NULL;
  foreach (array('fb_admin_token_select', 'fb_admin_token_generate') as $key) {
    if ($values[$key] && $values[$key] != -1) {
      $token = $values[$key];
    }
  }

  if ($variable) {
    if ($options['replace']) {
      $orig_token = variable_get($variable, NULL);
      db_query("DELETE FROM {fb_token} WHERE access_token = :token", array(
          ':token' => $orig_token,
        ));
      // @todo: delete any other variables that use the expired token.
    }
    variable_del($variable);
  }

  if (!$token) {
    drupal_set_message(t("No token chosen."));
    return;
  }

  extract($form_state['fb_graph']); // $app, $me.

  if ($token && $values[FB_VAR_PREFER_LONG_TOKEN]) {
    $long_token = fb_admin_long_lived_token($token, $app['id']);
  }

  if ($long_token) {
    $token = $long_token;
  }

  if ($variable) {
    variable_set($variable, $token);
  }
  if ($options['save']) {
    fb_token_save($token, array(
        'status' => FB_STATUS_FLAG_ADMIN | FB_STATUS_FLAG_VALID,
        'graph' => $form_state['fb_graph'],
      ));
  }

  drupal_set_message(t("Now using access token for %user_name via application %app_name.", array(
        '%user_name' => fb_get_name($me),
        '%app_name' => fb_get_name($app),
      )));
}


function fb_admin_token_page() {

  $token = variable_get(FB_VAR_ADMIN_ACCESS_TOKEN, NULL);

  if ($token) {
    try {
      $graph = fb_graph_batch(array('me', 'app'), $token);
      // dpm($graph, __FUNCTION__); // debug
    }
    catch (Exception $e) {
      //
    }
  }

  return array(
    'manage' => drupal_get_form('fb_admin_tokens_form'),
    'add' => drupal_get_form('fb_admin_add_token_form', array(
             'status' => FB_STATUS_FLAG_ADMIN,
           )),
  );
}

function fb_admin_add_token_form($form0, &$form_state, $options = array()) {
  $form_state['fb']['options'] = $options + array(
    // Default options.
    'status' => 0,
  );

  if ($user_token = fb_user_token()) {
    $already_saved = db_query("SELECT count(*) FROM {fb_token} WHERE access_token = :token", array(':token' => $user_token))->fetchField();
    if (!$already_saved) {
      try {
        $graph = fb_graph_batch(array('me', 'app'), $user_token);
        $form['user_token'] = array(
          '#type' => 'checkbox',
          '#title' => t('%user via %application', array(
                          '%user' => $graph['me']['name'],
                          '%application' => $graph['app']['name'],
                        )),
          '#return_value' => $user_token,
          '#default_value' => 0,
          '#description' => t('Use your current connection to facebook.'),
        );
        $form_state['fb_user_token'] = array(
          'user_token' => $user_token,
          'graph' => $graph,
        );
      }
      catch (Exception $e) {
        //
      }
    }
  }

  // Placeholder for values set in validate.
  $form['data'] = array(
    '#tree' => TRUE,
  );

  $form['token'] = array(
    // Use textarea because tokens can be very long.
    '#type' => 'textarea',
    '#rows' => 1,
    '#title' => t('Access token'),
    '#description' => t('Paste a complete access token.  Facebook provides developer tools to <a href=!explore_url target=_blank>create</a> and <a href=!tool_url target=_blank>list</a> tokens.', array(
                          '!explore_url' => 'https://developers.facebook.com/tools/explorer',
                          '!tool_url' => 'https://developers.facebook.com/tools/access_token',

                        )),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add token'),
  );

  // Wrap entire form in a fieldset.
  $form0['wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Access Token'),
    //'#description' => t('Add a new token.'),
    'form' => $form,
  );
  return $form0;
}

/**
 * This unlikely helper function speeds copy and paste from facebook.
 * Allows user to put both id and secret in one textfield.
 */
function _fb_admin_parse_textfield($text) {
  $id = NULL;
  $secret = NULL;
  $token = NULL;
  $split = preg_split('/\s+/', $text); // Note explode(' ', $text) not sufficient.  Need to handle spaces or tabs.

  // First figure out what was submitted.
  if (count($split) > 1) {
    foreach ($split as $item) {
      if (is_numeric($item)) {
        $id = trim($item);
      }
      else {
        $secret = trim($item);
      }
    }
  }
  elseif (is_numeric($text)) {
    $id = $text;
  }
  else {
    $token = $text;
  }

  // Get app token if possible.
  if ($id && $secret) {
    $token = fb_admin_get_app_token($id, $secret);
  }

  return array($id, $secret, $token ? $token : FB_TOKEN_NONE);
}

function fb_admin_add_token_form_validate(&$form, &$form_state) {
  extract($form_state['values']); // $token, $data, $options

  // Look for ID and Secret
  list($id, $secret, $token) = _fb_admin_parse_textfield($token);

  if ($secret) {
    // Save the secret in serialized data field.
    $data['secret'] = $secret;
    form_set_value($form['wrapper']['form']['data'], $data, $form_state);
  }
  if (!$token) {
    form_set_error('token', t('Could not parse token %token.', array('%token' => $form_state['values']['token'])));
    return;
  }

  if ($token) {
    try {
      $errors = array();
      $result = fb_graph_batch(array('me', 'app'), $token, array(), $errors);
      // Store result in $form_state for submit handler.
      $form_state['fb'] += $result;
      $form_state['fb']['token'] = $token; // trimmed
    }
    catch (exception $e) {
      $message = t('Access token %token is not valid.', array(
                     '%token' => $token,
                   ));
      fb_log_exception($e, $message);
      form_set_error('token', $message);
    }

    // If passed an app token, we will have app data, but not user data.
    // If no app data, that's a serious error.
    if (empty($result['app'])) {
      $message = t('Access token %token is not valid.', array(
                     '%token' => $token,
                   ));
      form_set_error('token', $message);
    }
  }

  if (!$token && !$user_token) {
    form_set_error('token', t('You have not given a token to add.'));
  }
}

function fb_admin_add_token_form_submit(&$form, &$form_state) {
  extract($form_state['fb']); // $app, $me, $token, $options
  $status = $options['status'];

  if ($token) {
    $status |= FB_STATUS_FLAG_VALID;
    $fba = $app['id']; // Application ID.

    if (empty($me)) {
      $record->status |= FB_STATUS_FLAG_APP; // It's an app token.
      $me = $app; // Use app ID for $fbu.
    }
    $fbu = $me['id']; // User ID.

    $data = empty($form_state['values']['data']) ? NULL : $form_state['values']['data'];

    $result = fb_token_save($token, array(
                              'fba' => $fba,
                              'fbu' => $fbu,
                              'status' => $status,
                            ));

    if ($result) {
      drupal_set_message(t('Saved !user\'s access token for !app.', array(
                             '!user' => l($me['name'], $me['link']),
                             '!app' => l($app['name'], $app['link']),
                           )));
    }
    else {
      drupal_set_message(t('Failed to save token!'), 'error');
    }
  }

  if (!empty($form_state['fb_user_token'])) {
    extract($form_state['fb_user_token']);
  }

  if (!empty($user_token)) {
    $result = fb_token_save($user_token, array(
                              'graph' => $graph,
                              'fba' => $graph['app']['id'],
                              'fbu' => $graph['me']['id'],
                              'status' => FB_STATUS_FLAG_VALID | FB_STATUS_FLAG_ADMIN,
                            ));

    if ($result) {
      drupal_set_message(t('Saved !user\'s access token for !app.', array(
                             '!user' => l($graph['me']['name'], $graph['me']['link']),
                             '!app' => l($graph['app']['name'], $graph['app']['link']),
                           )));
    }
    else {
      drupal_set_message(t('Failed to save token!'), 'error');
    }
  }
}

/**
 * TODO: make a filter similar to drupal's content admin page.
 */
function fb_admin_token_filter_form() {

}

function fb_admin_tokens_form() {
  // Build the sortable table header.
  $header = array(
    // Sort disabled, because only numerical sort possible, not alphabetical
    'fba' => array('data' => t('Application')),
    'fbu' => array('data' => t('Account')),
    'status' => array('data' => t('Status'), 'field' => 'fbt.status'),
    'token' => array('data' => t('Token'), 'style' => 'display:none;', 'class' => 'fb_access_token'),
    'changed' => array('data' => t('Updated'), 'field' => 'fbt.changed', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('fb_token', 'fbt')->extend('PagerDefault')->extend('TableSort');

  $query->leftjoin('cache_fb', 'fbc_fba', 'fbc_fba.cid = fbt.fba');
  $query->addField('fbc_fba', 'data', 'fba_data');
  $query->leftjoin('cache_fb', 'fbc_fbu', 'fbc_fbu.cid = fbt.fbu');
  $query->addField('fbc_fbu', 'data', 'fbu_data');

  $results = $query
    ->fields('fbt')
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  while ($tdata = $results->fetchObject()) {
    $options[$tdata->access_token] = array(
      // TODO: icon.
      // TODO: human readable name, link to fb.
      'fba' => $tdata->fba,
      'fbu' => $tdata->fbu,
      'status' => $tdata->status,
      'token' => array(
        'data' => $tdata->access_token,
        'class' => 'fb_access_token',
        'style' => 'display:none;',
      ),
      'changed' => format_date($tdata->changed, 'short'),
      'operations' => array(
        'data' => array(
          '#theme' => 'links',
          '#attributes' => array('class' => array('links', 'inline')),
          '#links' => array(
            'debug' => array(
              'title' => t('debug'),
              'href' => 'https://developers.facebook.com/tools/debug/access_token',
              'query' => array('q' => $tdata->access_token),
              'attributes' => array('target' => '_blank'),
            ),
            'access_token_info' => array(
              'title' => t('info'),
              'href' => 'https://graph.facebook.com/oauth/access_token_info',
              'query' => array(
                'access_token' => $tdata->access_token,
                'client_id' => $tdata->fba,
              ),
            ),
          ),
        ),
      ),
    );

    // Operations for App tokens only.
    if ($tdata->status & FB_STATUS_FLAG_APP) {

      $options[$tdata->access_token]['operations']['data']['#links']['settings'] = array(
        'title' => t('settings'),
        'href' => 'https://developers.facebook.com/apps/' . $tdata->fba,
      );


      $options[$tdata->access_token]['operations']['data']['#links']['explore'] = array(
        'title' => t('explore'),
        'href' => 'https://developers.facebook.com/tools/explorer/' . $tdata->fba,
        'query' => array(
          'method' => 'GET',
          'path' => 'me',
        ),
      );
    }

    // Use details from cache, if available.
    $fba_data = unserialize($tdata->fba_data);
    $fbu_data = unserialize($tdata->fbu_data);
    if (!empty($fba_data)) {
      $options[$tdata->access_token]['fba'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => '<img src="' . $fba_data['icon_url'] . '"/>&nbsp;' . $fba_data['name'],
          '#href' => $fba_data['link'],
          '#suffix' => '&nbsp;(' . $tdata->fba .')',
          '#options' => array('html' => TRUE),
        ),
      );
    }
    if (!empty($fbu_data)) {
      $options[$tdata->access_token]['fbu'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => fb_get_name($fbu_data),
          '#href' => fb_get_link($fbu_data),
          '#suffix' => '&nbsp;(' . $tdata->fbu .')',
        ),
      );
    }

    // Application tokens don't really have a user.
    if ($tdata->fba == $tdata->fbu) {
      $options[$tdata->access_token]['fbu'] = t('N/A');
    }
  }

  // Highlight current token.
  /*
  if (($user_token = fb_user_token()) &&
      !empty($options[$user_token])) {
    // TODO: find a better way to do this.
    $options[$user_token]['fbu'] .= '&nbsp;' . t('You!');
  }
  */

  $form['tokens'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No tokens found.'),
  );
  $form['toggle_tokens'] = array(
    '#type' => 'markup',
    '#markup' => '<a href=# onclick="jQuery(\'.fb_access_token\').toggle(); return false;">show/hide tokens</a>',
  );

  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

function fb_admin_tokens_form_validate() {
}
function fb_admin_tokens_form_submit() {
}

function fb_admin_applications_form($form, &$form_state) {
  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Name'), 'field' => 'fba.title', 'sort' => 'asc'),
    'fba' => array('data' => t('ID'), 'field' => 'fba.fba'), // Needed?
    'status' => array('data' => t('Status'), 'field' => 'fba.status'),
    'token' => array('data' => t('Token'), 'style' => 'display:none;', 'class' => 'fb_access_token'),
    'local' => t('Local Operations'),
    'remote' => array('data' => t('Facebook Operations')),
  );

  $query = db_select('fb_application', 'fba')->extend('PagerDefault')->extend('TableSort');

  $query->leftjoin('cache_fb', 'fbc_fba', 'fbc_fba.cid = fba.fba');
  $query->addField('fbc_fba', 'data', 'fba_data');

  $results = $query
    ->fields('fba')
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  $options = array();
  while ($adata = $results->fetchObject()) {
    $adata->data = unserialize($adata->sdata);
    $options[$adata->fba] = array(
      'fba' => $adata->fba,
      'title' => $adata->title,
      'status' => $adata->status,
      'token' => array(
        'data' => $adata->data['access_token'],
        'class' => 'fb_access_token',
        'style' => 'display:none;',
      ),
      'local' => array(
        'data' => array(
          '#theme' => 'links',
          '#attributes' => array('class' => array('links', 'inline')),
          '#links' => array(
            'edit' => array(
              'title' => t('edit'),
              'href' => FB_PATH_ADMIN_APPS . '/' . $adata->fba . '/edit',
            ),
          ),
        ),
      ),
      'remote' => array(
        'data' => array(
          '#theme' => 'links',
          '#attributes' => array('class' => array('links', 'inline')),
          '#links' => array(
            'settings' => array(
              'title' => t('settings'),
              'href' => 'https://developers.facebook.com/apps/' . $adata->fba . '/summary',
              'attributes' => array('target' => '_blank'),
            ),
            'explore' => array(
              'title' => t('explore'),
              'href' => 'https://developers.facebook.com/tools/explorer/' . $adata->fba,
              'attributes' => array('target' => '_blank'),
              'query' => array(
                'method' => 'GET',
                'path' => $adata->fba,
              ),
            ),
          ),
        ),
      ),
    );


    // Use details from cache, if available.
    $fba_data = unserialize($adata->fba_data);
    if (!empty($fba_data)) {
      $options[$adata->fba]['title'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => '<img src="' . $fba_data['icon_url'] . '"/>&nbsp;' . $fba_data['name'],
          '#href' => $fba_data['link'],
          '#options' => array('html' => TRUE),
        ),
      );

      if (!empty($fba_data['access_token'])) {
        $options[$adata->fba]['remote']['data']['#links']['debug'] = array(
          'title' => t('debug'),
          'href' => 'https://developers.facebook.com/tools/debug/access_token',
          'query' => array('q' => $adata->fba),
          'attributes' => array('target' => '_blank'),
        );

        // TODO: add remote settings link.
      }
    }

  }


  $form['applications'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No applications found.'),
  );
  $form['toggle_tokens'] = array(
    '#type' => 'markup',
    '#markup' => '<a href=# onclick="jQuery(\'.fb_access_token\').toggle(); return false;">show/hide tokens</a>',
  );

  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}
function fb_admin_applications_form_validate($form, &$form_state) {

}
function fb_admin_applications_form_submit($form, &$form_state) {

}

//// Add or edit an application.

/**
 * Builds the form used to edit an application.
 *
 * This form supports both create and edit.
 */
function fb_admin_application_edit_form($form, $form_state, $data = NULL) {
  if (is_numeric($data)) {
    $fba = $data;
  }
  elseif (!empty($data['id'])) {
    $fba = $data['id'];
  }
  else {
    $fba = 0;
  }

  if ($fba) {
    $fb_app = db_query("SELECT * FROM {fb_application} WHERE fba = :fba", array(
                         ':fba' => $fba,
                       ))->fetchAssoc();
  }
  if (empty($fb_app)) {
    $fb_app = array();
  }

  // Defaults for new app.
  $fb_app = $fb_app + array(
    'namespace' => NULL,
    'fba' => NULL,
    'status' => FB_STATUS_APP_ENABLED,
    'data' => NULL,
    'secret' => NULL,
    'title' => t('UNKNOWN'),
  );

  $form['#fb_app'] = $fb_app; // Similar to #node


  // ID, apikey and secret are shown on facebook.  User copies and pastes values.
  $form['fba'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook Application ID'),
    '#required' => TRUE,
    '#default_value' => $fb_app['fba'],
    '#description' => t('Copy and paste <em>App ID</em> (a.k.a. <em>API Key</em>) from <a href=!url target=_blank>Facebook</a>.', array(
                          '!url' => 'https://developers.facebook.com/apps',
                        )),
  );
  $form['secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret'),
    '#required' => FALSE,
    '#default_value' => $fb_app['secret'],
    '#description' => t('For locally hosted applications, copy and paste <em>App Secret</em> from Facebook.'),
  );

  // Placeholders, will be computed during validate.
  $form['data'] = array(
    '#tree' => TRUE,
  );
  $form['fb_app_data'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );
  foreach (array('namespace', 'title', 'status') as $key) {
    $form[$key] = array(
      '#type' => 'value',
      '#value' => $fb_app[$key],
    );
  }

  $default_app = variable_get(FB_VAR_DEFAULT_APP);
  $is_default = !$default_app || $default_app['client_id'] == $fb_app['fba'];
  $form['is_default'] = array(
    '#type' => 'value',
    '#value' => $is_default,
  );
  $form['make_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default application'),
    '#description' => t('If your website hosts only one Facebook Application, check this box.  If you host more than one, you may choose one primary application.'),
    '#default_value' => $is_default,
  );
  if (!$is_default) {
    $form['make_default']['#description'] .= '<br/>' . t('If checked, this application will replace %application as the default.', array(
      '%application' => $default_app['name'],
    ));
  }

  $form['states'] = array(
    '#tree' => TRUE,
  );
  $form['states']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => $fb_app['status'] | FB_STATUS_APP_ENABLED,
    '#return_value' => FB_STATUS_APP_ENABLED,
    '#description' => t('Uncheck if this server no longer hosts this application, but you prefer not to delete the settings.'),
  );
  // TODO: add other app states.

  $form['buttons'] = array();
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save application'),
    '#weight' => 20,
  );

  if ($fb_app['fba']) {
    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete application'),
      '#weight' => 21,
      '#submit' => array('fb_admin_form_delete_submit'),
    );
  }

  return $form;
}

/**
 * Gets app token from facebook.
 */
function fb_admin_get_app_token($fba, $secret) {
  $path = "https://graph.facebook.com/oauth/access_token?client_id=" . $fba . "&client_secret=" . $secret . "&grant_type=client_credentials";
  $http = drupal_http_request($path);
  if ($http->code == 200 && isset($http->data)) {
    $parse = explode('=', $http->data);
    $token = $parse[1];
    return $token;
  }
  else {
    drupal_set_message(t('Failed to get application token. %detail.', array(
                           '%detail' => $http->error,
                         )), 'error');
  }
}

/**
 * Form validation.
 *
 * TODO: this function may be calling fb_graph too often.  fb_token_save may be inefficient.
 */
function fb_admin_application_edit_form_validate($form, &$form_state) {
  $fb_app = $form_state['values'];

  if ($form_state['values']['op'] != t('Delete application')) {
    try {
      if (!empty($fb_app['secret'])) {
        $fb_app['data']['access_token'] = fb_admin_get_app_token($fb_app['fba'], $fb_app['secret']);
      }
      else {
        // Allow id and secret in one field.
        list($fba, $secret, $token) = _fb_admin_parse_textfield($fb_app['fba']);
        $fb_app['fba'] = $fba;
        $fb_app['secret'] = $secret;
        $fb_app['data']['access_token'] = $token;
      }

      // Change form values before submit.
      foreach (array('fba', 'secret', 'data') as $key) {
        form_set_value($form[$key], $fb_app[$key], $form_state);
      }

      if (!empty($fb_app['secret']) && empty($fb_app['data']['access_token'])) {
        form_set_error('secret', t('Unable to get an application token.  Are you sure your ID (%id) and Secret are correct?', array('%id' => $fb_app['fba'])));
      }
      else {
        $graph = fb_graph($fb_app['fba'], $fb_app['data']['access_token'], FB_CACHE_STORE) + array(
          // Defaults for values that Facebook might not return.
          'namespace' => NULL,
        );

        $form_state['fb']['graph'] = $graph;

        // Populate form with up-to-date values from facebook.
        $fb_app['data']['logo_url'] = $graph['logo_url'];
        form_set_value($form['data'], $fb_app['data'], $form_state);
        form_set_value($form['title'], $graph['name'], $form_state);
        form_set_value($form['namespace'], $graph['namespace'], $form_state);

        // If default checkbox selected, we need graph data during submit.
        form_set_value($form['fb_app_data'], array(
          'client_id' => $graph['id'],
          'name' => $graph['name'],
          'namespace' => $graph['namespace'],
        ), $form_state);
      }
    } catch (exception $e) {

      $msg = t('Failed to confirm application data with facebook. (%fba)', array('%fba' => $fb_app['fba']));
      fb_log_exception($e, $msg);
      form_set_error('fba', $msg);

      // TODO: figure out why this happens.
      if ($e->getCode() == 100) {
        drupal_set_message(t('Hint: Error code 100 may mean that an application has "social discovery" disabled.'), 'error');
      }
    }
  }
}

function fb_admin_application_edit_form_submit($form, &$form_state) {
  $fb_app = $form_state['values'];
  extract($form_state['fb']); // $graph
  try {
    if ($fb_app['data']['access_token']) {
      fb_token_save($fb_app['data']['access_token'], array(
                      'fba' => $fb_app['fba'],
                      'fbu' => $fb_app['fba'],
                      'status' => FB_STATUS_FLAG_VALID | FB_STATUS_FLAG_APP,
                    ));
    }

    fb_admin_app_save($fb_app);

    drupal_set_message(t('Saved facebook application setting for !app.', array(
                           '!app'=> l($graph['name'], $graph['link']),
                         )));

    /*
    // Should we just go ahead and flush the cache here automatically?
    drupal_set_message(t('Recommended: <a href=!cache_url>clear cached data</a> after changing Facebook app settings.', array(
                           '!cache_url'=> url('admin/config/development/performance'),
                         )), 'warning');
    */

    if ($form_state['values']['make_default']) {
      // Make this the default app.
      variable_set(FB_VAR_DEFAULT_APP, $form_state['values']['fb_app_data']);
    }
    elseif ($form_state['values']['is_default']) {
      // The app is no longer default.
      variable_del(FB_VAR_DEFAULT_APP);
    }

  }
  catch (Exception $e) {
    fb_log_exception($e, t('Failed to save application settings.'));
  }


  $form_state['redirect'] = FB_PATH_ADMIN_APPS;

}


/**
 * Save application to fb_application table.
 */
function fb_admin_app_save($fb_app) {
  $fb_app['sdata'] = serialize($fb_app['data']); // Serialize for database.
  $result = db_merge('fb_application')
    ->key(array('fba' => $fb_app['fba']))
    ->fields(array(
               'fba' => $fb_app['fba'],
               'secret' => $fb_app['secret'],
               'namespace' => $fb_app['namespace'],
               'title' => $fb_app['title'],
               'status' => $fb_app['status'],
               'sdata' => empty($fb_app['data']) ? NULL : serialize($fb_app['data']),
             ))
    ->execute();

  if ($result) {
    return $fb_app;
  }
}



/**
 * Button submit function.  Use has clicked delete, send them to confirm page.
 */
function fb_admin_form_delete_submit($form, &$form_state) {
  $destination = '';
  if (isset($_REQUEST['destination'])) {
    $destination = drupal_get_destination();
    unset($_REQUEST['destination']);
  }
  $fb_app = $form['#fb_app'];
  $form_state['redirect'] = array(FB_PATH_ADMIN_APPS . '/' . $fb_app['fba'] . '/delete', array('query' => $destination));
}


/**
 * Form creator -- ask for confirmation of deletion
 */
function fb_admin_application_delete_form($form, &$form_state, $fb_app) {
  $form['fb_admin_description'] = array(
    '#markup' => t('This will delete all local settings and access tokens associated with the facebook application %title.  The application will <strong>not</strong> be deleted from facebook.com.', array(
                     '%title' => fb_get_name($fb_app),
                   )),
    '#prefix' => '<p>', '#suffix' => '</p>',
  );

  // This is $fb_app from fb_graph() call, not from local database.  It has 'id' field, not 'fba'.
  $form['#fb_app'] = $fb_app;

  return confirm_form($form,
                      t('Are you sure you want to delete %title?', array('%title' => fb_get_name($fb_app))),
                      isset($_GET['destination']) ? $_GET['destination'] : FB_PATH_ADMIN_APPS . '/' . $fb_app['id'] . '/edit',
                      t('This action cannot be undone.'),
                      t('Delete'),
                      t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function fb_admin_application_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $fb_app = $form['#fb_app'];

    // @TODO: invoke hooks so that third-party modules may act.

    db_delete('fb_token')
      ->condition('fba', $fb_app['id'])
      ->execute();
    db_delete('fb_application')
      ->condition('fba', $fb_app['id'])
      ->execute();
  }

  $form_state['redirect'] = FB_PATH_ADMIN_APPS;
}






/**
 * Drupal form callback for general settings.
 */
function fb_admin_settings_form($form, &$form_state) {
  $form[FB_VAR_PREFER_LONG_TOKEN] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Long-lived Tokens'),
    '#description' => t('When storing access tokens, get longer-lived tokens when possible.  Normal tokens expire in a few hours, or when users log out of facebook.  Longer-lived tokens may last months or in some cases never expire.'),
    '#default_value' => variable_get(FB_VAR_PREFER_LONG_TOKEN, TRUE),
  );


  // Javascript settings
  $form[FB_VAR_ADD_JS] = array(
    '#type' => 'checkbox',
    '#title' => t('Javascript SDK'),
    '#description' => t('Initialize <a href=!fb_js_url target=_blank>facebook\'s javascript</a>.  This enables <a href=!fb_sp_url target=_blank>social plugins</a>, for example a <a href=!fb_lb_url target=_blank>like button</a> by simply adding <em>&lt;fb:like&gt;&lt/fb:like&gt;</em> markup.', array(
                          '!fb_js_url' => 'http://developers.facebook.com/docs/reference/javascript/',
                          '!fb_sp_url' => 'http://developers.facebook.com/docs/plugins/',
                          '!fb_lb_url' => 'http://developers.facebook.com/docs/reference/plugins/like/',
                        )),
    '#default_value' => variable_get(FB_VAR_ADD_JS, FALSE),
  );

  $form[FB_VAR_USE_JSON_BIGINT] = array(
    '#type' => 'checkbox',
    '#title' => t('Use native JSON bigint decoding'),
    '#description' => t('Use the JSON_BIGINT_AS_STRING flag when calling json_decode().  If the native bigint handling is not implemented on your system, uncheck this.'),
    '#default_value' => variable_get(FB_VAR_USE_JSON_BIGINT, defined('JSON_BIGINT_AS_STRING')),
    '#access' => defined('JSON_BIGINT_AS_STRING'),
  );

  return system_settings_form($form);
}

function fb_admin_settings_form_validate(&$form, &$form_state) {
  $values = $form_state['values'];
  if ($values[FB_VAR_USE_JSON_BIGINT]) {
    $test = json_decode('[1000000000000]', TRUE, 512, JSON_BIGINT_AS_STRING);
    if (!is_string($test[0])) {
      form_error($form[FB_VAR_USE_JSON_BIGINT], t('Your system does not appear to support JSON_BIGINT_AS_STRING!'));
    }
  }
}

/**
 * Form callback.
 */
function fb_admin_default_app_form($form, &$form_state) {
  drupal_set_title(t('Site-Wide Application'));
  $markup = array(
    '#type' => 'markup',
    '#prefix' => '<p>', '#suffix' => '</p>',
  );
  $form['description'][] = array(
    '#markup' => t('A <em>Facebook application</em> enables advanced Facebook integration features.'),
  ) + $markup;
  $form['description'][] = array(
    '#markup' => t('This form selects a default for this website.  Most websites need only a single application and should select it here.'),
  ) + $markup;

  return fb_admin_app_select_form($form, $form_state, array(
    'variable' => FB_VAR_DEFAULT_APP,
  ));
}

/**
 * Helper builds an array of known apps, for use in forms.
 */
function fb_admin_all_apps() {
  $all_apps = array();

  // Include apps from the fb_app table.
  $result = db_query("SELECT * FROM {fb_application} WHERE status & :is_enabled", array(
              ':is_enabled' => FB_STATUS_APP_ENABLED,
            ));
  while ($row = $result->fetchAssoc()) {
    $row['data'] = unserialize($row['sdata']);
    $all_apps[$row['fba']] = $row;
  }

  // Include apps from the fb_token table.
  $result = db_query("SELECT * FROM {fb_token} WHERE status & :is_app AND status & :is_valid", array(
              ':is_app' => FB_STATUS_FLAG_APP,
              ':is_valid' => FB_STATUS_FLAG_VALID,
            ));
  while ($row = $result->fetchAssoc()) {
    if (empty($all_apps[$row['fba']])) {
      $all_apps[$row['fba']] = $row;
    }
    else {
      $all_apps[$row['fba']] = $all_apps[$row['fba']] + $row; // Add access_token to array.
    }
  }
  return $all_apps;
}

// @todo replace this with a proper drupal element with #process callback.
function fb_admin_app_select_form($form, &$form_state, $params = array()) {
  // Default settings.
  $params = $params + array(
    'variable' => FB_VAR_DEFAULT_APP,
  );
  $form_state['fb']['params'] = $params;

  // Present administrator with a choice of applications.
  $all_apps = fb_admin_all_apps();

  // Include previously saved values.
  $current_app = variable_get($params['variable'], array('client_id' => ''));
  if ($current_app['client_id'] && empty($all_apps[$current_app['client_id']])) {
    $current_app['fba'] = $current_app['client_id'];
    $all_apps[$current_app['client_id']] = $current_app;
  }

  // batch API fails without token.
  //$graph = fb_graph_batch($fbas, FB_TOKEN_NONE);
  // Less efficient to call fb_graph for each app, but works with our without access token.
  foreach ($all_apps as $fba => $app) {
    $graph[$fba] = fb_graph($fba,
                   !empty($app['access_token']) ? $app['access_token'] : NULL);
  }

  if (!empty($graph)) {
    foreach ($graph as $fba_data) {
      $options[$fba_data['id']] = l('<img src=' . $fba_data['icon_url'] . '></img>&nbsp;' . $fba_data['name'], $fba_data['link'], array('html' => TRUE, 'attributes' => array('target' => '_blank')));
    }
  }

  $options[0] = t('None');

  $form['application'] = array(
    '#type' => 'fieldset',
    '#title' => t('Application'),
  );

  // Remember data for validate and submit handlers.
  $form_state['fb']['graph'] = !empty($graph) ? $graph : NULL;
  $form_state['fb']['all_apps'] = $all_apps;

  // Placeholder
  $form['fb_app_data'] = array(
    '#type' => 'value',
    '#value' => NULL,
  );

  if (in_array($current_app['client_id'], array_keys($options))) {
    $default_select = $current_app['client_id'];
    $default_text = NULL;
  }
  else {
    $default_select = 0;
    $default_text = $current_app['client_id'];
  }

  if (count($options) > 1) {
    $form['application']['client_id'] = array(
      '#type' => 'radios',
      '#title' => t('Application'),
      '#options' => $options,
      '#default_value' => $default_select,
      //'#required' => TRUE,
      '#description' => t('If your desired application is not shown, use the <a href=!add_app_url>add application form</a>.', array(
                        '!add_app_url' => url(FB_PATH_ADMIN_APPS . '/add'),
                      )),
    );


    $form['#validate'] = array('fb_admin_app_select_form_validate');
    $form['#submit'] = array('fb_admin_app_select_form_submit');

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Select application'),
    );

  }
  else {
    $form['application']['new'] = array(
      '#markup' => t('Use the <a href=!add_app_url>add application form</a> to configure an application.', array(
                   '!add_app_url' => url(FB_PATH_ADMIN_APPS . '/add'),
                 )),
    );
  }


  return $form;
}

function fb_admin_app_select_form_validate($form, &$form_state) {
  //dpm(func_get_args(), __FUNCTION__);
  extract($form_state['fb']); // $graph, $all_apps
  $values = $form_state['values'];

  if (!empty($values['client_id']) &&
      !empty($values['client_id_new'])) {
    form_error($form['application']['client_id_new'], t('Select either a local application radio button or paste a remote application ID.  Not both.'));
    return;
  }

  // Handle a new ID pasted in.
  $token = NULL;
  if (!empty($values['client_id_new'])) {
    // Probably contains ID, but parse just in case.
    list($id, $secret, $token) = _fb_admin_parse_textfield($token);
    $client_id = $id;
    $key = 'client_id_new'; // to pass into form_set_error.

    // TODO: save secret if we just learned one.
    if ($secret) {
      form_set_error($key, t('Use add application form to paste ID and secret.'));
      return;
    }
  }
  else {
    $client_id = $values['client_id'];
    $key = 'client_id';
  }

  // Make sure we have accurate information about the app.
  if (empty($graph[$client_id])) {
    try {
      $gdata = fb_graph($client_id, $token);
    }
    catch (exception $e) {
      fb_log_exception($e, t('Failed to look up application %id.', array('%id' => $client_id)));
      $gdata = NULL;
    }
  }
  else {
    $gdata = $graph[$client_id];
  }

  // TODO: FQL look up application table to confirm application exists.  For now, using namespace.
  if (empty($gdata) || empty($gdata['id'])) {
    form_set_error($key, t('Failed to look up application id %id.', array('%id' => $client_id)));
  }
  else {
    // Validation passed.  Build array to store in variable.

    // Save everything we need about this app.
    // Don't save tokens or secrets here.
    form_set_value($form['fb_app_data'], array(
      'client_id' => $gdata['id'],
      'name' => $gdata['name'],
      'namespace' => !empty($gdata['namespace']) ? $gdata['namespace'] : NULL,
    ), $form_state);
  }
}

function fb_admin_app_select_form_submit($form, &$form_state) {
  $app_data = $form_state['values']['fb_app_data'];
  extract($form_state['fb']); // $params

  if ($app_data['client_id']) {
    variable_set($params['variable'], $app_data);
  }
  else {
    variable_del($params['variable']);
  }

  drupal_set_message(t('Recommended: <a href=!cache_url>clear all caches</a> after changing Facebook app settings.', array(
                         '!cache_url'=> url('admin/config/development/performance'),
                       )), 'warning');

  drupal_set_message(t('The configuration options have been saved.'));
}



/**
 * Drupal form element process callback.
 */
function fb_admin_token_select_process(&$element, &$form_state, &$form) {

  // defaults
  $element = $element + array(
    '#default_value' => -1,
  );

  $element['#tree'] = TRUE;
  $all_tokens = array();
  $i = 0;
  $option_default = -1;

  // Query all admin tokens to figure out our options.
  $result = db_query("SELECT * FROM {fb_token} WHERE access_token = :current_token OR (status & :valid_flag AND status & :admin_flag) ORDER BY changed DESC", array(
              ':current_token' => $element['#default_value'],
              ':valid_flag' => FB_STATUS_FLAG_VALID,
              ':admin_flag' => FB_STATUS_FLAG_ADMIN,
            ));

  // Loop over the saved tokens.
  while ($token_data = $result->fetchAssoc()) {
    $i++;
    $all_tokens[$i] = $token_data;

    // In most cases, this call to fb_graph() hits the cache, not facebook.
    try {
      $fba_data = fb_graph($token_data['fba']);
      $fbu_data = fb_graph($token_data['fbu']);
    }
    catch (exception $e) {
      fb_log_exception($e, t('Failed to get token data.'));
    }

    if ($token_data['status'] & FB_STATUS_FLAG_ADMIN &&
      !($token_data['status'] & FB_STATUS_FLAG_APP)) {
      $options[$i] = t('%name via %app', array(
                       '%name' => fb_get_name($fbu_data),
                       '%app' => fb_get_name($fba_data),
                       '%time_ago' => format_interval(REQUEST_TIME - $token_data['changed']),
                     ));

      if (!($token_data['status'] & FB_STATUS_FLAG_VALID)) {
        $options[$i] .= '&nbsp;' . t('(This token has expired!)');
      }
      elseif ($token_data['access_token'] == $element['#default_value']) {
        $option_default = $i;
      }
    }
  } // End of admin token loop.


  // Pass values to validate and submit hooks.
  $form_state['fb']['all_tokens'] = $all_tokens;

  if (!empty($options)) {

    $options[-1] = t('Do not use a saved token');

    // If a new token was generated from a code, the user probably intends to submit that new token rather than the current saved one, so change default.
    if (!empty($_REQUEST['code'])) {
      $option_default = -1;
    }

    // Note our options array uses index rather than token, to avoid embedding tokens in the form markup.
    $element['fb_token_index'] = array(
      '#type' => 'radios',
      //'#title' => t('Current and previous tokens'),
      '#options' => $options,
      '#default_value' => $option_default,
    );
  }


  return $element;
}

function fb_admin_token_select_validate($element, &$form_state) {
  $token = NULL;
  extract($form_state['fb']); // $all_tokens
  if (!empty($element['#value'])) {
    extract($element['#value']); // $fb_token_index
  }
  else {
    $fb_token_index = -1;
  }

  if ($fb_token_index > 0) {
    $token = $all_tokens[$fb_token_index]['access_token'];

    // Confirm that the token works.
    try {
      $graph = fb_graph_batch(array('me', 'app'), $token);
      $form_state['fb_graph'] = $graph;

      // @todo confirm selected token has required permissions, if any.

    }
    catch (Exception $e) {
      form_error($element['fb_token_index'], t('Could not validate token.  %detail', array(
            '%detail' => $e->getMessage(),
          )));
      $token = NULL;
    }
  }

  // Simplify form value.
  form_set_value($element, $token, $form_state);
}



function fb_admin_token_generate_process(&$element, &$form_state, &$form) {
  // Defaults
  $element = $element + array(
    '#scope' => array(),
    '#options' => array(),
  );
  $element['#tree'] = TRUE;
  // Disable core validation, because options are not regenerated during submit.  Avoids "illegal choice".
  unset($element['#needs_validation']);

  // Known apps can be used to generate new tokens.
  $all_apps = fb_admin_all_apps();

  $scope = implode(',', $element['#scope']);

  $new_token_options = $element['#options'] + array(-1 => t('Do not save new token'));

  // TODO: keep track or learn which apps can support local auth.
  // or now we assume secret or app token means local.

  foreach ($all_apps as $app_data) {
    if (!empty($app_data['secret'])) {
      // App supports server-side auth.
      $auth_url = fb_server_auth_url(array(
                    'fba' => $app_data['fba'],
                    'scope' => $scope,
                  ));
    }
    elseif (!empty($app_data['data']['access_token'])) {
      // App supports client-side auth.
      $auth_url = fb_client_auth_url(array(
                    'fba' => $app_data['fba'],
                    'scope' => $scope,
                  ));
    }
    else {
      // Assume remote auth works.
      // @todo find some way to know whether remote auth will actually work.
      $auth_url = fb_remote_auth_url($app_data + array(
                    'scope' => $scope,
                  ));
    }

    $generate_links[] = array(
      'title' => t('Generate new token via %application', array(
                 '%application' => $app_data['title'],
               )),
      'href' => $auth_url,
      'html' => TRUE,
    );



    // Detect if the user has generated a new token for the current app.
    // Skip when form is submitted.
    if (empty($_POST) && $token = fb_auth_get_token($app_data)) {

      if (empty($new_token_options[$token])) {
        try {
          // TODO: consolodate graph api, use batch.
          $me = fb_graph('me', $token);
          $app = fb_graph('app', $token);
          $new_token_options[$token] = t('%user_name via the %app_name application', array(
                                         '%user_name' => fb_get_name($me),
                                         '%app_name' => fb_get_name($app),
                                       ));
          drupal_set_message(t('Generated a new access token, but not yet saved.  Remember to press the submit button.'), 'warning');
          $default_token = $token;

        }
        catch (exception $e) {
          fb_log_exception($e, t('Failed to save new access token for %application.', array(
                '%application' => $app['title'],
              )), $token);
          drupal_set_message(t('Failed to validate new token.  You may <a target=_blank href=!token_debug_url>debug the token on Facebook</a>.', array(
                '!token_debug_url' => 'https://developers.facebook.com/tools/debug/access_token?q=' . $token)), 'error');
        }

        try {
          // Only tokens with manage_pages will successfully query me/accounts.
          $accounts = fb_graph('me/accounts', $token);
          // TODO: support pagination if not all accounts returned.
          foreach ($accounts['data'] as $account_data) {
            if (!empty($account_data['access_token'])) {
              $new_token_options[$account_data['access_token']] = t('%account_name (%account_type) via %app_name', array(
                                                                    '%account_name' => fb_get_name($account_data),
                                                                    '%account_type' => $account_data['category'],
                                                                    '%app_name' => !empty($app) ? fb_get_name($app) : t('(could not determine application)'),
                                                                  ));
            }
          }
        }
        catch (Exception $e) {
          // Probably we just don't have permission to get accounts.
        }
      }
    }
  } // End application loop.

  if (count($new_token_options) > 1) {
    $element['fb_admin_token_generate_new'] = array(
      '#type' => 'radios',
      //'#title' => t('Select Access Token'),
      '#options' => $new_token_options,
      '#default_value' => $default_token,
      //'#element_validate' => array('fb_admin_token_generate_validate'), // works???
    );

  }

  if (!empty($generate_links)) {
    $element['fb_admin_token_generate_links'] = array(
      '#theme' => 'links',
      '#links' => $generate_links,
    );
  }


  return $element;
}

function fb_admin_token_generate_after_build(&$element, &$form_state) {
  return $element;
}

function fb_admin_token_generate_validate($element, &$form_state) {
  $token = NULL;

  if (!empty($element['#value']['fb_admin_token_generate_new'])) {
    // Validate newly generated token.
    $token = $element['#value']['fb_admin_token_generate_new'];
    try {
      $graph = fb_graph_batch(array('me', 'app'), $token);
      $form_state['fb_graph'] = $graph;
      extract($graph); // $me, $app.

      // If there's a new generated token, the URL has params (i.e. 'code' etc) that need to be removed before form is rendered again.
      if (empty($form_state['redirect'])) {
        $form_state['redirect'] = current_path();
      }
    }
    catch (Exception $e) {
      form_error($element, t('Could not validate new token.  %detail', array(
            '%detail' => $e->getMessage(),
          )));
      $token = NULL;
    }
  }

  // Simplify values for submit handlers.
  form_set_value($element, $token, $form_state);
}


function fb_admin_long_lived_token($token, $app_id = NULL) {
  // @todo determine app id if not passed in.
  // @todo be smart about page tokens, which cannot be converted.

  // Convert short-lived token to long-lived.
  $all_apps = fb_admin_all_apps();

  $app = $all_apps[$app_id];

  if ($app && $app['secret']) {
    try {
      $result = fb_graph('oauth/access_token', array(
                  'client_id' => $app['fba'],
                  'client_secret' => $app['secret'],
                  'grant_type' => 'fb_exchange_token',
                  'fb_exchange_token' => $token,
                ), FALSE);
      if (!empty($result['access_token'])) {
        drupal_set_message(t('Using longer-lived token which is set to expire in %duration.', array(
              '%token' => $result['access_token'],
              '%duration' => !empty($result['expires']) ? format_interval($result['expires']) : t('(could not determine expiration)'),
            )));
        return $result['access_token'];
      }
    }
    catch (Exception $e) {
      // This is reached whenever token belongs to an account instead of a user.  So we don't need to be verbose about it.
      drupal_set_message(t('Could not convert the token into a longer-lived token.  This is expected when token belongs to a page rather than a user.'));
    }
  }
}
