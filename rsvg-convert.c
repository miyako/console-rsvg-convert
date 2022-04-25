#include "rsvg-convert.h"

#ifdef _CONSOLE_
int main(int argc, char *argv[])
{
	return rsvg_convert(argc, argv);
}
#else
INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT iCmdShow)
{
	return 0;
}
#endif

extern char *optarg;
extern int optind, opterr, optopt;

static void version(void){
    fprintf(stderr, "rsvg-convert version %s\n", VERSION);
    exit(1);
}

static bool is_not_svg(std::string path){
    
    bool is_svg = false;
    
    std::string extension = path.substr(path.find_last_of(".") + 1);
    
    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char c){ return std::tolower(c); });
    
    is_svg = (extension == "svg");
    
    return !is_svg;
}

static void usage(void){
    
    fprintf(stderr, "usage: rsvg-convert [options]\n");

    fprintf(stderr, "-d, --dpi-x=<float> pixels per inch [optional; defaults to 90dpi]\n");
    fprintf(stderr, "-p, --dpi-y=<float> pixels per inch [optional; defaults to 90dpi]\n");
    fprintf(stderr, "-x, --x-zoom=<float> x zoom factor [optional; defaults to 1.0]\n");
    fprintf(stderr, "-y, --y-zoom=<float> y zoom factor [optional; defaults to 1.0]\n");
    fprintf(stderr, "-z, --zoom=<float> zoom factor [optional; defaults to 1.0]\n");
    fprintf(stderr, "-w, --width=<int> width [optional; defaults to the SVG's width]\n");
    fprintf(stderr, "-h, --height=<int> height [optional; defaults to the SVG's height]\n");
    fprintf(stderr, "-f, --format=[png, pdf, ps, svg] [optional; defaults to 'png']\n");
    fprintf(stderr, "-o, --output=<path> output filename [optional; defaults to stdout]\n");
    fprintf(stderr, "-a, --keep-aspect-ratio whether to preserve the aspect ratio [optional; defaults to FALSE]\n");
    fprintf(stderr, "-b, --background-color=[black, white, #abccee, #aaa...] set the background color [optional; defaults to none]\n");
    fprintf(stderr, "-u, --base-uri=<uri>\n");
    fprintf(stderr, "-v, --version show version information\n");

    exit(1);
}

#ifdef __APPLE__
#include <Foundation/Foundation.h>
void create_parent_folder(const char *utf8_path){
	
	NSString *filePath = (NSString *)CFBridgingRelease(CFStringCreateWithFileSystemRepresentation(kCFAllocatorDefault, utf8_path));
    
    switch ([[filePath pathComponents]count]) {
            
        case 0:
        case 1:

            break;
        default:
        {
            NSString *path = (NSString *)CFBridgingRelease(CFStringCreateWithFileSystemRepresentation(kCFAllocatorDefault, [[filePath stringByDeletingLastPathComponent]fileSystemRepresentation]));
            [[NSFileManager defaultManager] createDirectoryAtPath:path
                                       withIntermediateDirectories:YES
                                                        attributes:nil
                                                             error:NULL];
        }
            break;
    }

}
#endif

#ifdef __WINDOWS__
void create_parent_folder(const char *utf8_path){
    wchar_t fDrive[_MAX_DRIVE], fDir[_MAX_DIR], fName[_MAX_FNAME], fExt[_MAX_EXT];
    wchar_t utf16_path[_MAX_PATH] = {0};
    size_t utf8_len = strlen(utf8_path);
    size_t pos = 0;
    int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8_path, utf8_len, NULL, 0);
    if(len){
        if(MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8_path, utf8_len, (LPWSTR)utf16_path, len)){
            _wsplitpath_s(utf16_path, fDrive, fDir, fName, fExt);
            pos = wcslen(fDrive) + wcslen(fDir);
            utf16_path[pos] = 0x0;
            SHCreateDirectory(NULL, (PCWSTR)utf16_path);
        }
    }
}
 FILE *ufopen(const char *utf8_path, const wchar_t *mode){
	 create_parent_folder(utf8_path);
    wchar_t utf16_path[_MAX_PATH] = {0};
    size_t utf8_len = strlen(utf8_path);
    int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8_path, utf8_len, NULL, 0);
    if(len){
        if(MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8_path, utf8_len, (LPWSTR)utf16_path, len)){
            return _wfopen(utf16_path, mode);
        }
    }
    return 0;
}
#endif

