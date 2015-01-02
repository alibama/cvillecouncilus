<?php

drupal_add_css(drupal_get_path('module', 'fb') . '/fb.theme.css');

function theme_fb_markup($params) {
  $options = isset($params['options']) ? $params['options'] : array();

  $not_connected_markup = isset($params['not_connected']) ? $params['not_connected'] : NULL;
  $connected_markup = isset($params['connected']) ? $params['connected'] : NULL;

  if ($not_connected_markup == $connected_markup) {
    return $not_connected_markup;
  }

  // Merge in defaults
  $options += array(
    'wrapper' => 'div',
  );

  $output = '';
  if ($not_connected_markup) {
    $output .= '<' . $options['wrapper'] . ' class=fb_not_connected>' . $not_connected_markup . '</' . $options['wrapper'] . '>';
  }
  if ($connected_markup) {
    $output .= '<' . $options['wrapper'] . ' class=fb_connected>' . $connected_markup . '</' . $options['wrapper'] . '>';
  }
  return $output;
}


function theme_fb_login_button($params) {

  // Merge in defaults
  $params += array(
    'text' => t('Connect'),
    'attributes' => array(),
    'scope' => NULL,
  );
  if (empty($params['onclick'])) {
    $params['onclick'] = 'return FB_JS.clientAuth(null, ' . htmlspecialchars(json_encode($params['scope'])) . ');';
  }
  $params['attributes'] += array(
    'class' => 'fb_login_button_wrapper fb_button_style',
  );

  $markup = '<div ' . drupal_attributes($params['attributes']) . '><a href="#" onclick="' . $params['onclick'] . '"><span>' . $params['text'] . '</span></a></div>';

  return $markup;
}
