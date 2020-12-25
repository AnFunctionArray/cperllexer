#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "lib/pcre2.h"
#include <stdio.h>
//#include <boost/preprocessor/stringize.hpp>
#include <string.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "llvm/llvmgen.h"
#include <setjmp.h>

enum
{
	maxsize = 0xFFF
};

struct match
{
	int n;
	char *startmatch, *namedcpture;
};

struct debugstr
{
	int last_capture_n;
	char *pattern, *patternrest, *matching;
	struct match matches[maxsize];
	int ba, b;
} tmp;

static struct debugstr debugarray[maxsize];

static struct match *currmatch = tmp.matches;
static struct debugstr *currdebug = debugarray;

void debug_insert_common(int nlast, unsigned int a, char *pa, unsigned int b, char *pb, unsigned int c, char *pc, int ba)
{
	memset(&tmp, 0, sizeof tmp);
	tmp.last_capture_n = nlast;
	tmp.pattern = malloc(a + 1);
	sprintf(tmp.pattern, "%.*s", a, pa);
	tmp.patternrest = malloc(b + 1);
	sprintf(tmp.patternrest, "%.*s", b, pb);
	tmp.matching = malloc(c + 1);
	sprintf(tmp.matching, "%.*s", c, pc);
	tmp.b = b;
	tmp.ba = ba;
	currmatch = tmp.matches;
}

void debug_insert_match(int n, unsigned int a, char *pa, char *pnamedcapture)
{
	struct match match;

	match.n = n;
	match.startmatch = malloc(a + 1);
	sprintf(match.startmatch, "%.*s", a, pa);
	match.namedcpture = pnamedcapture;
	*currmatch++ = match;
}

extern void debug()
{
	*currdebug++ = tmp;

	//if (!(tmp.ba >= 17 && tmp.b <= 92))
	//_sleep(0);
	return;
}

int callout_test(pcre2_callout_block *a, void *b)
{
	struct calloutinfo *ptable = b;
	int n = //a->callout_number == 2 ? getnameloc("inner", *ptable) : getnameloc("middle", *ptable);
		//a->callout_number == 1 ? getnameloc("escape", *ptable) : a->callout_number == 5 ? getnameloc("numberliteral", *ptable) : getnameloc("text", *ptable);
		0,
		ntoclear;

	char *message = NULL, *namedcapture = 0;

	int szntoprint = 1, y, cond = 1;

	int ntoprint[0xFF] = {
		-1,
	};

	static bool stop_printing_declarations;

#ifdef DEBUG
	debug_insert_common(a->capture_last, (unsigned int)a->next_item_length, ptable->pattern + a->pattern_position,
						(unsigned int)(ptable->szpattern - (a->pattern_position + a->next_item_length)), ptable->pattern + a->pattern_position + a->next_item_length,
						(unsigned int)(a->subject_length - a->current_position), a->subject + a->current_position, a->pattern_position);
#endif

	//static int justacheckforescape = 0;
#ifdef SHOW_GROUP
	goto showgroup;
#endif
	//printf("callout id %d\n", a->callout_number);
	static int istypedefdecl, isinsidedecl, islocal;
#define GROUP_PTR_AND_SZ(n) a->subject + a->offset_vector[2 * (n)], (unsigned int)(a->offset_vector[2 * (n) + 1] - a->offset_vector[2 * (n)])
	switch (a->callout_number)
	{
#if 1
	case 254:
		cond = 0;

		break;
#ifdef TEST
	case 101:
		n = getnameloc("facetgroup", *ptable);
		break;
#endif
		//case 14:
		//justacheckforescape = !justacheckforescape;
	case 44:
		addtypedefsscope();
		break;
	case 45:
		removetypedefsscope();
		break;
	case 43:
		isinsidedecl = true;
		break;
	case 42:
		isinsidedecl = false;
		break;
	case 41:
		endconstantexpr();
		break;
	case 40:
		beginconstantexpr();
		break;
	case 39:;
		ntoclear = getnameloc("typedefkeyword", *ptable);
		if(!isinsidedecl) istypedefdecl = a->offset_vector[2 * ntoclear] != -1;
		if (istypedefdecl)
		{
			void addtotypedefs(const char *identifier, size_t szcontent);
			n = getnameloc(namedcapture = "identifiermine", *ptable) + 1;
			addtotypedefs(a->subject + a->offset_vector[2 * n], (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]));
			n = 0;
		}
		//a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		n = getnameloc(namedcapture = "identifiermine", *ptable);
		break;
	case 38:;
		int istypedefinvecotr(const char *identifier, size_t szcontent);
		n = getnameloc(namedcapture = "identifiermine2", *ptable) + 1;
		if (a->offset_vector[2 * n] != -1)
			return istypedefinvecotr(a->subject + a->offset_vector[2 * n], (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]));
		n = 0;
		break;
	case 35:
		message = "comma\n";
		break;
	case 34:
		message = "ternary0\n";
		break;
	case 33:
		message = "ternary1\n";
		break;
	case 32:
		message = "ternary2\n";
		break;

