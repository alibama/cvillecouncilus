Openlayers Heatmaps
-------------------
Once enabled in Openlayers module, this module provides a new behavior.
This behavior will allows you to create heatmaps from an existing layer.

Dependencies:
-------------
 * Openlayers module
 * Heatmaps.js library

How to install:
---------------
 1. Install the Openlayers Heatmaps module as any other module.
 2. Install the heatmap.js library:
   * Download it from http://www.patrick-wied.at/static/heatmapjs/ and put it
     in Drupal's default libraries folder, OR
   * Use the makefile in the module to automatically download it by running:
     drush make --no-core -y --contrib-destination=. modules/contrib/openlayers_heatmaps/openlayers_heatmaps.make
     from within the sites/all directory.

Contributors:
-------------
 * Patrick Wied, author of the Heatmap.js library.
 * Antonio Santiago, fix and improved the Heatmap.js Openlayers integration.
 * Pol Dell'Aiera, author of the Drupal's module.
