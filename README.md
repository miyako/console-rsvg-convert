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
-a, --keep-aspect-ratio whether to preserve the aspect ratio [optional; defaults to FALSE]
-b, --background-color=[black, white, #abccee, #aaa...] set the background color [optional; defaults to None]
-u, --base-uri=<uri>
-v, --version show version information
```

compared to GNU ``rsvg-convert`` ollowing cli options are supported  
```
-?, --help
-v, --version
-d, --dpi-x=<float> 
-p, --dpi-y=<float> 
-x, --x-zoom=<float>
-y, --y-zoom=<float> 
-z, --zoom=<float>
-w, --width=<int> 
-h, --height=<int>
-f, --format=[png, pdf, ps, svg] 
-u, --base-uri
```
the following cli options are **NOT** implemented  
```
-b, --background-color
-u, --unlimited
-f, --format=[eps, xml, recording]
--keep-image-data
--no-keep-image-data
```
