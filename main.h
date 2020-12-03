#pragma once
#include <boost/preprocessor/facilities/expand.hpp>

#define STRING_REGEX(a, b, c) "\"((\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|\"|t|n)))))(?C"BOOST_PP_STRINGIZE(a)")|(((?!(?2)|\").)++)(?C"BOOST_PP_STRINGIZE(b)"))*\"(?C"BOOST_PP_STRINGIZE(c)")"

//"((\"(?C4)((\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|\"|t|n)))))(?C1)|(((?!(?4)|\").)++)(?C2))*\"(?C3))|(((?!(?2)).)))++"
//"((?<nested>[(]?((?<first>(\"(?C4)((?<escape>\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|\"|t|n)))))(?C1)|(?<text>((?!(?&escape)|\").)++)(?C2))*\"(?C3)))|(((?!(?&first)).)))++[)]?)|(((?!(?&nested)|[)]).)++))++"

#define NUMBER_REGEX(addedprefix) "(?:(" addedprefix "x)[0-9a-fA-F]++|(?:(" addedprefix "b)[01]++|(?:(0)[0-7]++|\\d++)))"

//|(?(<begincharliteral>)'|\")|t|n

//#define TEST_REGEX "\\s*((?<inner>(((\"|(?<begincharliteral>'))(?C4)((?<escape>(\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|(?(<begincharliteral>)'|\")|t|n)))))(?C1))|" \
						"(?<text>(((?!(?&escape)|(?(<begincharliteral>)'|\")).)++)(?C2)))*+(?(<begincharliteral>)'|\")" \
						"|(?<numberliteral>((?:(0x)[0-9a-fA-F]++|(?:(0b)[01]++|(?:(0)[0-7]++|(\\d++)))))(?C5))" \
						"|(?<identifier>([_a-zA-Z][_a-zA-Z0-9]*+)(?C6)))))" \
						"\\s*(\\s*(\\[(?C7)(?R)\\]|(?&inparenths)|(?<arrowordot>([.]|->))(?C8)\\s*(?&identifier)|(?<postfixarith>([+][+]|--))(?C9))\\s*)*|(?<inparenths>[(](?R)[)])" \
						"|(?<unary>(?<prefixarith>([+][+]|--))(?R)(?C10)|(?<unaryop>([&*+-~!]))\\s*(?<castexpr>\\s*[(]\\s*"\
						"(\\bint\\b|\\bchar\\b|\\bshort\\b|\\blong\\b|\\bsigned\\b|\\bunsigned\\b|\\bfloat\\b|\\bdouble\\b|(?<qualifiers>\\bconst\\b|\\brestrict\\b|\\bvolatile\\b))*\\s*" \
						"(?<abstrdecl>(?<abstrinpar>[(]((?<abstrinner>(?<abstrptrrev>(?<abstrptr>\\s*[*]\\s*((?&qualifiers))*\\s*)(?!(?&abstrptrrev)(?C11))(?C11)|(?&abstrptr))?(?<abstrsub>\\s*\\[(.*?)\\]\\s*)*)"\
						"|(?&abstrdecl))[)]\\s*)|(?&abstrinner)?[)])(?R))))\\s*|\\s*"
#define TEST_REGEX_FILE "main.regex"
#define TEST_FILE "maintest.c"
//#define TEST_STRING  "\"test\\9test1\\9\\077\\xAF\\xAF\\t\\\"\\xAF\\\"\" [ ( ( !(char (*restrict(*restrict(*const)[1][2])[3])[4])3 ) ) ] ( ++ a ++ -> a7 ++ ) fkdfk '\\'\"' \"\" 0x89 ggfgfg \"tes t3\" fgdfdf (\"tes())\"(\"t\\4\"(\"\\xF9\")))"
#define PATTERN_FLAGS_BASE 0 //| PCRE2_AUTO_CALLOUT
#define PATTERN_FLAGS PATTERN_FLAGS_BASE
//#define DONT_EXPAND

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
//#define TEST_REGEX "\\s*[(]\\s*(\\bint\\b|\\bchar\\b|\\bshort\\b|\\blong\\b|\\bsigned\\b|\\bunsigned\\b|\\bfloat\\b|\\bdouble\\b|(?<qualifiers>\\bconst\\b|\\brestrict\\b|\\bvolatile\\b))*+"\
					"\\s*((?<abstrdecl>((?<abstrallptrs>(?<abstrptrrev>(?<abstrptr>\\s*[*]\\s*((?&qualifiers))*\\s*)(?!(?&abstrptrrev)(?C11))(?C11)|(?&abstrptr))|(?&abstrptr)++)"\
						"(?=((?<abstrsub>\\s*\\[(.*?)\\]\\s*)(?C12))*+)(?&abstrsub)*+)|[(](?&abstrdecl)[)](?&abstrsub)*+))[)]"
#define TEST_REGEX_FILE "test.regex"
#define TEST_FILE "test.c"
//#define TEST_STRING "(*restrict(*restrict(*const)[1][2])[3])[4]"
#endif

