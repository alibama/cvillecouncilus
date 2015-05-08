/**
 *
 */
function fivestar_compute_average(rating, count, stars) {
  try {
    return (rating * count / 100 * stars).toFixed(2);
  }
  catch (error) { console.log('fivestar_compute_rating - ' + error); }
}

/**
 *
 */
function fivestar_compute_base(stars) {
  try {
    // Fivestar uses a value system based on 100.
    return 100/stars;
  }
  catch (error) { console.log('fivestar_compute_base - ' + error); }
}

/**
 *
 */
function fivestar_compute_rating(star, base) {
  try {
    return star * base;
  }
  catch (error) { console.log('fivestar_compute_rating - ' + error); }
}

/**
 *
 */
function fivestar_container_id(entity_type, entity_id, delta) {
  try {
    var id = 'new';
    if (entity_id) { id = entity_id; }
    return 'fivestar_' + entity_type + '_' + entity_id + '_' + delta;
  }
  catch (error) { console.log('fivestar_container_id - ' + error); }
}

/**
 * Implements hook_form_alter().
 */
function fivestar_form_alter(form, form_state, form_id) {
  try {
    
    //dpm(form_id);
    //dpm(form);
    
    /*
    
    // Select list (rated while editing)
    field_info_instance.widget.type == 'fivestar_select'
    
    // Stars (rated while viewing)
    field_info_instance.widget.type == 'exposed'
    
    // Stars (rated while editing)
    field_info_instance.widget.type == 'stars'
    
    */
    
    // Only modify entity forms.
    if (!form.entity_type) { return; }
    
    // Make potential alterations to any entity form that has a fivestar field
    // on it. There are modifications to both entity add and edit forms.
    var new_entity = false;
    if (form.entity_type && !form.entity_id) { new_entity = true; }
    
    $.each(form.elements, function(name, element) {
        if (element.is_field && element.type == 'fivestar') {
          if (new_entity) {
            // Remove fivestar element(s) that have their widget type set to
            // "exposed", aka "Stars (rated while viewing)". Because a rating
            // can't happen until the entity has been created.
            if (element.field_info_instance.widget.type == 'exposed') {
              form.elements[name].access = false;
              form.elements[name].required = false;
            }
          }
          else {
            // Existing entity...
          }
        }
    });

  }
  catch (error) { console.log('fivestar_form_alter - ' + error); }
}

/**
 * Implements hook_assemble_form_state_into_field().
 */
