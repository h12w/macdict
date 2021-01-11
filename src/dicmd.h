#import <CoreServices/CoreServices.h>

typedef enum {
  dicmd_output_format_html = 0,
  dicmd_output_format_html_with_app_css = 1,
  dicmd_output_format_html_with_popover_css = 2,
  dicmd_output_format_text = 3,
} dicmd_output_format_t;

CFArrayRef dicmd_copy_names(void);
CFArrayRef dicmd_copy_short_names(void);
CFStringRef dicmd_copy_definition(char *term, dicmd_output_format_t format);
