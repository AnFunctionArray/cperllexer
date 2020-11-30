#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "lib/pcre2.h"
#include <stdio.h>
#include <string.h>

#define TEST_REGEX "(\\(((?<inner>(?>[^()]++))|(?1))*+(?C1)\\))"

#define TEST_STRING "\"tes t3\"(\"tes(t\\4\"(\"\\xF9\"))"

static struct { PCRE2_SPTR name_table; int namecount, name_entry_size; } nametable;

static int callout_test(pcre2_callout_block* a, void* b)
{
	int n;

	for (n = 0; n < a->capture_top; ++n)
			printf(
				"%d - %.*s\n",
				n,
				(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
				a->subject + a->offset_vector[2 * n]
			);
	return 0;
}

main()
{
	int error, rc;
	PCRE2_SIZE erroroffset;

	char* subject = TEST_STRING;

	pcre2_match_data* pmatch_data = pcre2_match_data_create(0xFFFF, 0);

	pcre2_match_context* match_context = pcre2_match_context_create(0);

	pcre2_set_callout(match_context, callout_test, 0);

	printf("%s\n", subject);


	pcre2_code* pcode = pcre2_compile(TEST_REGEX, PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	rc = pcre2_match(pcode, subject, PCRE2_ZERO_TERMINATED, 0, 0, pmatch_data, match_context);
}