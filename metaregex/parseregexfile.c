#include <stdio.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>
#include "../main.h"
#include <stdlib.h>

#define MAX_FACET_ADDED_SPACE 0xFFF

int callout_facets(pcre2_callout_block* a, void* b)
{
	struct calloutinfo* ptable = b;
	int n = getnameloc("facetgroup", *ptable) + 1;

	void addfacetor(const char* content, size_t szcontent);

	size_t szfilepattern = a->offset_vector[2 * n + 1] - a->offset_vector[2 * n];

	addfacetor(a->subject + a->offset_vector[2 * n], szfilepattern);

	return 0;
}

char* create_regex_facets(const char *regex, size_t *szregex, char **outprocessedfile, size_t *szoutfile)
{
	int error, ncaptures, namecount, name_entry_size, n, rc;
	PCRE2_SIZE erroroffset, * povector;

	char* pstr;

	uint32_t ovectorcount;

	PCRE2_SPTR name_table, tabptr;

	pcre2_match_data* pmatch_data = pcre2_match_data_create(0xFFFF, 0);

	pcre2_match_context* match_context = pcre2_match_context_create(0);

	pcre2_code* pcode;

	PCRE2_UCHAR* pnewsubstr[2], *ptmp;

	char errormsg[0xFF];

	struct calloutinfo nametable;

	size_t szstrin = *szregex;

	void beginfacetor();
	const char* retrievefacetor();

	beginfacetor();

	*szregex += MAX_FACET_ADDED_SPACE;

	1[pnewsubstr] = malloc(*szregex);

	pcode = pcre2_compile("\\s|\\n", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	error = pcre2_substitute(pcode, regex, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "", 0, 1[pnewsubstr], szregex);

	szstrin = *szregex;

	pcode = pcre2_compile(".*(?<facetgroup>[(]\\?<(\\w+)facet>)(?C1)(*F)|.*?(?&facetgroup)+", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	pcre2_set_callout(match_context, callout_facets, &nametable);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMETABLE, &nametable.name_table);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMECOUNT, &nametable.namecount);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMEENTRYSIZE, &nametable.name_entry_size);

	szstrin = *szregex;

	nametable.pattern = 1[pnewsubstr];
	nametable.szpattern = szstrin;

	rc = pcre2_match(pcode, 1[pnewsubstr], szstrin, 0, 0, pmatch_data, match_context);

	//if (rc != -1) {
	//	*szregex = 0; return "";
	//}

	pcre2_set_callout(match_context, 0, 0);

#define OPTION0 *pnewsubstr
#define OPTION1 1[pnewsubstr]

	OPTION0 = OPTION1;

	pcode = pcre2_compile("[(]\\?<((\\w)+?)[#]restrictoutsidefacet>", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

#define PRIOR OPTION1
#define CURR OPTION0
	*szregex += MAX_FACET_ADDED_SPACE;

	PRIOR = malloc(*szregex);

	*szoutfile = *szregex;

	error = pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "(?<$1>", PCRE2_ZERO_TERMINATED, PRIOR, szoutfile);

	pcode = pcre2_compile("[(]\\?<[#]restrictoutsidefacet>|[(]\\?<((\\w)+?)[#]restrictoutsidefacet>", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	szstrin = *szoutfile;

	*szoutfile += MAX_FACET_ADDED_SPACE;

	*outprocessedfile = malloc(*szoutfile);

	error = pcre2_substitute(pcode, PRIOR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "(", PCRE2_ZERO_TERMINATED, *outprocessedfile, szoutfile);

	szstrin = nametable.szpattern;

	error = pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_EXTENDED, pmatch_data, match_context, "(${1:+?<${1}>:}(*F)", PCRE2_ZERO_TERMINATED, PRIOR, szregex);

#define PRIOR OPTION0
#define CURR OPTION1

#define SWAP (ptmp = OPTION0, OPTION0 = OPTION1, OPTION1 = ptmp)

	szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	free(PRIOR); PRIOR = malloc(*szregex);

	pcode = pcre2_compile("(?<=[(])\\?<facet>", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "", 0, PRIOR, szregex);

	SWAP;