void set_background(RsvgHandle *rsvg,
                    cairo_t *cr,
                    guint32 background_color){
    cairo_set_source_rgb(cr,
                         ((background_color >> 16) & 0xFF) / 255.0,
                         ((background_color >> 8) & 0xFF) / 255.0,
                         ((background_color >> 0) & 0xFF) / 255.0);
    RsvgDimensionData dimensions;
    rsvg_handle_get_dimensions(rsvg, &dimensions);
    cairo_rectangle (cr, 0, 0, dimensions.width, dimensions.height);
}

void create_page(RsvgHandle *rsvg,
                 cairo_t *cr,
                 cairo_surface_t *surface,
                 RsvgOutputType format,
                 FILE *out,
                 int multiple_pages){

					 try{
   rsvg_handle_render_cairo (rsvg, cr);
					 }catch(...)
					 {};
    
    switch (format)
    {
        case RSVG_OUT_PNG:
            if(!multiple_pages) cairo_surface_write_to_png_stream (surface, rsvg_cairo_write_func, (void *)out);
            break;
        case RSVG_OUT_SVG:
            if(!multiple_pages) cairo_show_page(cr);
            break;
        case RSVG_OUT_PDF:
        case RSVG_OUT_PS:
            cairo_show_page(cr);
            break;
    }
    
}

void cairo_modify_surface(RsvgHandle *rsvg,
                          cairo_surface_t *surface,
                          int width,
                          int height,
                          double x_zoom,
                          double y_zoom,
                          int keep_aspect_ratio,
                          RsvgOutputType format) {
    
    RsvgDimensionData dimensions = {0};
    
    rsvg_handle_get_dimensions (rsvg, &dimensions);
    
    if((dimensions.width != 0) && (width != -1))
    dimensions.width = width * x_zoom;
    
    if((dimensions.height != 0) && (height != -1))
    dimensions.height = height * y_zoom;
    
    //TODO:keep_aspect_ratio not implemented
    
    if((dimensions.height == 0) || (dimensions.width == 0))
    dimensions.height = dimensions.width = DEFAULT_SIZE;
    
    if(surface) {
        switch(format)
        {
             case RSVG_OUT_PDF:
                cairo_pdf_surface_set_size(surface, dimensions.width, dimensions.height);
                break;
                default:
                break;
        }
    }
}

cairo_surface_t *create_surface(RsvgHandle *rsvg,
                                int width,
                                int height,
                                double x_zoom,
                                double y_zoom,
                                int keep_aspect_ratio,
                                RsvgOutputType format,
                                FILE *out,
                                int multiple){

    cairo_surface_t *surface = NULL;

	RsvgDimensionData dimensions = {0};

    rsvg_handle_get_dimensions (rsvg, &dimensions);

	if((dimensions.width != 0) && (width != -1))
		dimensions.width = width * x_zoom;

	if((dimensions.height != 0) && (height != -1))
		 dimensions.height = height * y_zoom;

	//TODO:keep_aspect_ratio not implemented

	if((dimensions.height == 0) || (dimensions.width == 0)) 
		dimensions.height = dimensions.width = DEFAULT_SIZE; 

    switch(format)
    {
        case RSVG_OUT_PNG:
            if(!multiple) surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, dimensions.width, dimensions.height);
            break;
        case RSVG_OUT_PS:
            surface = cairo_ps_surface_create_for_stream(rsvg_cairo_write_func,
                                                         (void *)out,
                                                         dimensions.width, dimensions.height);
            break;
        case RSVG_OUT_SVG:
            if(!multiple) surface = cairo_svg_surface_create_for_stream(rsvg_cairo_write_func,
                                                          (void *)out,
                                                          dimensions.width, dimensions.height);
            break;
        case RSVG_OUT_PDF:
            surface = cairo_pdf_surface_create_for_stream(rsvg_cairo_write_func,
                                                          (void *)out,
                                                          dimensions.width, dimensions.height);
            break;
    }
    
    return surface;
}

