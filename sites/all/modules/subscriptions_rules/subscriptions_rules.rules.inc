<?php

/**
 * @file rules integration for the subscriptions_rules module
 */

/**
 * Implements hook_rules_action_info() on behalf of the node module.
 */
function subscriptions_rules_rules_action_info() {
  $base = array(
    'type' => 'subscriptions',
    'parameter' => array(
      'node' => array('type' => 'node', 'label' => t('Content')),
      'user' => array('type' => 'user', 'label' => t('User')),
      'type' => array(
        'type' => 'text',
        'label' => t('Subscription type'),
        'optional' => TRUE,
        'options list' => 'subscriptions_rules_rules_type_list',
        'default value' => 'node',
      ),
      'send_interval' => array(
        'type' => 'textfield',
        'label' => t('Send Interval'),
        'optional' => TRUE,
        'description' => t('Leave blank to use user\'s setting. This will be in calculated in seconds.'),
      ),
    ),
    'group' => t('Subscriptions'),
    'access callback' => 'subscriptions_rules_add_user_content_access',
  );

  $type_base = array(
    'type' => 'subscriptions',
    'parameter' => array(
      'type' => array(
        'type' => 'list<text>',
        'label' => t('Content types'),
        'options list' => 'node_type_get_names',
        'description' => t('The content type(s) to check for.'),
        'restriction' => 'input',
      ),
      'user' => array('type' => 'user', 'label' => t('User')),
      'send_interval' => array(
        'type' => 'textfield',
        'label' => t('Send Interval'),
        'optional' => TRUE,
        'description' => t('Leave blank to use user\'s setting. This will be in calculated in seconds.'),
      ),
    ),
    'group' => t('Subscriptions'),
    'access callback' => 'subscriptions_rules_add_user_content_access',
  );

  $term_base = array(
    'type' => 'subscriptions',
    'parameter' => array(
      'term' => array('type' => 'taxonomy_term', 'label' => t('Taxonomy term')),
      'user' => array('type' => 'user', 'label' => t('User')),
      'send_interval' => array(
        'type' => 'textfield',
        'label' => t('Send Interval'),
        'optional' => TRUE,
        'description' => t('Leave blank to use user\'s setting. This will be in calculated in seconds.'),
      ),
    ),
    'group' => t('Subscriptions'),
    'access callback' => 'subscriptions_rules_add_user_content_access',
  );

  // Action subsribe a user to a content
  $items['subscriptions_rules_add_user_content'] = array(
    'label' => t('Subscribe user to node'),
    'base' => 'subscriptions_rules_add_user_content_callback',
  ) + $base;

  // Action unsubsribe a user to a content
  $items['subscriptions_rules_remove_user_content'] = array(
    'label' => t('Unsubscribe user from node'),
    'base' => 'subscriptions_rules_remove_user_content_callback',
  ) + $base;

  // Action subsribe a user to a content type
  $items['subscriptions_rules_add_user_content_type'] = array(
    'label' => t('Subscribe user to node type'),
    'base' => 'subscriptions_rules_add_user_content_type_callback',
  ) + $type_base;

  // Action unsubsribe a user to a content type
  $items['subscriptions_rules_remove_user_content_type'] = array(
    'label' => t('Unsubscribe user from node type'),
    'base' => 'subscriptions_rules_remove_user_content_type_callback',
  ) + $type_base;

  // Action subscribe a user to a taxonomy term
  $items['subscriptions_rules_add_user_term'] = array(
    'label' => t('Subscribe user to taxonomy term'),
    'base' => 'subscriptions_rules_add_user_term_callback',
  ) + $term_base;

  // Action unsubscribe a user to a taxonomy
  $items['subscriptions_rules_remove_user_term'] = array(
    'label' => t('Unsubscribe user from taxonomy term'),
    'base' => 'subscriptions_rules_remove_user_term_callback',
  ) + $term_base;

  $items['subscriptions_rules_stop_sending'] = array(
    'label' => t('Stop email sending'),
    'base' => 'subscriptions_rules_stop_sending_callback',
    'parameter' => array(
      'clear' => array(
        'type' => 'boolean',
        'label' => t('Clear existed mail queue'),
        'optional' => TRUE,
        'description' => t('Clear existed mail queue before stopping email sending'),
      ),
    ),
    'access callback' => 'subscriptions_rules_stop_sending_callback_access',
  ) + $base;

  $items['subscriptions_rules_start_sending'] = array(
    'label' => t('Start email sending'),
    'base' => 'subscriptions_rules_start_sending_callback',
    'parameter' => array(), // TODO: Add checkbox to clear existed queue.
    'access callback' => 'subscriptions_rules_stop_sending_callback_access',
  ) + $base;

  $items['subscriptions_rules_subscription_suspend'] = array(
    'label' => t('Suspend subscription'),
    'base' => 'subscriptions_rules_subscription_suspend_callback',
    'parameter' => array(
      'user' => array('type' => 'user', 'label' => t('User')),
    ),
    'access callback' => 'subscriptions_rules_stop_sending_callback_access',
  ) + $base;

  $items['subscriptions_rules_subscription_resume'] = array(
    'label' => t('Resume subscription'),
    'base' => 'subscriptions_rules_subscription_resume_callback',
    'parameter' => array(
      'user' => array('type' => 'user', 'label' => t('User')),
    ),
    'access callback' => 'subscriptions_rules_stop_sending_callback_access',
  ) + $base;

  return $items;
}

