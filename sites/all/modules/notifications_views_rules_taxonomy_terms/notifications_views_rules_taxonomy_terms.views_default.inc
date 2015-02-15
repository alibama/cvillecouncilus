<?php
/**
 * @file
 * notifications_views_rules_taxonomy_terms.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function notifications_views_rules_taxonomy_terms_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'cross_join_profiles_and_content';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'profile';
  $view->human_name = 'cross join profiles and content';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'join on profiles and issues terms';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Relationship: Profile: My Interests (field_my_interests) */
  $handler->display->display_options['relationships']['field_my_interests_tid']['id'] = 'field_my_interests_tid';
  $handler->display->display_options['relationships']['field_my_interests_tid']['table'] = 'field_data_field_my_interests';
  $handler->display->display_options['relationships']['field_my_interests_tid']['field'] = 'field_my_interests_tid';
  /* Relationship: Taxonomy term: Content using Alignment with Council Vision Areas and Strategic Plan */
  $handler->display->display_options['relationships']['reverse_field_alignment_with_council_vis_node']['id'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['relationships']['reverse_field_alignment_with_council_vis_node']['table'] = 'taxonomy_term_data';
  $handler->display->display_options['relationships']['reverse_field_alignment_with_council_vis_node']['field'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['relationships']['reverse_field_alignment_with_council_vis_node']['relationship'] = 'field_my_interests_tid';
  /* Relationship: Profile: User uid */
  $handler->display->display_options['relationships']['user']['id'] = 'user';
  $handler->display->display_options['relationships']['user']['table'] = 'profile';
  $handler->display->display_options['relationships']['user']['field'] = 'user';
  /* Field: Profile: My Interests */
  $handler->display->display_options['fields']['field_my_interests']['id'] = 'field_my_interests';
  $handler->display->display_options['fields']['field_my_interests']['table'] = 'field_data_field_my_interests';
  $handler->display->display_options['fields']['field_my_interests']['field'] = 'field_my_interests';
  $handler->display->display_options['fields']['field_my_interests']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_my_interests']['delta_offset'] = '0';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['relationship'] = 'node';
  /* Field: Content: Alignment with Council Vision Areas and Strategic Plan */
  $handler->display->display_options['fields']['field_alignment_with_council_vis']['id'] = 'field_alignment_with_council_vis';
  $handler->display->display_options['fields']['field_alignment_with_council_vis']['table'] = 'field_data_field_alignment_with_council_vis';
  $handler->display->display_options['fields']['field_alignment_with_council_vis']['field'] = 'field_alignment_with_council_vis';
  $handler->display->display_options['fields']['field_alignment_with_council_vis']['relationship'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['fields']['field_alignment_with_council_vis']['type'] = 'taxonomy_term_reference_plain';
  $handler->display->display_options['fields']['field_alignment_with_council_vis']['delta_offset'] = '0';
  /* Field: User: E-mail */
  $handler->display->display_options['fields']['mail']['id'] = 'mail';
  $handler->display->display_options['fields']['mail']['table'] = 'users';
  $handler->display->display_options['fields']['mail']['field'] = 'mail';
  $handler->display->display_options['fields']['mail']['relationship'] = 'user';
  $handler->display->display_options['fields']['mail']['label'] = '';
  $handler->display->display_options['fields']['mail']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['mail']['link_to_user'] = '0';
  /* Field: Content: Background */
  $handler->display->display_options['fields']['field_background']['id'] = 'field_background';
  $handler->display->display_options['fields']['field_background']['table'] = 'field_data_field_background';
  $handler->display->display_options['fields']['field_background']['field'] = 'field_background';
  $handler->display->display_options['fields']['field_background']['relationship'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['fields']['field_background']['type'] = 'text_plain';
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['relationship'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['fields']['path']['label'] = '';
  $handler->display->display_options['fields']['path']['element_label_colon'] = FALSE;
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['relationship'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['arguments']['nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'raw';
  $handler->display->display_options['arguments']['nid']['default_argument_options']['index'] = '1';
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Profile: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'profile';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'main' => 'main',
  );
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type_1']['id'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['table'] = 'node';
  $handler->display->display_options['filters']['type_1']['field'] = 'type';
  $handler->display->display_options['filters']['type_1']['relationship'] = 'reverse_field_alignment_with_council_vis_node';
  $handler->display->display_options['filters']['type_1']['value'] = array(
    'issues' => 'issues',
  );

  /* Display: Rules */
  $handler = $view->new_display('views_rules', 'Rules', 'views_rules_1');
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['rules_parameter'] = array(
    'nid' => array(
      'enabled' => 1,
      'type' => 'integer',
      'label' => '(field_alignment_with_council_vis) Content: Nid',
      'name' => 'nid',
    ),
  );
  $handler->display->display_options['rules_variables'] = array(
    'field_my_interests' => array(
      'enabled' => 0,
      'rendered' => 0,
      'type' => '',
      'label' => 'My Interests',
      'name' => 'field_my_interests',
    ),
    'title' => array(
      'enabled' => 1,
      'rendered' => 0,
      'type' => 'text',
      'label' => 'Title',
      'name' => 'title',
    ),
    'field_alignment_with_council_vis' => array(
      'enabled' => 0,
      'rendered' => 0,
      'type' => '',
      'label' => '(field_alignment_with_council_vis) Alignment with Council Vision Areas and Strategic Plan',
      'name' => 'field_alignment_with_council_vis',
    ),
    'mail' => array(
      'enabled' => 1,
      'rendered' => 0,
      'type' => 'text',
      'label' => '(User) User: E-mail',
      'name' => 'mail',
    ),
    'field_background' => array(
      'enabled' => 1,
      'rendered' => 1,
      'type' => 'text',
      'label' => '(field_alignment_with_council_vis) Background',
      'name' => 'field_background',
    ),
    'path' => array(
      'enabled' => 1,
      'rendered' => 1,
      'type' => 'text',
      'label' => '(field_alignment_with_council_vis) Content: Path',
      'name' => 'path',
    ),
  );

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['block_description'] = 'test rules view';
  $export['cross_join_profiles_and_content'] = $view;

  return $export;
}
