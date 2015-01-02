<?php

/**
 * @file
 * Include this file in your settings.php.
 */

$fb_settings = &drupal_static('fb_settings');
if (!$fb_settings) {
  $fb_settings = array();
}

// Drupal bootstrap has set $_GET['q'].
if (strpos($_GET['q'], 'fb__') === 0) {
  $args = explode('/', $_GET['q']);
  $fb_domain = '';

  // The beginning of the path is a series of name/value pairs.  Anything starting 'fb__' is a name.
  // We use fb__ rather than fb_ to avoid conflicts with modules that have paths starting fb_.
  while (!empty($args) && strpos($args[0], 'fb__') === 0) {
    $key = array_shift($args);
    $value = array_shift($args);
    $fb_settings[$key] = $value;
    $fb_domain .= "$key=$value/";
  }

  // Force canvas pages and page tabs to have different cookie domain from regular pages.
  if (count($fb_settings)) {
    $GLOBALS['cookie_domain'] = $fb_domain;
    $fb_settings['cookie_domain'] = $GLOBALS['cookie_domain']; // for debug
  }
}
