#define PERL_NO_GET_CONTEXT
#define PERL_EUPXS_ALWAYS_EXPORT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "src/main.h"

MODULE = AnFunctionArray

PROTOTYPES: ENABLE


extern startmatching(SV *in, SV *in1, SV *in2, SV *in3)
PREINIT:
    STRLEN len;
    STRLEN secondlen;
    STRLEN thirdlen;
    STRLEN fourthlen;
    char* s;
    char* s1;
    char* s2;
    char* s3;
CODE:
    {
        int secondmain(char *subject, size_t szsubject, char *pattern, size_t szpattern, char *modulename, size_t szmodulename,
            char*, size_t);
        s = SvPVutf8(in, len);
        s1 = SvPVutf8(in1, secondlen);
        s2 = SvPVutf8(in2, thirdlen);
        s3 = SvPVutf8(in3, fourthlen);
        secondmain(s, len, s1, secondlen, s2, thirdlen, s3, fourthlen);
    }

=pod
#SV *
callout(SV *in, ...)
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

extern callout(SV *in, HV *hash)
CODE:
    {
        extern void do_callout(SV* in, HV* hash);

        do_callout(in, hash);
    }

=pod
extern startmetaregex(SV *in, AV *hashes, SV *out)
CODE:
    {
        extern void dostartmetaregex(SV* in, AV* hashes, SV*);

        dostartmetaregex(in, hashes, out);
    }
=cut

extern startmodule(SV *in)
PREINIT:
    STRLEN len;
    char* s;
CODE:
    {
        void startmodule(const char* modulename, size_t szmodulename);
        s = SvPVutf8(in, len);
        
        startmodule(s, len);
    }
