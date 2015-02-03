<?php
/**
 * @file
 * Twitter API functions
 */

module_load_include('php', 'oauth_common', 'lib/OAuth');
require_once('twitter.lib.php');

/**
 * Connect to the Twitter API.
 *
 * @param object $account
 *   An authenticated twitter_account object to be used to authenticate against
 *   Twitter.
 * @return
 *   a Twitter object ready to be used to query the Twitter API or FALSE.
 */
function twitter_connect($account = NULL) {
  if (!$account) {
    // Load the first authenticated account.
    $accounts = twitter_load_authenticated_accounts();
    if (count($accounts)) {
      $account = reset($accounts);
    }
  }

  if ($account) {
    $auth = $account->get_auth();
    if (isset($auth['oauth_token']) && isset($auth['oauth_token_secret'])) {
      return new Twitter(variable_get('twitter_consumer_key', ''), variable_get('twitter_consumer_secret', ''),
                              $auth['oauth_token'], $auth['oauth_token_secret']);
    }
  }
  return FALSE;
}

/**
 * Loads a Twitter account by its screen_name.
 *
 * @param string $screen_name
 *   A Twitter account name.
 * @return
 *   A TwitterAccount object or FALSE.
 */
function twitter_account_load($screen_name) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'twitter_account')
    ->propertyCondition('screen_name', $screen_name);
  $result = $query->execute();

  $ids = !empty($result['twitter_account']) ? array_keys($result['twitter_account']) : array();
  if (count($ids)) {
    return entity_load_single('twitter_account', array_pop($ids));
  }
  else {
    return FALSE;
  }
}

/**
 * Loads all Twitter accounts added by a Drupal user.
 *
 * This excludes Twitter accounts added automatically when,
 * for example, pulling mentions of an account from the Twitter API.
 *
 * @return
 *   array of TwitterAccount objects.
 */
function twitter_load_accounts() {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'twitter_account')
    ->propertyCondition('uid', 0, '<>')
    ->propertyOrderBy('screen_name');
  $result = $query->execute();

  $ids = !empty($result['twitter_account']) ? array_keys($result['twitter_account']) : array();
  if (count($ids)) {
    return entity_load('twitter_account', $ids);
  }
  else {
    return array();
  }
}

/**
 * Returns a list of authenticated Twitter accounts.
 *
 * @return
 *   array of TwitterAccount objects.
 */
function twitter_load_authenticated_accounts() {
  $accounts = twitter_load_accounts();
  $auth_accounts = array();
  foreach ($accounts as $index => $account) {
    if ($account->is_auth()) {
      $auth_accounts[] = $account;
    }
  }
  return $auth_accounts;
}

/**
 * Saves a TwitterStatus object to {twitter}
 */
function twitter_status_save($status) {
  $twitter_status = new TwitterStatus();
  $twitter_status->twitter_id = $status->id;
  $twitter_status->screen_name = $status->user->screen_name;
  $twitter_status->created_time = strtotime($status->created_at);
  $twitter_status->text = $status->text;
  $twitter_status->source = $status->source;
  $twitter_status->in_reply_to_status_id = ($status->in_reply_to_status_id > 0) ? (string) $status->in_reply_to_status_id : NULL;
  $twitter_status->in_reply_to_user_id = (int) $status->in_reply_to_user_id;
  $twitter_status->in_reply_to_screen_name = $status->in_reply_to_screen_name;
  $twitter_status->truncated = (int) $status->truncated;
  $twitter_status->is_new = entity_load_single('twitter_status', $twitter_status->twitter_id) ? FALSE : TRUE;
  entity_save('twitter_status', $twitter_status);

  // Let other modules know that a status has been saved.
  module_invoke_all('twitter_status_save', $status);
}

/**
 * Post a message to twitter
 *
 * @param $twitter_account
 *   object with a Twitter account.
 * @param $status
 *   string message to publish.
 * @return
 *   array response from Twitter API.
 */
function twitter_set_status($twitter_account, $status) {
  $twitter = twitter_connect($twitter_account);
  return $twitter->statuses_update($status);
}

/**
 * Fetches a user's timeline.
 */
function twitter_fetch_user_timeline($id) {
  $account = entity_load_single('twitter_account', $id);
  $since = db_query("SELECT MAX(twitter_id) FROM {twitter} WHERE screen_name = :screen_name", array(':screen_name' => $account->screen_name))->fetchField();

  // Connect to the Twitter's API.
  $twitter = twitter_connect();
  $params = array();
  if ($since) {
    $params['since_id'] = $since;
  }

  // Fetch tweets.
  $statuses = $twitter->user_timeline($id, $params);
  foreach ($statuses as $status) {
    twitter_status_save($status);
  }
  // Update Twitter account data.
  if (count($statuses) > 0) {
    twitter_account_save($statuses[0]->user);
  }
}

/**
 * Fetches user's mentions of an authenticated account.
 */
function twitter_fetch_mentions_timeline($id) {
  $account = entity_load_single('twitter_account', $id);
  // Connect to Twitter's API using the authenticated account to fetch mentions.
  $twitter = twitter_connect($account);

  $params = array();
  $statuses = $twitter->mentions_timeline($params);
  foreach ($statuses as $twitter_status) {
    twitter_account_save($twitter_status->user);
    twitter_status_save($twitter_status);
  }
}

/**
 * Pulls tweets from the database.
 *
 * @param string $screen_name
 *   Optionally provide a screen_name to filter.
 */
function twitter_tweets($screen_name = NULL) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'twitter_status');
  if (isset($screen_name)) {
    $query->entityCondition('screen_name', $screen_name);
  }
  $result = $query->execute();
  $ids = !empty($result['twitter_status']) ? array_keys($result['twitter_status']) : array();
  $twitter_statuses = entity_load('twitter_status', $ids);

  return $twitter_statuses;
}

/**
 * Saves a twitter account.
 *
 * @param object $twitter_account
 *   A TwitterAccount instance.
 */
function twitter_account_save($twitter_account) {
  $twitter_account_db = entity_load_single('twitter_account', $twitter_account->twitter_uid);
  // Check whether this account exists and load its defaults prior to save.
  $twitter_account->is_new = $twitter_account_db ? FALSE : TRUE;
  if ($twitter_account_db) {
    $twitter_account = (object) array_merge((array) $twitter_account_db, (array) $twitter_account);
  }
  entity_save('twitter_account', $twitter_account);
}

/**
 * Delete a twitter account and its statuses.
 *
 * @param $twitter_uid
 *   An integer with the Twitter UID.
 */
function twitter_account_delete($twitter_uid) {
  $account = entity_load_single('twitter_account', $twitter_uid);

  // Delete from {twitter_account}.
  entity_delete('twitter_account', $twitter_uid);

  // Delete from {twitter}.
  $twitter_statuses = twitter_tweets($account->screen_name);
  entity_delete_multiple('twitter_status', array_keys($twitter_statuses));

  // Delete from {authmap}.
  $query = db_delete('authmap');
  $query->condition('authname', $twitter_uid);
  $query->condition('module', 'twitter');
  $query->execute();
}
