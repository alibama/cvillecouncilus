/**
 * @file
 * JS Implementation of OpenLayers behavior.
 */

Drupal.openlayers.addBehavior('openlayers_heatmaps_behavior', function (context, options) {
  var map = context.openlayers;
  var layers = [];

  for (var i in options.layers) {
    layeroptions = options.layers[i];
    if (layeroptions.layer == 1) {
      var selectedLayer = map.getLayersBy('drupalID', i);
      if (typeof selectedLayer[0] != 'undefined') {
        layers.push(selectedLayer[0]);
      }
    }
  }

  // If no layer is selected, just return.
  if (layers.length < 1) {
    return;
  }

  jQuery(layers).each(function(index, layer) {
    if (layer instanceof OpenLayers.Layer.Vector) {
      var layersoptions = options['layers'];

      var drupalID = layer.drupalID;
      var layeroptions = layersoptions[drupalID];
      var radius = parseInt(layeroptions.radius, 10);
      var intensity = parseInt(layeroptions.intensity, 10);
      var distance = parseInt(layeroptions.distance, 10);
      var threshold = parseInt(layeroptions.threshold, 10);
      var opacity = parseFloat(layeroptions.opacity, 10);
      var visibility = layeroptions.hide_original;
      var heatmap_name = layeroptions.heatmap_name;
      var enable_cluster = layeroptions.enable_cluster;

      var heatmapdata = { max:0, data:[] };

      var heatmap = new OpenLayers.Layer.Heatmap(heatmap_name, map, layer,
      {visible:true, radius:radius},
      {alwaysInRange:true, isBaseLayer:false, opacity:opacity, projection: map.getProjectionObject()});

      var cluster = new OpenLayers.Strategy.Cluster({'distance':distance, 'threshold':threshold});
      cluster.features = layer.features.slice();
      if (enable_cluster == 1) {
        layer.addOptions({ 'strategies': [cluster] });
        cluster.setLayer(layer);
        cluster.activate();
        cluster.cluster();
      }
      if (visibility == 1) {
        layer.setVisibility(false);
      }

      for (var j in cluster.features) {
        var feature = cluster.features[j];
        if (feature.CLASS_NAME == 'OpenLayers.Feature.Vector') {
          if (typeof feature.attributes.count == 'undefined') {
            count = intensity;
          } else {
            count = feature.attributes.count*intensity;
          }
          heatmapdata.data.push({lonlat:new OpenLayers.LonLat(feature.geometry.x, feature.geometry.y), count:count});
        }
      }

      heatmapdata.max = heatmapdata.data.length;
      heatmap.setDataSet(heatmapdata);
      map.addLayer(heatmap);
    }
  });
});
