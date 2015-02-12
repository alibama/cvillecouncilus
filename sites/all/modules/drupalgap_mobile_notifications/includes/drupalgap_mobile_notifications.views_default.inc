<?php
/**
 * @file
 * This file is used by views to install default views for
 * this module.
 */

/**
 * Implements hook_views_default_views().
 */
function drupalgap_mobile_notifications_views_default_views(){

$view = new view();
$view->name = 'drupalgap_mobile_notifications';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Mobile notifications';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Mobile notifications';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'views_json';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Posted';
$handler->display->display_options['fields']['created']['date_format'] = 'medium';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Attachment */
$handler->display->display_options['fields']['mobile_attachment']['id'] = 'mobile_attachment';
$handler->display->display_options['fields']['mobile_attachment']['table'] = 'field_data_mobile_attachment';
$handler->display->display_options['fields']['mobile_attachment']['field'] = 'mobile_attachment';
$handler->display->display_options['fields']['mobile_attachment']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['mobile_attachment']['settings'] = array(
  'link' => 0,
);
/* Field: Content: Message */
$handler->display->display_options['fields']['mobile_message']['id'] = 'mobile_message';
$handler->display->display_options['fields']['mobile_message']['table'] = 'field_data_mobile_message';
$handler->display->display_options['fields']['mobile_message']['field'] = 'mobile_message';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'drupalgap_mobile_notifications' => 'drupalgap_mobile_notifications',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'mobile-notifications';


  
  $views[$view->name] = $view;

  return $views;
}
