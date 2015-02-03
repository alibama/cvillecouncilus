
(function ($) {

/**
 * Add key bindings when the Styles plugin is enabled.
 *
 * List of key bindings can be found at
 * http://www.weverwijk.net/wordpress/2010/03/23/key-events-in-javascript/
 * https://github.com/jeresig/jquery.hotkeys
 *
 * More inspiration :
 * - http://rikrikrik.com/jquery/shortkeys/#download
 * - http://code.google.com/p/js-hotkeys/
 * - http://code.google.com/p/js-hotkeys/wiki/about
 */

var kb_popup = '';

/**
 * Bind the keys.
 */
$(document).ready(function() {
  $.each(Drupal.settings.sweaver['kb'], function (index, key_binding) {
    if (key_binding.element != '' && $(key_binding.element).length == 0) {
      return;
    }
    $(document).bind('keydown', key_binding.kb_button, function(event) {
      Drupal.Sweaver.kbShowPopup(event, key_binding);
    });
  });
});

/**
 * Show or close the popup.
 */
Drupal.Sweaver.kbShowPopup = function(event, key_binding) {
  if (event.keyCode == parseInt(key_binding.kb_code) && key_binding.element != '') {
    if (key_binding.kb_button != kb_popup) {
      kb_popup = key_binding.kb_button;
      Drupal.Sweaver.showPopup($(key_binding.element), '400px', '200px');
    }
  }
  else if (event.keyCode == parseInt(key_binding.kb_code) && key_binding.page_callback != '') {
    // This is to understand functions present in objects
    var objects = key_binding.page_callback.split('.');
    var function_to_call = window;
    for ( var i = 0; i < objects.length; i++ ) {
      function_to_call = function_to_call[objects[i]];
    }
    function_to_call();
  
    if (key_binding.message != '') {
      Drupal.Sweaver.setMessage(key_binding.message, 2000);
    }
  }
  else {
    kb_popup = '';
    Drupal.Sweaver.hidePopup();
  }
}

// Undo and Redo functionalities
Drupal.Sweaver.undoModification = function () {
  if (Drupal.Sweaver.lastModifications['done'].length) {
    modification = Drupal.Sweaver.lastModifications['done'].pop();
    // We put this modification in undone array and then erase it from the screen
    Drupal.Sweaver.lastModifications['undone'].push(modification);
    Drupal.Sweaver.deleteProperty(modification['selector'], modification['property']);
    // We do not need to reload the interface, the delete function is taking care of that part
  }
}

Drupal.Sweaver.redoModification = function () {
  if (Drupal.Sweaver.lastModifications['undone'].length) {
    modification = Drupal.Sweaver.lastModifications['undone'].pop();
    Drupal.Sweaver.lastModifications['done'].push(modification);
    
    // We change the Active path for the modification
    actualActivePath = Drupal.Sweaver.activePath;
    Drupal.Sweaver.activePath = modification['selector'];
    Drupal.Sweaver.setValue(modification['property'], modification['value']); 
    Drupal.Sweaver.activePath = actualActivePath;
    Drupal.Sweaver.writeCss();
    Drupal.Sweaver.updateForm();
  }
}

})(jQuery);
