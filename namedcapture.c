#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "lib/pcre2.h"
#include <stdio.h>
//#include <boost/preprocessor/stringize.hpp>
#include <string.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include <setjmp.h>
#include <Windows.h>

enum {
	maxsize = 0xFFF
};

struct match {
	int n;
	char *startmatch, *namedcpture;
};

struct debugstr {
	int last_capture_n;
	char *pattern, *patternrest, *matching;
	struct match matches[maxsize];
	int ba, b;
} tmp;

static struct debugstr	debugarray[maxsize];

static struct match *currmatch = tmp.matches;
static struct debugstr *currdebug = debugarray;

void debug_insert_common(int nlast, unsigned int a, char* pa, unsigned int b, char* pb, unsigned int c, char* pc, int ba)
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

void debug_insert_match(int n, unsigned int a, char* pa, char* pnamedcapture)
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

	if (!(tmp.ba >= 17 && tmp.b <= 92))
		_sleep(0);
	return;
}

int callout_test(pcre2_callout_block* a, void* b)
{
	struct calloutinfo* ptable = b;
	int n = //a->callout_number == 2 ? getnameloc("inner", *ptable) : getnameloc("middle", *ptable);
		//a->callout_number == 1 ? getnameloc("escape", *ptable) : a->callout_number == 5 ? getnameloc("numberliteral", *ptable) : getnameloc("text", *ptable);
		0, ntoclear;

	char* message = NULL, * namedcapture = 0;

	int szntoprint = 1, y, cond = 1;

	int ntoprint[0xFF] = { -1, };

	static bool stop_printing_declarations;

#ifdef DEBUG
	debug_insert_common(a->capture_last, (unsigned int)a->next_item_length, ptable->pattern + a->pattern_position,
			(unsigned int)(ptable->szpattern - (a->pattern_position + a->next_item_length)), ptable->pattern + a->pattern_position + a->next_item_length,
			(unsigned int)(a->subject_length - a->current_position), a->subject + a->current_position, a->pattern_position);
#endif
	switch (a->callout_number)
	{
	case 17:
		ntoclear = getnameloc("cond0", *ptable);
		a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = 0;
		//message = "begin reverse\n";
		break;
	case 18:
		ntoclear = getnameloc("cond0", *ptable);
		a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		break;
	default:
		ntoclear = getnameloc("cond0", *ptable);

		if (a->offset_vector[2 * ntoclear + 1] != -1)
		{
			//n = (getnameloc(namedcapture = "abstrptr", *ptable)); break;
		}
		//else
			//return 0;
	}

	//static int justacheckforescape = 0;
#ifdef SHOW_GROUP
	goto showgroup;
#endif
	//printf("callout id %d\n", a->callout_number);
	switch (a->callout_number)
	{
#if 1
	case 254:
		__debugbreak();
		break;
#ifdef TEST
	case 101:
		n = getnameloc("facetgroup", *ptable);
		break;
#endif
		//case 14:
			//justacheckforescape = !justacheckforescape;
	case 35:
		message = "comma\n"; break;
	case 34:
		message = "ternary0\n"; break;
	case 33:
		message = "ternary1\n"; break;
	case 32:
		message = "ternary2\n"; break;
	case 30:
		n = getnameloc(namedcapture = "assignop", *ptable); break;
	case 29:
		n = getnameloc(namedcapture = "orlogicop", *ptable); break;
	case 28:
		n = getnameloc(namedcapture = "andlogicop", *ptable); break;
	case 27:
		n = getnameloc(namedcapture = "orop", *ptable); break;
	case 26:
		n = getnameloc(namedcapture = "xorop", *ptable); break;
	case 25:
		n = getnameloc(namedcapture = "andop", *ptable); break;
	case 24:
		n = getnameloc(namedcapture = "eqop", *ptable); break;
	case 23:
		n = getnameloc(namedcapture = "relop", *ptable); break;
	case 22:
		n = getnameloc(namedcapture = "shiftop", *ptable); break;
	case 20:
		n = getnameloc(namedcapture = "addop", *ptable); break;
	case 21:
		n = getnameloc(namedcapture = "mulop", *ptable); break;
	case 19:
		n = getnameloc(namedcapture = "unaryop", *ptable); break;
	case 9:
		printf("postfix arithmetic:\n");
		n = getnameloc(namedcapture = "postfixarith", *ptable); break;
	case 10:
		printf("prefix arithmetic:\n");
		n = getnameloc(namedcapture = "prefixarith", *ptable); break;
	case 1:
		n = getnameloc(namedcapture = "escape", *ptable); break;
	case 5:
		n = getnameloc(namedcapture = "numberliteral", *ptable); break;
	case 2:
		n = getnameloc(namedcapture = "text", *ptable); break;
	case 6:
		printf("identifier:\n");
		n = getnameloc(namedcapture = "identifier", *ptable);
		//cond = n = 0; break;
		break;
	case 4:
		message = "start string\n"; break;
	case 7:
		//cond = n = 0; break;
		/*; static test = 0;
		if (a->capture_top <= getnameloc("arrowordot", *ptable))
			if(!test++)*/
		message = "start function call\n"; break;
		/*	else cond = n = 0;
		else ; break;*/
	case 13:
		message = "end function call\n"; break;
	case 8:
		printf("member access operator:\n");
		n = getnameloc(namedcapture = "arrowordot", *ptable); break;
	case 12:

		ntoclear = getnameloc("typenamerev", *ptable);
		//if (a->offset_vector[2 * ntoclear] == a->offset_vector[2 * ntoclear + 1] && a->offset_vector[2 * ntoclear + 1] == -1)
		{
			ntoclear = getnameloc("cond0", *ptable);

			if (a->offset_vector[2 * ntoclear + 1] != -1)
			{
				//n = (getnameloc(namedcapture = "abstrptr", *ptable)); break;
			}
			//else
			//return 0;
		}


		//ntoclear = getnameloc("abstrptrrev", *ptable);
		//if (a->offset_vector[2 * ntoclear + 1] != -1)
		//{
			//ntoclear = getnameloc("abstdeclinside", *ptable);
			//if (a->offset_vector[2 * ntoclear] == a->offset_vector[2 * ntoclear + 1] && a->offset_vector[2 * ntoclear + 1] == -1)
				//return 0;
		//}

		n = getnameloc(namedcapture = "abstrsubs", *ptable); break;
	case 14:
		message = "end sizeof\n"; break;
	case 15:
		message = "begin sizeof\n"; break;
	case 16:
		ntoclear = getnameloc("typenamerev", *ptable);

		if (a->offset_vector[2 * ntoclear + 1] != -1)
		{
			//ntoclear = getnameloc("cond0", *ptable);
			//a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = 0;
		}

		for (ntoclear = getnameloc("typenamebegin", *ptable); ntoclear <= getnameloc("typenameend", *ptable); ++ntoclear)
			a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		break;
		/*case 17:
			ntoclear = getnameloc("cond0", *ptable);
			a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = 0;
			//message = "begin reverse\n";
			break;
		case 18:
			ntoclear = getnameloc("cond0", *ptable);
			a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
			break;*/
	case 11:
		ntoclear = getnameloc("typenamerev", *ptable);

		//if (a->offset_vector[2 * ntoclear] == a->offset_vector[2 * ntoclear + 1] && a->offset_vector[2 * ntoclear + 1] == -1)
		{
			ntoclear = getnameloc("cond0", *ptable);

			if (a->offset_vector[2 * ntoclear + 1] != -1)
			{
				//n = (getnameloc(namedcapture = "abstrptr", *ptable)); break;
			}
			//else
				//return 0;
		}

		//ntoclear = getnameloc("abstrptrrev", *ptable);

		//if (a->offset_vector[2 * ntoclear + 1] != -1)
			//return 0;

		n = (getnameloc(namedcapture = "abstrptr", *ptable)); break;
#else
	case 12:
		n = getnameloc(namedcapture = "abstrsubs", *ptable); //cond = 0; break;
		break;
	case 13:
		for (ntoclear = getnameloc("typenamebegin", *ptable); ntoclear <= getnameloc("typenameend", *ptable); ++ntoclear)
			a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		break;
	case 11:
		n = getnameloc(namedcapture = "abstrptr", *ptable); break;
		//cond = 0; break;
		//ntoprint[1] = getnameloc("abstrsub", *ptable);
		//printf("callout id %d\n", a->callout_number); break;
#endif
	}
	if (n) ntoprint[0] = ++n;
	else szntoprint = 0;
	//else cond = n, printf("\n\n");

#ifdef SHOW_GROUP
	showgroup :
	//if(0[ptable->pattern + a->pattern_position - 1] == ')')
	if (a->capture_last >= SHOW_GROUP && a->capture_last <= SHOW_GROUP_LAST)
	{
		jmp_buf buff;
		switch (setjmp(buff))
		{
		case -2:
		case 0:
			testifitsusedalready(a->capture_last, &buff); break;
		default:
			return 0;
		case -1:printf("capture n - %d\n", a->capture_last);
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
	if (szntoprint && szntoprint == y) --y;

	//if (cond)
	if (ntoprint[y] != n)
		if (!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) && cond)
			continue; else;
	else
#ifndef HIDE_DETAILS
		printf("namedcapture: %s\n", namedcapture);
#else 
		;
#endif

	printf(
		"%s %d - %.*s\n",
		getnameloc(-n, *ptable),
		n,
		(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
		a->subject + a->offset_vector[2 * n]
	);
#ifdef DEBUG
	debug_insert_match(n, (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]), a->subject + a->offset_vector[2 * n], namedcapture);
#endif
}

#ifdef DEBUG
debug();
#endif

if (!cond || PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
printf("\n\n");

#ifdef DEBUG
if(!((1 << 15) & GetAsyncKeyState(VK_RETURN)))
_sleep(1000);
system("cls");
#endif

return 0;
}
