#import "dicmd.h"

void list_dics(Boolean is_long_name) {
  CFArrayRef names = is_long_name ? dicmd_copy_names() : dicmd_copy_short_names();
  CFStringRef name_list_cfstr = CFStringCreateByCombiningStrings(kCFAllocatorDefault, names, CFSTR("\n"));
  _apply_cstr(name_list_cfstr, ^(char *cstr) {
      fprintf(stdout, "Available dictionaries:\n%s", cstr);
    });
  CFRelease(names);
  CFRelease(name_list_cfstr);
}

void usage() {
  fprintf(stderr, "\
Usage:\n\
macdict -t term -d dictionary_name -f format\n\
-t: required, the term you need to lookup\n\
-d: optional, default is `English', use `macdict -l 0/1' to get available dictionaries.\n\
-f: optional, default is 3\n\
0: pure html\n\
1: html with app css\n\
2: html with popover css\n\
3: text\n\n\
macdict -l 0/1\n\
list available dictionary names, 0 for short, 1 for long names");
}

int main(int argc, char *argv[]) {
  opterr = 0;

  int opt = 0;
  int list_flag = 0;
  int list_value = 0;
  char *term = NULL;
  char *dic_name = "English";
  dicmd_output_format_t format = dicmd_output_format_text;

  while ((opt = getopt(argc, argv, "l:t:d:f:h")) != -1) {
    switch (opt) {
    case 'l':
      list_flag = 1;
      list_value = atoi(optarg);
      break;
    case 't':
      term = optarg;
      break;
    case 'd':
      dic_name = optarg;
      break;
    case 'f':
      format = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(EXIT_SUCCESS);
    case '?':
      if (optopt == 't') {
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      }

      // value for -l is optional, default is 0
      if (optopt == 'l') {
	list_flag = 1;
      }

      if (optopt != 'f' && optopt != 'd' && optopt != 'l') {
	fprintf(stderr, "Unknown option: %c\n", optopt);
      }
      break;
    default:
      usage();
    }
  }

  if (list_flag) {
    list_dics(list_value);
    return EXIT_SUCCESS;
  }

  if (!term) {
    usage();
    exit(EXIT_FAILURE);
  }

  if (format > dicmd_output_format_text || format < dicmd_output_format_html)
    format = dicmd_output_format_text;

  CFStringRef term_cfstr = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, term, kCFStringEncodingUTF8, kCFAllocatorNull);
  CFStringRef dic_name_cfstr = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, dic_name, kCFStringEncodingUTF8, kCFAllocatorNull);
  CFStringRef def = dicmd_copy_definition(term_cfstr, dic_name_cfstr, format);
  _apply_cstr(def, ^(char *cstr) { fprintf(stdout, "%s", cstr); });
  CFRelease(def);
  CFRelease(term_cfstr);
  CFRelease(dic_name_cfstr);
  return EXIT_SUCCESS;
}
