#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/GlobalIFunc.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <vector>
#include <string>
#include <sstream>
#include <array>
#include <bitset>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ranges>
#include <locale>
#include <stdint.h>

unsigned constexpr stringhash(char const* input)
{
	return *input ? static_cast<unsigned int>(*input) + 33 * stringhash(input + 1) : 5381;
}

constexpr inline auto operator"" _h(char const* p, size_t)
{
	return stringhash(p);
}

static struct nonconstructable
{
	nonconstructable() {}
	~nonconstructable() {}
	union {
		llvm::Module mainmodule;
	};
} nonconstructable;

static std::vector<llvm::Function*> funcs{};

static llvm::Function* currfunc;

static std::vector<std::unique_ptr<llvm::GlobalVariable>> globals{};

static llvm::LLVMContext llvmctx;

static llvm::IRBuilder<> llvmbuilder{ llvmctx };

typedef std::bitset<3> pointrtypequalifiers;
/*
0 - const
1 - restrict
2 - volatile
*/

struct bascitypespec
{
	std::array<std::string, 4> basic;
	pointrtypequalifiers qualifiers;
};
/*
0 - last signed/unsigned
1 - last basic type
2 - last storage specifier
3 - last typedef name
*/


pointrtypequalifiers parsequalifiers(const std::string& qualifs)
{
	pointrtypequalifiers ret;
	std::stringstream ssqualifs{ qualifs };
	std::string spec{};
	while (ssqualifs >> spec) switch (stringhash(spec.c_str()))
	{
	case "const"_h:
		ret[0] = 1; break;
	case "restrict"_h:
		ret[1] = 1; break;
	case "volatile"_h:
		ret[2] = 1;
		break;
	default:
		std::cerr << "invalid specifier: " << spec << std::endl;
		std::terminate();
	}
	return ret;
}

bascitypespec parsebasictype(const std::vector<std::string>& qualifs)
{
	bascitypespec ret;

	for (const auto& a : qualifs) switch (stringhash(a.c_str()))
	{
	case "unsigned"_h:
	case "signed"_h:
		ret.basic[0] = a; break;
	case "int"_h:
		if (ret.basic[1] != "") break;
	case "long"_h:
	case "short"_h:
	case "char"_h:
	case "float"_h:
	case "double"_h:
		ret.basic[1] = a; break;
	case "static"_h:
	case "extern"_h:
	case "auto"_h:
		ret.basic[2] = a; break;
	case "const"_h:
		ret.qualifiers[0] = 1; break;
	case "restrict"_h:
		ret.qualifiers[1] = 1; break;
	case "volatile"_h:
		ret.qualifiers[2] = 1;
		break;
	default:
		std::cerr << "invalid specifier: " << a << std::endl;
		std::terminate();
	}
	return ret;
}
struct var;

struct type {
	enum typetype {
		BASIC,
		POINTER,
		ARRAY,
		FUNCTION
	} uniontype;

	type(typetype a) : spec{ a }, uniontype{ a } {};

	typedef uint64_t arraytype;
	struct functype {
		std::vector<var> parametertypes;
		bool bisvariadic;
	};

	type(const type& a) : spec{ a.uniontype }, uniontype{ a.uniontype } {

		*this = a;
	}

	~type()
	{
		std::array lambdas = { std::function{[&] {spec.basicdeclspec.~bascitypespec(); }},
								std::function{[&] {spec.ptrqualifiers.~bitset(); }},
								std::function{[&] {}},
								std::function{[&] {spec.func.~functype(); }} };
		lambdas[uniontype]();
	}

	type& operator=(const type& type)
	{

		if (type.uniontype != uniontype) this->~type(), new (this) ::type{ type.uniontype };
		std::array lambdas = { std::function{[&] {spec.basicdeclspec = type.spec.basicdeclspec; }},
							   std::function{[&] {spec.ptrqualifiers = type.spec.ptrqualifiers; }},
							   std::function{[&] {spec.arraysz = type.spec.arraysz; }},
							   std::function{[&] {spec.func = type.spec.func; }} };
		lambdas[uniontype]();

		return *this;
	}

