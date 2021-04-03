#pragma once
//#include <boost/preprocessor/facilities/expand.hpp>

#define TEST_REGEX_FILE "main.regex"
#define TEST_FILE "maintest.c"
#define PATTERN_FLAGS_BASE 0 | PCRE2_DUPNAMES  //| PCRE2_AUTO_CALLOUT
#define PATTERN_FLAGS PATTERN_FLAGS_BASE
//#define DONT_EXPAND

//#define DEBUG

//#define DEBUG_PHYSIC

#if !(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
#define HIDE_DETAILS
#endif

//#define DO_EXPAND

//#define SHOW_PATTERN

//#define SHOW_GROUP 27

#ifdef SHOW_GROUP
#define PATTERN_FLAGS PATTERN_FLAGS_BASE | PCRE2_AUTO_CALLOUT
#endif

#if defined(SHOW_GROUP) & !defined(SHOW_GROUP_LAST)
#define SHOW_GROUP_LAST SHOW_GROUP
#endif

//#define TEST
#ifdef TEST
#define TEST_REGEX_FILE "test.regex"
#define TEST_FILE "test.c"
#endif
	

struct calloutinfo { PCRE2_SPTR name_table, pattern; int namecount, name_entry_size; size_t szpattern; };

char* getnameloc(long long int ntocompare, struct calloutinfo nametable);

char* getnameloc2(long long int ntocompare, struct calloutinfo nametable, pcre2_callout_block* pcurrblock, int displ);

struct namelocopts {
	unsigned rev : 1;
	unsigned last : 1;
	unsigned dontsearchforclosest : 1;
};

char* getnameloc3(long long int ntocompare, struct calloutinfo nametable, pcre2_callout_block* pcurrblock, int displ, struct namelocopts);

int compile_pattern_and_execute(const char* pattern, const char* subject, int (*callback)(pcre2_callout_enumerate_block*, void*), size_t szpattern, size_t szsubject, int msgs, size_t* plen, int flags);

char* openfile(char* chname, size_t* szfileout);

char* glueregexfile(char* filename);

extern FILE* foutput;

#if !!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) & !defined(DEBUG)
#define printf(format, ...) (fprintf(foutput, format, __VA_ARGS__))//, printf(format, __VA_ARGS__))
#else
#define printf(format, ...) (fprintf(foutput, format, ##__VA_ARGS__), printf(format, ##__VA_ARGS__))
#endif