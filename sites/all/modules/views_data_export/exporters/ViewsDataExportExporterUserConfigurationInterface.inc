<?php


interface ViewsDataExportExporterUserConfigurationInterface extends ViewsDataExportExporterInterface {

  function options_form(&$form, &$form_state, $field_labels);

  function options_submit(&$form, &$form_state);

  function options_validate(&$form, &$form_state);
}
