#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

static std::string gluedregex, facetor;

static std::vector<std::vector<std::string>> typedefs{ 1 };

std::vector<std::string> qualifsandtypes{};

extern std::string currdeclspectypedef;

extern "C" void settypedefname(const char*typedefname, size_t szstr)
{
	currdeclspectypedef.assign(typedefname, szstr);
}

extern "C" void addqualifortype(const char* currqualifortype, size_t szstr)
{
	qualifsandtypes.push_back(std::string{ currqualifortype, szstr });
}

extern "C" bool flushqualifortype()
{
	if(qualifsandtypes.empty()) return false;
	std::cout << "decl with base spec: ";
	for (const auto& a : qualifsandtypes)
		std::cout << a << " ";

	qualifsandtypes.clear();

	return true;
}

extern "C" void addtypedefsscope()
{
	typedefs.push_back({});
}

extern "C" void removetypedefsscope()
{
	typedefs.pop_back();
}

extern "C" void addtotypedefs(const char* identifier, size_t szcontent)
{
	std::string contentstr;

	contentstr.assign(identifier, szcontent);

	typedefs.back().push_back(contentstr);
}

extern "C" int istypedefinvecotr(const char* identifier, size_t szcontent)
{
	std::string contentstr;

	contentstr.assign(identifier, szcontent);

	int res = 1;

	for (const auto& a : typedefs)
		if (!(res = res && std::find(a.begin(), a.end(), contentstr) == a.end())) break;

	return res;
}

extern "C" void beginregex()
{
	gluedregex = "((*F)(";
}

extern "C" void beginfacetor()
{
	facetor = "";
}

extern "C" void addregexfile(const char* content, size_t szcontent)
{
	std::stringstream newregex;
	std::string contentstr;

	contentstr.assign(content);

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

extern "C" const char* retrievefinalregex(const char* rest, size_t szrest)
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