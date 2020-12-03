#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#include "lib/pcre2.h"
#include <stdio.h>
#include <boost/preprocessor/stringize.hpp>
#include <string.h>

#include "main.h"
#include <setjmp.h>

void testifitsusedalready(int val, jmp_buf *pbuff)
{
	static int stack[0xFF], *pstackptr = 1[&stack], *pstackiter = 1[&stack];

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
		a->callout_number == 1 ? getnameloc("escape", *ptable) : a->callout_number == 5 ? getnameloc("numberliteral", *ptable) : getnameloc("text", *ptable);

	char* message = NULL;

	int cond = 1, y;

	int ntoprint[0xFF] = { -1, };
#ifdef SHOW_GROUP
	goto showgroup;
#endif

	switch (a->callout_number)
	{
#ifndef TEST
	case 9:
		printf("postfix arithmetic:\n");
		n = getnameloc("postfixarith", *ptable); break;
	case 10:
		printf("prefix arithmetic:\n");
		n = getnameloc("prefixarith", *ptable); break;
	case 1:
		n = getnameloc("escape", *ptable); break;
	case 5:
		n = getnameloc("numberliteral", *ptable); break;
	case 2:
		n = getnameloc("text", *ptable); break;
	case 6:
		printf("identifier:\n");
		n = getnameloc("identifier", *ptable); break;
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
		n = getnameloc("arrowordot", *ptable); break;
	case 12:
		n = getnameloc("abstrsubs", *ptable); break;
	case 11:
		n = (getnameloc("abstrptr", *ptable)); break;
#else
	case 12:
		n = getnameloc("abstrsubs", *ptable); //cond = n = 0; break;
		break;
	default:
		n = getnameloc("abstrptr", *ptable);
		//ntoprint[1] = getnameloc("abstrsub", *ptable);
		printf("callout id %d\n", a->callout_number); break;
#endif
	}
#if (!!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) & !defined(DONT_EXPAND)) | defined(DO_EXPAND)
	cond = n = 0;
#elif !defined(DONT_EXPAND)
	if (n) ntoprint[0] = ++n;
	else cond = n;
#else 
	;
#endif
#if !defined(DONT_EXPAND) & !defined(SHOW_PATTERN)
	if (!n)
#endif
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
				testifitsusedalready(a->capture_last, &buff); break;
			default:
				return 0;
			case -1:printf("capture n - %d\n", a->capture_last);
			}
#endif
			printf("pattern - %.*s\n", (unsigned int)a->next_item_length, ptable->pattern + a->pattern_position),
				printf("pattern rest - %.*s\n", (unsigned int)ptable->szpattern, ptable->pattern + a->pattern_position);

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
	else
#if !!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) & !defined(DONT_EXPAND)
		cond = 0;
#endif
	for (n = 0; n < a->capture_top; ++n)
	{
	print:
		for (y = 0; y < cond; ++y)
			if (ntoprint[y] == n)
				break;
		if (cond && cond == y) --y;

		if (ntoprint[y] != n && cond)
			continue;
#ifndef DONT_EXPAND
		printf(
			"%d - %.*s\n",
			n,
			(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
			a->subject + a->offset_vector[2 * n]
		);
#else
		printf(
			"%d - @%d\n",
			n,
			(unsigned int)a->offset_vector[2 * n]
		);
#endif
	}


	return 0;
}