/**
 * Rule subscriptions_add_user_content callback
 */
function subscriptions_rules_add_user_content_callback($node, $account, $type = 'node', $send_interval = "") {
  global $user;
  if (!isset($account->uid)) {
    $account = $user;
  }
  $args = array (
    'node',
    'nid',
    $node->nid,
    -1,
    $account->uid,
    is_numeric($send_interval) ? $send_interval : _subscriptions_get_setting('send_interval', $account),
    $type == 'node',
    $type == 'comment',
  );

  if (empty($account->uid)) {
    return subscriptions_rules_anon_write($account, $args);
  }

  return subscriptions_rules_add_user_content_normal_user_callback($args);
}

function subscriptions_rules_add_user_content_normal_user_callback($args) {
  // Get existed
  $sid = subscriptions_get_subscription($args[4], $args[0], $args[1], $args[2]);
  if ($sid) {
    // Add more value to existed subscription_content row
    $update = db_update('subscriptions')
    ->condition('module', $args[0])
    ->condition('field', $args[1])
    ->condition('value', $args[2])
    ->condition('author_uid', $args[3])
    ->condition('recipient_uid', $args[4]);

    if ($args[6]) {
      $update->fields(array(
        'send_updates' => 1,
      ));
    }

    if ($args[7]) {
      $update->fields(array(
        'send_comments' => 1,
      ));
    }

    $ret = $update->execute();
  }
  else {
    $ret = call_user_func_array('subscriptions_write_subscription', $args);
  }

  return $ret;
}

/**
 * Rule subscriptions_remove_user_content callback
 */
function subscriptions_rules_remove_user_content_callback($node, $account, $type = 'node') {
  global $user;
  if (!isset($account->uid)) {
    $account = $user;
  }
  $args = array (
    'node',
    'nid',
    $node->nid,
    -1,
    $account->uid,
    _subscriptions_get_setting('send_interval', $account),
    $type == 'node',
    $type == 'comment'
  );

  if (empty($account->uid)) {
    return subscriptions_rules_anon_write($account, $args, TRUE);
  }

  // Delete single rows
  $delete = db_delete('subscriptions')
    ->condition('module', $args[0])
    ->condition('field', $args[1])
    ->condition('value', $args[2])
    ->condition('author_uid', $args[3])
    ->condition('recipient_uid', $args[4]);

  // Update row that has both comments and update
  $update = db_update('subscriptions')
    ->condition('module', $args[0])
    ->condition('field', $args[1])
    ->condition('value', $args[2])
    ->condition('author_uid', $args[3])
    ->condition('recipient_uid', $args[4]);

  // Remove node rows
  if ($args[6]) {
    $delete->condition('send_updates', 1);
    $delete->condition('send_comments', 0);
    $update->fields(array(
      'send_updates' => 0
    ));
  }

  // Remove comments rows
  if ($args[7]) {
    $delete->condition('send_updates', 0);
    $delete->condition('send_comments', 1);
    $update->fields(array(
      'send_comments' => 0,
    ));
  }

  $ret = $delete->execute() || $update->execute();

  return $ret;
}

