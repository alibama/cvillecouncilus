<?php

/**
 * @file
 * Service's resources callbacks.
 */

/**
 * Select votes or vote results.
 *
 * @param $type
 *  Type of voting results.
 *  Allowed values: 'votes' or 'results'
 *
 * @param $single
 *  Boolean flag. Indicates whether result should be single or not.
 *
 * @param $criteria
 *   A keyed array used to build the select query. Keys can contain
 *   a single value or an array of values to be matched.
 *   $criteria['vote_cache_id']     (If this is set, all other keys are skipped)
 *   $criteria['entity_id']
 *   $criteria['entity_type']
 *   $criteria['value_type']
 *   $criteria['tag']
 *   $criteria['uid'] (Only for 'votes' select type)
 *   $criteria['function'] (Only for 'results' select type)
 *   $criteria['timestamp']   (If this is set, records with timestamps
 *      GREATER THAN the set value will be selected.)
 *
 * @param $limit
 *   An optional integer specifying the maximum number of votes to return.
 *
 * @return array
 *   An array of vote results matching the criteria.
 */
function _services_votingapi_resource_select_votes($type, $single, $criteria, $limit) {

  if ($type == 'votes') {
    // Build single or multiple results.
    $select_function = $single ? 'votingapi_select_single_vote_value' : 'votingapi_select_votes';
  }
  elseif ($type == 'results') {
    // Build single or multiple results.
    $select_function = $single ? 'votingapi_select_single_result_value' : 'votingapi_select_results';
  }
  else {
    return services_error(t('Wrong votingapi type.'));
  }

  // Return selected votes or vote results.
  return call_user_func($select_function, $criteria, $limit);
}

/**
 *
 * Vote for an entity.
 *
 * @param $votes
 *   An array of votes, each with the following structure:
 *   $vote['entity_type']  (Optional, defaults to 'node')
 *   $vote['entity_id']    (Required)
 *   $vote['value_type']    (Optional, defaults to 'percent')
 *   $vote['value']         (Required)
 *   $vote['tag']           (Optional, defaults to 'vote')
 *   $vote['uid']           (Optional, defaults to current user)
 *   $vote['vote_source']   (Optional, defaults to current IP)
 *   $vote['timestamp']     (Optional, defaults to REQUEST_TIME)
 *
 * @param $criteria
 *   A keyed array used to determine what votes will be deleted when the current
 *   vote is cast. If no value is specified, all votes for the current content
 *   by the current user will be reset. If an empty array is passed in, no votes
 *   will be reset and all incoming votes will be saved IN ADDITION to existing
 *   ones.
 *   $criteria['vote_id']
 *   $criteria['entity_type']
 *   $criteria['entity_type']
 *   $criteria['value_type']
 *   $criteria['tag']
 *   $criteria['uid']
 *   $criteria['vote_source']
 *   $criteria['timestamp']   (If this is set, records with timestamps
 *      GREATER THAN the set value will be selected.)
 *
 * @return array
 *   Result of voting.
 *
 * @see votingapi_set_votes() to learn more.
 */
function _services_votingapi_resource_set_votes($votes, $criteria) {
  foreach ($votes as $key => $vote) {
    if (empty($vote['entity_id'])) {
      return services_error(t('Missing entity ID for vote ') . $key);
    }
    if (empty($vote['value'])) {
      return services_error(t('Missing votes value for vote ') . $key);
    }
  }
  return votingapi_set_votes($votes, $criteria);
}

/**
 *
 * Delete current user's vote(s) for an entity, if the user has permission.
 *
 * @param $votes
 *   An array of votes, each with the following structure:
 *   $vote['entity_type']  (Required)
 *   $vote['entity_id']  (Required)
 *   $vote['tag']  (Required)
 *
 * @return array
 *   Result of deletion.
 *
 * @see votingapi_delete_votes() to learn more.
 */
function _services_votingapi_resource_delete_votes($votes) {
  // @TODO - add support for multiple deletions.
  global $user;
  $admin = FALSE;
  if (user_access('administer voting api')) { $admin = TRUE; }
  $entities = array(); // Track the entities that get changed so we can clear their cache later.
  foreach ($votes as $key => $vote) {
    // Lookup the vote_id. While looking it up, check the user's permission.
    $vote_id = null;
    $id = votingapi_current_user_identifier();
    $query = db_select('votingapi_vote', 'v');
    $query->condition('v.entity_type', $vote['entity_type']);
    $query->condition('v.entity_id', $vote['entity_id']);
    $query->condition('v.tag', $vote['tag']);
    $query->condition('v.uid', $user->uid, '=');
    $query->fields('v', array('vote_id', 'uid', 'vote_source'));
    $result = $query->execute()->fetch();
    if (!$result) {
      return services_error(t('Failed to load vote id for vote ') . $key);
    }
    else {
      $vote_id = $result->vote_id;
      $votes[$key]['vote_id'] = $vote_id;
      if (
        !$admin &&
        (
          $result->uid != $user->uid ||
          ($result->uid == 0 && $result->vote_source != $id['vote_source'])
        )
      ) {
        return services_error(t('You do not have permission to delete vote ') . $key);  
      }
    }
    if (empty($vote_id)) {
      return services_error(t('Missing vote cache id for vote ') . $key);
    }
    // Hold onto the entity type(s), id(s) an tag(s) so we can delete their
    // cache(s) later.
    $entity = new stdClass();
    $entity->entity_type = $vote['entity_type'];
    $entity->entity_id = $vote['entity_id'];
    $entity->tag = $vote['tag'];
    $entities[] = $entity;
    break; // Only process one for now...
  }
  votingapi_delete_votes($votes);
  votingapi_recalculate_results($votes[0]['entity_type'], $votes[0]['entity_id']);
  $criteria = array(
    'entity_type' => $votes[0]['entity_type'],
    'entity_id' => $votes[0]['entity_id'],
    'tag' => $votes[0]['tag']
  );
  return _services_votingapi_resource_select_votes(
    'results',
    0,
    $criteria,
    0
  );
}

