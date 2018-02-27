#include "rsvg-convert.h"

#ifdef _CONSOLE_

int main(int argc, char *argv[]){

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
    fprintf(stderr, "-b, --background-color=[black, white, #abccee, #aaa...] set the background color [optional; defaults to None]\n");
    fprintf(stderr, "-u, --base-uri=<uri>\n");
    fprintf(stderr, "-v, --version show version information\n");

    exit(1);
}


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
                width = atoi(optarg);
                break;
            case 'h':
                height = atoi(optarg);
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
                        background_color = rsvg_css_parse_color(background_color_str, FALSE);
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
         
         for(i = 0;i < argc;++i)
         {
			 input = argv[i];
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
                     
                     /* create surface only for page 1 */
                     if(!surface)
                     {
                         surface = create_surface(rsvg, width, height, x_zoom, y_zoom, keep_aspect_ratio, format, out, multiple_pages);
                         if(surface)
                         {
                             cr = cairo_create(surface);
                         }
                     }
                     
                     if(cr)
                     {
                         if(add_background)
                         {
                             set_background(rsvg, cr, background_color);
                         }
                         
                         create_page(rsvg, cr, surface, format, out, multiple_pages);

 /*
cairo_set_source_rgb(cr, 0, 0, 0);
cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
CAIRO_FONT_WEIGHT_NORMAL);
cairo_set_font_size(cr, 40.0);
cairo_move_to(cr, 10.0, 50.0);
cairo_show_text(cr, "Disziplin ist Macht.");
cairo_surface_write_to_png(surface, "image.png");
*/
                     }//cr

                     rsvg_handle_close(rsvg, &error);
                     rsvg_handle_free(rsvg);
                 }//rsvg
                 free(buf);
             }//fp
   
         }//for
         
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
