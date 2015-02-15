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
