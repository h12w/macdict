#import "dicmd.h"

extern CFSetRef DCSCopyAvailableDictionaries(void);
extern CFStringRef DCSDictionaryGetName(DCSDictionaryRef dictionary);
extern CFStringRef DCSDictionaryGetShortName(DCSDictionaryRef dictionary);
extern CFArrayRef DCSCopyRecordsForSearchString(DCSDictionaryRef dictionary, CFStringRef string, void *, void *);
extern CFStringRef DCSRecordGetHeadword(CFTypeRef record);
extern CFStringRef DCSRecordCopyData(CFTypeRef record, long version);

void set_applier(const void *value, void *context) {
  void **ctx = context;
  CFMutableArrayRef names = (CFMutableArrayRef)ctx[0];
  Boolean isLongName = *(Boolean *)ctx[1];
  DCSDictionaryRef dic = (DCSDictionaryRef)value;
  CFStringRef name =
    isLongName
    ? DCSDictionaryGetName(dic)
    : DCSDictionaryGetShortName(dic);
  CFArrayAppendValue(names, name);
}

CFArrayRef dicmd_copy_names() {
  CFSetRef dics = DCSCopyAvailableDictionaries();
  CFMutableArrayRef names = CFArrayCreateMutable(kCFAllocatorDefault, CFSetGetCount(dics), &kCFTypeArrayCallBacks);
  Boolean isLongName = true;
  void * context[] = {names, &isLongName};
  CFSetApplyFunction(dics, set_applier, &context);
  CFRelease(dics);
  return names;
}

CFArrayRef dicmd_copy_short_names() {
  CFSetRef dics = DCSCopyAvailableDictionaries();
  CFMutableArrayRef names = CFArrayCreateMutable(kCFAllocatorDefault, CFSetGetCount(dics), &kCFTypeArrayCallBacks);
  Boolean isLongName = false;
  void * context[] = {names, &isLongName};
  CFSetApplyFunction(dics, set_applier, &context);
  CFRelease(dics);
  return names;
}

CFStringRef dicmd_copy_definition(char *term, dicmd_output_format_t format) {
  return CFSTR("hello world");
}
