<?php
/**
 * @file
 * notifications_views_rules_taxonomy_terms.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function notifications_views_rules_taxonomy_terms_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function notifications_views_rules_taxonomy_terms_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function notifications_views_rules_taxonomy_terms_node_info() {
  $items = array(
    'issues' => array(
      'name' => t('Issues'),
      'base' => 'node_content',
      'description' => t('Create an issue for other people to vote on'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => t('Please review your posting before hitting save...  '),
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}

/**
 * Implements hook_default_profile2_type().
 */
function notifications_views_rules_taxonomy_terms_default_profile2_type() {
  $items = array();
  $items['main'] = entity_import('profile2_type', '{
    "userCategory" : true,
    "userView" : true,
    "type" : "main",
    "label" : "Main profile",
    "weight" : "0",
    "data" : { "registration" : true, "use_page" : true },
    "rdf_mapping" : []
  }');
  return $items;
}