#define BINARY_OP(name) \
	n = getnameloc(namedcapture = name, *ptable)
		//ntoclear = n = a->offset_vector[((n = getnameloc(namedcapture = name, *ptable)) + 1)*2] != -1 ? n : 0

	case 30:
		BINARY_OP("assignop");
		break;
	case 29:
		BINARY_OP("orlogicop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 28:
		BINARY_OP("andlogicop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 27:
		BINARY_OP("orop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 26:
		BINARY_OP("xorop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 25:
		BINARY_OP("andop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 24:
		BINARY_OP("eqop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 23:
		BINARY_OP("relop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 22:
		BINARY_OP("shiftop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 20:
		BINARY_OP("addop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 21:
		BINARY_OP("mulop");
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 19:
		n = getnameloc(namedcapture = "unaryop", *ptable);
		break;
	case 9:
		printf("postfix arithmetic:\n");
		n = getnameloc(namedcapture = "postfixarith", *ptable);
		break;
	case 10:
		printf("prefix arithmetic:\n");
		n = getnameloc(namedcapture = "prefixarith", *ptable);
		break;
	case 1:
		n = getnameloc(namedcapture = "escape", *ptable);
		break;
	case 5:
		n = getnameloc(namedcapture = "numberliteral", *ptable);
		insertinttoimm(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 2:
		n = getnameloc(namedcapture = "text", *ptable);
		break;
	case 6:;
		printf("identifier:\n");
		n = getnameloc(namedcapture = "identifier", *ptable);
		//cond = n = 0; break;
		break;
	case 4:
		message = "start string\n";
		break;
	case 7:
		//cond = n = 0; break;
		/*; static test = 0;
		if (a->capture_top <= getnameloc("arrowordot", *ptable))
			if(!test++)*/
		message = "start function call\n";
		break;
		/*	else cond = n = 0;
		else ; break;*/
	case 13:
		message = "end function call\n";
		break;
	case 8:
		printf("member access operator:\n");
		n = getnameloc(namedcapture = "arrowordot", *ptable);
		break;
	case 12:

		n = getnameloc(namedcapture = "abstrsubs", *ptable);
		break;
	case 14:
		message = "end sizeof\n";
		break;
	case 15:
		message = "begin sizeof\n";
		break;

	case 11:
		n = (getnameloc(namedcapture = "abstrptr", *ptable));
		break;
#else
	case 12:
		n = getnameloc(namedcapture = "abstrsubs", *ptable); //cond = 0; break;
		break;
	case 13:
		for (ntoclear = getnameloc("typenamebegin", *ptable); ntoclear <= getnameloc("typenameend", *ptable); ++ntoclear)
			a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		break;
	case 11:
		n = getnameloc(namedcapture = "abstrptr", *ptable);
		break;
		//cond = 0; break;
		//ntoprint[1] = getnameloc("abstrsub", *ptable);
		//printf("callout id %d\n", a->callout_number); break;
#endif
	}
	if (n)
		ntoprint[0] = ++n;
	else
		szntoprint = 0;
		//else cond = n, printf("\n\n");

#ifdef SHOW_GROUP
showgroup:
	//if(0[ptable->pattern + a->pattern_position - 1] == ')')
	if (a->capture_last >= SHOW_GROUP && a->capture_last <= SHOW_GROUP_LAST)
	{
		jmp_buf buff;
		switch (setjmp(buff))
		{
		case -2:
		case 0:
			testifitsusedalready(a->capture_last, &buff);
			break;
		default:
			return 0;
		case -1:
			printf("capture n - %d\n", a->capture_last);
		}
#elif PATTERN_FLAGS & PCRE2_AUTO_CALLOUT
	printf("capture n - %d\n", a->capture_last),
#endif
#ifndef HIDE_DETAILS
		printf("pattern - %.*s\n", (unsigned int)a->next_item_length, ptable->pattern + a->pattern_position),
			printf("pattern rest - %.*s\n", (unsigned int)(ptable->szpattern - (a->pattern_position + a->next_item_length)), ptable->pattern + a->pattern_position + a->next_item_length),
			printf("matching - %.*s\n", (unsigned int)(a->subject_length - a->current_position), a->subject + a->current_position);
#endif
		0;
#ifdef SHOW_GROUP
	}
	return 0;
#endif

	//if (a->callout_number == 1)
	//if (a->capture_top == 1)
	//	return printf("end\n"), 0;
	//else return 0;

	if (message)
		printf(message);
	if (!message || PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
		for (n = 0; n < a->capture_top; ++n)
		{
		print:
			for (y = 0; y < szntoprint; ++y)
				if (ntoprint[y] == n)
					break;
			if (szntoprint && szntoprint == y)
				--y;

			//if (cond)
			if (ntoprint[y] != n)
				if (!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) && cond)
					continue;
				else
					;
			else
#ifndef HIDE_DETAILS
				printf("namedcapture: %s\n", namedcapture);
#else
			{
				printf(
					" %d - %.*s\n",
					n,
					(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
					a->subject + a->offset_vector[2 * n]);
				break;
			}
#endif

			printf(
				"%s %d - %.*s\n",
				getnameloc(-n, *ptable),
				n,
				(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
				a->subject + a->offset_vector[2 * n]);

			//if(n && n == ntoclear)
			//	a->offset_vector[2 * n] = a->offset_vector[2 * n + 1] = -1;
#ifdef DEBUG
			debug_insert_match(n, (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]), a->subject + a->offset_vector[2 * n], namedcapture);
#endif
		}

#ifdef DEBUG
	debug();
#endif

	if (!cond || PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
		printf("\n\n");

#ifdef DEBUG_PHYSIC
	if (!((1 << 15) & GetAsyncKeyState(VK_RETURN)))
		_sleep(1000);
	system("cls");
#endif

	if (a->callout_number == 254)
		exit(-1);

	return 0;
}
