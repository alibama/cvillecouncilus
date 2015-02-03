<?php
/**
 * @file
 * Field configuration for twitter_post module.
 */

/**
 * Implements hook_field_info()
 */
function twitter_post_field_info() {
  return array(
    'twitter_post' => array(
      'label' => t('Twitter'),
      'description' => t('Allows posting to Twitter.com.'),
      'default_widget' => 'twitter_post_widget',
      'default_formatter' => 'text_text',
    ),
  );
}

/**
 * Implement hook_field_widget_info().
 */
function twitter_post_field_widget_info() {
  return array(
    'twitter_post_widget'=> array(
      'label' => t('Post to Twitter'),
      'field types' => array('twitter_post'),
    ),
  );
}

/**
 * Implement hook_field_widget().
 */
function twitter_post_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta = 0, $element) {
  $status = isset($items[$delta]['status']) ? $items[$delta]['status'] : FALSE;
  $default_message = _twitter_post_default_message($instance['entity_type']);
  $message = isset($items[$delta]['message']) ? $items[$delta]['message'] : $default_message;
  $element += array(
    'status' => array(
      '#type' => 'checkbox',
      '#title' => t('Announce this post on Twitter'),
      '#default_value' => $status,
      '#id' => 'twitter-toggle',
    ),
    'message' => array(
      '#type' => 'textfield',
      '#title' => t('Twiter.com status'),
      '#default_value' => $message,
      '#description' => t('The given text will be posted to Twitter.com. See Replacement Patterns below for a list of available tokens.'),
      '#maxlength' => 150,
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'twitter_post') . '/twitter_post.js'),
      ),
      '#attributes' => array('class' => array('twitter-post-message')),
    ),
    'token_help' => array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('Prefer raw-text replacements for text to avoid problems with HTML entities!'),
      'help' => array(
        '#theme' => 'token_tree',
        '#token_types' => array($instance['entity_type']),
        '#global_types' => TRUE,
        '#click_insert' => TRUE,
      ),
    ),

  );

  return $element;
}

/**
 * Sets default messages for core entities.
 *
 * @param string $entity_type
 *   The entity type name.
 * @return
 *   A string with a suggested default message for the tweet.
 */
function _twitter_post_default_message($entity_type) {
  switch ($entity_type) {
    case 'comment':
      $message = 'New comment: [comment:url:absolute]';
      break;
    case 'node':
      $message = 'New post: [node:title] [node:url:absolute]';
      break;
    case 'user':
      $message = 'New user: [current-user:name]';
      break;
    default:
      $message = 'Select replacement patterns below to build your tweet.';
      break;
  }
  return $message;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function twitter_post_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];


  if ($settings['type'] != 'none') {
    $summary = t('Default status: @status. Message: @message', array(
      '@status' => $settings['status'],
      '@message' => $settings['message'],
    ));
  }
  else {
    $summary = t('Twitter post field is disabled.');
  }

  return $summary;
}

/**
 * Implement hook_field_is_empty().
 */
function twitter_post_field_is_empty($item, $field) {
  return strlen($item['message']) == 0;
}

/**
 * Implements hook_field_attach_form()
 *
 * Adds a select field to choose a Twitter account.
 */
function twitter_post_field_attach_form($entity_type, $entity, &$form, &$form_state, $langcode) {
  $langcode = empty($langcode) ? entity_language($entity_type, $entity) : $langcode;
  $options = array();
  module_load_include('inc', 'twitter');
  $twitter_accounts = twitter_load_authenticated_accounts();
  foreach ($twitter_accounts as $twitter_account) {
    $options[$twitter_account->twitter_uid] = $twitter_account->screen_name;
  }
  $twitter_uid = key($options);
  $screen_name = current($options);
  foreach ($form_state['field'] as $field_name => $field) {
    if (isset($field[$langcode]['field']['type']) && ($field[$langcode]['field']['type'] == 'twitter_post')) {
      $element = array();
      if (count($options) > 1) {
        $element['account'] = array(
          '#type' => 'select',
          '#title' => t('Twitter Account'),
          '#options' => $options,
          '#id' => 'twitter-account',
        );
      }
      else {
        $element['account'] = array(
          '#type' => 'value',
          '#value' => $twitter_uid,
        );
        $element['account_name'] = array(
          '#type' => 'item',
          '#title' => t('Twitter account'),
          '#markup' => _twitter_user_profile($screen_name),
        );
      }
      foreach ($form[$field_name][$langcode] as $delta => $field_instance) {
        if (is_int($delta)) {
          $form[$field_name][$langcode][$delta] += $element;
        }
      }
    }
  }
}

/**
 * Implements hook_field_access()
 */
function twitter_post_field_access($op, $field, $entity_type, $entity, $account) {
  if ($field['type'] == 'twitter_post') {
    return user_access('post to twitter', $account);
  }
}
