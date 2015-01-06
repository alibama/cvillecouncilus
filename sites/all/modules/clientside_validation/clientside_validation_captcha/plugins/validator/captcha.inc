<?php

$plugin = array(
  'label' => t('Captcha'),
  'validator' => array(
    'class' => 'CvCoreCaptchaValidator',
  ),
);


class CvCoreCaptchaValidator extends ClientsideValidationValidator {

  public function supports(array $element, array &$form_state) {
    return (
      parent::supports($element, $form_state)
      && isset($element['#type'])
      && $element['#type'] == 'captcha'
      && isset($element['captcha_widgets']['captcha_response'])
    );

  }

  public function getJavascriptSettings(array &$element, array &$form_state) {
    $name = $this->getName($element['captcha_widgets']['captcha_response']);
    return array(
      'rules' => array(
        $name => array(
          'captcha' => $element['#captcha_validate'],
        ),
      ),
      'messages' => array(
        $name => array(
          'captcha' => $this->getMessage($element),
        ),
      ),
    );
  }

  public function jsFiles(array &$element) {
    $files = parent::jsFiles($element);
    $files[] = drupal_get_path('module', 'clientside_validation_captcha') . '/plugins/validator/js/captcha.cv.js';
    return $files;
  }

  public function getMessage(array $element) {
    return t('Wrong answer for !title.', array('!title' => $element['#title']));
  }

}
