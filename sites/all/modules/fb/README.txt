Drupal for Facebook
-------------------

More information:
http://www.drupalforfacebook.org, http://drupal.org/project/fb

Branch: 7.x-4.x (version 4.x for Drupal 7.x)

This is an early release of modules/fb for developers.  Use extreme
caution when using this version on your website.  It is in a volatile
state of development.  Most users should wait until 4.0 release,
although it is unclear how long that will take.

To upgrade:
-----------

This version (4.x) is not a drop in replacement for the previous
version (3.x).  This is a rewrite, not backward compatible.  You may
find many features of the previousl version are not supported here.


To install:
-----------

Unlike previous versions of this module, you need not install
Facebook's PHP SDK.  There is nothing extra to download.

Simply enable the modules you need, as is normal when working with
Drupal.

See admin >> configuration >> facebook, for a variety of settings and
options.  More details here as the modules become more stable.


Canvas pages:
-------------

Modern Canvas Pages (a.k.a. apps on Facebook) are iframes.  So in the simplest cases you can simply refer to your Drupal URL on the Facebook Application Settings form.  However, if you want fb_canvas.module to provide some helpful features (i.e. rewriting links so they point to canvas page URLs) enable fb_canvas.module and do the following.

To best support Canvas Pages, modules/fb needs a change to your settings.php file.  At the end of that file, include fb_settings.inc.  The exact line you need to add depends on where you've placed modules/fb.  For most, this will work:

  include "sites/all/modules/fb/fb_settings.inc";

On your Facebook Application Settings form (on developers.facebook.com), specify a Canvas URL like "http://mydomain.com/path/to/drupal/fb__canvas/APP_ID".  Note you must change every part of that URL except the "http://" and the "fb__canvas".  So, for example,  apps.facebook.com/drupalforfacebook uses "http://www.drupalforfacebook.org/fb__canvas/4998126732/".

Note there are _two_ underbars in fb__canvas.  Specify a Secure Canvas URL if your site supports HTTPS.

Troubleshooting:
----------------

Enable fb_devel.module.  It may provide warnings you would not
otherwise see.

Check admin >> reports >> status report.  You may see errors or
warnings there.

Also check admin >> reports >> recent log messages.


Bug reports and feature requests may be submitted.  Check the issue
queue for similar issues before you submit, because often the answer
is already there.

http://drupal.org/project/issues/fb

If you do submit an issue, start the description with "I read the
README.txt from start to finish," and you will get a faster, more
thoughtful response.  Seriously, prove that you read this far.  And
read the instructions when submitting the issue, the form asks for
very specific information and if you provide that it will be easier to
answer questions and provide support.