function fivestar_assemble_form_state_into_field(entity_type, bundle,
  form_state_value, field, instance, langcode, delta, field_key) {
  try {
    field_key.use_key = false;
    var result = {
      rating: form_state_value,
      target: null
    };
    return result;
  }
  catch (error) {
    console.log('fivestar_assemble_form_state_into_field - ' + error);
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function fivestar_field_formatter_view(entity_type, entity, field, instance, langcode, items, display) {
  try {
    // @todo - for some reason instance is coming in as null...? Is it possible
    // the hook's invoker (DG Core) is sending the instance in the field position by accident?
    // @UPDATE - yes, it turns out DG core was not sending it, and it was sending
    // them in the wrong order, it is now being sent, but they are actually backwards, le sigh.
    //dpm('fivestar_field_formatter_view');
    //dpm(entity_type);
    //dpm(entity);
    //dpm(field);
    //dpm(instance);
    //dpm(items);
    //dpm(display);
    //dpm(items);
    // Grab the entity primary key and start building the element.
    var key = entity_primary_key(entity_type);
    var element = {};
    // Iterate over each item and assemble the element.
    var item_count = 0;
    $.each(items, function(delta, item) {
        // Attach the pageshow handler for the widget.
        var container_id = fivestar_container_id(entity_type, entity[key], delta); 
        element[delta] = {
          markup: _fivestar_field_formatter_view(entity_type, entity, field, instance, langcode, items, display, container_id, key, item, delta)
        };
        item_count++;
    });
    // If there are no items on this fivestar field, then it is probably
    // a field that is set as the parent target of another fivestar field.
    // That means we need to manually retrieve the data from Drupal, and then
    // inject it into the fivestar element container.
    // @see https://drupal.org/node/1308114
    if (item_count == 0 && entity_type == 'node') {
      var container_id = fivestar_container_id(entity_type, entity[key], 0); 
      element[0] = {
        markup: _fivestar_field_formatter_view(entity_type, entity, field, instance, langcode, items, display, container_id, key, null, 0)
      };
    }
    return element;
  }
  catch (error) { console.log('fivestar_field_formatter_view - ' + error); }
}

/**
 *
 */
function _fivestar_field_formatter_view(entity_type, entity, field, instance, langcode, items, display, container_id, key, item, delta) {
  try {
    // Setup the pageshow args.
    var jqm_page_event_args = {
        container_id: container_id,
        entity_type: entity_type,
        entity_id: entity[key],
        entity_uid: entity.uid,
        stars: field.settings.stars,
        style: display.settings.style,
        allow_clear: field.settings.allow_clear,
        allow_ownvote: field.settings.allow_ownvote,
        allow_revote: field.settings.allow_revote,
        expose: display.settings.expose,
        style: display.settings.style
    };
    // If we have an item, attach it's rating to the pageshow args.
    if (item) { jqm_page_event_args.rating = item.rating; }
    // Return the html.
    return '<div id="' + container_id + '"></div>' +
      drupalgap_jqm_page_event_script_code({
          page_id: drupalgap_get_page_id(),
          jqm_page_event: 'pageshow',
          jqm_page_event_callback: '_fivestar_field_formatter_view_pageshow',
          jqm_page_event_args: JSON.stringify(jqm_page_event_args)
      },
      container_id
    );
  }
  catch (error) { console.log('_fivestar_field_formatter_view - ' + error); }
}

/**
 *
 */
function _fivestar_field_formatter_view_pageshow(options) {
  try {
    // First, declare the succes callback function.
    var _success = function(result) {
      // Place the options into the result under the fivestar property.
      result.fivestar = options;
      // Theme the widget and the average, then inject them into the page.
      var base = fivestar_compute_base(options.stars);
      var average = fivestar_compute_average(
        result.average.value,
        result.count.value,
        options.stars
      );
      // Did the user already vote (star) this entity?
      var user_rated = true;
      var user_value = null;
      if (result.user) {
        if ($.isArray(result.user) && result.user.length == 0) { user_rated = false; }
        else {
          user_value = result.user.value;
        }
      }
      // Build the variables to theme the fivestar.
      var variables = {
        stars: options.stars,
        base: base,
        rating: result.average.value,
        entity_type: options.entity_type,
        entity_id: options.entity_id,
        expose: options.expose,
        allow_clear: options.allow_clear,
        allow_ownvote: options.allow_ownvote,
        allow_revote: options.allow_revote,
        user_rated: user_rated,
        user_value: user_value
      };
      // If it was a user style, set the exposed bit to true/false depending
      // on the allow_revote setting. Revoting only happens if the current
      // user is the author of the entity.
      // Styles: average, user
      //   user - This is rendered in a comment that collected a user rating
      //          at the time of posting, and no one else can vote on it. Not
      //          entirely sure if this formatter gets rendered anywhere else.
      if (options.style == 'user') {
        if (options.allow_revote && Drupal.user.uid != 0 && Drupal.user.uid == options.entity_uid) {
          variables.expose = true;
        }
        else { variables.expose = false; }
      }
      // Finally, render the fivestar.
      var html = '';
      html += theme('fivestar', variables);
      if (variables.expose) {
        html += theme('fivestar_average', result);
      }
      $('#' + options.container_id).html(html).trigger('create');
    };
    //dpm('_fivestar_field_formatter_view_pageshow');
    // If we have a rating, we don't need to fetch it from the site. This
    // happens (I think) for all fivestar fields that do not have a parent
    // target set.
    if (options.rating) {
      // We need to assemble a fake result object to get it to theme.
      var _result = {
        average: { value: options.rating },
        count: { value: 1 },
        fivestar: { },
        user: { value: options.rating }
      };
      _success(_result);
    }
    else {
      // We don't have a rating, fetch it from the site. This (I think) only
      // happens when the fivestar field is set as a parent node target.
      fivestar_retrieve(options.entity_type, options.entity_id, null, null, {
          success: _success
      });
    }
  }
  catch (error) { console.log('_fivestar_field_formatter_view_pageshow - ' + error); }
}

/**
 * Implements hook_field_widget_form().
 */
function fivestar_field_widget_form(form, form_state, field, instance, langcode, items, delta, element) {
  try {
    /*dpm(form);
    dpm(form_state);
    dpm(field);
    dpm(instance);
    dpm(element);*/
    // @TODO - get the user_rated and user_value variables set here, see
    // _fivestar_field_formatter_view_pageshow() for an example.
    // We'll just hide the actual input, and populate it later.
    items[delta].type = 'hidden';
    var html = theme('fivestar', {
        input_id: items[delta].id,
        stars: instance.settings.stars,
        base: fivestar_compute_base(instance.settings.stars),
        entity_type: form.entity_type,
        entity_id: form.entity_id,
        expose: instance.display.default.settings.expose,
        allow_clear: instance.settings.allow_clear,
        allow_ownvote: instance.settings.allow_ownvote,
        allow_revote: instance.settings.allow_revote
    });
    items[delta].children.push({ markup: html });
  }
  catch (error) { console.log('fivestar_field_widget_form - ' + error); }
}

/**
 *
 * @param {Object} link The star anchor link that was just clicked.
 * @param {String} id The id attribute value of the hidden input associated with
                      the widget. This only appears on entity edit forms,
                      otherwise it is null.
 */
function _fivestar_widget_click(link, id, star, rating, entity_type, entity_id, expose) {
  try {

    // If it is not exposed, ignore the click. Note the theme_fivestar function
    // should not attach the onclick, so we should never arrive here if it
    // isn't exposed, we just add this for one extra layer of protection.
    if (!expose) { return; }
    
    // Activate the link that was clicked, and each sibling preceding it.
    var classes = 'ui-btn-active ui-state-persist';
    $(link).removeClass(classes).siblings().removeClass(classes);
    $(link).addClass(classes).prevAll().addClass(classes);
    
    // If we have an input id, set the 'star' attribute equal to the the star
    // number that was clicked, and set the value equal to the rating value.
    if (id) { $('#' + id).attr('star', star).val(rating); }
    // If the widget is exposed, make a service call to save the rating.
    if (expose && entity_id) {
      // Note, the Services Fivestar module takes our rating and multiplies it
      // by 20, so we need to divide our rating by 20.
      // @TODO - is this a bug in that module?
      var data = {
        id: entity_id,
        rating: rating / 20,
        entity_type: entity_type,
        uid: Drupal.user.uid
      };
      fivestar_rate({
          data: JSON.stringify(data),
          success: function(result) {
            dpm(result);
          }
      });
    }

  }
  catch (error) { console.log('_fivestar_widget_click - ' + error); }
}

/**
 * Themes a fivestar widget.
 */
function theme_fivestar(variables) {
  try {
    //dpm('variables');
    //dpm(variables);
    var html = '';
    // Iterate over each star and place them into a controlgroup.
    var html = '<div class="fivestar" data-role="controlgroup" data-type="horizontal">';
    var link_classes = null;
    for (var star = 1; star <= variables.stars; star++) {
      link_classes = ' ui-btn ui-corner-all ui-icon-star ui-btn-icon-left ';
      // Compute the rating value of this star.
      var rating = fivestar_compute_rating(star, variables.base);
      // Set the hidden input's id as a string, if it exists.
      var input_id = null;
      if (variables.input_id) { input_id = "'" + variables.input_id + "'"; }
      // Set the entity id, if it exists.
      var entity_id = null;
      if (variables.entity_id) { entity_id = variables.entity_id; }
      // Set the expose bool, if it exists.
      var expose = false;
      if (variables.expose) { expose = variables.expose; }
      // If there is a current rating from the user, highlight it and each before it.
      // Otherwise check to see if there is a current average rating, then highlight
      // it and each before it.
      var active_classes = ' ui-btn-active ui-btn-persist ';
      if (variables.user_rated && rating <= variables.user_value) {
        link_classes += active_classes;
      }
      else if (!variables.user_rated && variables.rating && rating <= variables.rating) {
        link_classes += active_classes;
      }
      // Build the link options.
      var options = {
        attributes: {
          'class': link_classes,
          onclick: "_fivestar_widget_click(this, " +
            input_id + ", " +
            star + ", " +
            rating + ", " +
            "'" + variables.entity_type + "', " +
            entity_id + ', ' +
            expose +
          ")"
        }
      };
      // If it isn't exposed, remove the onclick handler.
      if (!expose) { delete(options.attributes.onclick); }
      // If the user already voted, and they're not allowed to revote, remove
      // the onclick handler.
      if (variables.user_rated && !variables.allow_revote) {
        delete(options.attributes.onclick);
      }
      html += l(star, null, options);
    }
    html += '</div>';
    return html;
  }
  catch (error) { console.log('theme_fivestar - ' + error); }
}

/**
 * Themes a fivestar average.
 */
function theme_fivestar_average(variables) {
  try {
    variables.attributes.class += ' fivestar_average ';
    var average = fivestar_compute_average(
      variables.average.value,
      variables.count.value,
      variables.fivestar.stars
    );
    var count = variables.count.value;
    var html = '<div ' + drupalgap_attributes(variables.attributes) + '>' +
      '<p>Average: ' + (average/count).toFixed(1) +
      ' (' +
        count + ' ' +
        drupalgap_format_plural(count, 'vote', 'votes')  +
      ')</p>'
    '</div>';
    
    return html;
  }
  catch (error) { console.log('theme_fivestar_average - ' + error); }
}

/**
 * The Fivestar Rate Service.
 */
function fivestar_rate(options) {
  try {
    options.method = 'POST';
    options.path = 'fivestar/rate.json';
    options.service = 'fivestar';
    options.resource = 'rate';
    Drupal.services.call(options);
  }
  catch (error) { console.log('fivestar_rate - ' + error); }
}

/**
 * The Fivestar Retrieve Service.
 */
function fivestar_retrieve(entity_type, entity_id, tag, uid, options) {
  try {
    options.method = 'GET';
    options.path = 'fivestar/' + entity_id + '.json';
    if (entity_type) { options.path += '&entity_type=' + encodeURIComponent(entity_type); }
    if (tag) { options.path += '&tag=' + encodeURIComponent(tag); }
    if (uid) { options.path += '&uid=' + encodeURIComponent(uid); }
    options.service = 'fivestar';
    options.resource = 'retrieve';
    Drupal.services.call(options);
  }
  catch (error) { console.log('fivestar_retrieve - ' + error); }
}

