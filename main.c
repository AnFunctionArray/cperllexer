#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#define _LARGEFILE64_SOURCE

#if !defined(_WIN32) & !defined(_WIN64)
#include <pcre2.h>
#else
#include <pcre/pcre2.h>
#endif
#include <stdio.h>
//#include <boost/preprocessor/stringize.hpp>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "main.h"

FILE *foutput;

int callout_test(pcre2_callout_block *a, void *b);

char *getnameloc(long long int ntocompare, struct calloutinfo nametable)
{
	PCRE2_SPTR tabptr = nametable.name_table;

	char *str = (char *)ntocompare;

	int namecount = nametable.namecount, n;

	for (; namecount--; tabptr += nametable.name_entry_size)
	{
		n = (tabptr[0] << 8) | tabptr[1];

		if (ntocompare > 0 && !strcmp(tabptr + 2, str))
			return n;
		else if (n == -ntocompare)
			return tabptr + 2;
	}

	return "";
}

int compile_pattern_and_execute(const char *pattern, const char *subject, int (*callback)(pcre2_callout_enumerate_block *, void *), size_t szpattern, size_t szsubject, int msgs, size_t *plen, int flags)
{
	int error, ncaptures, namecount, name_entry_size, n, rc;
	PCRE2_SIZE erroroffset, *povector;

	char *pstr;

	uint32_t ovectorcount;

	/*char* exprregex = "(\\b(\\w+)\\b)|(\\b(((0x)|(0b)|(0))(\\d+)|([0-9a-fA-F]))\\b)"
		"|(\\b\'(\\\\((x)|(b)|(0))(\\d+)|([0-9a-fA-F])|(\\')|(n)|(t))|(.)\'\\b)"
		"|(\\b\"(\\\\((x)|(b)|(0))(\\d+)|([0-9a-fA-F])|(\\\")|(n)|(t))|(.)\"\\b)";*/

	PCRE2_SPTR name_table, tabptr;

	pcre2_match_data *pmatch_data = pcre2_match_data_create(0xFFFF, 0);

	pcre2_match_context *match_context = pcre2_match_context_create(0);

	pcre2_code *pcode;

	PCRE2_UCHAR *pnewsubstr = pattern; //malloc(szpattern);

	PCRE2_SIZE newsubstrlen = szpattern;

	char errormsg[0xFF];

	struct calloutinfo nametable;

	//"\"(((\\\\((x)|(b)|(0))(\\d+)|([0-9a-fA-F]+)|(\\\")|(n)|(t))*(?C2))|((.*)(?C3)))*\"(?C1)"

	if (msgs)
		printf("%.*s\n", szsubject, subject);
	//pcode = pcre2_compile("\\s|\\n", PCRE2_ZERO_TERMINATED, 0, &error, &erroroffset, 0);

	//pcre2_substitute(pcode, pattern, szpattern, 0, PCRE2_SUBSTITUTE_GLOBAL, pmatch_data, match_context, "", 0, pnewsubstr, &newsubstrlen);

	//pcre2_match_context_free(match_context), pcre2_match_data_free(pmatch_data);

	//pmatch_data = pcre2_match_data_create(0xFFFF, 0);

	//match_context = pcre2_match_context_create(0);

	if (msgs)
		printf("%.*s\n\n", (unsigned int)newsubstrlen, pnewsubstr);

	//free(pattern);

	pcode = pcre2_compile(pnewsubstr, newsubstrlen, flags, &error, &erroroffset, 0);

	if (error != 100)
		pcre2_get_error_message(error, errormsg, 0xFF),
			printf("pattern error %d at %s : %.*s\n", error, errormsg, (unsigned int)(newsubstrlen - erroroffset), pnewsubstr + erroroffset);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMETABLE, &nametable.name_table);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMECOUNT, &nametable.namecount);

	pcre2_pattern_info(pcode, PCRE2_INFO_NAMEENTRYSIZE, &nametable.name_entry_size);

	nametable.pattern = pnewsubstr;
	nametable.szpattern = newsubstrlen;

	//static int dequeadd(val, pqueue, ptail, phead, empty, full, size)

	//dequeadd(nametable, nametablequeue, &ptail, &phead, &empty, &full, _countof(nametablequeue));

	pcre2_set_callout(match_context, callback, &nametable);

	if (msgs)
		printf("\n\n", 0);

	rc = pcre2_match(pcode, subject, szsubject, 0, PCRE2_PARTIAL_HARD, pmatch_data, match_context);

	printf("%d\n", rc);

	ovectorcount = pcre2_get_ovector_count(pmatch_data);

	povector = pcre2_get_ovector_pointer(pmatch_data);

	printf("full match is: %d - %d, %.*s\n", povector[0], povector[1], (unsigned int)(povector[1] - povector[0]),
		   subject + povector[0]);

	if (plen)
		*plen = povector[1] - povector[0];

	pcre2_match_data_free(pmatch_data);

	pcre2_match_context_free(match_context);

	return rc;
}
char *openfile(char *chname, size_t *szfileout)
{
	FILE *fregex = fopen(chname, "rt");
	size_t szfile;
	char *filecontent;

	fseek(fregex, 0, SEEK_END);
	fgetpos(fregex, &szfile);

	filecontent = malloc(szfile + !szfileout);

	rewind(fregex);

	fread(filecontent, szfile, 1, fregex);

	fclose(fregex);

	while (--szfile && (filecontent[szfile] == '\xCD' || filecontent[szfile] == '\x9'))
		; //printf(" %x ", (unsigned char)filecontent[szfile]);

	if (szfileout)
		*szfileout = szfile + 1;
	else
		filecontent[szfile + 1] = '\0';

	return filecontent;
}

#include "llvm/llvmgen.h"

secondmain(char* subject, size_t szsubject, char* pattern, size_t szpattern, char* modulename, size_t szmodulename)
{
	foutput = fopen("output.txt", "wt");
	startmodule(modulename, szmodulename);
	int stat = compile_pattern_and_execute(pattern, subject, callout_test, szpattern, szsubject, 1, 0, PATTERN_FLAGS);
	endmodule();
}

#include <EXTERN.h> /* from the Perl distribution     */
#include <perl.h>	/* from the Perl distribution     */
#include <XSUB.h>

XS__startmatching();

static void
	xs_init(pTHX)
{
	newXS("startmatching", XS__startmatching, __FILE__);
}

static PerlInterpreter *my_perl; /***    The Perl interpreter    ***/

int main(int argc, char **argv, char **env)
{
	PERL_SYS_INIT3(&argc, &argv, &env);
	my_perl = perl_alloc();
	perl_construct(my_perl);
	PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
	perl_parse(my_perl, xs_init, argc, argv, (char **)NULL);
	perl_run(my_perl);
	perl_destruct(my_perl);
	perl_free(my_perl);
	PERL_SYS_TERM();
	exit(EXIT_SUCCESS);
}

#if 0
main()
{
	size_t szfilepattern, szsubject;

	foutput = fopen("output.txt", "wt");

	char* pfilepattern = openfile(TEST_REGEX_FILE, &szfilepattern), * pfilesubject = openfile(TEST_FILE, &szsubject);
#ifndef TEST
	char *res = glueregexfile(TEST_REGEX_FILE);

	int stat = compile_pattern_and_execute(res, pfilesubject, callout_test, strlen(res), szsubject, 1, 0, PATTERN_FLAGS);
#else
	int stat = compile_pattern_and_execute(pfilepattern, pfilesubject, callout_test, szfilepattern, szsubject, 1, 0, PATTERN_FLAGS);
#endif

	fclose(foutput);

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
#endif

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