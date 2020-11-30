#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "lib/pcre2.h"
#include <stdio.h>
#include <boost/preprocessor/stringize.hpp>
#include <string.h>

#include "actualcontent.h"

struct calloutinfo { PCRE2_SPTR name_table, pattern; int namecount, name_entry_size; };

int getnameloc(const char* str, struct calloutinfo);

int compile_pattern_and_execute(const char* psubject, const char* subject, int (*callback)(pcre2_callout_enumerate_block*, void*));

int callout_test(pcre2_callout_block* a, void* b);

int getnameloc(const char* str, struct calloutinfo nametable)
{
	PCRE2_SPTR tabptr = nametable.name_table;

	int namecount = nametable.namecount, n;

	for (; namecount--; tabptr += nametable.name_entry_size)
	{
		n = (tabptr[0] << 8) | tabptr[1];

		if (!strcmp(tabptr + 2, str)) return n;
	}

	return 0;
}

int compile_pattern_and_execute(const char* pattern, const char* subject, int (*callback)(pcre2_callout_enumerate_block*, void*))
{
	int error, ncaptures, namecount, name_entry_size, n, rc;
	PCRE2_SIZE erroroffset;//, *povector;

	uint32_t ovectorcount;

	/*char* exprregex = "(\\b(\\w+)\\b)|(\\b(((0x)|(0b)|(0))(\\d+)|([0-9a-fA-F]))\\b)"
		"|(\\b\'(\\\\((x)|(b)|(0))(\\d+)|([0-9a-fA-F])|(\\')|(n)|(t))|(.)\'\\b)"
		"|(\\b\"(\\\\((x)|(b)|(0))(\\d+)|([0-9a-fA-F])|(\\\")|(n)|(t))|(.)\"\\b)";*/

	PCRE2_SPTR name_table, tabptr;

	pcre2_match_data* pmatch_data = pcre2_match_data_create(0xFFFF, 0);

	pcre2_match_context* match_context = pcre2_match_context_create(0);


	//"\"(((\\\\((x)|(b)|(0))(\\d+)|([0-9a-fA-F]+)|(\\\")|(n)|(t))*(?C2))|((.*)(?C3)))*\"(?C1)"

	printf("%s\n", subject);
	printf("%s\n", pattern);

	struct calloutinfo nametable;


	pcre2_code* pcode = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PATTERN_FLAGS, &error, &erroroffset, 0);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMETABLE, &nametable.name_table);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMECOUNT, &nametable.namecount);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMEENTRYSIZE, &nametable.name_entry_size);

	nametable.pattern = pattern;

	//static int dequeadd(val, pqueue, ptail, phead, empty, full, size)
	
	//dequeadd(nametable, nametablequeue, &ptail, &phead, &empty, &full, _countof(nametablequeue));

	pcre2_set_callout(match_context, callback, &nametable);

	rc = pcre2_match(pcode, subject, PCRE2_ZERO_TERMINATED, 0, 0, pmatch_data, match_context);

	pcre2_match_data_free(pmatch_data);

	pcre2_match_context_free(match_context);

	return rc;
}

main()
{
	return compile_pattern_and_execute(TEST_REGEX, TEST_STRING, callout_test);

#if 0
	ovectorcount = pcre2_get_ovector_count(pmatch_data);

	povector = pcre2_get_ovector_pointer(pmatch_data);

	for (n = 0; n < rc; ++n)
		printf(
			"%d - %.*s\n",
			n,
			(unsigned int)(povector[2 * n + 1] - povector[2 * n]),
			subject + povector[2 * n]
		);
#endif
}

/*
* pcre2_pattern_info(pcode, PCRE2_INFO_NAMETABLE, &name_table);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMECOUNT, &namecount);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMEENTRYSIZE, &name_entry_size);

	tabptr = name_table;

	for (;namecount--; tabptr += name_entry_size)
	{
		n = (tabptr[0] << 8) | tabptr[1];

		printf("(%d) %*s: %.*s\n", (tabptr[0] << 8) | tabptr[1], name_entry_size - 3, tabptr + 2,
			(unsigned int)(povector[2 * n + 1] - povector[2 * n]), subject + povector[2 * n]);
	}
*/

/*static int dequeadd(val, pqueue, ptail, phead, empty, full, size)
int **empty, **full;
struct nametable* pqueue, **ptail, **phead, val;
{
	if (*full) return -1;

	**ptail = val;

	if ((*ptail - pqueue) + 1 == size)
		*ptail = pqueue;
	else ++*ptail;

	if (*ptail == *phead) *full = 1;

	return 0;
}

static int dequeremove(val, pqueue, ptail, phead, empty, full, size)
int ** empty, ** full;
struct nametable* pqueue, ** ptail, ** phead, *val;
{
	if (*empty) return -1;

	if (val) *val = **phead;

	if ((*phead - pqueue) + 1 == size)
		*phead = pqueue;
	else ++* phead;

	if (*ptail == *phead) *empty = 1;

	return 0;
}

static struct nametable nametablequeue[0xFF], *ptail = nametablequeue, *phead = nametablequeue;

static int full = 0, empty = 1;*/