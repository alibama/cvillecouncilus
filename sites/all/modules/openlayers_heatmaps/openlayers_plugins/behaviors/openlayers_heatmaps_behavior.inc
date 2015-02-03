<?php
/**
 * @file
 * Implementation of OpenLayers behavior.
 */

/**
 * Ctools plugin definition.
 */
function openlayers_heatmaps_openlayers_heatmaps_behavior_openlayers_behaviors() {
  $plugin = array(
    'title' => t('Provide a Heatmap.'),
    'description' => t('Select a layer that you want to be transformed into a Heatmap.'),
    'type' => 'map',
    'class' => 'openlayers_heatmaps_behavior',
    'behavior' => array(
      'file' => 'openlayers_heatmaps_behavior.inc',
      'class' => 'openlayers_heatmaps_behavior',
      'parent' => 'openlayers_behavior',
    ),
  );
  return $plugin;
}

/**
 * Zoom to Max Extent Behavior
 */
class openlayers_heatmaps_behavior extends openlayers_behavior {

  /**
   * Provide initial values for options.
   */
  function options_init() {
    return array(
      'layers' => array()
    ) + parent::options_init();
  }

  /**
   * Form defintion for per map customizations.
   */
  function options_form($defaults = array()) {
    $form = parent::options_form(array());

    // Only prompt for vector layers.
    $vector_layers = array();
    foreach ($this->map['layers'] as $id => $name) {
      $layer = openlayers_layer_load($id);
      if (isset($layer->data['vector']) && $layer->data['vector'] == TRUE) {
        $vector_layers[$id] = $name;
      }
    }

    foreach ($vector_layers as $id => $name) {

      $layer = openlayers_layer_load($id);

      $form['layers'][$id] = array(
        '#type' => 'fieldset',
        '#title' => t('Options for layer @layer', array('@layer' => $name)),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      $form['layers'][$id]['layer'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable the Heatmap ?'),
        '#description' => t("If selected, the layer will be replaced by it's Heatmap") .
            '<br/>(' . check_plain($layer->name) .
            ': ' . check_plain($layer->description) . ')',
        '#default_value' => !empty($defaults['layers'][$id]['layer']) ? 1 : FALSE,
      );
      $form['layers'][$id]['hide_original'] = array(
        '#type' => 'checkbox',
        '#title' => t('Hide the source layer ?'),
        '#description' => t("If selected, the source layer will not be displayed and will be available in the layer switcher."),
        '#default_value' => !empty($defaults['layers'][$id]['hide_original']) ? 1 : FALSE,
      );
      $form['layers'][$id]['heatmap_name'] = array(
        '#title' => t('Name of the Heatmap layer'),
        '#type' => 'textfield',
        '#description' => t('This will be the name displayed in the layer switcher.'),
        '#default_value' => !empty($defaults['layers'][$id]['heatmap_name']) ? $defaults['layers'][$id]['heatmap_name'] : $layer->description . ' heatmap',
      );
      $form['layers'][$id]['enable_cluster'] = array(
        '#title' => t('Enable the cluster on the original layer ?'),
        '#type' => 'checkbox',
        '#description' => t('If selected, the original layer will use a cluster strategy.'),
        '#default_value' => !empty($defaults['layers'][$id]['enable_cluster']) ? 1 : FALSE,
      );
      $form['layers'][$id]['radius'] = array(
        '#title' => t('Radius'),
        '#type' => 'textfield',
        '#description' => t('Select radius (pixels) to apply to Heatmap circles.'),
        '#default_value' => isset($defaults['layers'][$id]['radius']) ? $defaults['layers'][$id]['radius'] : 150,
        '#element_validate' => array('element_validate_integer_positive'),
      );
      $form['layers'][$id]['intensity'] = array(
        '#title' => t('Intensity'),
        '#type' => 'textfield',
        '#description' => t('Select the color intensity.'),
        '#default_value' => isset($defaults['layers'][$id]['intensity']) ? $defaults['layers'][$id]['intensity'] : 200,
        '#element_validate' => array('element_validate_integer_positive'),
      );
      $form['layers'][$id]['distance'] = array(
        '#title' => t('Distance'),
        '#type' => 'textfield',
        '#description' => t('Select radius (pixels) to apply to Heatmap cluster.'),
        '#default_value' => isset($defaults['layers'][$id]['distance']) ? $defaults['layers'][$id]['distance'] : 20,
        '#element_validate' => array('element_validate_integer_positive'),
      );
      $form['layers'][$id]['threshold'] = array(
        '#title' => t('Threshold'),
        '#type' => 'textfield',
        '#description' => t('Select threshold (pixels) to apply to Heatmap cluster.'),
        '#default_value' => isset($defaults['layers'][$id]['threshold']) ? $defaults['layers'][$id]['threshold'] : NULL,
        '#element_validate' => array('element_validate_integer_positive'),
      );
      $form['layers'][$id]['opacity'] = array(
        '#title' => t('Opacity'),
        '#type' => 'textfield',
        '#description' => t('Select the opacity, between 0 and 1.'),
        '#default_value' => isset($defaults['layers'][$id]['opacity']) ? $defaults['layers'][$id]['opacity'] : 0.5,
        '#element_validate' => array('element_validate_number'),
      );
    }

    return $form;
  }

  /**
   * Render.
   */
  function render(&$map) {
    // TODO: check if library is there
    $return = libraries_load('heatmap.js');
    drupal_add_js(drupal_get_path('module', 'openlayers_heatmaps') .
      '/openlayers_plugins/behaviors/openlayers_heatmaps_behavior.js');
    return $this->options;
  }
}
