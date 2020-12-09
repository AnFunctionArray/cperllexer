#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "lib/pcre2.h"
#include <stdio.h>
#include <boost/preprocessor/stringize.hpp>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include <setjmp.h>

void testifitsusedalready(int val, jmp_buf* pbuff)
{
	static int stack[0xFF], * pstackptr = 1[&stack], * pstackiter = 1[&stack];

	if (pstackiter == pstackptr)
	{
		*--pstackptr = val;
		pstackiter = 1[&stack];
		longjmp(*pbuff, -1);
	}
	if (*--pstackiter == val)
	{
		pstackiter = 1[&stack];
		longjmp(*pbuff, val);
	}

	longjmp(*pbuff, -2);
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

	//static int justacheckforescape = 0;
#ifdef SHOW_GROUP
	goto showgroup;
#endif
	switch (a->callout_number)
	{
	case 17:
		push_condstack(1);
		//message = "begin reverse\n";
		break;
	case 18:
		pop_condstack();
		break;
	case 19:
		push_condstack(0);
		break;
	default:
		//ntoclear = getnameloc("cond0", *ptable);

		if (!size_condstack() || top_condstack())
		{
			//n = (getnameloc(namedcapture = "abstrptr", *ptable)); break;
		}
		else
			return 0;
	}
	//printf("callout id %d\n", a->callout_number);
	switch (a->callout_number)
	{
#if 1
		//case 14:
			//justacheckforescape = !justacheckforescape;
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
	case 19:

		n = getnameloc(namedcapture = "unaryop", *ptable); break;
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


		ntoclear = getnameloc("abstrptrrev", *ptable);
		if (a->offset_vector[2 * ntoclear + 1] != -1)
		{
			ntoclear = getnameloc("abstdeclinside", *ptable);
			if (a->offset_vector[2 * ntoclear] == a->offset_vector[2 * ntoclear + 1] && a->offset_vector[2 * ntoclear + 1] == -1)
				return 0;
		}

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

		ntoclear = getnameloc("abstrptrrev", *ptable);

		if (a->offset_vector[2 * ntoclear + 1] != -1)
			return 0;
		
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
}

if (!cond || PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
printf("\n\n");

#ifdef DEBUG
_sleep(1000);
system("cls");
#endif

return 0;
}
