// Create global variables to hold onto the coordinates and the map.
var _forlaura_user_latitude = null;
var _forlaura_user_longitude = null;
var _forlaura_map = null;

/**
 * Implements hook_menu().
 */

function forlaura_menu() {
  try {
    var items = {};
    items['map'] = {
      title: 'Map',
      page_callback: 'forlaura_map',
      pageshow: 'forlaura_map_pageshow'
    };
    return items;
  }
  catch (error) { console.log('forlaura_menu - ' + error); }
}

/**
 * The map page callback.
 */
function forlaura_map() {
  try {
    var content = {};
    var map_attributes = {
      id: 'forlaura_map',
      style: 'width: 100%; height: 320px;'
    };

content['find_nearby_locations'] = {
  theme: 'button',
  text: 'Find Nearby Locations',
  attributes: {
    onclick: "forlaura_map_button_click()",
    'data-theme': 'b'
  }
};


    content['map'] = {
      markup: '<div ' + drupalgap_attributes(map_attributes) + '></div>'
    };

content['location_results'] = {
  theme: 'jqm_item_list',
  items: [],
  attributes: {
    id: 'location_results_list'
  }
};
    return content;
  }
  catch (error) { console.log('forlaura_map - ' + error); }
}





/**
 * The "Find Nearby Locations" click handler.
 */
function forlaura_map_button_click() {
  try {
    // Build the path to the view to retrieve the results.
    var range = 40000; // Search within a 4 mile radius, for illustration purposes.
    var path = 'where-stuff-lives.json/' +
      _forlaura_user_latitude + ',' + _forlaura_user_longitude + '_' + range;
      
    // Call the server.
    views_datasource_get_view_result(path, {
        success: function(data) {
          
          if (data.nodes.length == 0) {
            drupalgap_alert('Sorry, we did not find any nearby locations!');
            return;
          }

          // Iterate over each spot, add it to the list and place a marker on the map.
          var items = [];
          $.each(data.nodes, function(index, object) {
              
              // Render a nearby location, and add it to the item list.
              var row = object.node;
              var image_html = theme('image', { path: row.field_where_its_at_image.src });
              var distance =
                row.field_geofield_distance + ' ' +
                drupalgap_format_plural(row.field_geofield_distance, 'yard', 'yards');
              var description =
                '<h2>' + distance + '</h2>' +
                '<p>' + row.type + '</p>';
              var link = l(image_html + description, 'node/' + row.nid);
              items.push(link);
              
              // Add a marker on the map for the location.
              var locationLatlng = new google.maps.LatLng(row.latitude, row.longitude);
              var marker = new google.maps.Marker({
                  position: locationLatlng,
                  map: _forlaura_map,
                  data: row
              });
              
          });
          drupalgap_item_list_populate("#location_results_list", items);

        }
    });
  }
  catch (error) { console.log('_forlaura_map_button_click - ' + error); }
}




/**
 * The map pageshow callback.
 */
function forlaura_map_pageshow() {
  try {
    navigator.geolocation.getCurrentPosition(
      
      // Success.
      function(position) {

        // Set aside the user's position.
        _forlaura_user_latitude = position.coords.latitude;
        _forlaura_user_longitude = position.coords.longitude;
        
        // Build the lat lng object from the user's current position.
        var myLatlng = new google.maps.LatLng(
          _forlaura_user_latitude,
          _forlaura_user_longitude
        );
        
        // Set the map's options.
        var mapOptions = {
          center: myLatlng,
          zoom: 11,
          mapTypeControl: true,
          mapTypeControlOptions: {
            style: google.maps.MapTypeControlStyle.DROPDOWN_MENU
          },
          zoomControl: true,
          zoomControlOptions: {
            style: google.maps.ZoomControlStyle.SMALL
          }
        };
        
        // Initialize the map, and set a timeout to resize it properly.
        _forlaura_map = new google.maps.Map(
          document.getElementById("forlaura_map"),
          mapOptions
        );
        setTimeout(function() {
            google.maps.event.trigger(_forlaura_map, 'resize');
            _forlaura_map.setCenter(myLatlng);
        }, 500);
        
        // Add a marker for the user's current position.
        var marker = new google.maps.Marker({
            position: myLatlng,
            map: _forlaura_map,
            icon: 'http://maps.google.com/mapfiles/ms/icons/blue-dot.png'
        });
        
      },
      
            // Error
      function(error) {
        
        // Provide debug information to developer and user.
        console.log(error);
        drupalgap_alert(error.message);
        
        // Process error code.
        switch (error.code) {

          // PERMISSION_DENIED
          case 1:
            break;

          // POSITION_UNAVAILABLE
          case 2:
            break;

          // TIMEOUT
          case 3:
            break;

        }

      },
      
      // Options
      { enableHighAccuracy: true }
      
    );
  }
  catch (error) { console.log('forlaura_map_pageshow - ' + error); }
}
