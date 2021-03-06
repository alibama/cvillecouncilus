<?php

/**
 * @file
 * Hooks provided by the OAuth2 Server module.
 */

/**
 * Execute operations before oauth2_server_authorize() main logic.
 *
 * Allow modules to perform additional operations at the very beginning of
 * the OAuth2 authorize callback.
 */
function hook_oauth2_server_pre_authorize() {
  // Make sure we're not in the middle of a running operation.
  if (empty($_SESSION['oauth2_server_authorize'])) {
    global $user;
    // Ensure that the current session is killed before authorize.
    module_invoke_all('user_logout', $user);
    // Destroy the current session, and reset $user to the anonymous user.
    session_destroy();
  }
}

/**
 * Returns claims about the provided account for OpenID Connect purposes.
 *
 * The provided claims can be included in the id_token and / or returned from
 * the /oauth2/UserInfo endpoint.
 *
 * Groups of claims are returned based on the requested scopes. No group
 * is required, and no claim is required.
 *
 * Note: OAuth2 Server already provides claims for the email scope, so they
 * don't need to be returned by this hook unless they need to be overriden.
 *
 * @param $account
 *   The user account for which claims should be returned.
 * @param array $requested_scopes
 *   The requested scopes.
 *   Scopes with matching claims: profile, email, address, phone.
 *
 * @return
 *   An array in the claim => value format.
 *
 * @see http://openid.net/specs/openid-connect-core-1_0.html#ScopeClaims
 */
function hook_oauth2_server_user_claims($account, $requested_scopes) {
  $claims = array();
  if (in_array('profile', $requested_scopes)) {
    $claims += array(
      'name' => '',
      'family_name' => '',
      'given_name' => '',
      'middle_name' => '',
      'nickname' => '',
      'preferred_username' => $account->name,
      'profile' => '',
      'picture' => '',
      'website' => '',
      'gender' => '',
      'birthdate' => '',
      'zoneinfo' => $account->timezone,
      'updated_at' => $account->created,
    );
  }
  if (in_array('address', $requested_scopes)) {
    $claims += array(
      'formatted' => '',
      'street_address' => '',
      'locality' => '',
      'region' => '',
      'postal_code' => '',
      'country' => '',
    );
  }
  if (in_array('phone', $requested_scopes)) {
    $claims += array(
      'phone_number' => '',
      'phone_number_verified' => '',
    );
  }

  return $claims;
}

/**
 * Returns the default scope for the provided server.
 *
 * Invoked by OAuth2_Scope_Drupal.
 * If no hook implementation returns a default scope for the current server,
 * then the one from $server->settings['default_scope'] is used.
 *
 * This hook runs on "authorize" and "token" requests and has access to the
 * client_id in $_GET (for "authorize") or via
 * oauth2_server_get_client_credentials() (for "token").
 * Note that client_id in this case corresponds to $client->client_key.
 *
 * @return
 *   An array of default scopes (their machine names).
 */
function hook_oauth2_server_default_scope($server) {
  // For the "test" server, grant the user any scope he has access to.
  if ($server->name == 'test') {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'oauth2_server_scope');
    $query->propertyCondition('server', $server->name);
    $query->addTag('oauth2_server_scope_access');
    $query->addMetaData('oauth2_server', $server);
    $results = $query->execute();

    if ($results) {
      $scope_ids = array_keys($results['oauth2_server_scope']);
      $scopes = entity_load('oauth2_server_scope', $scope_ids);
      $default_scopes = array();
      foreach ($scopes as $scope) {
        $default_scopes[] = $scope->name;
      }

      return $default_scopes;
    }
  }
}

/**
 * An example hook_entity_query_alter() implementation for scope access.
 */
function example_entity_query_alter($query) {
  global $user;

  // This is an EFQ used to get all scopes available to the user
  // (inside the Scope class, or when showing the authorize form, for instance).
  if (!empty($query->tags['oauth2_server_scope_access'])) {
    $server = $query->metaData['oauth2_server'];

    // On the "test" server only return scopes that have the current user
    // in an example "users" entityreference field.
    if ($server->name == 'test' && $user->uid) {
      $query->fieldCondition('users', 'target_id', $user->uid);
    }
  }

  // This is an EFQ used to get all exportable scopes.
  if (!empty($query->tags['oauth2_server_scope_export'])) {
    $server = $query->metaData['oauth2_server'];

    // On the "test" server only export scopes assigned to admin.
    if ($server->name == 'test') {
      $query->fieldCondition('users', 'target_id', 1);
    }
  }
}
