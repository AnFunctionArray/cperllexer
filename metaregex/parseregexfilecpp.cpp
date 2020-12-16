#include <string>
#include <sstream>

static std::string gluedregex, facetor;

extern "C" void beginregex()
{
	gluedregex = "((*F)(";
}

extern "C" void beginfacetor()
{
	facetor = "";
}

extern "C" void addregexfile(const char *content, size_t szcontent)
{
	std::stringstream newregex;
	std::string contentstr;

	contentstr.assign(content, szcontent);

	newregex << "(" << contentstr << ")";

	gluedregex += newregex.str();
}

extern "C" void addfacetor(const char* content, size_t szcontent)
{
	std::stringstream newregex;
	std::string contentstr;

	contentstr.assign(content, szcontent);

	newregex << contentstr << "|";

	facetor += newregex.str();
}

extern "C" const char* retrievefinalregex(const char *rest, size_t szrest)
{
	std::string contentstr;

	contentstr.assign(rest, szrest);

	gluedregex += "))|";
	gluedregex += contentstr;
	return gluedregex.c_str();
}

extern "C" const char* retrievefacetor()
{
	if (!facetor.empty()) facetor.pop_back();
	else return "(?<=[(])\\?<((\\w)+?facet)>";

	std::stringstream newregex;

	newregex << "(?<=[(])\\?<((\\w)+?facet|" << facetor << ")>";

	facetor = newregex.str();

	return facetor.c_str();
}