	union typespec {
		typespec(typetype type)
		{
			std::array lambdas = { std::function{[&] {new (&basicdeclspec) bascitypespec{}; }},
								std::function{[&] {new (&ptrqualifiers) pointrtypequalifiers{}; }},
								std::function{[&] {arraysz = 0; }},
								std::function{[&] {new (&func) functype{}; }} };
			lambdas[type]();
		}
		~typespec() {}
		bascitypespec basicdeclspec;
		pointrtypequalifiers ptrqualifiers;
		arraytype arraysz;
		functype func;
	} spec;
};

struct var {
	std::vector<::type> type;
	bool bistypedef;
	std::string identifier;
	llvm::Type* pllvmtype;
};

static std::vector<llvm::BasicBlock*> pcurrblock;

/*struct valueorconstant {
	template<class T> requires std::same_as<T, llvm::Value*> || std::same_as<T, llvm::Constant*> || std::same_as<T, void*>
	operator T()
	{
		return (T)p;
	}

	template<class T> requires std::same_as<T, llvm::Value*> || std::same_as<T, llvm::Constant*> || std::same_as<T, void*>
	valueorconstant& operator=(T p)
	{
		this->p = p;
		return *this;
	}

	void* p;
};*/

llvm::Type* createllvmtype(std::vector<type> decltypevector);

struct basehndl
{
	virtual llvm::Value* CreateCastInst(llvm::Instruction::CastOps op, llvm::Value* S, llvm::Type* Ty)
	{
		return llvm::CastInst::Create(op, S, Ty, "", pcurrblock.back());
	}

	struct valandtype { llvm::Value* value; std::vector<::type> currtype; } integralpromotions(valandtype in)
	{
		assert(in.currtype.size() == 1);

		switch (stringhash(in.currtype.back().spec.basicdeclspec.basic[1].c_str()))
		{
		case "char"_h:
		case "short"_h:
			switch (stringhash(in.currtype.back().spec.basicdeclspec.basic[0].c_str()))
			{
			default:
			case "signed"_h:
				in.value = CreateCastInst(llvm::Instruction::CastOps::SExt, in.value, llvm::Type::getInt32Ty(llvmctx)); break;
			case "unsigned"_h:
				in.value = CreateCastInst(llvm::Instruction::CastOps::ZExt, in.value, llvm::Type::getInt32Ty(llvmctx)); break;
			}
			in.currtype.back().spec.basicdeclspec.basic[0] = "";
			in.currtype.back().spec.basicdeclspec.basic[1] = "int";
		}
		return in;
	}

