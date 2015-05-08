  /**
   * Implements hook_menu().
   */
  function cville_menu() {
    var items = {};
    items['issues'] = {
      title: 'Issues',
      page_callback: 'cville_issues_page'
    };
    return items;
  }

  /**
   * The page callback to display the view.
   */
  function cville_issues_page() {
    try {
      var content = {};
      content['cville_issues_list'] = {
        theme: 'view',
        format: 'ul',
        path: 'agenda/json', /* the path to the view in Drupal */
        row_callback: 'cville_issue_list_row',
        empty_callback: 'cville_issue_list_empty',
        attributes: {
          id: 'issue_endpoints'
        }
      };
      return content;
    }
    catch (error) { console.log('cville_issue_page - ' + error); }
  }

  /**
   * The row callback to render a single row.
   */
  function cville_issue_list_row(view, row) {
    try {
      return l(row.title, 'node/' + row.nid);
    }
    catch (error) { console.log('cville_issue_list_row - ' + error); }
  }

  /**
   *
   */
  function cville_issue_list_empty(view) {
    try {
      return 'Sorry, no articles were found.';
    }
    catch (error) { console.log('cville_issue_list_empty - ' + error); }
  }

