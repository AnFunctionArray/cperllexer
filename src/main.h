#pragma once
//#include <boost/preprocessor/facilities/expand.hpp>

#define TEST_REGEX_FILE "main.regex"
#define TEST_FILE "maintest.c"
#define PATTERN_FLAGS_BASE 0 | PCRE2_DUPNAMES | PCRE2_NO_AUTO_CAPTURE  //| PCRE2_AUTO_CALLOUT
#define MATCH_FLAGS 0 //| PCRE2_PARTIAL_HARD
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


#if 0
struct calloutinfo {
	PCRE2_SPTR name_table, pattern; uint32_t namecount, name_entry_size; size_t szpattern;
	pcre2_match_data* pmatchdata;
	pcre2_code* pcode;
};

char* getnameloc(long long int ntocompare, struct calloutinfo nametable);

char* getnameloc2(long long int ntocompare, struct calloutinfo nametable, pcre2_callout_block* pcurrblock, int displ);

struct namelocopts {
	unsigned rev : 1;
	unsigned last : 1;
	unsigned dontsearchforclosest : 1;
	unsigned printdispl;
};

char* getnameloc3(long long int ntocompare, struct calloutinfo nametable, pcre2_callout_block* pcurrblock, int displ, struct namelocopts);

int compile_pattern_and_execute(const char* pattern, const char* subject, int (*callback)(pcre2_callout_enumerate_block*, void*), size_t szpattern, size_t szsubject, int msgs, size_t* plen, int flags);

char* openfile(char* chname, size_t* szfileout);

char* glueregexfile(char* filename);

extern FILE* foutput, * foutput2;

#if !!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) & !defined(DEBUG)
#define printf(format, ...) (fprintf(foutput, format, __VA_ARGS__))//, printf(format, __VA_ARGS__))
#define printf3(format, ...) (fprintf(foutput2, format, __VA_ARGS__))
#define printf1p3(format, ...) (printf3(format, __VA_ARGS__), printf(format, __VA_ARGS__))
#else
#define printf(format, ...) ((printf)(format, __VA_ARGS__)) //(fprintf(foutput, format, ##__VA_ARGS__))//, printf(format, ##__VA_ARGS__))
#endif

#define printf2(format, ...) ((printf)(format, __VA_ARGS__))

#define GROUP_PTR_AND_SZ(n) a->subject + a->offset_vector[2 * (n)], (unsigned int)(a->offset_vector[2 * (n) + 1] - a->offset_vector[2 * (n)])
#define GROUP_SZ_AND_PTR(n) (unsigned int)(a->offset_vector[2 * (n) + 1] - a->offset_vector[2 * (n)]), a->subject + a->offset_vector[2 * (n)]
#define GROUP_SZ_AND_PTR_TRUNC(n) (unsigned int)(a->offset_vector[2 * (n) + 1] - a->offset_vector[2 * (n)]) & 0xFF, a->subject + a->offset_vector[2 * (n)]
#define CHECKED_PTR_AND_SZ_START_END(start, end) ((start) != -1 ? a->subject + start : (PCRE2_SPTR)""), \
							(end) != -1 ? (unsigned int)((end) - (start)) : 0
#define CHECKED_PTR_AND_SZ(n) CHECKED_PTR_AND_SZ_START_END(a->offset_vector[2 * (n)], a->offset_vector[2 * (n) + 1])

struct compiling_plain {
	struct {
		void (*vtbl[])(void*);
	}*lpVtbl;
};

#ifdef __cplusplus
struct parsing_plain {
	struct {
		void (*vtbl[])(void*, std::string &);
	}*lpVtbl;
};
#endif

struct retgetnamevalue {
	const char* ptr;
	size_t sz;
};
struct retgetnamevalue getnamevalue(const char* nametoget);
#endif

#define PTR_AND_SZ_N(n) pargs[n], szargs[n]
#define FIRST_ARG_PTR_AND_SZ PTR_AND_SZ_N(1)



#ifdef __cplusplus
#define INTRANSIT extern "C"
#else
#define INTRANSIT
#endif

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define DLL_IMPORT INTRANSIT __declspec(dllimport)
#define DLL_EXPORT INTRANSIT __declspec(dllexport)
#define DLL_LOCAL  INTRANSIT
#else
#if __GNUC__ >= 4
#define DLL_IMPORT INTRANSIT __attribute__ ((visibility ("default")))
#define DLL_EXPORT INTRANSIT __attribute__ ((visibility ("default")))
#define DLL_LOCAL  INTRANSIT __attribute__ ((visibility ("hidden")))
#else
#define DLL_IMPORT
#define DLL_EXPORT
#define DLL_LOCAL
#endif
#endif