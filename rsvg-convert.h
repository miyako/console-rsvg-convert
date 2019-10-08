#ifndef __WASVG_H__
#define __WASVG_H__

#define BUFFER_SIZE 8192

#include <stdlib.h>
#include <stdio.h>
 #ifndef __linux__
     #ifndef __APPLE__
 #define __WINDOWS__
 #include "Shlobj.h"//SHCreateDirectory
 #include "Windows.h"
     #endif
 #endif
#include "getopt.h"
#include <string.h>
#include <locale.h>
#include <rsvg.h>
#include "rsvg-css.h"
#include <cairo-ps.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <string>
#include <map>

 #define _CONSOLE_

struct option longopts[] = {
    {"help", no_argument, NULL, '?'},
    {"dpi-x", required_argument, NULL, 'd'},
    {"dpi-y", required_argument, NULL, 'p'},
    {"x-zoom", required_argument, NULL, 'x'},
    {"y-zoom", required_argument, NULL, 'y'},
    {"zoom", required_argument, NULL, 'z'},
    {"width", required_argument, NULL, 'w'},
    {"height", required_argument, NULL, 'h'},
    {"format", required_argument, NULL, 'f'},
    {"output", required_argument, NULL, 'o'},
    {"keep-aspect-ratio", required_argument, NULL, 'a'},
    {"background-color", required_argument, NULL, 'b'},
    {"base-uri", required_argument, NULL, 'u'},
    {"version", no_argument, NULL, 'v'},
    {0, 0, 0, 0}
};

#define VERSION "2.0"

#define OPT_LIST "d:p:x:y:z:w:h:f:o:a:b:u:v"

#ifdef __APPLE__
#include <cairo-quartz.h>
#else
#include <cairo-win32.h>
#endif

typedef enum {
    RSVG_OUT_PNG,
    RSVG_OUT_PS,
    RSVG_OUT_SVG,
    RSVG_OUT_PDF
} RsvgOutputType;

#define DEFAULT_SIZE 1024

static cairo_status_t rsvg_cairo_write_func(void *data, const unsigned char *bytes, unsigned int len);

int  rsvg_convert(int argc, char *argv[]);

#endif
