<?php

require_once 'views_data_export_exporter.inc';

/**
 * This exporter creates a DOC file readable by Microsoft Word.
 * The content is an HTML table, as used by the old XLS export mechanism.
 */
class ViewsDataExportExporterDOC extends ViewsDataExportExporter {
  /**
   * Regular expression that checks for a valid ISO 8601 date/time.
   */
  const DATE_REGEX_ANY = '/^((\d{4})(-(\d{2}))(-(\d{2})))?(([T \s]?(\d{2}))(:(\d{2}))(:(\d{2}))?)?$/';
  const DATE_REGEX_DATE = '/^((\d{4})(-(\d{2}))(-(\d{2})))$/';
  const DATE_REGEX_TIME = '/^(([T \s]?(\d{2}))(:(\d{2}))(:(\d{2}))?)?$/';

  function __construct($options) {
    $this->options = $options;

    parent::__construct($options);
  }

  /**
   * Add a row to our (HTML table) word document.
   *
   * @param $file_handle
   * @param array $data
   * @param int $row_count
   * @param $field_titles
   */
  function add_row(&$file_handle, $data, $row_count, $field_titles) {
    $stripes = array("odd", "even");
    $row = $row_count + 1;

    $output = '        <tr class="' . $stripes[($row_count%2)] . '">';

    foreach($data as $key => $value) {
      $output .= '<td>' . $value . '</td>';
    }

    $output .= '</tr>';
    $output .= PHP_EOL;
    $row++;

    fwrite($file_handle, $output);
  }

  function bof(&$file_handle) {
    $output = '<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  </head>
  <body>
    <table>
      <thead><tr>';
    foreach($this->options['field_labels'] as $field) {
      $output .= '<th>' . $field . '</th>';
    }
    $output .= '</tr></thead>
      <tbody>';
    fwrite($file_handle, $output . "\n");
  }

  function eof(&$file_handle, $row_count, $col_count) {
    $output = '      </tbody>
    </table>
  </body>
</html>';

    fwrite($file_handle, $output);
  }

  function post_process(&$results) {

  }

  function get_headers($filename) {
    $headers = parent::get_headers($filename);
    $headers['Content-Type'] = 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet';
    $headers['Content-Disposition'] = "attachment; filename=$filename.doc";
    return $headers;
  }

}