#define PRIOR OPTION0
#define CURR OPTION1

	*szregex += MAX_FACET_ADDED_SPACE;

	free(PRIOR); PRIOR = malloc(*szregex);

	pcode = pcre2_compile(retrievefacetor(), PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	error = pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "", 0, PRIOR, szregex);

	pcode = pcre2_compile("([(]\\?{.*?;\\s*}\\s*[)])", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

#define PRIOR OPTION1
#define CURR OPTION0

	szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	free(PRIOR); PRIOR = malloc(*szregex);

	pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "", 0, PRIOR, szregex);

	pcode = pcre2_compile("[(]\\?&((\\w(?!facet))++)[)]", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

#define PRIOR OPTION0
#define CURR OPTION1

	szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	free(PRIOR); PRIOR = malloc(*szregex);

	pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "(?&$1facet)", PCRE2_ZERO_TERMINATED, PRIOR, szregex);

#define PRIOR OPTION1
#define CURR OPTION0

	szstrin = *szregex;

	*szregex += MAX_FACET_ADDED_SPACE;

	free(PRIOR); PRIOR = malloc(*szregex);

	pcode = pcre2_compile("[(]\\?<((\\w(?!facet))+?)>", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	pcre2_substitute(pcode, CURR, szstrin, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "(?<$1facet>", PCRE2_ZERO_TERMINATED, PRIOR, szregex);

	pcre2_match_context_free(match_context), pcre2_match_data_free(pmatch_data); free(CURR);

	return PRIOR;
}

static char* pentry;

static size_t szentry;

void addregexfile(const char *content, size_t szcontent);

int callout_regex(pcre2_callout_block* a, void* b)
{
	struct calloutinfo* ptable = b;
	int n = getnameloc(a->callout_number == 1 ? "filename" : "entrygroup", *ptable), y = getnameloc("prefix", *ptable);

	size_t szfilepattern = a->offset_vector[2 * n + 1] - a->offset_vector[2 * n], szfacet = a->offset_vector[2 * y + 1] - a->offset_vector[2 * y];

	if (a->callout_number == 2)
	{
		char* filename = malloc(szentry = szfilepattern + sizeof "(^(?&))");

		sprintf(filename, "(%.*s(?&%.*s))", (unsigned int)szfacet, a->subject + a->offset_vector[2 * y],(unsigned int)szfilepattern, a->subject + a->offset_vector[2 * n]);

		pentry = filename;
		return 0;
	}

	char* filename = malloc(szfilepattern + 1);

	sprintf(filename, "%.*s", (unsigned int)szfilepattern, a->subject + a->offset_vector[2 * n]);

	char* pfilepattern = openfile(filename, &szfilepattern), *pprcoessedfile, *pregexfacet = (szfacet = szfilepattern, create_regex_facets(pfilepattern, &szfacet, &pprcoessedfile, &szfilepattern));

	free(pfilepattern);

	addregexfile(pprcoessedfile, szfilepattern);

	free(pprcoessedfile);

	addregexfile(pregexfacet, szfacet);

	free(pregexfacet);

	return 0;
}

char* glueregexfile(char* filename)
{
	void beginregex();
	const char* retrievefinalregex(const char* rest, size_t szrest);
	size_t plen;
	size_t szfilepattern, szregexpattern, szfacet;
	char* pfilepattern = openfile(filename, &szfilepattern), *pregexpattern = openfile("./metaregex/regex.regex", &szregexpattern), *facet, * pprcoessedfile;

	beginregex();

	compile_pattern_and_execute(pregexpattern, pfilepattern, callout_regex, szregexpattern, szfilepattern, 0, &plen, 0);

	szfacet = szfilepattern - plen;

	facet = create_regex_facets(pfilepattern + plen, &szfacet, &pprcoessedfile, &szfilepattern);

	free(pfilepattern);

	addregexfile(pprcoessedfile, szfilepattern);

	free(pprcoessedfile);

	addregexfile(facet, szfacet);

	free(facet);

	return retrievefinalregex(pentry, szentry);
}