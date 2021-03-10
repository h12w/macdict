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
  if (name) CFArrayAppendValue(names, name);
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

DCSDictionaryRef copy_dic_by_name(CFStringRef name) {
  CFSetRef dics = DCSCopyAvailableDictionaries();
  CFIndex count = CFSetGetCount(dics);
  DCSDictionaryRef dics_arr[count];
  CFSetGetValues(dics, (void *)dics_arr);
  DCSDictionaryRef dic = NULL;
  for (int i = 0; i < count; i++) {
    DCSDictionaryRef d = dics_arr[i];
    CFStringRef n = DCSDictionaryGetShortName(d);
    if (!n) continue;
    if (CFStringCompare(name, n, 0) == kCFCompareEqualTo) {
      dic = d;
      break;
    }

    n = DCSDictionaryGetName(d);
    if (CFStringCompare(name, n, 0) == kCFCompareEqualTo) {
      dic = d;
      break;
    }
  }

  if (dic) {
    CFRetain(dic);
    CFRelease(dics);
  }

  return dic;
}

CFStringRef dicmd_copy_definition(CFStringRef term, CFStringRef dic_name, dicmd_output_format_t format) {
  DCSDictionaryRef dic = copy_dic_by_name(dic_name);
  if (!dic) {
    _apply_cstr(dic_name, ^(char *cstr) {
	fprintf(stderr, "Dictionary `%s' can not be found", cstr);
      });
    return CFSTR("");
  }

  CFRange range = DCSGetTermRangeInString(dic, term, 0);
  if (range.location == kCFNotFound) {
    _apply_cstr(term, ^(char *term_cstr) {
	_apply_cstr(dic_name, ^(char *dic_name_cstr){
	    fprintf(stderr, "`%s' can not be found in dictionary `%s'", term_cstr, dic_name_cstr);
	  });
      });
    return CFSTR("");
  }

  CFStringRef searchStr = CFStringCreateWithSubstring(kCFAllocatorDefault, term, range);
  CFArrayRef records = DCSCopyRecordsForSearchString(dic, searchStr, NULL, NULL);
  CFIndex count = CFArrayGetCount(records);
  CFTypeRef record_vals[count];
  CFArrayGetValues(records, CFRangeMake(0, count), (void *)record_vals);
  CFMutableStringRef ret = CFStringCreateMutable(kCFAllocatorDefault, 0);
  for (CFIndex i = 0; i < count; i++) {
    CFStringRef data = DCSRecordCopyData(record_vals[i], format);
    if (format == dicmd_output_format_text) {
      CFStringAppend(ret, data);
      if (i < count - 1) CFStringAppend(ret, CFSTR("\n\n---\n\n"));
    } else {
      CFStringAppendFormat(ret, NULL, CFSTR("<template>%@</template>"), data);
    }
    CFRelease(data);
  }

  CFRelease(searchStr);
  CFRelease(records);
  return ret;
}


void _apply_cstr(CFStringRef cfstr, void (^apply)(char *)) {
  char *cstr = (char *)CFStringGetCStringPtr(cfstr, kCFStringEncodingUTF8);
  Boolean needToFree = false;
  if (!cstr) {
    CFIndex len = CFStringGetLength(cfstr);
    CFIndex size = CFStringGetMaximumSizeForEncoding(len, kCFStringEncodingUTF8) + 1;
    char *buf = (char *)malloc(size);
    if (CFStringGetCString(cfstr, buf, size, kCFStringEncodingUTF8)) {
      cstr = buf;
      needToFree = true;
    }
  }
  apply(cstr);
  if (needToFree) free(cstr);
}
