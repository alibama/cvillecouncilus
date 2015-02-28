<?php


interface ViewsDataExportExporterInterface {

  /**
   * Constructor for ViewsDataExportExporter classes.
   *
   * @param $options
   *   The array of export options as provided by the user-interface.
   */
  function __construct($options);

  function option_definition();

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
   * @return
   */
  function add_row(&$file_handle, $data, $row_count, $field_titles);

  /**
   * Provide headers to the page when an export file is being downloaded.
   *
   * @param $filename
   *   The name of the file being downloaded. e.g. export.xls.
   */
  function get_headers($filename);

  /**
   * Write the start of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function bof(&$file_handle);

  /**
   * Write the end of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function eof(&$file_handle, $row_count, $col_count);

  /**
   * Allow final processing of the results.
   *
   * @param $results
   *   An array of result data, including:
   *   - file_name: The full file path of the generated file.
   *   - row_count: The final number of rows in the generated file.
   */
  function post_process(&$results);

}