	std::array<valandtype, 2> usualarithmeticconversions(std::array<valandtype, 2> ops)
	{
		for (auto i = 0; i < 2; ++i)
			if (ops[i].currtype.back().spec.basicdeclspec.basic[1] == "long" && ops[i].currtype.back().spec.basicdeclspec.basic[0] == "unsigned")
				if (ops[!i].currtype.back().spec.basicdeclspec.basic[1] != "long" || ops[!i].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
					return ops[!i].currtype.back().spec.basicdeclspec.basic[1] = "long", ops[!i].currtype.back().spec.basicdeclspec.basic[0] = "unsigned",
					ops[!i].value = CreateCastInst(ops[!i].currtype.back().spec.basicdeclspec.basic[0] == "unsigned" ?
						llvm::Instruction::CastOps::ZExt : llvm::Instruction::CastOps::SExt, ops[!i].value, llvm::Type::getInt64Ty(llvmctx)), ops;
				else return ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].currtype.back().spec.basicdeclspec.basic[1] == "long" && ops[i].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
				if (ops[!i].currtype.back().spec.basicdeclspec.basic[1] == "int" && ops[!i].currtype.back().spec.basicdeclspec.basic[0] == "unsigned")
					return ops[!i].currtype.back().spec.basicdeclspec.basic[1] = "long", ops[!i].currtype.back().spec.basicdeclspec.basic[0] = "",
					ops[!i].value = CreateCastInst(llvm::Instruction::CastOps::ZExt, ops[!i].value, llvm::Type::getInt64Ty(llvmctx)), ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].currtype.back().spec.basicdeclspec.basic[1] == "long" && ops[i].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
				return ops[!i].currtype.back().spec.basicdeclspec.basic[1] = "long", ops[!i].currtype.back().spec.basicdeclspec.basic[0] = "",
				ops[!i].value = CreateCastInst(ops[!i].currtype.back().spec.basicdeclspec.basic[0] == "unsigned" ?
					llvm::Instruction::CastOps::ZExt : llvm::Instruction::CastOps::SExt, ops[!i].value, llvm::Type::getInt64Ty(llvmctx)), ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].currtype.back().spec.basicdeclspec.basic[1] == "int" && ops[i].currtype.back().spec.basicdeclspec.basic[0] == "unsigned")
				return ops[!i].currtype.back().spec.basicdeclspec.basic[1] = "int", ops[!i].currtype.back().spec.basicdeclspec.basic[0] = "unsigned",
				ops[!i].value = CreateCastInst(ops[!i].currtype.back().spec.basicdeclspec.basic[0] == "unsigned" ?
					llvm::Instruction::CastOps::ZExt : llvm::Instruction::CastOps::SExt, ops[!i].value, llvm::Type::getInt32Ty(llvmctx)), ops;

		return ops;
	}

	auto getops(bool busual = true)
	{
		auto ops = std::array{ integralpromotions(*(immidiates.end() - 2)),
			integralpromotions(immidiates.back()) };

		if (busual) ops = usualarithmeticconversions(ops);

		immidiates.erase(immidiates.end() - 2, immidiates.end());

		return ops;
	}

	virtual void mlutiplylasttwovalues()
	{
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateMul(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void dividelasttwovalues()
	{
		std::array ops = getops();

		if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateSDiv(ops[0].value, ops[1].value);
		else ops[0].value = llvmbuilder.CreateUDiv(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void modulolasttwovalues()
	{
		std::array ops = getops();

		if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateSRem(ops[0].value, ops[1].value);
		else ops[0].value = llvmbuilder.CreateURem(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void addlasttwovalues(bool bminus)
	{
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateAdd(ops[0].value, !bminus ? ops[1].value : llvmbuilder.CreateNeg(ops[1].value));

		immidiates.push_back(ops[0]);
	}

	virtual void shifttwovalues(bool bright)
	{
		std::array ops = getops(false);

		if (!bright) ops[0].value = llvmbuilder.CreateShl(ops[0].value, ops[1].value);
		else if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateAShr(ops[0].value, ops[1].value);
		else ops[0].value = llvmbuilder.CreateLShr(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred)
	{
		std::array ops = getops();

		if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateICmp(pred, ops[0].value, ops[1].value);
		else ops[0].value = llvmbuilder.CreateCmp(pred, ops[0].value, ops[1].value);

		ops[0].value = CreateCastInst(llvm::Instruction::CastOps::ZExt, ops[0].value, llvm::Type::getInt32Ty(llvmctx));

		ops[0].currtype.back().spec.basicdeclspec.basic[1] = "int", ops[0].currtype.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void bitwisetwovalues(llvm::Instruction::BinaryOps op)
	{
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateBinOp(op, ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void logictwovalues(bool bisand)
	{
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[0].value, llvm::ConstantInt::getIntegerValue(ops[0].value->getType(), llvm::APInt{}));

		ops[1].value = llvmbuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[1].value, llvm::ConstantInt::getIntegerValue(ops[1].value->getType(), llvm::APInt{}));

		ops[0].value = !bisand ? llvmbuilder.CreateOr(ops[0].value, ops[1].value) : llvmbuilder.CreateAnd(ops[0].value, ops[1].value);

		ops[0].value = llvm::CastInst::Create(llvm::Instruction::CastOps::ZExt, ops[0].value, llvm::Type::getInt32Ty(llvmctx), "", pcurrblock.back());

		ops[0].currtype.back().spec.basicdeclspec.basic[1] = "int", ops[0].currtype.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	void insertinttoimm(const char* str, size_t szstr, unsigned int type)
	{
		std::string imm;

		imm.assign(str, szstr);

		std::vector<::type> currtype = { 1, ::type::BASIC };

		const bool isunsigned = type & 1;

		const bool islong = type & 2;

		type >>= 2;

		const int base[] = { 16, 2, 7, 10 };

		const auto val = std::stoull(imm, nullptr, base[type]);

		if (type == 3 && !isunsigned && !islong)
			if (val <= INT_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "int";
			else if (val <= LLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long";
			else if (val <= ULLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long",
				currtype.back().spec.basicdeclspec.basic[0] = "unsigned";
			else;
		else if (!isunsigned && !islong)
			if (val <= INT_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "int";
			else if (val <= UINT_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "int",
				currtype.back().spec.basicdeclspec.basic[0] = "unsigned";
			else if (val <= LLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long";
			else if (val <= ULLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long",
				currtype.back().spec.basicdeclspec.basic[0] = "unsigned";
			else;
		else if (isunsigned && !islong)
			if (val <= UINT_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "int",
				currtype.back().spec.basicdeclspec.basic[0] = "unsigned";
			else if (val <= ULLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long",
				currtype.back().spec.basicdeclspec.basic[0] = "unsigned";
			else;
		else if (!isunsigned && islong)
			if (val <= LLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long";
			else if (val <= ULLONG_MAX)
				currtype.back().spec.basicdeclspec.basic[1] = "long",
				currtype.back().spec.basicdeclspec.basic[0] = "unsigned";
			else;
		else if (isunsigned && islong)
			currtype.back().spec.basicdeclspec.basic[1] = "long",
			currtype.back().spec.basicdeclspec.basic[0] = "unsigned";

		auto valval = llvm::ConstantInt::get(createllvmtype(currtype), val);

		immidiates.push_back(valandtype{ valval, currtype });
	}

	std::vector<valandtype> immidiates;
};

struct handlecnstexpr : basehndl
{
	struct valandtype { llvm::Constant* value; std::vector<::type> currtype; };

	std::vector<valandtype>& immidiates = reinterpret_cast<std::vector<valandtype>&>(basehndl::immidiates);

	auto getops(bool busual = true)
	{
		auto ops = basehndl::getops();

		return reinterpret_cast<std::array<valandtype, 2>&&>(ops);
	}

	virtual void mlutiplylasttwovalues()
	{
		std::array ops = getops();

		ops[0].value = llvm::ConstantExpr::getMul(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void dividelasttwovalues()
	{
		std::array ops = getops();

		if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvm::ConstantExpr::getSDiv(ops[0].value, ops[1].value);
		else ops[0].value = llvm::ConstantExpr::getUDiv(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void modulolasttwovalues()
	{
		std::array ops = getops();

		if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvm::ConstantExpr::getSRem(ops[0].value, ops[1].value);
		else ops[0].value = llvm::ConstantExpr::getURem(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void addlasttwovalues(bool bminus)
	{
		std::array ops = getops();

		ops[0].value = llvm::ConstantExpr::getAdd(ops[0].value, !bminus ? ops[1].value : llvm::ConstantExpr::getNeg(ops[1].value));

		immidiates.push_back(ops[0]);
	}

	virtual void shifttwovalues(bool bright)
	{
		std::array ops = getops(false);

		if (!bright) ops[0].value = llvm::ConstantExpr::getShl(ops[0].value, ops[1].value);
		else if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvm::ConstantExpr::getAShr(ops[0].value, ops[1].value);
		else ops[0].value = llvm::ConstantExpr::getLShr(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred)
	{
		std::array ops = getops();

		if (ops[0].currtype.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvm::ConstantExpr::getICmp(pred, ops[0].value, ops[1].value);
		else ops[0].value = llvm::ConstantExpr::getCompare(pred, ops[0].value, ops[1].value);

		ops[0].value = CreateCastInst(llvm::Instruction::CastOps::ZExt, ops[0].value, llvm::Type::getInt32Ty(llvmctx));

		ops[0].currtype.back().spec.basicdeclspec.basic[1] = "int", ops[0].currtype.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void bitwisetwovalues(llvm::Instruction::BinaryOps op)
	{
		std::array ops = getops();

		ops[0].value = llvm::ConstantExpr::get(op, ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void logictwovalues(bool bisand)
	{
		std::array ops = getops();

		ops[0].value = llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[0].value, llvm::ConstantInt::getIntegerValue(((llvm::Constant*)ops[0].value)->getType(), llvm::APInt{}));

		ops[1].value = llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[1].value, llvm::ConstantInt::getIntegerValue(((llvm::Constant*)ops[1].value)->getType(), llvm::APInt{}));

		ops[0].value = !bisand ? llvm::ConstantExpr::getOr(ops[0].value, ops[1].value) : llvm::ConstantExpr::getAnd(ops[0].value, ops[1].value);

		ops[0].value = CreateCastInst(llvm::Instruction::CastOps::ZExt, ops[0].value, llvm::Type::getInt32Ty(llvmctx));

		ops[0].currtype.back().spec.basicdeclspec.basic[1] = "int", ops[0].currtype.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual llvm::Constant* CreateCastInst(llvm::Instruction::CastOps op, llvm::Constant* S, llvm::Type* Ty)
	{
		return llvm::ConstantExpr::getCast(op, S, Ty);
	}
};

static handlecnstexpr hndlcnstexpr{};

static basehndl hndlbase{};

static basehndl* phndl = &hndlbase;

static std::remove_cvref<decltype(basehndl::immidiates)>::type::iterator cnstexpriterstart{};

//static std::string currdeclidentifier{};

enum class currdecltypeenum {
	TYPEDEF,
	CAST,
	PARAMS,
	NORMAL,
	UNKNOWN,
	NONE
};

static currdecltypeenum currdecltype{ currdecltypeenum::NONE };

std::string currdeclspectypedef;

extern std::vector<std::string> qualifsandtypes;

static std::vector<std::vector<var>> scopevar{ 1 };

static var currdecltypevector{};

static std::vector<var*> currtypevectorbeingbuild{ 1, &currdecltypevector };

static std::string currstring;

extern "C" void obtainvalbyidentifier(const char* identifier, size_t szstr)
{
	std::string ident{ identifier, szstr };

	const ::var* var;

	std::find_if(scopevar.begin(), scopevar.end(), [&](const std::vector<::var>& scope) {
		auto iter = std::find_if(scope.begin(), scope.end(), [&](const ::var& scopevar) {
			return scopevar.identifier == ident;
			});

		if (iter != scope.end())
			return (bool)(var = &*iter);
		return false;
		});

	if(var->type.front().uniontype == ::type::FUNCTION)
		phndl->immidiates.push_back({ nonconstructable.mainmodule.getFunction(ident), var->type });
}

extern "C" void addplaintexttostring(const char* str, size_t szstr)
{
	currstring += std::string{ str,szstr };
}

extern "C" void addescapesequencetostring(const char* str, size_t szstr)
{
	std::string escape{ str, szstr };

	switch (stringhash(escape.c_str()))
	{
	case "\\n"_h:
		currstring += '\n';
		break;
	}
}


extern "C" void constructstring()
{
	std::vector<::type> stirngtype{1, ::type::ARRAY};

	stirngtype.back().spec.arraysz = currstring.size() + 1;

	stirngtype.push_back({ ::type::BASIC });

	stirngtype.back().spec.basicdeclspec.basic[1] = "char";

	hndlbase.immidiates.push_back({ llvmbuilder.CreateGlobalStringPtr(currstring, "", 0, &nonconstructable.mainmodule), stirngtype });
}

void begindeclarationifnone()
{
	if (!currtypevectorbeingbuild.back()->type.size() ||
		currtypevectorbeingbuild.back()->type.back().uniontype != type::FUNCTION)
	{
		if (currdecltype != currdecltypeenum::NONE) return;
		currdecltype = currdecltypeenum::UNKNOWN;
	add_basic_type:
		{
			type basic{ type::BASIC };

			if (qualifsandtypes.empty())
				if (scopevar.size() > 1)
					throw (currdecltype = currdecltypeenum::NONE, std::invalid_argument{ "invalid default int - not a declaration" }); //default int is only valid in file scope
				else basic.spec.basicdeclspec.basic[1] = "int";
			else basic.spec.basicdeclspec = parsebasictype(::qualifsandtypes);

			basic.spec.basicdeclspec.basic[3] = ::currdeclspectypedef;

			currdeclspectypedef = "";

			currtypevectorbeingbuild.back()->type.push_back(basic);
		}
	}
	else if (currdecltype == currdecltypeenum::UNKNOWN)
	{
		currdecltype = currdecltypeenum::PARAMS;
		currtypevectorbeingbuild.back()->type.back().spec.func.parametertypes.push_back({});
		currtypevectorbeingbuild.push_back(&currtypevectorbeingbuild.back()->type.back().spec.func.parametertypes.back());

		goto add_basic_type;
	}
}

void addvar()
{
	const var& lastvar = currdecltypevector;

	const char* lastvartypestoragespec = lastvar.type.back().spec.basicdeclspec.basic[2].c_str();

	switch (scopevar.size())
	{
	case 1:
		llvm::GlobalValue::LinkageTypes linkagetype;

		switch (stringhash(lastvartypestoragespec))
		{
		default:
		case "extern"_h:
			linkagetype = llvm::GlobalValue::LinkageTypes::ExternalLinkage; break;
		case "static"_h:
			linkagetype = llvm::GlobalValue::LinkageTypes::InternalLinkage;
		}

		switch (lastvar.type[0].uniontype)
		{
		case type::POINTER:
		case type::ARRAY:
		case type::BASIC:
			globals.push_back(std::make_unique<llvm::GlobalVariable>(lastvar.pllvmtype, false, linkagetype, nullptr, lastvar.identifier));
			scopevar.front().push_back(lastvar);
			break;
		case type::FUNCTION:
			funcs.push_back(llvm::Function::Create(llvm::dyn_cast<llvm::FunctionType>(lastvar.pllvmtype), linkagetype, lastvar.identifier, nonconstructable.mainmodule));
			scopevar.front().push_back(lastvar);
			break;
		}
		break;
	default:
		scopevar.back().push_back(currdecltypevector);

		break;
	}

	currdecltypevector.~var();

	new (&currdecltypevector) var{};
}

extern "C" void startdeclaration(const char* str, size_t szstr, int isinsidedecl, bool bistypedef) try
{
	begindeclarationifnone();

	currtypevectorbeingbuild.back()->identifier.assign(str, szstr);

	if (!isinsidedecl && bistypedef) currdecltype = currdecltypeenum::TYPEDEF;
}
catch (std::invalid_argument exc) {
	return;
}

void finalizedecl();

extern "C" void finalizedeclaration()
{
	finalizedecl();

	addvar();

	currdecltypevector.~var();
	new (&currdecltypevector) var{};

	assert(currtypevectorbeingbuild.size() == 1);

	currdecltype = currdecltypeenum::NONE;
}

llvm::Type* createllvmtype(std::vector<type> decltypevector)
{
	llvm::Type* pcurrtype;

	std::array lambdas = { std::function{[&](const type& type) {
			switch (stringhash(type.spec.basicdeclspec.basic[1].c_str()))
			{
				case "int"_h:
					pcurrtype = dyn_cast<llvm::Type>(llvm::Type::getInt32Ty(llvmctx)); break;
				break;
				case "short"_h:
					pcurrtype = dyn_cast<llvm::Type>(llvm::Type::getInt16Ty(llvmctx)); break;
				break;
				case "long"_h:
					pcurrtype = dyn_cast<llvm::Type>(llvm::Type::getInt64Ty(llvmctx)); break;
				break;
				case "char"_h:
					pcurrtype = dyn_cast<llvm::Type>(llvm::Type::getInt8Ty(llvmctx)); break;
				break;
				case "float"_h:
					pcurrtype = dyn_cast<llvm::Type>(llvm::Type::getFloatTy(llvmctx)); break;
				break;
				case "double"_h:
					pcurrtype = dyn_cast<llvm::Type>(llvm::Type::getDoubleTy(llvmctx)); break;
				break;
			}
		}},
		std::function{[&](const type& type) {
				pcurrtype = dyn_cast<llvm::Type>(pcurrtype->getPointerTo());
			}},
		std::function{[&](const type& type) {
				pcurrtype = dyn_cast<llvm::Type>(llvm::ArrayType::get(pcurrtype, type.spec.arraysz));
			}},
		std::function{[&](const type& type) {
			std::vector<llvm::Type*> paramtype;
			for (auto& a : type.spec.func.parametertypes)
				paramtype.push_back(a.pllvmtype);
			pcurrtype = dyn_cast<llvm::Type>(llvm::FunctionType::get(pcurrtype, paramtype, type.spec.func.bisvariadic));
		}} };

	std::reverse(decltypevector.begin(), decltypevector.end());

	for (const auto& type : decltypevector)
		lambdas[type.uniontype](type);

	return pcurrtype;
}

extern std::vector<llvm::BasicBlock*> pcurrblock;

void finalizedecl();

extern "C" void beginscope()
{
	if (currdecltype != currdecltypeenum::NONE) //if begin a function definiton
		finalizedeclaration();
	if (scopevar.size() == 1)
		currfunc = funcs.back();

	pcurrblock.push_back(llvm::BasicBlock::Create(llvmctx, "", currfunc));
	llvmbuilder.SetInsertPoint(pcurrblock.back());
	scopevar.push_back({});
}

extern "C" void endscope()
{
	//nonconstructable.mainmodule.
	scopevar.pop_back();
}

extern "C" void endfunctioncall()
{
	auto lastblock = pcurrblock.back();

	llvm::Function* callee = llvm::dyn_cast<llvm::Function>(hndlbase.immidiates.front().value);

	hndlbase.immidiates.erase(hndlbase.immidiates.begin());

	llvmbuilder.SetInsertPoint(lastblock);

	std::vector<llvm::Value*> immidiates;

	std::transform(hndlbase.immidiates.begin(), hndlbase.immidiates.end(), std::back_inserter(immidiates), [](const basehndl::valandtype& elem) { return elem.value; });

	llvmbuilder.CreateCall(callee->getFunctionType(), llvm::dyn_cast<llvm::Value>(callee), immidiates);
}

extern "C" void endreturn()
{
	llvmbuilder.SetInsertPoint(pcurrblock.back());
	llvmbuilder.CreateRet(hndlbase.immidiates.back().value);
}

extern "C" void endfunctionparamdecl(bool bisrest)
{
	if (currdecltype == currdecltypeenum::NONE)
		currtypevectorbeingbuild.pop_back();
	else
		currdecltype = currdecltypeenum::UNKNOWN;
	currtypevectorbeingbuild.back()->type.back().spec.func.bisvariadic = bisrest;
}

extern "C" void enddeclaration()
{
	begindeclarationifnone();

	addvar();

	currdecltype = currdecltypeenum::NONE;
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr);

extern "C" void startfunctionparamdecl() try
{
	begindeclarationifnone();

	if (currtypevectorbeingbuild.size() > 1) //if declaring a function inside another declaration
		addptrtotype("", 0);
	//currdecltype = currdecltypeenum::PARAMS;
	currtypevectorbeingbuild.back()->type.push_back({ type::FUNCTION });
}
catch (std::invalid_argument exc) {
	return;
}

void finalizedecl()
{
	auto& currtype = currtypevectorbeingbuild.back()->type;

	std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	currtypevectorbeingbuild.back()->pllvmtype = createllvmtype(currtype);
}

extern "C" void finalizedeclarationtypename()
{
	begindeclarationifnone();

	finalizedecl();

	currdecltype = currdecltypeenum::NONE;
}

extern "C" void addsubtotype()
{
	begindeclarationifnone();

	type arraytype{ type::ARRAY };

	auto res = llvm::dyn_cast<llvm::ConstantInt>(hndlcnstexpr.immidiates.back().value);

	arraytype.spec.arraysz = *res->getValue().getRawData();
	currtypevectorbeingbuild.back()->type.push_back(arraytype);
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr)
{
	begindeclarationifnone();

	type ptrtype{ type::POINTER };

	ptrtype.spec.ptrqualifiers = parsequalifiers(std::string{ quailifers, szstr });

	currtypevectorbeingbuild.back()->type.push_back(ptrtype);
}

extern "C" void insertinttoimm(const char* str, size_t szstr, int type) { phndl->insertinttoimm(str, szstr, type); }

extern "C" void beginconstantexpr()
{
	cnstexpriterstart = phndl->immidiates.end();

	phndl = &hndlcnstexpr;
}

extern "C" void endconstantexpr()
{
	assert(cnstexpriterstart == phndl->immidiates.end() - 1);

	//auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

	hndlcnstexpr.immidiates.pop_back();

	//std::cout << "computed value: " << *res->getValue().getRawData() << std::endl;

	phndl = &hndlbase;
}

extern "C" void startmodule(const char* modulename, size_t szmodulename)
{
	new (&nonconstructable.mainmodule) llvm::Module{ std::string{modulename, szmodulename}, llvmctx };
}

extern "C" void endmodule()
{
	std::error_code code{};
	llvm::raw_fd_ostream output{ std::string{nonconstructable.mainmodule.getName()} + ".bc", code };
	llvm::WriteBitcodeToFile(nonconstructable.mainmodule, output);
	nonconstructable.mainmodule.~Module();
}

extern "C" void binary(const char* str, size_t szstr)
{
	std::string imm;

	imm.assign(str, szstr);

	switch (stringhash(imm.c_str()))
	{
	case "*"_h:
		phndl->mlutiplylasttwovalues(); break;
	case "/"_h:
		phndl->dividelasttwovalues(); break;
	case "%"_h:
		phndl->modulolasttwovalues(); break;
	case "+"_h:
		phndl->addlasttwovalues(false); break;
	case "-"_h:
		phndl->addlasttwovalues(true); break;
	case "<<"_h:
		phndl->shifttwovalues(false); break;
	case ">>"_h:
		phndl->shifttwovalues(true); break;
	case ">"_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SGT); break;
	case "<"_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SLT); break;
	case "<="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SLE); break;
	case ">="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SGE); break;
	case "=="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_EQ); break;
	case "!="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_NE); break;
	case "&"_h:
		phndl->bitwisetwovalues(llvm::Instruction::And); break;
	case "^"_h:
		phndl->bitwisetwovalues(llvm::Instruction::Xor); break;
	case "|"_h:
		phndl->bitwisetwovalues(llvm::Instruction::Or); break;
	case "&&"_h:
		phndl->logictwovalues(true); break;
	case "||"_h:
		phndl->logictwovalues(false); break;
	}
}

//llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());
//std::cout << "base type of " << currdeclidentifier << " is typedef " << !bwhich << " :" << spec << ", is declaring a typedef :" << bistypedef << std::endl;