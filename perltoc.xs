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

void startmatching(SV *in, SV *secondin)
PREINIT:
    STRLEN len;
    STRLEN secondlen;
    char* s;
    char* s1;
CODE:
    {
        int secondmain(char *subject, size_t szsubject, char *pattern, size_t szpattern);
        s = SvPVutf8(in, len);
        s1 = SvPVutf8(secondin, secondlen);
        secondmain(s, len, s1, secondlen);
    }