# console-rsvg-convert
[rsvg-convert](https://github.com/brion/librsvg/blob/master/rsvg-convert.c) clone

### Usage

```
-d, --dpi-x=<float> pixels per inch [optional; defaults to 90dpi]
-p, --dpi-y=<float> pixels per inch [optional; defaults to 90dpi]
-x, --x-zoom=<float> x zoom factor [optional; defaults to 1.0]
-y, --y-zoom=<float> y zoom factor [optional; defaults to 1.0]
-z, --zoom=<float> zoom factor [optional; defaults to 1.0]
-w, --width=<int> width [optional; defaults to the SVG's width]
-h, --height=<int> height [optional; defaults to the SVG's height]
-f, --format=[png, pdf, ps, svg] [optional; defaults to 'png']
-o, --output=<path> output filename [optional; defaults to stdout]
-b, --background-color=[black, white, #abccee, #aaa...] set the background color [optional; defaults to None]
-u, --base-uri=<uri>
-v, --version show version information
```

the following cli options are **NOT** implemented

```
-u, --unlimited
-f, --format=[eps, xml, recording]
-a, --keep-aspect-ratio whether to preserve the aspect ratio [optional; defaults to FALSE]
--keep-image-data
--no-keep-image-data
```

**Note**: it seems ``-a`` is not even implemented in the original ``rsvg-convert``...

### Enhancements

* ``UTF-8`` path support on Windows. (you might need to change the ``cmd.exe`` charset)

* Intermediate folders are automatically created.

* base64 data-uri images are rendered.

## Examples

multiple file input, single file output 

```
rsvg-convert page-1.svg page-2.svg page-3.svg --format=pdf --output=sample.pdf
```

short arguments

```
rsvg-convert page-1.svg page-2.svg page-3.svg -f pdf -o sample.pdf
```

``stdin`` input, ``stdout`` output (4D)

```
SET ENVIRONMENT VARIABLE("_4D_OPTION_CURRENT_DIRECTORY";$RSVG_DIR)

$CMD:="rsvg-convert -f pdf"

C_BLOB($SVG;$PDF;$ERR)
DOCUMENT TO BLOB(System folder(Desktop)+"sample.svg";$SVG)
LAUNCH EXTERNAL PROCESS($CMD;$SVG;$PDF;$ERR)
BLOB TO DOCUMENT(System folder(Desktop)+"sample.pdf";$PDF)
```

## Credits 

Windows port of ``getopt_long`` by [takamin](https://github.com/takamin/win-c)

## Build Information  

added ``RSVG_EXPORT`` to the following:  

``rsvg_cleanup``  
``rsvg_css_parse_color``  
``rsvg_handle_new_from_data``  
``rsvg_handle_new_from_free``  

Modified [``rsvg-io``](https://github.com/miyako/console-rsvg-convert/commit/472833462091dcff6a767d2447baadef34cc996a) to support base64 embedded ``png`` or ``jpeg`` images (data-uri)

You can create static library versions of ``glib`` with

```sh
meson build --default-library=both
```

However, static ``gdk_pixbuf`` will not work.

don't use homebrew cairo; instead build from source

```
--enable-xlib-xrender=no --without-x --enable-xlib=no --enable-xcb=no
```

source is [`2.37.0`](https://github.com/GNOME/librsvg/releases/tag/2.37.0)
