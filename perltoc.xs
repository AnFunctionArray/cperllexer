#define PERL_NO_GET_CONTEXT
#define PERL_EUPXS_ALWAYS_EXPORT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include <pcre2.h>
#include "main.h"

MODULE = _6a4h8

PROTOTYPES: ENABLE

void startmatching(SV *in, SV *in1, SV *in2)
PREINIT:
    STRLEN len;
    STRLEN secondlen;
    STRLEN thirdlen;
    char* s;
    char* s1;
    char* s2;
CODE:
    {
        int secondmain(char *subject, size_t szsubject, char *pattern, size_t szpattern, char *modulename, size_t szmodulename);
        s = SvPVutf8(in, len);
        s1 = SvPVutf8(in1, secondlen);
        s2 = SvPVutf8(in2, thirdlen);
        secondmain(s, len, s1, secondlen, s2, thirdlen);
    }