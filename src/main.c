#import "dicmd.h"

int main(int argc, char *argv[]) {
  opterr = 0;

  int opt = 0;
  int listFlag = 0;
  int listValue = 0;
  char *term = NULL;
  char *dicname = NULL;
  dicmd_output_format_t format = dicmd_output_format_text;

  while ((opt = getopt(argc, argv, "l:t:d:f:")) != -1) {
    switch (opt) {
    case 'l':
      listFlag = 1;
      listValue = atoi(optarg);
      break;
    case 't':
      term = optarg;
      break;
    case 'd':
      dicname = optarg;
    case 'f':
      format = atoi(optarg);
      break;
    case '?':
      if (optopt == 't') {
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      } else if (optopt != 'f' && optopt != 'd' && optopt != 'l') {
	fprintf(stderr, "Unknown option: %c\n", optopt);
      }
      break;
    }
  }

  if (listFlag) {
    CFArrayRef names = listValue ? dicmd_copy_names() : dicmd_copy_short_names();
    CFStringRef nameListStr = CFStringCreateByCombiningStrings(kCFAllocatorDefault, names, CFSTR("\n"));
    char *cstr = (char *)CFStringGetCStringPtr(nameListStr, kCFStringEncodingUTF8);
    Boolean needToFree = false;
    if (!cstr) {
      CFIndex len = CFStringGetLength(nameListStr);
      CFIndex size = CFStringGetMaximumSizeForEncoding(len, kCFStringEncodingUTF8) + 1;
      char *buf = (char *)malloc(size);
      if (CFStringGetCString(nameListStr, buf, size, kCFStringEncodingUTF8)) {
	cstr = buf;
	needToFree = true;
      }
    }
    fprintf(stdout, "Available dictionaries:\n%s", cstr);
    if (needToFree) free(cstr);
    CFRelease(names);
    CFRelease(nameListStr);
    return EXIT_SUCCESS;
  }

  if (!term) {
    fprintf(stderr, "\
Usage:\n\
dicmd -t term -d dictionary_name -f format\n\
-t: required, the term you need to lookup\n\
-d: optional, default is system default dictionary, use `dicmd -l 0/1' to get available dictionaries.\n\
-f: optional, default is 4\n\
1: pure html\n\
2: html with app css\n\
3: html with popover css\n\
4: text\n");
    exit(EXIT_FAILURE);
  }

  if (format > dicmd_output_format_text)
    format = dicmd_output_format_text;

  CFStringRef def = dicmd_copy_definition(term, format);
  fprintf(stdout, "%s", CFStringGetCStringPtr(def, kCFStringEncodingUTF8));

  return EXIT_SUCCESS;
}
