<?php

require_once 'ViewsDataExportExporterInterface.inc';
/**
 * Base class defining the common methods available to exporters.
 */
class ViewsDataExportExporter implements ViewsDataExportExporterInterface {
  public $options = array();

  /**
   * Constructor for views_data_export_exporter classes.
   *
   * @param $options
   *   The array of export options as provided by the user-interface.
   */
  function __construct($options) {
    $this->options = $options;
  }

  /**
   * Tell the world whether we support Hide If Empty views option
   */
  function supports_hide_if_empty() {
    return FALSE;
  }

  /**
   * Add a single row to the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   * @param array $data
   *   An array of formatted data for this row. One cell per item.
   * @param int $row_count
   *   The current number of rows in the export file.
   * @param $field_titles
   */
  function add_row(&$file_handle, $data, $row_count, $field_titles) {
  }

  /**
   * Provide headers to the page when an export file is being downloaded.
   *
   * @param $filename
   *   The name of the file being downloaded. e.g. export.xls.
   */
  function get_headers($filename) {
    $headers = array();
    $headers['Content-Type'] = 'application/force-download';
    $headers['Pragma'] = 'public';
    $headers['Cache-Control'] = 'max-age=0';
    return $headers;
  }

  /**
   * Write the start of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function bof(&$file_handle) {
  }

  /**
   * Write the end of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function eof(&$file_handle, $row_count, $col_count) {
  }

  /**
   * Allow final processing of the results.
   *
   * @param $results
   *   An array of result data, including:
   *   - file_name: The full file path of the generated file.
   *   - row_count: The final number of rows in the generated file.
   */
  function post_process(&$results) {
  }

  /**
   * Set options fields and default values.
   *
   * @return
   * An array of options information.
   */
  function option_definition() {
  }

  function clean_xml_tag($tag) {
    // These characters are not valid at the start of an XML tag:
    $invalid_start_chars = '-.0123456789';

    // Need to trim invalid characters from the start of the string:
    $tag = ltrim($tag, $invalid_start_chars);

    // Need to remove always invalid characters from the tag:
    $tag = strtr($tag, array(
      '>' => '',
      '<' => '',
      '&' => '',
    ));

    // As a last line of defense, if we've stripped out everything, set it to
    // something.
    if (empty($tag)) {
      $tag = 'invalid-tag-name';
    }

    return $tag;
  }
}
