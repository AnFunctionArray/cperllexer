#define PERL_NO_GET_CONTEXT
#define PERL_EUPXS_ALWAYS_EXPORT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#if !defined(_WIN32) & !defined(_WIN64)
#include <pcre2.h>
#else
#include <pcre2/pcre2.h>
#endif
#include "src/main.h"

MODULE = _6a4h8

PROTOTYPES: ENABLE

=pod
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

#SV *
void callout(SV *in, ...)
PREINIT:
    const char *argsarr[0xFF];
    size_t argsarrlen[0xFF];
CODE:
    {
        extern /*const char * */ void docall(const char **, size_t *);
        memset(argsarr, 0, sizeof argsarr);
        memset(argsarrlen, 0, sizeof argsarrlen);
        for(int i = 0; i < items; ++i)
            argsarr[i] = SvPVutf8(ST(i), argsarrlen[i]);
        docall(argsarr, argsarrlen);
        #RETVAL = newSVpv(callout_test(argsarr, argsarrlen), 0);
    }
#OUTPUT:
    #RETVAL
=cut

void callout(SV *in, HV *hash)
CODE:
    {
        extern void do_callout(SV* in, HV* hash);

        do_callout(in, hash);
    }

void startmodule(SV *in)
PREINIT:
    STRLEN len;
    char* s;
CODE:
    {
        void startmodule(const char* modulename, size_t szmodulename);
        s = SvPVutf8(in, len);
        
        startmodule(s, len);
    }