//(? #\s * [(]\s * (\bint\b | \bchar\b | \bshort\b | \blong\b | \bsigned\b | \bunsigned\b | \bfloat\b | \bdouble\b | (? <qualifiers>\bconst\b | \brestrict\b | \bvolatile\b)) * +\
	\s*)(? #[)])

//(?<abstrptrrev>(?<abstrptr>\s*[*]\s*((?<qualifiers>\bconst\b|\brestrict\b|\bvolatile\b))*\s*)(?!(?&abstrptrrev)(?C11))(?<abstrsubs>\s*\[(.*?)\]\s*(?C12))*+(?C11))
	

struct calloutinfo { PCRE2_SPTR name_table, pattern; int namecount, name_entry_size; size_t szpattern; };

int getnameloc(const char* str, struct calloutinfo);

int compile_pattern_and_execute(const char* pattern, const char* subject, int (*callback)(pcre2_callout_enumerate_block*, void*), size_t szpattern, size_t szsubject);

//"((?<abstrptrrev>(?<abstrptr>\\s*[*]\\s*((?&qualifiers))*\\s*)(?!(?&abstrptrrev)(?C11))(?C11)|(?&abstrptr))|(?&abstrptr)++)[)]"
//#define TEST_REGEX "(?<dot>([*]\\d))(?!(?R)(?C1))(?C1)|(?&dot)"
//"((?<first>\\(((?<inner>(?>[^()]++))(?C2)|(?&first))*+\\))(?C1)|(?<middle>((?!(?&first)).))(?C3))++"
//"((?<first>\\(((?<inner>(?>[^()]++))|(?&first))*+(?C1)\\))|(((?!(?&first)).)))++"
//"((?<first>(\"(?C4)((?<escape>\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|\"|t|n)))))(?C1)|(?<text>((?!(?&escape)|\").)++)(?C2))*\"(?C3)))|(((?!(?&first)).)))++"
//"((?<first>\\(((?<inner>(\"|(?<begincharliteral>'))(?C4)((?<escape>\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|(?(<begincharliteral>)'|\")|t|n)))))(?C1)|" \
						"(?<text>((?!(?&escape)|(?(<begincharliteral>)'|\")).)++)(?C2))*+(?(<begincharliteral>)'|\")" \
						"|(?<numberliteral>(?:(0x)[0-9a-fA-F]++|(?:(0b)[01]++|(?:(0)[0-7]++|(\\d++)))))(?C5))" \
						"(?#C2)|(?&first))*+\\)|(?&inner))(?#C1)|(?<middle>((?!(?&first)).))(?#C3))++"

//					"|(?<unaryop>([&*+-~!]))?" \
						"\\s*(?<castexpr>\\s*[(]\\s*(\\bint\\b|\\bchar\\b|\\bshort\\b|\\blong\\b|\\bsigned\\b|\\bunsigned\\b|\\bfloat\\b|\\bdouble\\b|(?<qualifiers>\\bconst\\b|\\brestrict\\b|\\bvolatile\\b))*" \
						"\\s*(((?<abstrdecl>((?<pointer>\\s*[*]\\s*((?&qualifiers))\\s*)|(?&pointer)\\s*(?&directabstrdecl)))|(?<directabstrdecl>\\s*[(]\\s*(?&abstrdecl)\\s*[)]\\s*|(?&directabstrdecl)?" \
						"(\\s*\\[(?R)\\]\\s*)))?\\s*[)])?)(?&castexpr)|(?&unary)|(?R)))\\s*|\\s*"

//"((?<first>\\s*\\((?<inner>\\s*(((\"|(?<begincharliteral>'))(?C4)((?<escape>\\\\(?:(x)[0-9a-fA-F]++|(?:(b)[01]++|(?:(0)[0-7]++|(\\d++|(?(<begincharliteral>)'|\")|t|n)))))(?C1)|" \
						"(?<text>((?!(?&escape)|(?(<begincharliteral>)'|\")).)++)(?C2))*+(?(<begincharliteral>)'|\")" \
						"|(?<numberliteral>(?:(0x)[0-9a-fA-F]++|(?:(0b)[01]++|(?:(0)[0-7]++|(\\d++)))))(?C5)" \
						"|(?<identifier>[_a-zA-Z][_a-zA-Z0-9]++)(?C6))\\s*)" \
						"(?#C2)|(?&first))*+\\)\\s*)|(?&inner))(\\[(?R)\\])*\\s*"

//"(?<abstrdecl>(?<abstrinpar>[(]((?<abstrinner>((?<abstrptrrev>(?<abstrptr>\\s*[*]\\s*((?&qualifiers))*\\s*)(?!(?&abstrptrrev)(?C11))(?C11)|(?&abstrptr))|(?&abstrptr)*+)(?<abstrsub>\\s*\\[(.*?)\\]\\s*)*"\
						"|(?&abstrdecl))[)]\\s*)|(?&abstrinner)))?[)]\\s*"