#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#if !defined(_WIN32) & !defined(_WIN64)
#include <pcre2.h>
#else
#include <pcre/pcre2.h>
#endif
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
	char* startmatch, * namedcpture;
};

struct debugstr
{
	int last_capture_n;
	char* pattern, * patternrest, * matching;
	struct match matches[maxsize];
	int ba, b;
} tmp;

static struct debugstr debugarray[maxsize];

static struct match* currmatch = tmp.matches;
static struct debugstr* currdebug = debugarray;

void setypedefspec(size_t* typedefname, const char* subject);
void printqualifntype(size_t* typedefname, const char* subject);

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

	//if (!(tmp.ba >= 17 && tmp.b <= 92))
	//_sleep(0);
	return;
}

static size_t wholepart[2] = { -1, -1 }, fractionpart[2] = { -1, -1 },
exponent[2] = { -1, -1 }, exponent_sign[2] = { -1, -1 };

static char prefix[260];

int callout_test(pcre2_callout_block* a, void* b)
{
	struct calloutinfo* ptable = b;
	//ptable->pcurrblock = a;
	//ptable->compat = 1;
	int n = //a->callout_number == 2 ? getnameloc("inner", *ptable) : getnameloc("middle", *ptable);
		//a->callout_number == 1 ? getnameloc("escape", *ptable) : a->callout_number == 5 ? getnameloc("numberliteral", *ptable) : getnameloc("text", *ptable);
		0,
		ntoclear;

	char* message = NULL, * namedcapture = 0;

	int szntoprint = 1, y, cond = 1;

	int ntoprint[0xFF] = {
		-1,
	}, ntoclearauto[0xFF] = { 0 };

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
	static size_t typedefname[2] = { -1, -1 };
	int res = 0;
#define GROUP_PTR_AND_SZ(n) a->subject + a->offset_vector[2 * (n)], (unsigned int)(a->offset_vector[2 * (n) + 1] - a->offset_vector[2 * (n)])
#define GROUP_SZ_AND_PTR(n) (unsigned int)(a->offset_vector[2 * (n) + 1] - a->offset_vector[2 * (n)]), a->subject + a->offset_vector[2 * (n)]
#define CHECKED_PTR_AND_SZ_START_END(start, end) (start) != -1 ? a->subject + start : "", \
							(end) != -1 ? (unsigned int)((end) - (start)) : 0
#define CHECKED_PTR_AND_SZ(n) CHECKED_PTR_AND_SZ_START_END(a->offset_vector[2 * (n)], a->offset_vector[2 * (n) + 1])

	if (PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) {

		printf("%.*s\n", a->next_item_length, ptable->pattern + a->pattern_position);

		printf("%.*s\n", a->subject_length - a->current_position, a->subject + a->current_position);

		if (a->callout_number == 255) return 0;
		else printf("#region capture %d\n", a->callout_number);
	}
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
	/*case 50:
		flushqualifortype();
		n = getnameloc(namedcapture = "identifiermine2", *ptable) + 1;
		if(a->offset_vector[2 * n] != - 1)
			printf("and typedef name: %.*s", GROUP_SZ_AND_PTR(n));

		printf("\n");
		n = 0;
		break;*/
	case 86:
		message = "end binary before\n";
		endbinarybeforerevlogicops();
		break;
	case 85:
		beginbinary();
		message = "begin binary\n";
		strcat(prefix, "\t");
		break;
	case 80:
		addbreak();
		break;
	case 79:
		addcontinue();
		break;
	case 77:
		startdowhileloop();
		break;
	case 78:
		enddowhileloop();
		break;
	case 76:
		endforloopcond();
		break;
	case 73:
		startforloopcond();
		break;
	case 74:
		addforloopiter();
		break;
	case 75:
		endforloop();
		break;
	case 54:
		subscript();
		message = "end subscript\n";
		break;
	case 53:
		message = "end statement\n";
		endexpression();
		break;
	case 52:
		message = "end return\n";
		endreturn();
		break;
	case 51:
		constructstring();
		break;
	case 49:
		n = getnameloc("typesandqualifiers", *ptable);
		void addqualifortype(const char* currqualifortype, size_t szstr);
		addqualifortype(GROUP_PTR_AND_SZ(n + 1));

		n = 0;
		break;
	case 48:
		message = "end of function declr\n";
		n = getnameloc("rest", *ptable);
		endfunctionparamdecl(a->offset_vector[2 * n] != -1);
		if (a->offset_vector[2 * n] == -1) n = 0;
		else printf(
			" %d - %.*s\n",
			n,
			(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
			a->subject + a->offset_vector[2 * n]), a->offset_vector[2 * n] = a->offset_vector[2 * n + 1] = -1;
		break;
	case 44:
		strcat(prefix, "\t");
		addtypedefsscope();
		beginscope();
		break;
	case 45:
		prefix[strlen(prefix) - 1] = '\0';
		removetypedefsscope();
		endscope();
		break;
	case 42:
		isinsidedecl = false;
		//typedefname[0] = typedefname[1] = -1;
		finalizedeclaration();
		break;
	case 41:
		addsubtotype();
		endconstantexpr();
		break;
	case 65:
		addCase();
		endconstantexpr();
		break;
	case 40:
		//setypedefspec(typedefname, a->subject);
		beginconstantexpr();
		//printqualifntype(typedefname, a->subject);
		break;
	case 64:
		startswitch();
		break;
	case 66:
		endswitch();
		break;
	case 67:
		addDefaultCase();
		break;
	case 39:;
		n = getnameloc(namedcapture = "identifierminedecl", *ptable) + 1;

		setypedefspec(typedefname, a->subject);

		ntoprint[0] = getnameloc2("typedefkeyword", *ptable, a, 0), ntoprint[1] = getnameloc2("typedefkeywordmine", *ptable, a, 0);
		if (!isinsidedecl) istypedefdecl = a->offset_vector[2 * ntoprint[0]] != -1 || a->offset_vector[2 * ntoprint[1]] != -1;
		if (istypedefdecl)
		{
			void addtotypedefs(const char* identifier, size_t szcontent);
			addtotypedefs(a->subject + a->offset_vector[2 * n], (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]));
			//startdeclaration(GROUP_PTR_AND_SZ(n), isinsidedecl, true, GROUP_PTR_AND_SZ(ntoprint[1] + 1), bwhich);
		} //else startdeclaration(GROUP_PTR_AND_SZ(n), isinsidedecl, false, GROUP_PTR_AND_SZ(ntoprint[1] + 1), bwhich);
		ntoclear = ntoprint[0];
		a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		ntoclear = ntoprint[1];
		a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		ntoclear = 0;

		adddeclarationident(GROUP_PTR_AND_SZ(n), istypedefdecl);

		n--;

		//printqualifntype(typedefname, a->subject);

		break;
	case 63:
		puts("goto\n");
		n = getnameloc(namedcapture = "identifierminegoto", *ptable);
		gotolabel(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 62:
		puts("label\n");
		n = getnameloc(namedcapture = "identifierminelabel", *ptable);
		splitbb(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 61:
		message = "applying cast\n";
		applycast();
		break;
	case 60:
		message = "end building\n";
		endbuildingstructorunion();
		break;
	case 59:
		message = "start building\n";
		startbuildingstructorunion();
		break;
	case 58:
		n = getnameloc(namedcapture = "identifierminestruct", *ptable);
		ntoprint[1] = getnameloc(namedcapture = "structorunion", *ptable) + 1;
		setcurrstructorunion(GROUP_PTR_AND_SZ(ntoprint[1]), GROUP_PTR_AND_SZ(n + 1));
		szntoprint++;
		puts("begin struct\n");
		break;
	case 57:
		message = "end sizeof typename\n";
		endsizeoftypename();
		break;
	case 56:
		message = "end sizeof\n";
		endsizeofexpr();
		break;
	case 55:
		startdeclaration(CHECKED_PTR_AND_SZ_START_END(typedefname[0], typedefname[1]));
		break;
	case 43:
		continuedeclaration();
		break;
	case 47:
		startfunctionparamdecl();
		message = "start function params\n";
		break;
	case 38:;
		int istypedefinvecotr(const char* identifier, size_t szcontent);
		n = getnameloc(namedcapture = "identifierminetypedef", *ptable) + 1;
		if (a->offset_vector[2 * n] != -1)
		{
			if (!(res = istypedefinvecotr(a->subject + a->offset_vector[2 * n], (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]))))
			{
				typedefname[0] = a->offset_vector[2 * n];
				typedefname[1] = a->offset_vector[2 * n + 1];
			}
		}
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
		binary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 29:
		//BINARY_OP("orlogicop");
		//BINARY_OP("binop");
		n = getnameloc3(namedcapture = "binop", *ptable, a, 1, (struct namelocopts) { .last = 0, .rev = 1 });
		if (n != -1 && a->offset_vector[2 *(n + 1)] != -1)
			binary(GROUP_PTR_AND_SZ(n + 1)),
			ntoclearauto[0] = n + 1;
		else n = 0;
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
		unary(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 9:
		printf("postfix arithmetic:\n", 0);
		n = getnameloc(namedcapture = "postfixarith", *ptable);
		unaryincdec(GROUP_PTR_AND_SZ(n + 1), true);
		break;
	case 10:
		printf("prefix arithmetic:\n", 0);
		n = getnameloc(namedcapture = "prefixarith", *ptable);
		unaryincdec(GROUP_PTR_AND_SZ(n + 1), false);
		break;
	case 70:
		startifstatement();
		message = "starting if\n";
		break;
	case 71:
		continueifstatement();
		message = "starting else if\n";
		break;
	case 72:
		endifstatement();
		message = "ending if\n";
		break;
	case 1:
		n = getnameloc(namedcapture = "escape", *ptable);
		addescapesequencetostring(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 68:
	{
		if (a->offset_vector[2 * (ntoclear = getnameloc("wholeopt", *ptable))] == -1)
			if (a->offset_vector[2 * (ntoclear = getnameloc("whole", *ptable))] == -1)
				if (a->offset_vector[2 * (ntoclear = getnameloc("wholenodot", *ptable))] == -1)
					goto rest;

		switch (wholepart[0]) case -1: switch (wholepart[1]) case -1:
			wholepart[0] = a->offset_vector[2 * ntoclear],
				wholepart[1] = a->offset_vector[2 * ntoclear + 1];
rest:
		ntoclear = getnameloc("fraction", *ptable);

	switch (fractionpart[0]) case -1: switch (fractionpart[1]) case -1:
		fractionpart[0] = a->offset_vector[2 * ntoclear],
			fractionpart[1] = a->offset_vector[2 * ntoclear + 1];

		ntoclear = getnameloc("sign", *ptable);

	switch (exponent_sign[0]) case -1: switch (exponent_sign[1]) case -1:
		exponent_sign[0] = a->offset_vector[2 * ntoclear],
			exponent_sign[1] = a->offset_vector[2 * ntoclear + 1];

		ntoclear = getnameloc("exponent", *ptable) + 2;

	switch (exponent[0]) case -1: switch (exponent[1]) case -1:
		exponent[0] = a->offset_vector[2 * ntoclear],
			exponent[1] = a->offset_vector[2 * ntoclear + 1];

		break;
	}
	case 69:
	{
		ntoclear = getnameloc("flt", *ptable);

		insertfloattoimm(CHECKED_PTR_AND_SZ(ntoclear), CHECKED_PTR_AND_SZ_START_END(wholepart[0], wholepart[1]),
			CHECKED_PTR_AND_SZ_START_END(fractionpart[0], fractionpart[1]),
			CHECKED_PTR_AND_SZ_START_END(exponent[0], exponent[1]),
			CHECKED_PTR_AND_SZ_START_END(exponent_sign[0], exponent_sign[1]));

		exponent[0] = exponent[1] = exponent_sign[0] = exponent_sign[1] =
			fractionpart[0] = fractionpart[1] = wholepart[0] =
			wholepart[1] = -1;

		break;
	}
	case 5:;
	{
		unsigned int type;
		n = getnameloc(namedcapture = "numberliteral", *ptable);

		//ntoprint[0] = getnameloc("uns", *ptable);

		ntoprint[1] = getnameloc("lng", *ptable);

		const char* groups[] = { "hex", "bin", "oct", "dec" };

		for (const char** pgroup = groups; pgroup != 1[&groups]; ++pgroup)
		{
			ntoclear = getnameloc(*pgroup, *ptable);
			if (a->offset_vector[2 * ntoclear] != -1)
			{
				type = pgroup - groups; //<< 2;//| (a->offset_vector[2 * ntoprint[0]] != -1) | (a->offset_vector[2 * ntoprint[1]] != -1) << 1;

				insertinttoimm(GROUP_PTR_AND_SZ(ntoclear), CHECKED_PTR_AND_SZ(ntoprint[1]), type);

				//a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;

				break;
			}
		}

		//ntoclear = ntoprint[0];
		//a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;
		//ntoclear = ntoprint[1];
		//a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;

		ntoclear = 0;
	}
	break;
	case 2:
		n = getnameloc(namedcapture = "text", *ptable);
		addplaintexttostring(GROUP_PTR_AND_SZ(n + 1));
		break;
	case 6:;
		printf("identifier:\n", 0);
		n = getnameloc(namedcapture = "identifier", *ptable);
		obtainvalbyidentifier(GROUP_PTR_AND_SZ(n + 1));
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
			//startfunctioncall();
		message = "start function call\n";
		startfunctioncall();
		break;
		/*	else cond = n = 0;
		else ; break;*/
	case 13:
		endfunctioncall();
		message = "end function call\n";
		break;
	case 8:
		printf("member access operator:\n", 0);
		n = getnameloc(namedcapture = "identifierminemember", *ptable);
		ntoprint[1] = getnameloc(namedcapture = "arrowordot", *ptable) + 1;
		memberaccess(GROUP_PTR_AND_SZ(ntoprint[1]), GROUP_PTR_AND_SZ(n + 1));
		szntoprint++;
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
		n = (getnameloc2(namedcapture = "abstrptr", *ptable, a, 1));

		//n = 0;

		addptrtotype(GROUP_PTR_AND_SZ(n + 1));

		printf("pointer\n", 0);

		break;
	case 81:
		n = (getnameloc2(namedcapture = "cc", *ptable, a, 1)) + 1;
		if (a->offset_vector[2 * n] == -1)
			n = 0;
		else --n;
		break;
	//case 82:
		//beginop(false);
		//break;
	case 83:
		message = "end binary\n";
		endbinary();
		prefix[strlen(prefix) - 1] = '\0';
		break;
	case 84:
		//beginop(true);
		beginlogicalop(false);
		message = "begin branch\n";
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
	showgroup :
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
printf("%s%s", prefix, message);
if (!message || PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
for (n = 0; n < a->capture_top; ++n)
{
print:
	for (y = 0; y < szntoprint; ++y)
		if (ntoprint[y] == n)
			break;

	//if (cond)
	if (szntoprint == y)
		if (!(PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) && cond)
			goto continueloop;
		else
			;
	else
#ifndef HIDE_DETAILS
		printf("namedcapture: %s\n", namedcapture);
#else
	{
		printf(
			"%s %d - %.*s\n",
			prefix,
			n,
			(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
			a->subject + a->offset_vector[2 * n]);
		goto continueloop;
	}
#endif

	printf(
		"%s %s %d - %.*s\n",
		prefix,
		getnameloc(-n, *ptable),
		n,
		(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]),
		a->subject + a->offset_vector[2 * n]);

	//if(n && n == ntoclear)
		//a->offset_vector[2 * n] = a->offset_vector[2 * n + 1] = -1;
continueloop:
	for (int* ntoclear = ntoclearauto; *ntoclear; ++ntoclear)
		if (*ntoclear == n)
			a->offset_vector[2 * n] = a->offset_vector[2 * n + 1] = -1;
#ifdef DEBUG
	debug_insert_match(n, (unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n]), a->subject + a->offset_vector[2 * n], namedcapture);
#endif
}

#ifdef DEBUG
debug();
#endif

if (!cond || PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
printf("\n\n", 0);

#ifdef DEBUG_PHYSIC
if (!((1 << 15) & GetAsyncKeyState(VK_RETURN)))
_sleep(1000);
system("cls");
#endif

if (a->callout_number == 254)
exit(-1);

if (PATTERN_FLAGS & PCRE2_AUTO_CALLOUT)
	printf("#endregion\n", 0);

return res;
	}

	void setypedefspec(size_t* typedefname, const char* subject)
	{
		if (typedefname[0] != -1) settypedefname(subject + typedefname[0], typedefname[1] - typedefname[0]);
	}

	void printqualifntype(size_t* typedefname, const char* subject)
	{
		bool flushqualifortype(), isanythingprinted = flushqualifortype();

		if (typedefname[0] != -1)
			printf("and typedef name: %.*s", typedefname[1] - typedefname[0], subject + typedefname[0]);

		if (isanythingprinted) printf("\n", 0);

		typedefname[0] = typedefname[1] = -1;
	}