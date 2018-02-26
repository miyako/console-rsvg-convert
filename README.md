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

**Note**: it seems ``-a`` is not even implemented in the original ``rsvg-convert`` either...

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
## Build Information

Windows DLL were compiled using MinGW ``--host=x86_64-w64-mingw32``

``pango`` was compiles with flags ``--with-included-modules=arabic-lang,basic-fc,basic-win32,basic-coretext,indic-lang``

``gdk-pixbuf`` was compiles with flags ``--with-included-loaders=ani,icns,pcx,ras,tga,png,pnm,wbmp,xbm,xpm,qtif,bmp,gif,ico,jpeg,tiff --disable-glibtest ``

## Credits 

Windows port of ``getopt_long`` by [takamin](https://github.com/takamin/win-c)

---

## Notes

with MSVC projects from ImageMagick, the ``rsvg`` package contains:  
``rsvg, xml2, jp2, tiff, pango, cairo, jpeg, png16, glib, z``  
PDF is created but the paint server does not seem to be doing anything (all black).

with MSVC projects from ImageMagick, the ``cairo`` package contains:  
``pixman, freetype, fontconfig``  
but the results are the same.

with MINGW projects, the ``rsvg`` package contains:  
``croco, ffi, gdk-pixbuf, gio, glib, gmodule, gobject, harfbuzz, iconv, intl, pangocairo, pangoft2, pangowin32``  
but the results are the same. also direct ``text`` content (not ``tspan``) seem to cause critical error. (maybe ``rsvg-text.c``?)

siliencing the assertion errors in ``g_utf8_to_utf16`` and ``g_once_init_leave`` does not solve the problem.  

seem like a problem in the MINGW ``rsvg`` or one of the libraries linked to it (except ``glib``)  

evidently not ``iconv`` referenced from ``rsvg, xml2``  

need to study ``gdk-pixbuf, pangocairo, croco``


---

Based on libraries from ``ImageMagick-7.0.7-24``  

add ``RSVG_EXPORT`` to the following:  

```
rsvg_cleanup
rsvg_css_parse_color
rsvg_handle_new_from_data
rsvg_handle_new_from_free
```
