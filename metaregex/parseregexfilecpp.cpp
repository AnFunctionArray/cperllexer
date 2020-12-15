#include <string>
#include <sstream>

static std::string gluedregex;

extern "C" void beginregex()
{
	gluedregex = "((*F)(";
}

extern "C" void addregexfile(const char *content, size_t szcontent)
{
	std::stringstream newregex;
	std::string contentstr;

	contentstr.assign(content, szcontent);

	newregex << "(" << contentstr << ")";

	gluedregex += newregex.str();
}

extern "C" const char* retrievefinalregex(const char *rest, size_t szrest)
{
	std::string contentstr;

	contentstr.assign(rest, szrest);

	gluedregex += "))|";
	gluedregex += contentstr;
	return gluedregex.c_str();
}