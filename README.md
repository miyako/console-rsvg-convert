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

with MSVC projects from ImageMagick, the package contains ``rsvg, xml2, jp2, tiff, pango, cairo, jpeg, png16, glib, z``

PDF is created but some CSS fill colors such as ``rgb(242,247,252)`` are lost.
