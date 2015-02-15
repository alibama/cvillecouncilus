<?php
/**
 * @file
 * notifications_views_rules_taxonomy_terms.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function notifications_views_rules_taxonomy_terms_default_rules_configuration() {
  $items = array();
  $items['rules_views_rules_term_notifications'] = entity_import('rules_config', '{ "rules_views_rules_term_notifications" : {
      "LABEL" : "views rules term notifications",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "rules", "views_rules" ],
      "ON" : { "node_insert--issues" : { "bundle" : "issues" } },
      "IF" : [
        { "list_contains" : {
            "list" : [ "node:field-alignment-with-council-vis" ],
            "item" : [ "node:field-alignment-with-council-vis:0" ]
          }
        }
      ],
      "DO" : [
        { "VIEW LOOP" : {
            "VIEW" : "cross_join_profiles_and_content",
            "DISPLAY" : "views_rules_1",
            "USING" : { "nid" : [ "node:nid" ] },
            "ROW VARIABLES" : {
              "title" : { "title" : "Title" },
              "mail" : { "mail" : "(User) User: E-mail" },
              "field_background" : { "field_background" : "(field_alignment_with_council_vis) Background" },
              "path" : { "path" : "(field_alignment_with_council_vis) Content: Path" }
            },
            "DO" : [
              { "mail" : {
                  "to" : "[mail:value]",
                  "subject" : "[title:value] - content fixed :)",
                  "message" : "Hi Folks, this is the second go at automatic notifications on the site....  with some content and links :)\\r\\n\\u003Ca href=\\u0022https:\\/\\/www.cvillecouncil.us[path:value]\\u0022\\u003E [title:value]\\u003C\\/a\\u003E\\u003Cbr\\u003E\\u003Cbr\\u003E\\r\\n- go here to vote\\r\\n\\u003Cbr\\u003E\\u003Cbr\\u003E\\r\\nand a bit of history...\\r\\n[field-background:value]\\r\\n\\r\\nIf you\\u0027re receiving this email it\\u0027s because your interests (found here https:\\/\\/www.cvillecouncil.us\\/user\\/me\\/edit\\/main) are reflected in the present issue.  Please feel free to change your interests at any time :)",
                  "language" : [ "" ]
                }
              }
            ]
          }
        }
      ]
    }
  }');
  return $items;
}
