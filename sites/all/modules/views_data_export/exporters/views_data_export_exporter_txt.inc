<?php

require_once 'views_data_export_exporter.inc';

/**
 * Exporter for creating TXT files.
 */
class ViewsDataExportExporterTXT extends ViewsDataExportExporter {

  function __construct($options) {
    $this->options = $options;

    parent::__construct($options);
  }

  /**
   * Tell the world whether we support Hide If Empty views option
   */
  function supports_hide_if_empty() {
    return TRUE;
  }

  /**
   * Set options fields and default values.
   *
   * @return
   * An array of options information.
   */
  function option_definition() {
    //$options = parent::option_definition();
    $options = array();
    return $options;
  }

  function add_row(&$file_handle, $data, $row_count, $field_titles) {
    $row = '';
    foreach ($data as $key => $value) {
      $row .= '[' . $field_titles[$key] . ']' . PHP_EOL . PHP_EOL;
      $row .= $value . PHP_EOL;

    }
    $row .= '----------------------------------------' . PHP_EOL . PHP_EOL;

    fwrite($file_handle, $row);
  }

  function add_header(&$file_handle, $filename) {
    return 'file header';
  }

  function get_headers($filename) {
    $headers = parent::get_headers($filename);

    $extension = 'txt';
    $content_type = 'text/txt';

    $headers['Content-Type'] = $content_type;
    $headers['Content-Disposition'] = "attachment; filename=$filename.$extension";
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


}