/**
 * TODO: user_access
 */
function subscriptions_rules_add_user_content_access() {
  return TRUE;
}

/**
 * Options list callback for the operation parameter of condition user has role.
 */
function subscriptions_rules_rules_type_list() {
  return array(
    'node' => t('Node'),
    'comment' => t('Comment'),
  );
}

/**
 * Options list callback for the operation parameter of condition user has role.
 */
function subscriptions_rules_rules_type_type_list() {
  return array(
    'node' => t('wiki'),
    'comment' => t('Comment'),
  );
}

/**
 * Rule subscriptions_rules_add_user_content_type callback
 */
function subscriptions_rules_add_user_content_type_callback($node_types, $account, $send_interval = "") {
  global $user;
  if (!isset($account->uid)) {
    $account = $user;
  }
  $ret = TRUE;
  // Load existed subscriptions
  foreach ($node_types as $type) {
    $sid = subscriptions_get_subscription($account->uid, 'node', 'type', $type);
    $args = array (
      0 => 'node',
      1 => 'type',
      2 => $type,
      3 => -1,
      4 => $account->uid,
      5 => is_numeric($send_interval) ? $send_interval : _subscriptions_get_setting('send_interval', $account),
      6 => TRUE,
      7 => TRUE,
    );

    // Write to subscriptions table
    if ($sid) {
      // Add more value to existed subscription_content row
      $update = db_update('subscriptions')
      ->condition('module', $args[0])
      ->condition('field', $args[1])
      ->condition('value', $args[2])
      ->condition('author_uid', $args[3])
      ->condition('recipient_uid', $args[4]);

      if ($args[6]) {
        $update->fields(array(
          'send_updates' => 1,
        ));
      }

      if ($args[7]) {
        $update->fields(array(
          'send_comments' => 1,
        ));
      }

      $ret = $update->execute() && $ret;
    }
    else {
      $ret = call_user_func_array('subscriptions_write_subscription', $args) && $ret;
    }
  }

  return $ret;
}

/**
 * Rule subscriptions_rules_remove_user_content_type callback
 */
function subscriptions_rules_remove_user_content_type_callback($node_types, $account, $send_interval = "") {
  global $user;
  if (!isset($account)) {
    $account = $user;
  }
  $ret = TRUE;
  foreach ($node_types as $type) {
    $args = array (
      0 => 'node',
      1 => 'type',
      2 => $type,
      3 => -1,
      4 => $account->uid,
      5 => is_numeric($send_interval) ? $send_interval : _subscriptions_get_setting('send_interval', $account),
      6 => TRUE,
      7 => TRUE,
    );

    $delete = db_delete('subscriptions')
      ->condition('module', $args[0])
      ->condition('field', $args[1])
      ->condition('value', $args[2])
      ->condition('author_uid', $args[3])
      ->condition('recipient_uid', $args[4]);

    $ret = $delete->execute() && $ret;
  }

  return $ret;
}

/**
 * Rule subscriptions_add_user_term callback
 */
