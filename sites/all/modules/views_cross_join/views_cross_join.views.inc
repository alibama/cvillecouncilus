<?php 



/**
 * Implementation of hook_views_data()
 */
function views_cross_join_views_data() {
  $data['views_cross_join']['table']['group'] = t('Cross Join');  
  $data['views_cross_join']['table']['join'] = array(
    '#global' => array(),
  );

  return $data;
}




/**
 * Implementation of hook_views_data()
 */
function views_cross_join_views_data_alter(&$data) {
  
  foreach($data as $key => $entity) {
    if (isset($entity['table']['base'])) {
      // Add relationship to user table.
      $data['views_cross_join'][$key] = array(
        'title' => $entity['table']['base']['title'],
        'help' => t('Join in all items of the given entity. You need to filter them by yourself.'),
        'relationship' => array(
          'handler' => 'views_cross_join_handler_relationship',
          'field' => $entity['table']['base']['field'],
          'base' => $key,
        ),
      ); 
      $data[$key]['views_cross_join'] = array(
        'title' => t('Cross Join Filter: %entity', array('%entity' => $entity['table']['base']['title'])),
        'help' => t('Filter %entity by Id. You may use agruments in here.', array('%entity' => $entity['table']['base']['title'])),
        'filter' => array(
          'handler' => 'views_cross_join_handler_entity_filter',
          'field' => $entity['table']['base']['field'],
        ),
      );
    }
  }

}



