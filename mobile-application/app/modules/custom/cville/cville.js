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
function cville_form_alter(form, form_state, form_id) {
  try {
    if (form_id == 'node_edit' && form.elements.type.default_value == 'where_it_s_at') {
      // The site is using Automatic node titles, disable access to the default
      // title field and make it optional.
      form.elements.title.access = false;
      form.elements.title.required = false;
      // Disable access to other fields not needed.
      //form.elements.field_contest_entry.access = false;
      //form.elements.field_place_finish.access = false;
      //form.elements.field_addthis.access = false;
      // Redirect the node edit form submission to the user's gallery.
      // form.action = 'gallery/my';
      }
     }
    catch (error) { console.log('tree_form_error - ' + error); }
}