int rsvg_convert(int argc, char *argv[])
{
    /* defaults */
    double dpi_x = -1.0;
    double dpi_y = -1.0;
    int width = -1;
    int height = -1;
    
    std::map<int, int>widths;
    std::map<int, int>heights;
    
    double x_zoom = 1.0;
    double y_zoom = 1.0;
    double zoom = 0;
    int keep_aspect_ratio = 1;
    int input_is_stream = 1;
    int output_is_stream = 1;
    int add_background = 0;
    int multiple_pages = 0;
    guint32 background_color = 0;
    char *background_color_str = NULL;
    RsvgOutputType format = RSVG_OUT_PNG;
    char *output = NULL;
	char *input = NULL;
    char *uri = NULL;
    unsigned int i = 0;
    
    int opt, longoptind;
    
    while ((opt = getopt_long(argc, argv, OPT_LIST, longopts, &longoptind)) != -1)
    {
        switch(opt)
        {
                /* "d:p:x:y:z:w:h:f:o:a:b:u:v" */
            case 'd':
                if(optarg) dpi_x = atof(optarg);
                break;
            case 'p':
                if(optarg) dpi_y = atof(optarg);
                break;
            case 'x':
                if(optarg) x_zoom = atoi(optarg);
                break;
            case 'y':
                if(optarg) y_zoom = atoi(optarg);
                break;
            case 'z':
                if(optarg) x_zoom = y_zoom = zoom = atoi(optarg);
                break;
            case 'w':
            {
                std::string ww = (optarg);
                size_t f = ww.find(':');
                if(f != std::string::npos)
                {
                    int p  = atoi(ww.substr(0, f).c_str());
                    int v = atoi(ww.substr(f+1).c_str());
                    widths.insert(std::map<int, int>::value_type(p, v));
                }else{
                  width = atoi(optarg);
                }
                
            }
                break;
            case 'h':
            {
                std::string hh = (optarg);
                size_t f = hh.find(':');
                if(f != std::string::npos)
                {
                    int p  = atoi(hh.substr(0, f).c_str());
                    int v = atoi(hh.substr(f+1).c_str());
                    heights.insert(std::map<int, int>::value_type(p, v));
                }else{
                    height = atoi(optarg);
                }
            }
                break;
            case 'f':
                if (optarg)
                {
                    if (!strcmp(optarg, "ps")) format = RSVG_OUT_PS;
                    else if (!strcmp(optarg, "svg")) format = RSVG_OUT_SVG;
                    else if (!strcmp(optarg, "pdf")) format = RSVG_OUT_PDF;
                    else if (!strcmp(optarg, "png")) format = RSVG_OUT_PNG;
                }
                break;
            case 'o':

                output = optarg;

                output_is_stream = 0;
                break;
            case 'a':
                if(optarg) keep_aspect_ratio = atoi(optarg);
                break;
            case 'b':
                if (optarg)
                {
                    if (strcmp(optarg, "none"))
                    {
						background_color_str = optarg;
//#if WIN32
						background_color = rsvg_css_parse_color(background_color_str, FALSE);
//#else
//                        RsvgCssColorSpec background_color_ = rsvg_css_parse_color_(background_color_str);
//                        if (background_color_.kind == RSVG_CSS_COLOR_SPEC_ARGB)
//                        {
//                            background_color = background_color_.argb;
//                        }
//#endif
                        add_background = 1;
                    }
                }
                break;
            case 'u':
                uri = optarg;
                break;
            case 'v':
                version();
                break;
            default:
                usage();
                break;
        }
        
    }

    argc -= optind;
    argv += optind;
    
    if(argc) input_is_stream = 0;
    
    RsvgHandle *rsvg;
    cairo_surface_t *surface = NULL;
    cairo_t *cr = NULL;
    GError *error = NULL;
    
    FILE *out = NULL;
    
    if(!output_is_stream)
    {
#ifdef __WINDOWS__
		 out = ufopen(output, L"wb");
#else
			create_parent_folder(output);
			out = fopen(output, "wb");
#endif
        if(!out)
        {
            exit(1);
        }
    }//!output_is_stream
    else
    {
        out = stdout;
    }//output_is_stream
    
     rsvg_set_default_dpi_x_y (dpi_x, dpi_y);
    
     if(input_is_stream)
     {
         unsigned int size = BUFFER_SIZE;
         guint8 *buf = (guint8 *)calloc(size, sizeof(char));
         
         if(!buf)
         {
             exit(1);
         }
         
         unsigned int ret, i = 0;
         
         while((ret = (unsigned int)fread(&buf[i], sizeof(char), BUFFER_SIZE, stdin)) > 0)
         {
             i += ret;
             size += BUFFER_SIZE;
             buf = (guint8 *)realloc(buf, size);
             if(!buf)
             {
                 exit(1);
             }
         }
         
         rsvg = rsvg_handle_new_from_data((const guint8 *)buf, i, &error);
         
         if(rsvg)
         {
             if(uri) rsvg_handle_set_base_uri(rsvg, uri);
             surface = create_surface(rsvg, width, height, x_zoom, y_zoom, keep_aspect_ratio, format, out, 0);
             
             if(surface)
             {
                 cr = cairo_create(surface);
                 
                 if(cr)
                 {
                     if(add_background)
                     {
                         set_background(rsvg, cr, background_color);
                     }
                     create_page(rsvg, cr, surface, format, out, multiple_pages);
                 }//cr
                 
             }//surface
             rsvg_handle_close(rsvg, &error);
             rsvg_handle_free(rsvg);
         }//rsvg
         free(buf);
     }//input_is_stream
     else
     {
         multiple_pages = (argc > 1);
         
         std::vector<std::string> paths;
         
         for(i = 0;i < argc;++i)
         {
             input = argv[i];
             
             if(is_not_svg(input)){
                 
#ifdef __WINDOWS__
         FILE *fp = ufopen(input, L"rb");
#else
        FILE *fp = fopen(input, "rb");
#endif
                 if(fp)
                 {
                     char line[MAX_LINE];
                     while(fgets(line, MAX_LINE, fp)){
                         line[strcspn(line, "\r\n")] = 0;
                         paths.push_back(line);
                     }
                     fclose(fp);
                 }
                 
             }else{
                 paths.push_back(input);
   
             }
             
         }
         
         for (std::vector<std::string>::iterator it = paths.begin() ; it != paths.end(); ++it){
             
             input = (char *)it->data();
             
#ifdef __WINDOWS__
         FILE *fp = ufopen(input, L"rb");
#else
        FILE *fp = fopen(input, "rb");
#endif

             if(fp)
             {
                 unsigned int ret, p = 0;

                 unsigned int size = BUFFER_SIZE;
                 guint8 *buf = (guint8 *)calloc(size, sizeof(char));
                 
                 if(!buf)
                 {
                     exit(1);
                 }
                 
                 while((ret = (unsigned int)fread(&buf[p], sizeof(char), BUFFER_SIZE, fp)) > 0)
                 {
                     p += ret;
                     size += BUFFER_SIZE;
                     buf = (guint8 *)realloc(buf, size);
                     if(!buf)
                     {
                         exit(1);
                     }
                 }
                 
                 fclose(fp);

                 rsvg = rsvg_handle_new_from_data((const guint8 *)buf, p, &error);

                 if(rsvg)
                 {
                     if(uri) rsvg_handle_set_base_uri(rsvg, uri);
                     
                     int page_width = width;
                     int page_height = height;
                     
                     std::map<int, int>::iterator pos;
                     
                     pos = widths.find(i+1);
                     if(pos != widths.end()) {
                         page_width = pos->second;
                     }
                     
                     pos = heights.find(i+1);
                     if(pos != heights.end()) {
                         page_height = pos->second;
                     }

                     if(!surface)
                     {
                         surface = create_surface(rsvg, page_width, page_height,
                                                  x_zoom, y_zoom, keep_aspect_ratio,
                                                  format, out, multiple_pages);
                         if(surface)
                         {
                             cr = cairo_create(surface);
                         }
                     }else {
                         cairo_modify_surface(rsvg, surface,
                                              page_width, page_height,
                                              x_zoom, y_zoom,
                                              keep_aspect_ratio, format);
                     }
                     
                     if(cr)
                     {
                         if(add_background)
                         {
                             set_background(rsvg, cr, background_color);
                         }
                         
                         create_page(rsvg, cr, surface, format, out, multiple_pages);

                     }//cr

                     rsvg_handle_close(rsvg, &error);
                     rsvg_handle_free(rsvg);
                     
                 }//rsvg
                 free(buf);
             }//fp
         }

     }//!input_is_stream
    
    if(cr)
    {
        cairo_destroy(cr);
    }
    
    if(surface)
    {
        cairo_surface_destroy(surface);
    }
    
    if(!output_is_stream)
    {
        fclose(out);
    }
    
	return 0;
}

static cairo_status_t rsvg_cairo_write_func(void *data, const unsigned char *bytes, unsigned int len)
{	    
    if(len)
	{  
        fwrite(bytes, 1, len, (FILE *)data);  
    }    
    
    return CAIRO_STATUS_SUCCESS;
}