function subscriptions_rules_add_user_term_callback($term, $user, $type = 'node', $send_interval = "") {
  // Get existing
  $sid = subscriptions_get_subscription($user->uid, 'node', 'tid', $term->tid);
  $args = array (
    0 => 'node',
    1 => 'tid',
    2 => $term->tid,
    3 => -1,
    4 => $user->uid,
    5 => is_numeric($send_interval) ? $send_interval : _subscriptions_get_setting('send_interval', $user),
    6 => TRUE,
    7 => TRUE,
  );

  if ($sid) {
    // Add more value to existed subscription_content row
    $update = db_update('subscriptions')
    ->condition('module', $args[0])
    ->condition('field', $args[1])
    ->condition('value', $args[2])
    ->condition('author_uid', $args[3])
    ->condition('recipient_uid', $args[4]);

    if ($args[6]) {
      $update->fields(array(
        'send_updates' => 1,
      ));
    }

    if ($args[7]) {
      $update->fields(array(
        'send_comments' => 1,
      ));
    }

    $ret = $update->execute();
  }
  else {
    $ret = call_user_func_array('subscriptions_write_subscription', $args);
  }

  return $ret;
}

/**
 * Rule subscriptions_remove_user_term callback
 */
function subscriptions_rules_remove_user_term_callback($term, $user, $type = 'node') {
  $args = array (
    0 => 'node',
    1 => 'tid',
    2 => $term->tid,
    3 => -1,
    4 => $user->uid,
    5 => _subscriptions_get_setting('send_interval', $user),
    6 => TRUE,
    7 => TRUE,
  );

  // Delete single rows
  $delete = db_delete('subscriptions')
    ->condition('module', $args[0])
    ->condition('field', $args[1])
    ->condition('value', $args[2])
    ->condition('author_uid', $args[3])
    ->condition('recipient_uid', $args[4]);

  $ret = $delete->execute();

  return $ret;
}

function subscriptions_rules_stop_sending_callback_access() {
  // TODO: access callback
  return TRUE;
}

function subscriptions_rules_stop_sending_callback($clear) {
  $ret = TRUE;
  $existed = variable_get('subscriptions_cron_percent', 50);
  if ($existed != 0) {
    variable_set('subscriptions_cron_percent_backup', $existed);
  }
  variable_set('subscriptions_cron_percent', 0);

  // Clear the queue
  if ($clear) {
    subscriptions_purge_queue();
    drupal_get_messages('status'); // TODO: Only the message created in  subscriptions_purge_queue
  }
  return $ret;
}

function subscriptions_rules_start_sending_callback() {
  $ret = TRUE;
  $existed = variable_get('subscriptions_cron_percent_backup', 0);
  if ($existed != 0) {
    variable_set('subscriptions_cron_percent', $existed);
  }
  variable_del('subscriptions_cron_percent_backup');
  return $ret;
}

function subscriptions_rules_subscription_suspend_callback($account) {
  db_update('subscriptions_user')
      ->fields(array(
          'suspended' => 1,
        ))
      ->condition('uid', $account->uid)
      ->execute();
}

function subscriptions_rules_subscription_resume_callback($account) {
  db_update('subscriptions_user')
      ->fields(array(
          'suspended' => 0,
        ))
      ->condition('uid', $account->uid)
      ->execute();
}

function subscriptions_rules_anon_write($account, $args, $remove = FALSE) {
  $session_id = subscriptions_rules_get_anon_sid($account);

  if (!$session_id) {
    return FALSE;
  }

  $args[8] = $session_id;

  if (!$remove) {
    db_merge('subscriptions_rules_anonymous')
      ->key(array(
        'module'        => $args[0],
        'field'         => $args[1],
        'value'         => $args[2],
        'recipient_uid' => $args[4],
        'author_uid'    => $args[3],
        'session_id' => $args[8]
      ))
      ->fields(array(
        'send_interval' => $args[5],
        'send_updates'  => $args[6] ? 1 : 0,
        'send_comments' => $args[7] ? 1 : 0,
      ))
      ->execute();
  }
  else {
    subscriptions_rules_anonymous_remove($args);
  }
}

function subscriptions_rules_anonymous_remove($args) {
  $delete = db_delete('subscriptions_rules_anonymous')
      ->condition('module', $args[0])
      ->condition('field', $args[1])
      ->condition('value', $args[2])
      ->condition('author_uid', $args[3])
      ->condition('recipient_uid', $args[4])
      ->condition('session_id', $args[8])
      ->execute();
}
