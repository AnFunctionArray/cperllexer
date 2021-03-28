#include "llvm/IR/Instructions.h"
#include <array>
#include <bitset>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <llvm/Support/Error.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalIFunc.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <locale>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/iterator/common_iterator.hpp>
#include <range/v3/algorithm/find.hpp>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <vector>
#include <string_view>

unsigned constexpr stringhash(char const* input) {
	return *input ? static_cast<unsigned int> (*input) +
		33 * stringhash(input + 1)
		: 5381;
}

void printtype(llvm::Type* ptype, std::string identifier) {
	std::string type_str;
	llvm::raw_string_ostream rso(type_str);
	ptype->print(rso);
	std::cout << identifier << " is " << rso.str() << std::endl;
}

constexpr inline auto operator"" _h(char const* p, size_t) {
	return stringhash(p);
}

static struct nonconstructable {
	nonconstructable() {}
	~nonconstructable() {}
	union {
		llvm::Module mainmodule;
	};
} nonconstructable;

llvm::DataLayout* pdatalayout;

bool bareweinabrupt(bool barewe = false);

static llvm::LLVMContext llvmctx;

static llvm::IRBuilder<> llvmbuilder{ llvmctx };

typedef std::bitset<3> pointrtypequalifiers;
/*
0 - const
1 - restrict
2 - volatile
*/

struct bascitypespec {
	std::array<std::string, 4> basic;
	/*
		0 - last signed/unsigned or struct/union
		1 - last basic type
		2 - last storage specifier
		3 - last typedef or struct/union name
	*/
	size_t longspecsn{}; //amount of long qualifiers
	pointrtypequalifiers qualifiers;

	bascitypespec& operator=(const bascitypespec& source) { // basic assignment
		basic[0] = source.basic[0];
		basic[1] = source.basic[1];
		basic[3] = source.basic[3];
		longspecsn = source.longspecsn;
		return *this;
	}

	bool operator==(const bascitypespec& comparer) {
		return comparer.basic[0] == basic[0] &&
			comparer.basic[1] == basic[1] &&
			comparer.basic[3] == basic[3] &&
			comparer.longspecsn == longspecsn;
	}

	bool compareSign(const bascitypespec& comparer) {
		return ranges::contains(std::array{ "", "signed" }, comparer.basic[0]) &&
			ranges::contains(std::array{ "", "signed" }, basic[0]) ||
			comparer.basic[0] == basic[0];
	}

};

pointrtypequalifiers parsequalifiers(const std::string& qualifs) {
	pointrtypequalifiers ret;
	std::stringstream ssqualifs{ qualifs };
	std::string spec{};
	while (ssqualifs >> spec)
		switch (stringhash(spec.c_str())) {
		case "const"_h:
			ret[0] = 1;
			break;
		case "restrict"_h:
			ret[1] = 1;
			break;
		case "volatile"_h:
			ret[2] = 1;
			break;
		default:
			std::cerr << "invalid specifier: " << spec << std::endl;
			std::terminate();
		}
	return ret;
}

bascitypespec parsebasictype(const std::vector<std::string>& qualifs) {
	bascitypespec ret;

	for (const auto& a : qualifs)
		switch (stringhash(a.c_str())) {
		case "unsigned"_h:
		case "signed"_h:
			ret.basic[0] = a;
			break;
		case "long"_h:
			ret.longspecsn++;
			if (ret.basic[1].empty())
				ret.basic[1] = a;
			break;
		case "__int64"_h:
		case "int"_h:
		case "short"_h:
		case "char"_h:
		case "float"_h:
		case "double"_h:
			ret.basic[1] = a;
			break;
		case "static"_h:
		case "extern"_h:
		case "auto"_h:
			ret.basic[2] = a;
			break;
		case "const"_h:
			ret.qualifiers[0] = 1;
			break;
		case "restrict"_h:
			ret.qualifiers[1] = 1;
			break;
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
	enum typetype { BASIC, POINTER, ARRAY, FUNCTION } uniontype;

	type(typetype a) : spec{ a }, uniontype{ a } {};

	typedef uint64_t arraytype;
	struct functype {
		std::list<std::list<var>> parametertypes_list{ 1 };
		bool bisvariadic;
	};

	type(const type& a) : spec{ a.uniontype }, uniontype{ a.uniontype } {
		*this = a;
	}

	~type() {
		std::array lambdas = {
			std::function{[&] { spec.basicdeclspec.~bascitypespec(); }},
			std::function{[&] { spec.ptrqualifiers.~bitset(); }},
			std::function{[&] {}},
			std::function{[&] { spec.func.~functype(); }} };
		lambdas[uniontype]();
	}

	type& operator= (const type& type) {

		if (type.uniontype != uniontype)
			this->~type(), new (this)::type{ type.uniontype };
		std::array lambdas = {
			std::function{
				[&] { spec.basicdeclspec = type.spec.basicdeclspec; }},
			std::function{
				[&] { spec.ptrqualifiers = type.spec.ptrqualifiers; }},
			std::function{[&] { spec.arraysz = type.spec.arraysz; }},
			std::function{[&] { spec.func = type.spec.func; }} };
		lambdas[uniontype]();

		return *this;
	}

	union typespec {
		typespec(typetype type) {
			std::array lambdas = {
				std::function{[&] { new (&basicdeclspec) bascitypespec{}; }},
				std::function{
					[&] { new (&ptrqualifiers) pointrtypequalifiers{}; }},
				std::function{[&] { arraysz = 0; }},
				std::function{[&] { new (&func) functype{}; }} };
			lambdas[type]();
		}
		~typespec() {}
		bascitypespec basicdeclspec;
		pointrtypequalifiers ptrqualifiers;
		arraytype arraysz;
		functype func;
	} spec;
};

struct valandtype {
	union {
		llvm::Value* value{};
		llvm::Constant* constant;
	};
	std::vector<::type> type;
};

struct val : valandtype {
	std::list<valandtype> lvalues = {};
	std::string originident = "[[immidiate]]";
};

struct var {
	std::vector<::type> type;
	bool bistypedef;
	std::string identifier;
	llvm::Type* pllvmtype;
	llvm::Value* pValue;

	operator val() { return { pValue, type, {}, identifier }; }
};

static std::list<var>::iterator currfunc;

static std::vector<llvm::BasicBlock*> pcurrblock;

/*struct valueorconstant {
		template<class T> requires std::same_as<T, llvm::Value*> ||
std::same_as<T, llvm::Constant*> || std::same_as<T, void*> operator T()
		{
				return (T)p;
		}

		template<class T> requires std::same_as<T, llvm::Value*> ||
std::same_as<T, llvm::Constant*> || std::same_as<T, void*> valueorconstant&
operator=(T p)
		{
				this->p = p;
				return *this;
		}

		void* p;
};*/

static std::list<std::list<::val>::iterator> callees{};

llvm::Type* createllvmtype(std::vector<type> decltypevector);

val convertTo(val target, std::vector<::type> to);

/*auto gen_pointer_to_elem_if_ptr_to_array(llvm::Value* value,
	llvm::Type** p_type = nullptr) {
	llvm::Type* type_tmp,
		*& type = p_type
		? *p_type
		: (type_tmp = value->getType()->getPointerElementType());
	if (type->isArrayTy()) {
		type = type->getArrayElementType();
		value = llvmbuilder.CreatePointerCast(value, type->getPointerTo());
	}
	return value;
}*/

static std::list<val> immidiates;

bool bIsBasicInteger(const type& type);

struct basehndl {
	virtual llvm::Value* CreateCastInst(llvm::Value* C, llvm::Type* Ty, bool bissigned) {
		return llvmbuilder.CreateIntCast(C, Ty, bissigned);
	}

	virtual llvm::Constant* getValZero(val val) {
		llvm::APInt apint{};

		if (bIsBasicInteger(val.type.front()))
			apint = llvm::APInt{ dyn_cast<llvm::IntegerType>(val.value->getType())->getBitWidth(), 0 };

		return llvm::ConstantInt::getIntegerValue(val.value->getType(), apint);
	}

	using val = ::val;

	std::list<val>& immidiates = ::immidiates;

	virtual void getpositive() {
		immidiates.back() = integralpromotions(immidiates.back());
	}

	virtual void getnegative() {
		immidiates.back() = integralpromotions(immidiates.back());

		immidiates.back().value = llvmbuilder.CreateNeg(immidiates.back().value);
	}

	virtual void getbitwisenot() {
		immidiates.back() = integralpromotions(immidiates.back());

		immidiates.back().value = llvmbuilder.CreateNot(immidiates.back().value);
	}

	virtual void getlogicalnot() {
		auto& op = immidiates.back();

		extern void printvaltype(val);

		printvaltype(op);

		op.value = llvmbuilder.CreateICmp(
			llvm::CmpInst::Predicate::ICMP_EQ, op.value,
			getValZero(op));
		op.type.erase(op.type.begin(), --op.type.end());

		op.value = CreateCastInst(op.value, llvm::Type::getInt32Ty(llvmctx), false);

		extern void printvaltype(::val val);

		printvaltype(op);

		op.type.back().spec.basicdeclspec.basic[1] = "int",
			op.type.back().spec.basicdeclspec.basic[0] = "";
	}

	val integralpromotions(val in) {
		assert(in.type.size() == 1);

		std::cout << "promoting" << std::endl;

		extern void printvaltype(val);

		printvaltype(in);

		switch (
			stringhash(in.type.back().spec.basicdeclspec.basic[1].c_str())) {
		case "char"_h:
		case "short"_h:
			switch (stringhash(
				in.type.back().spec.basicdeclspec.basic[0].c_str())) {
			default:
			case "signed"_h:
				in.value =
					CreateCastInst(in.value, llvm::Type::getInt32Ty(llvmctx), true);
				break;
			case "unsigned"_h:
				in.value =
					CreateCastInst(in.value, llvm::Type::getInt32Ty(llvmctx), false);
				break;
			}
			in.type.back().spec.basicdeclspec.basic[0] = "";
			in.type.back().spec.basicdeclspec.basic[1] = "int";
		}
		return in;
	}

	static int getrank(::type basictyp) {
		switch (stringhash(basictyp.spec.basicdeclspec.basic[0].c_str()))
		{
		default:
		case "signed"_h:
		case "unsigned"_h:
			switch (stringhash(basictyp.spec.basicdeclspec.basic[1].c_str()))
			{
			case "char"_h:
				return 0;
			case "short"_h:
				return 1;
			case "int"_h:
			case "long"_h:
				return 2 + basictyp.spec.basicdeclspec.longspecsn;
			case "__int64"_h:
				return 4;
			}
		}

		throw std::exception{ "not a integer" };
	}

	std::array<val, 2> usualarithmeticconversions(std::array<val, 2> ops_in) {
		std::array ops = { &ops_in[0], &ops_in[1] };
		assert(ops[0]->type.size() == 1 && ops[1]->type.size() == 1);

		std::array refspecops = { &ops[0]->type.back().spec.basicdeclspec,  &ops[1]->type.back().spec.basicdeclspec };

		extern void printvaltype(val);

		printvaltype(*ops[0]);

		printvaltype(*ops[1]);

		for (auto i = 0; i < 2; ++i)
			if (ranges::contains(std::array{ "double", "float" }, refspecops[i]->basic[1]))
				for (ops_in[!i] = convertTo(*ops[!i], ops[i]->type);;)
					return ops_in;

		ops_in[0] = integralpromotions(*ops[0]);

		ops_in[1] = integralpromotions(*ops[1]);

		refspecops = { &ops[0]->type.back().spec.basicdeclspec,  &ops[1]->type.back().spec.basicdeclspec };

		if (*refspecops[0] == *refspecops[1])
			return ops_in;

		if (refspecops[0]->compareSign(*refspecops[1])) {
			if (getrank(ops[0]->type.back()) < getrank(ops[1]->type.back()))
				std::swap(ops[0], ops[1]), std::swap(refspecops[0], refspecops[1]);

			ops[1]->value = CreateCastInst(ops[1]->value, ops[0]->value->getType(), refspecops[1]->basic[0] != "unsigned");

			*refspecops[1] = *refspecops[0];

			return ops_in;
		}

		if (refspecops[0]->basic[0] != "unsigned")
			std::swap(ops[0], ops[1]), std::swap(refspecops[0], refspecops[1]);

		if (getrank(ops[0]->type.back()) >= getrank(ops[1]->type.back())) {
			ops[1]->value = CreateCastInst(ops[1]->value, ops[0]->value->getType(), true);

			*refspecops[1] = *refspecops[0];

			return ops_in;
		}

		if (ranges::contains(std::array{ refspecops[1]->longspecsn, refspecops[0]->longspecsn }, 2)) //if at least one is long long
			//otherwise there is no difference between int and long on windows

			ops[0]->value = CreateCastInst(ops[0]->value, ops[1]->value->getType(), false),

			* refspecops[0] = *refspecops[1];
		else {
			auto tmptype = ops[1]->type.back();

			tmptype.spec.basicdeclspec.basic[0] = "unsigned";

			ops[0]->value = CreateCastInst(ops[0]->value, createllvmtype({ tmptype }), false),

				* refspecops[0] = tmptype.spec.basicdeclspec;

			ops[1]->value = CreateCastInst(ops[1]->value, createllvmtype({ tmptype }), true),

				* refspecops[1] = tmptype.spec.basicdeclspec;
		}

		return ops_in;

		/*for (auto y = 0;; ++y)
			for (auto i = 0; i < 2; ++i) {
				switch (y)
		case 0:
			if (ranges::contains(std::array{ "double", "float" }, ops[i].type.back().spec.basicdeclspec.basic[1]))
				for (ops[!i] = convertTo(ops[!i], ops[i].type);;)
					return ops;
			else default:
				if (auto& longspncurr = ops[i].type.back().spec.basicdeclspec.longspecsn,
					&longspnother = ops[!i].type.back().spec.basicdeclspec.longspecsn; 1)
					switch (auto& typecurr = ops[i].type.back().spec.basicdeclspec.basic[0],
						&typeother = ops[!i].type.back().spec.basicdeclspec.basic[0]; y)
		if (1) case 1: switch (i) {
		case 0:
			ops[0] = integralpromotions(ops[0]);

			ops[1] = integralpromotions(ops[1]);
		case 1:
			if (longspncurr > 1 && typecurr == "unsigned")
					for (ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt64Ty(llvmctx), typeother != "unsigned"),
						longspnother = 2, typeother = "unsigned";;)
						return ops;
				else
					return ops;
		} else case 2: if(1)
			if (longspncurr > 1 && typecurr != "unsigned")
				if (longspnother == 1 && typeother == "unsigned")
					for (longspnother = 2, typeother = "",
						ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt64Ty(llvmctx), false);;)
						return ops;
				else
					return ops;
			else case 3: if(1) if (longspncurr > 1 && typecurr != "unsigned")
					for (longspnother = 2, typeother = "",
						ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt64Ty(llvmctx), false);;)
						return ops;
				else
					return ops;
			}
		ops[0] = integralpromotions(ops[0]);

		ops[1] = integralpromotions(ops[1]);

		if (ops[0].type.back().spec.basicdeclspec == ops[1].type.back().spec.basicdeclspec)
			return ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.longspecsn > 1 &&
				ops[i].type.back().spec.basicdeclspec.basic[0] == "unsigned")
				if (ops[!i].type.back().spec.basicdeclspec.longspecsn <= 1 ||
					ops[!i].type.back().spec.basicdeclspec.basic[0] != "unsigned")
					return ops[!i].type.back().spec.basicdeclspec.longspecsn = 2,
					ops[!i].type.back().spec.basicdeclspec.basic[0] = "unsigned",
					ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt64Ty(llvmctx), ops[!i].type.back().spec.basicdeclspec.basic[0] != "unsigned"),
					ops;
				else
					return ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.longspecsn > 1 &&
				ops[i].type.back().spec.basicdeclspec.basic[0] != "unsigned")
				if (ops[!i].type.back().spec.basicdeclspec.longspecsn == 1 &&
					ops[!i].type.back().spec.basicdeclspec.basic[0] == "unsigned")
					return ops[!i].type.back().spec.basicdeclspec.longspecsn = 2,
					ops[!i].type.back().spec.basicdeclspec.basic[0] = "",
					ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt64Ty(llvmctx), false),
					ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.longspecsn > 1 &&
				ops[i].type.back().spec.basicdeclspec.basic[0] != "unsigned")
				return ops[!i].type.back().spec.basicdeclspec.longspecsn = 2,
				ops[!i].type.back().spec.basicdeclspec.basic[0] = "",
				ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt64Ty(llvmctx),
					ops[!i].type.back().spec.basicdeclspec.basic[0] != "unsigned"),
				ops;



		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.basic[1] == "long" &&
				ops[i].type.back().spec.basicdeclspec.basic[0] == "unsigned")
				if (ops[!i].type.back().spec.basicdeclspec.basic[1] !=
					"long" ||
					ops[!i].type.back().spec.basicdeclspec.basic[0] !=
					"unsigned")
					return ops[!i]
					.type.back()
					.spec.basicdeclspec
					.basic[1] = "long",
					ops[!i].type.back().spec.basicdeclspec.basic[0] = "unsigned",
					ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt32Ty(llvmctx),
						ops[!i].type.back().spec.basicdeclspec.basic[0] != "unsigned"),
					ops;
				else
					return ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.basic[1] == "long" &&
				ops[i].type.back().spec.basicdeclspec.basic[0] != "unsigned")
				if (ops[!i].type.back().spec.basicdeclspec.basic[1] == "int" &&
					ops[!i].type.back().spec.basicdeclspec.basic[0] == "unsigned")
					return ops[!i]
					.type.back()
					.spec.basicdeclspec.basic[1] = "long",
					ops[!i].type.back().spec.basicdeclspec.basic[0] =
					"",
					ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt32Ty(llvmctx), false),
					ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.basic[1] == "long" &&
				ops[i].type.back().spec.basicdeclspec.basic[0] != "unsigned")
				return ops[!i]
				.type.back()
				.spec.basicdeclspec.basic[1] = "long",
				ops[!i].type.back().spec.basicdeclspec.basic[0] = "",
				ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt32Ty(llvmctx),
					ops[!i].type.back().spec.basicdeclspec.basic[0] != "unsigned"),
				ops;

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.back().spec.basicdeclspec.basic[1] == "int" &&
				ops[i].type.back().spec.basicdeclspec.basic[0] == "unsigned")
				return ops[!i]
				.type.back()
				.spec.basicdeclspec.basic[1] = "int",
				ops[!i].type.back().spec.basicdeclspec.basic[0] =
				"unsigned",
				ops[!i].value = CreateCastInst(ops[!i].value, llvm::Type::getInt32Ty(llvmctx),
					ops[!i].type.back().spec.basicdeclspec.basic[0] != "unsigned"),
				ops;

		return ops;*/
	}

	auto getops(bool busual = true, bool bassign = false) {
		extern valandtype getrvalue(valandtype lvalue);

		auto ops = std::array{ *(----immidiates.end()), immidiates.back() };

		//auto op0type = ops[0].value->getType();

		//if (!bassign)
			//ops[0] = integralpromotions(ops[0]);

		//if (ops[1].type.front().uniontype == type::BASIC)
			//ops[1] = integralpromotions(ops[1]);

		//ops[0].value =
		//	/*gen_pointer_to_elem_if_ptr_to_array*/ (ops[0].value);

		/*if (busual)
			ops = usualarithmeticconversions(ops);
		else if (ops[1].type.front().uniontype == type::BASIC &&
			op0type != ops[1].value->getType())
			ops[1].type = ops[0].type,
			ops[1].value = llvmbuilder.CreateIntCast(
				ops[1].value, op0type,
				ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned");*/

		if (busual)
			ops = usualarithmeticconversions(ops);

		immidiates.erase(----immidiates.end(), immidiates.end());

		return ops;
	}

	virtual void mlutiplylasttwovalues() {
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateMul(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void dividelasttwovalues() {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateSDiv(ops[0].value, ops[1].value);
		else
			ops[0].value = llvmbuilder.CreateUDiv(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void modulolasttwovalues() {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateSRem(ops[0].value, ops[1].value);
		else
			ops[0].value = llvmbuilder.CreateURem(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void addlasttwovalues(bool bminus, bool busual = true) {
		extern valandtype getrvalue(valandtype lvalue);
		if (subscripttwovalues(bminus)) {
			getaddress();
			return;
		}
		std::array ops = std::array{ *(----immidiates.end()), immidiates.back() };

		immidiates.erase(----immidiates.end(), immidiates.end());

		extern ::val decay(::val lvalue);

		ops[0] = decay(ops[0]);

		ops[1] = decay(ops[1]);

		extern void printvaltype(val);

		if (ops[0].value->getType()->isPointerTy() &&
			ops[1].value->getType()->isPointerTy()) {
			assert(bminus);
			ops[0].value =
				llvmbuilder.CreatePtrDiff(ops[0].value, ops[1].value);
			type ptr_diff{ type::BASIC };
			ptr_diff.spec.basicdeclspec.basic[1] = "int";
			ops[0].type = { ptr_diff };
		}
		else
			busual && (ops = usualarithmeticconversions(ops), 0),
			printvaltype(ops[0]),
			printvaltype(ops[1]),
			ops[0].value = llvmbuilder.CreateAdd(
				ops[0].value,
				!bminus ? ops[1].value : llvmbuilder.CreateNeg(ops[1].value));

		immidiates.push_back(ops[0]);
	}

	virtual void shifttwovalues(bool bright) {
		std::array ops = getops(false);

		if (!bright)
			ops[0].value = llvmbuilder.CreateShl(ops[0].value, ops[1].value);
		else if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value = llvmbuilder.CreateAShr(ops[0].value, ops[1].value);
		else
			ops[0].value = llvmbuilder.CreateLShr(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred) {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value =
			llvmbuilder.CreateICmp(pred, ops[0].value, ops[1].value);
		else
			ops[0].value =
			llvmbuilder.CreateCmp(pred, ops[0].value, ops[1].value);

		ops[0].value =
			CreateCastInst(ops[0].value, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());;

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void bitwisetwovalues(llvm::Instruction::BinaryOps op) {
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateBinOp(op, ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void logictwovalues(bool bisand) {
		std::array ops = getops(false);

		ops[0].value = llvmbuilder.CreateICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[0].value, getValZero(ops[0]));

		ops[1].value = llvmbuilder.CreateICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[1].value, getValZero(ops[1]));

		ops[0].value = !bisand
			? llvmbuilder.CreateOr(ops[0].value, ops[1].value)
			: llvmbuilder.CreateAnd(ops[0].value, ops[1].value);

		ops[0].value = llvm::CastInst::Create(
			llvm::Instruction::CastOps::ZExt, ops[0].value,
			llvm::Type::getInt32Ty(llvmctx), "", pcurrblock.back());

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void assigntwovalues() {
		std::array ops = getops(false, true);

		extern ::val decay(::val lvalue);

		ops[1] = decay(ops[1]);

		ops[1] = convertTo(ops[1], ops[0].lvalues.back().type);

		printtype(ops[0].lvalues.back().value->getType(),
			ops[0].originident);
		printtype(ops[1].value->getType(), ops[1].originident);

		llvmbuilder.CreateStore(ops[1].value, ops[0].lvalues.back().value);

		immidiates.push_back({ ops[1].value, ops[1].type, ops[1].lvalues });
	}

	virtual bool subscripttwovalues(bool bminus = false) {
		extern valandtype getrvalue(valandtype lvalue);

		auto ops = std::array{ *(----immidiates.end()), immidiates.back() };

		for (auto i = 0; i < 2; ++i)
			if (ops[i].type.front().uniontype == type::POINTER ||
				ops[i].type.front().uniontype == type::ARRAY)
				if (ops[!i].type.front().uniontype == type::BASIC) {
					extern ::val decay(::val lvalue);
					ops[i] = decay(ops[i]);
					immidiates.erase(----immidiates.end(), immidiates.end());
					ops[i].type.erase(ops[i].type.begin());
					ops[!i].value = !bminus
						? ops[!i].value
						: llvmbuilder.CreateNeg(ops[!i].value);
					llvm::Value* lvalue = llvmbuilder.CreateGEP(ops[i].value,

						ops[!i].value);
					immidiates.push_back(
						{ ops[i].type.front().uniontype != type::FUNCTION
							 ? llvmbuilder.CreateLoad(lvalue)
							 : lvalue,
						 ops[i].type, ops[i].lvalues, ops[i].originident });
					immidiates.back().lvalues.push_back(
						{ lvalue, ops[i].type });
					printtype(lvalue->getType(), ops[i].originident);
					printtype(immidiates.back().value->getType(),
						ops[i].originident);
					printtype(ops[!i].value->getType(), ops[!i].originident);
					return true;
				}
		return false;
	}

	virtual void getaddress() {
		extern void printvaltype(val);
		auto& val = immidiates.back();
		assert(val.lvalues.size());
		auto lval = val.lvalues.back();
		val.value = lval.value;
		val.type.insert(val.type.begin(), { type::POINTER });
		val.lvalues.pop_back();
		printvaltype(val);
	}

	virtual void applyindirection() {
		extern valandtype getrvalue(valandtype lvalue);
		extern ::val decay(::val lvalue);

		unsigned int type = 3;// << 2 | 2;

		auto immlvalue = immidiates.back();

		std::cout << "applying indirection" << std::endl;

		insertinttoimm("0", sizeof "0" - 1, "ul", sizeof "ul" - 1, type);

		subscripttwovalues();
	}

	void insertinttoimm(const char* str, size_t szstr, const char* suffix_in, size_t szstr1, int type) {
		std::string imm, suffix{ suffix_in, szstr1 };

		imm.assign(str, szstr);

		std::vector<::type> currtype = { 1, ::type::BASIC };

		const bool isunsigned = suffix.find("u") != suffix.npos || suffix.find("U") != suffix.npos;

		const bool islonglong = suffix.find("ll") != suffix.npos || suffix.find("LL") != suffix.npos;

		const bool islong = !islonglong && (suffix.find("l") != suffix.npos || suffix.find("L") != suffix.npos);

		//type >>= 2;

		const int base[] = { 16, 2, 7, 10 };

		const unsigned long long val = isunsigned ? std::stoull(imm, nullptr, base[type]) : std::stoll(imm, nullptr, base[type]);

#define CHECK_VAL_RANGE(range) isunsigned ? val <= (range) : val <= ((long long)range)

		auto& currbasictype = currtype.back().spec.basicdeclspec;

		if (islonglong && isunsigned) goto ulonlong;
		else if (islonglong) goto longlong;
		else if (islong && isunsigned) goto ulong;
		else if (islong) goto slong;
		else if (isunsigned) goto uint;

		if (CHECK_VAL_RANGE(INT_MAX))
			currbasictype.basic[1] = "int";
		else uint: if (CHECK_VAL_RANGE(UINT_MAX) && (base[type] != 10 || isunsigned))
			currbasictype.basic[1] = "int",
			currbasictype.basic[0] = "unsigned";
		else slong : if (CHECK_VAL_RANGE(LONG_MAX))
			currbasictype.basic[1] = "long",
			currbasictype.longspecsn = 1;
		else ulong : if (CHECK_VAL_RANGE(ULONG_MAX))
			currbasictype.basic[0] = "unsigned",
			currbasictype.basic[1] = "long",
			currbasictype.longspecsn = 1;
		else longlong : if (CHECK_VAL_RANGE(LLONG_MAX))
			currbasictype.basic[1] = "long",
			currbasictype.longspecsn = 2;
		else ulonlong : if (CHECK_VAL_RANGE(ULLONG_MAX) && (base[type] != 10 || isunsigned))
			currbasictype.basic[0] = "unsigned",
			currbasictype.basic[1] = "long",
			currbasictype.longspecsn = 2;

		auto valval = llvm::ConstantInt::get(createllvmtype(currtype), val, !isunsigned);

		immidiates.push_back({ valval, currtype });
	}
};

const llvm::fltSemantics& getfloatsembytype(val val) {
	if (val.type.front().spec.basicdeclspec.basic[1] == "float")
		return llvm::APFloatBase::IEEEsingle();
	else switch (val.type.front().spec.basicdeclspec.longspecsn)
		case 1: return llvm::APFloatBase::IEEEquad();
	return llvm::APFloatBase::IEEEdouble();
}

struct handlefpexpr : basehndl {

	virtual void getnegative() override {
		//immidiates.back() = integralpromotions(immidiates.back());

		immidiates.back().value = llvmbuilder.CreateFNeg(immidiates.back().value);
	}

	virtual void getlogicalnot() override {
		auto& op = immidiates.back();

		op.value = llvmbuilder.CreateFCmp(
			llvm::CmpInst::Predicate::FCMP_OEQ, op.value,
			llvm::ConstantFP::get(llvmctx, llvm::APFloat{ getfloatsembytype(op) }));

		op.type.erase(op.type.begin(), --op.type.end());

		op.value = CreateCastInst(op.value, llvm::Type::getInt32Ty(llvmctx), false);

		extern void printvaltype(::val val);

		printvaltype(op);

		op.type.back().spec.basicdeclspec.basic[1] = "int",
			op.type.back().spec.basicdeclspec.basic[0] = "";
	}

	virtual void mlutiplylasttwovalues() override {
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateFMul(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void dividelasttwovalues() override {
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateFDiv(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void modulolasttwovalues() override {
		std::array ops = getops();


		ops[0].value = llvmbuilder.CreateFRem(ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	virtual void addlasttwovalues(bool bminus, bool busual) override {

		std::array ops = std::array{ *(----immidiates.end()), immidiates.back() };

		immidiates.erase(----immidiates.end(), immidiates.end());


		busual && (ops = usualarithmeticconversions(ops), 0),
			ops[0].value = llvmbuilder.CreateFAdd(
				ops[0].value,
				!bminus ? ops[1].value : llvmbuilder.CreateFNeg(ops[1].value));

		immidiates.push_back(ops[0]);
	}

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred) override {
		std::array ops = getops();

		ops[0].value =
			llvmbuilder.CreateFCmp(pred, ops[0].value, ops[1].value);

		ops[0].value =
			CreateCastInst(ops[0].value, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void logictwovalues(bool bisand)  override {
		std::array ops = getops(false);

		ops[0].value = llvmbuilder.CreateFCmp(
			llvm::CmpInst::Predicate::FCMP_ONE, ops[0].value,
			llvm::ConstantFP::get(llvmctx, llvm::APFloat{ getfloatsembytype(ops[0]) }));

		ops[1].value = llvmbuilder.CreateFCmp(
			llvm::CmpInst::Predicate::FCMP_ONE, ops[1].value,
			llvm::ConstantFP::get(llvmctx, llvm::APFloat{ getfloatsembytype(ops[1]) }));

		ops[0].value = !bisand
			? llvmbuilder.CreateOr(ops[0].value, ops[1].value)
			: llvmbuilder.CreateAnd(ops[0].value, ops[1].value);

		ops[0].value = CreateCastInst(ops[0].value, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}
};

struct handlecnstexpr : basehndl {

	using val = ::val;

	std::list<val>& immidiates = basehndl::immidiates;

	/*auto getops(bool busual = true) {
		auto ops = basehndl::getops();

		return reinterpret_cast<std::array<val, 2>&&> (ops);
	}*/

	virtual void mlutiplylasttwovalues() override {
		std::array ops = getops();

		ops[0].constant = llvm::ConstantExpr::getMul(ops[0].constant, ops[1].constant);

		immidiates.push_back(ops[0]);
	}

	virtual void dividelasttwovalues() override {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].constant =
			llvm::ConstantExpr::getSDiv(ops[0].constant, ops[1].constant);
		else
			ops[0].constant =
			llvm::ConstantExpr::getUDiv(ops[0].constant, ops[1].constant);

		immidiates.push_back(ops[0]);
	}

	virtual void modulolasttwovalues() override {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].constant =
			llvm::ConstantExpr::getSRem(ops[0].constant, ops[1].constant);
		else
			ops[0].constant =
			llvm::ConstantExpr::getURem(ops[0].constant, ops[1].constant);

		immidiates.push_back(ops[0]);
	}

	virtual void addlasttwovalues(bool bminus, bool busual) override {
		assert(busual);
		std::array ops = getops();

		ops[0].constant = llvm::ConstantExpr::getAdd(
			ops[0].constant,
			!bminus ? ops[1].constant : llvm::ConstantExpr::getNeg(ops[1].constant));

		immidiates.push_back(ops[0]);
	}

	virtual void shifttwovalues(bool bright) override {
		std::array ops = getops(false);

		ops[0] = integralpromotions(ops[0]);

		ops[1] = integralpromotions(ops[1]);

		if (ops[1].type.front().uniontype == type::BASIC &&
			ops[0].constant->getType() != ops[1].constant->getType())
			ops[1].type = ops[0].type,
			ops[1].constant = CreateCastInst(
				ops[1].constant, ops[0].constant->getType(),
				ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned");

		if (!bright)
			ops[0].constant =
			llvm::ConstantExpr::getShl(ops[0].constant, ops[1].constant);
		else if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].constant =
			llvm::ConstantExpr::getAShr(ops[0].constant, ops[1].constant);
		else
			ops[0].constant =
			llvm::ConstantExpr::getLShr(ops[0].constant, ops[1].constant);

		immidiates.push_back(ops[0]);
	}

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred) override {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].constant =
			llvm::ConstantExpr::getICmp(pred, ops[0].constant, ops[1].constant);
		else
			ops[0].constant = llvm::ConstantExpr::getCompare(pred, ops[0].constant,
				ops[1].constant);

		ops[0].constant =
			CreateCastInst(ops[0].constant, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());;

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void bitwisetwovalues(llvm::Instruction::BinaryOps op) override {
		std::array ops = getops();

		ops[0].constant = llvm::ConstantExpr::get(op, ops[0].constant, ops[1].constant);

		immidiates.push_back(ops[0]);
	}

	virtual void logictwovalues(bool bisand) override {
		std::array ops = getops(false);

		ops[0].constant = llvm::ConstantExpr::getICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[0].constant, getValZero(ops[0]));

		ops[1].constant = llvm::ConstantExpr::getICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[1].constant, getValZero(ops[1]));

		ops[0].constant =
			!bisand ? llvm::ConstantExpr::getOr(ops[0].constant, ops[1].constant)
			: llvm::ConstantExpr::getAnd(ops[0].constant, ops[1].constant);

		ops[0].constant =
			CreateCastInst(ops[0].constant,
				llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());;

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual llvm::Constant* CreateCastInst(llvm::Value* C, llvm::Type* Ty, bool bissigned) override {
		return llvm::ConstantExpr::getIntegerCast(llvm::dyn_cast<llvm::Constant>(C), Ty, bissigned);
	}
};

static handlecnstexpr hndlcnstexpr{};

static handlefpexpr hndlfpexpr{};

static basehndl hndlbase{};

static basehndl* phndl = &hndlbase;

//static std::remove_reference<decltype (basehndl::immidiates)>::type::iterator
//cnstexpriterstart{};

size_t szcnstexprinitial;

// static std::string currdeclidentifier{};

enum class currdecltypeenum { TYPEDEF, CAST, PARAMS, NORMAL, UNKNOWN, NONE };

std::string currdeclspectypedef;

extern std::vector<std::string> qualifsandtypes;

static std::list<std::list<var>> scopevar{ 1 };

static std::list<std::list<std::list<var>>> structorunionmembers{ 1 };

struct currtypevectorbeingbuild {
	std::list<std::list<var>>::iterator p;
	currdecltypeenum currdecltype;
};

std::list<currtypevectorbeingbuild> currtypevectorbeingbuild = {
	{scopevar.begin(), currdecltypeenum::NORMAL} };

static std::string currstring;

void printvaltype(val val) {
	if (!val.value)
		return;
	std::string type_str;
	llvm::raw_string_ostream rso(type_str);
	val.value->getType()->print(rso);
	std::string name = val.value->getName().str();
	name = name.size() ? name : val.originident;
	std::cout << name << " is " << rso.str() << std::endl;
}

extern valandtype getrvalue(valandtype lvalue);

extern "C" void obtainvalbyidentifier(const char* identifier, size_t szstr) {
	std::string ident{ identifier, szstr };

	const ::var* var = nullptr;

	std::find_if(scopevar.rbegin(), scopevar.rend(),
		[&](const std::list<::var>& scope) {
			auto iter =
				std::find_if(scope.rbegin(), scope.rend(),
					[&](const ::var& scopevar) {
						return scopevar.identifier == ident;
					});

			if (iter != scope.rend())
				return var = &*iter, true;
			return false;
		});

	/*if (var->type.front ().uniontype == ::type::FUNCTION)
		phndl->immidiates.push_back (
			{pglobal = nonconstructable.mainmodule.getFunction (ident),
			 var->type, true});
	else if (pglobal = nonconstructable.mainmodule.getGlobalVariable (ident))
		phndl->immidiates.push_back ({pglobal, var->type, true});*/

	val immidiate;
	auto& currfunctype =
		currfunc->type.front().spec.func.parametertypes_list.front();

	if (auto findparam = std::find_if(
		currfunctype.begin(), currfunctype.end(),
		[&](const ::var& param) { return param.identifier == ident; });
		findparam != currfunctype.end())
		var = &*findparam;

	llvm::Value* pglobal;

	immidiate.type = var->type;

	pglobal = immidiate.value = var->pValue;

	immidiate.originident = var->identifier;

	immidiate.lvalues.push_back(immidiate);

	printvaltype(immidiate);

	if (immidiate.value && !immidiate.value->getType()->getPointerElementType()->isFunctionTy())
		immidiate.value = llvmbuilder.CreateLoad(immidiate.value);

	phndl->immidiates.push_back(immidiate);

	printvaltype(immidiate);
}

extern "C" void addplaintexttostring(const char* str, size_t szstr) {
	currstring += std::string{ str, szstr };
}

extern "C" void addescapesequencetostring(const char* str, size_t szstr) {
	std::string escape{ str, szstr };

	switch (stringhash(escape.c_str())) {
	case "\\n"_h:
		currstring += '\n';
		break;
	}
}

extern "C" void constructstring() {
	std::vector<::type> stirngtype{ 1, ::type::ARRAY };

	stirngtype.back().spec.arraysz = currstring.size() + 1;

	stirngtype.push_back({ ::type::BASIC });

	stirngtype.back().spec.basicdeclspec.basic[1] = "char";

	auto lvalue = llvmbuilder.CreateGlobalStringPtr(
		currstring, "", 0, &nonconstructable.mainmodule);

	hndlbase.immidiates.push_back(
		{ llvmbuilder.CreateLoad(lvalue), stirngtype, {}, "[[stringliteral]]" });
	hndlbase.immidiates.back().lvalues.push_back(hndlbase.immidiates.back());
	hndlbase.immidiates.back().lvalues.back().value = lvalue;
	currstring = "";
}

void addvar(var& lastvar) {

	const char* lastvartypestoragespec =
		lastvar.type.back().spec.basicdeclspec.basic[2].c_str();

	if (lastvar.bistypedef)
		for (lastvar.type.back().spec.basicdeclspec.basic[2] = "typedef";;)
			for(lastvar.pValue = nullptr/*(llvm::Value *)lastvar.pllvmtype*/;;)
				return;

	switch (scopevar.size()) {
	case 1:
		llvm::GlobalValue::LinkageTypes linkagetype;

		switch (stringhash(lastvartypestoragespec)) {
		default:
		case "extern"_h:
			linkagetype = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			break;
		case "static"_h:
			linkagetype = llvm::GlobalValue::LinkageTypes::InternalLinkage;
		}

		switch (lastvar.type[0].uniontype) {
		case type::POINTER:
		case type::ARRAY:
		case type::BASIC:
			if (scopevar.size() == 1)
				lastvar.pValue = new llvm::GlobalVariable(
					nonconstructable.mainmodule, lastvar.pllvmtype, false,
					linkagetype,
					llvm::Constant::getNullValue(lastvar.pllvmtype),
					lastvar.identifier);
			// scopevar.front ().push_back (lastvar);
			break;
		case type::FUNCTION:
			lastvar.pValue = llvm::Function::Create(
				llvm::dyn_cast<llvm::FunctionType> (lastvar.pllvmtype),
				linkagetype, lastvar.identifier, nonconstructable.mainmodule);
			// scopevar.front ().push_back (lastvar);
			break;
		}
		break;
	default:
		lastvar.pValue = llvmbuilder.CreateAlloca(lastvar.pllvmtype, nullptr,
			lastvar.identifier);
		break;
	}
}

extern std::pair<std::string, std::string> currstruct;

extern "C" void startbuildingstructorunion() {
	var tmp;
	type typestruct{ type::BASIC };
	typestruct.spec.basicdeclspec.basic[3] = currstruct.second;
	typestruct.spec.basicdeclspec.basic[0] = currstruct.first;
	tmp.type.push_back(typestruct);
	tmp.identifier = currstruct.second;
	tmp.pllvmtype = llvm::StructType::create(llvmctx);
	structorunionmembers.back().push_back({ tmp });
	currtypevectorbeingbuild.push_back(
		{ --structorunionmembers.back().end(), currdecltypeenum::PARAMS });
	currstruct.second.clear();
	currstruct.first.clear();
}

extern "C" void endbuildingstructorunion() {
	auto& lastmembers = structorunionmembers.back().back();
	auto& structvar = lastmembers.front();

	for (auto& a : lastmembers | ranges::views::drop(1))
		a.pllvmtype = createllvmtype(a.type);

	std::vector<llvm::Type*> structtypes;

	std::transform(++lastmembers.begin(), lastmembers.end(),
		std::back_inserter(structtypes),
		[](const var& elem) { return elem.pllvmtype; });

	assert(structvar.type.back().spec.basicdeclspec.basic[0] == "struct");

	dyn_cast<llvm::StructType> (structvar.pllvmtype)->setBody(structtypes);

	currtypevectorbeingbuild.pop_back();

	currstruct.first = structvar.type.back().spec.basicdeclspec.basic[0];
	currstruct.second = structvar.type.back().spec.basicdeclspec.basic[3];
}

extern "C" void startdeclaration(const char *str_typedefname, size_t szstr) {
	std::string typedefname{ str_typedefname , szstr };
	var var;

	type basic{ type::BASIC };

	if (qualifsandtypes.empty() && currstruct.first.empty() && typedefname.empty())
		assert(!(scopevar.size() > 1)),
		basic.spec.basicdeclspec.basic[1] = "int";
	else
		basic.spec.basicdeclspec = parsebasictype(::qualifsandtypes);

	if(!(basic.spec.basicdeclspec.basic[3] = typedefname).empty())
		obtainvalbyidentifier(basic.spec.basicdeclspec.basic[3].data(), basic.spec.basicdeclspec.basic[3].size()),
		var.type = immidiates.back().type,
		immidiates.pop_back();

	currdeclspectypedef = "";

	qualifsandtypes.clear();

	if (basic.spec.basicdeclspec.basic[3].empty())
		if (!(basic.spec.basicdeclspec.basic[3] = currstruct.second).empty())
			basic.spec.basicdeclspec.basic[0] = currstruct.first;

	currstruct.first.clear();

	currstruct.second.clear();

	if(var.type.empty())
		var.type.push_back(basic);

	currtypevectorbeingbuild.back().p->push_back(var);
}

bool bIsBasicFloat(const type& type);

bool bIsBasicInteger(const type& type) {
	std::set integertraits{ "unsigned", "signed", "" };
	return type.uniontype == type::BASIC && ranges::find(integertraits, type.spec.basicdeclspec.basic[0]) != integertraits.end() && !bIsBasicFloat(type);
}

bool bIsBasicFloat(const type& type) {
	return type.uniontype == type::BASIC && ranges::contains(std::array{ "float", "double" }, type.spec.basicdeclspec.basic[1]);
}

val convertTo(val target, std::vector<::type> to) {
	if (bIsBasicInteger(to.front()))
		if (bIsBasicInteger(target.type.front()))
			target.value = llvmbuilder.CreateIntCast(
				target.value, createllvmtype(to),
				target.type.front().spec.basicdeclspec.basic[0] != "unsigned");
		else if (bIsBasicFloat(target.type.front()))
			if (to.back().spec.basicdeclspec.basic[0] == "unsigned")
				target.value = llvmbuilder.CreateFPToUI(target.value, createllvmtype(to));
			else
				target.value = llvmbuilder.CreateFPToSI(target.value, createllvmtype(to));
		else;
	else if (bIsBasicFloat(to.front()))
		if (bIsBasicInteger(target.type.front()))
			if (target.type.back().spec.basicdeclspec.basic[0] == "unsigned")
				target.value = llvmbuilder.CreateUIToFP(target.value, createllvmtype(to));
			else
				target.value = llvmbuilder.CreateSIToFP(target.value, createllvmtype(to));
		else if (bIsBasicFloat(target.type.front()))
			target.value = llvmbuilder.CreateFPCast(
				target.value, createllvmtype(to));
		else;
	else target.value = llvmbuilder.CreateBitCast(target.value, createllvmtype(to));

	target.type = to;

	//target.lvalues.clear();

	return target;
}

extern "C" void applycast() {
	auto lasttypevar = currtypevectorbeingbuild.back().p->back();

	currtypevectorbeingbuild.back().p->pop_back();

	auto& currtype = lasttypevar.type;

	std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	auto target = phndl->immidiates.back();

	phndl->immidiates.pop_back();

	target = convertTo(target, currtype);

	phndl->immidiates.push_back(target);
}

void endpriordecl() {
	auto& currtype = currtypevectorbeingbuild.back().p->back().type;

	std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	if (currtypevectorbeingbuild.back().currdecltype !=
		currdecltypeenum::PARAMS)
		currtypevectorbeingbuild.back()
		.p->back()
		.pllvmtype = createllvmtype(currtype),
		addvar(currtypevectorbeingbuild.back().p->back());
}

size_t getbasictypesz(type basictype) {
	if (basictype.uniontype == type::POINTER)
		return sizeof(void*);

	assert(basictype.uniontype == type::BASIC);

	if (basictype.spec.basicdeclspec.basic[0] == "struct")
		return pdatalayout->getTypeStoreSize(createllvmtype({ basictype }));

	switch (stringhash(basictype.spec.basicdeclspec.basic[1].c_str())) {
	case "int"_h:
		return sizeof(int);
	case "char"_h:
		return sizeof(char);
	case "long"_h:
		return sizeof(long);
	case "short"_h:
		return sizeof(short);
	}

	throw "invalid type";
}

void pushsizeoftype(std::vector<type> type) {
	size_t szoftype = 1;

	::type sztype{ ::type::BASIC };

	sztype.spec.basicdeclspec.basic[0] = "unsigned";

	sztype.spec.basicdeclspec.basic[1] = "long";

	for (;;)
		if (type.front().uniontype == type::ARRAY)
			szoftype *= type.front().spec.arraysz, type.erase(type.begin());

		else if (type.front().uniontype == type::BASIC ||
			type.front().uniontype == type::POINTER)
			return (void)(szoftype *= getbasictypesz(type.front()),
				phndl->immidiates.push_back(
					{ llvm::ConstantInt::getIntegerValue(
						 llvm::IntegerType::get(llvmctx, 64),
						 llvm::APInt{64, szoftype}),
					 {sztype},
					 {},
					 "[[sizeoftypename]]" }));
}

extern const std::list<::var>* getstructorunion(std::string ident);

extern "C" void memberaccess(const char* arrowordot, size_t szstr,
	const char* ident, size_t szstr1) {
	if (std::string{ arrowordot, szstr } == "->")
		phndl->applyindirection();

	auto& lastvar = phndl->immidiates.back();

	auto pliststruct =
		getstructorunion(lastvar.type.front().spec.basicdeclspec.basic[3]);

	auto imember = 0;

	auto listiter = ++pliststruct->begin();

	for (; listiter != pliststruct->end() &&
		listiter->identifier != std::string{ ident, szstr1 };
		++imember, ++listiter)
		;

	auto& lastlvalue = lastvar.lvalues.back();

	auto& member = *listiter;

	llvm::Value* lvalue = llvmbuilder.CreateGEP(
		lastlvalue.value->getType()->getPointerElementType(),
		lastlvalue.value,

		{ llvmbuilder.getInt32(0), llvmbuilder.getInt32(imember) });

	printtype(member.pllvmtype->getPointerTo(), "");

	// lvalue = llvmbuilder.CreatePointerCast(lvalue,
	// member.pllvmtype->getPointerTo());

	printtype(lvalue->getType(),
		lastvar.originident + " " + std::to_string(imember));

	llvm::Value* rvalue = llvmbuilder.CreateLoad(lvalue);

	lastvar.type = member.type;

	lastvar.originident = member.identifier;

	lastvar.value = rvalue;

	printvaltype(lastvar);

	lastvar.lvalues.push_back({ lvalue, lastvar.type });

	return;
}

extern "C" void endsizeoftypename() {

	auto currtype = currtypevectorbeingbuild.back().p->back().type;

	std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	currtypevectorbeingbuild.back().p->pop_back();

	pushsizeoftype(currtype);
}

std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>> ifstatements;

extern "C" llvm::BranchInst * splitbb(const char* identifier, size_t szident);

extern "C" void insertinttoimm(const char* str, size_t szstr, const char* suffix, size_t szstr1, int type);

extern "C" void startifstatement() {
	auto dummyblock = llvm::BasicBlock::Create(llvmctx, "", dyn_cast<llvm::Function> (currfunc->pValue));

	if (!immidiates.size())
		insertinttoimm("1", sizeof "1" - 1, "", 0, 3);

	phndl->getlogicalnot();

	phndl->getlogicalnot();

	auto value = llvmbuilder.CreateIntCast(immidiates.back().value, llvm::IntegerType::get(llvmctx, 1), true);

	ifstatements.push_back({ {llvmbuilder.CreateCondBr(value, dummyblock, dummyblock)}, dummyblock });

	splitbb("", 0);

	ifstatements.back().first[0]->setSuccessor(0, pcurrblock.back());

	immidiates.pop_back();
}

extern "C" void continueifstatement() {
	ifstatements.back().first[1] = splitbb("", 0);
	ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());
}

extern "C" void endifstatement() {
	splitbb("", 0);
	if (auto lastbranchif = ifstatements.back().first[1])
		lastbranchif->setSuccessor(0, pcurrblock.back());

	if (ifstatements.back().first[0]->getSuccessor(1) == ifstatements.back().second)
		ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());

	ifstatements.back().second->eraseFromParent();
	ifstatements.pop_back();
}

extern "C" void endsizeofexpr() {
	auto lastimmtype = phndl->immidiates.back().type;
	printtype(createllvmtype(lastimmtype),
		phndl->immidiates.back().originident);
	phndl->immidiates.pop_back();
	pushsizeoftype(lastimmtype);
}

extern "C" void adddeclarationident(const char* str, size_t szstr,
	bool bistypedef) {
	std::string ident{ str, szstr };

	currtypevectorbeingbuild.back().p->back().identifier = ident;

	currtypevectorbeingbuild.back().p->back().bistypedef = bistypedef;
}

extern "C" void finalizedeclaration() { endpriordecl(); }

const std::list<::var>* getstructorunion(std::string ident) {
	const std::list<::var>* var = nullptr;

	std::find_if(structorunionmembers.rbegin(), structorunionmembers.rend(),
		[&](const std::list<std::list<::var>>& scope) {
			auto iter = std::find_if(
				scope.rbegin(), scope.rend(),
				[&](const std::list<::var>& scopevar) {
					return scopevar.front().identifier == ident;
				});

			if (iter != scope.rend())
				return var = &*iter, true;
			return false;
		});
	return var;
}

llvm::Type* createllvmtype(std::vector<type> decltypevector) {
	llvm::Type* pcurrtype;

	std::array lambdas = {
		std::function{[&](const type& type) {
			switch (stringhash(type.spec.basicdeclspec.basic[1].c_str())) {
			case "short"_h:
				pcurrtype =
					dyn_cast<llvm::Type> (llvm::Type::getInt16Ty(llvmctx));
				break;
				break;
			case "int"_h:
			case "long"_h:
				if (type.spec.basicdeclspec.longspecsn > 1) case "__int64"_h:
					pcurrtype = dyn_cast<llvm::Type> (llvm::Type::getInt64Ty(llvmctx));
				else pcurrtype = dyn_cast<llvm::Type> (llvm::Type::getInt32Ty(llvmctx));
				break;
				break;
			case "char"_h:
				pcurrtype =
					dyn_cast<llvm::Type> (llvm::Type::getInt8Ty(llvmctx));
				break;
				break;
			case "float"_h:
				pcurrtype =
					dyn_cast<llvm::Type> (llvm::Type::getFloatTy(llvmctx));
				break;
				break;
			case "double"_h:
				pcurrtype =
					dyn_cast<llvm::Type> (llvm::Type::getDoubleTy(llvmctx));
				break;
				break;
			default:
				switch (
					stringhash(type.spec.basicdeclspec.basic[0].c_str())) {
				case "struct"_h:
					pcurrtype =
						getstructorunion(type.spec.basicdeclspec.basic[3])
							->front()
							.pllvmtype;
					break;
				case "union"_h:
					break;
				default: { // typedef
					obtainvalbyidentifier(type.spec.basicdeclspec.basic[3].data(), type.spec.basicdeclspec.basic[3].size());
					pcurrtype = (llvm::Type*)immidiates.back().value;
					immidiates.pop_back();
					break;
				}
				}
			}
		}},
		std::function{[&](const type& type) {
			pcurrtype = dyn_cast<llvm::Type> (pcurrtype->getPointerTo());
		}},
		std::function{[&](const type& type) {
			pcurrtype = dyn_cast<llvm::Type> (
				llvm::ArrayType::get(pcurrtype, type.spec.arraysz));
		}},
		std::function{[&](const type& type) {
			std::vector<llvm::Type*> paramtype;
			for (auto& a : type.spec.func.parametertypes_list.front())
				paramtype.push_back(a.pllvmtype);
			pcurrtype = dyn_cast<llvm::Type> (llvm::FunctionType::get(
				pcurrtype, paramtype, type.spec.func.bisvariadic));
		}} };

	std::reverse(decltypevector.begin(), decltypevector.end());

	for (const auto& type : decltypevector)
		lambdas[type.uniontype](type);

	return pcurrtype;
}

extern std::vector<llvm::BasicBlock*> pcurrblock;

void finalizedecl();

static std::list<std::pair<llvm::SwitchInst*, llvm::BasicBlock*>> currswitch;

extern "C" llvm::BranchInst * splitbb(const char* identifier, size_t szident);

extern "C" void beginscope() {
	bool beginofafnuc = scopevar.size() == 1;
	if (beginofafnuc) {
		var current_arg;
		currfunc = --scopevar.back().end();
		auto iter_params = currfunc->type.front()
			.spec.func.parametertypes_list.front()
			.begin();
		for (auto& arg : dyn_cast<llvm::Function> (currfunc->pValue)->args())
			iter_params++->pValue = &arg;
	}

	splitbb("", 0);
	//pcurrblock.push_back (llvm::BasicBlock::Create (
		//llvmctx, "", dyn_cast<llvm::Function> (currfunc->pValue)));
	//llvmbuilder.SetInsertPoint (pcurrblock.back ());
	scopevar.push_back({});
	currtypevectorbeingbuild.push_back(
		{ --scopevar.end(), currdecltypeenum::NORMAL });

	var OrigParamValue;

	if (beginofafnuc) for (auto& param
		: currfunc->type.front().spec.func.parametertypes_list.front()) {
		auto origparamvalue = param.pValue;

		addvar(param);

		llvmbuilder.CreateStore(origparamvalue, param.pValue);
	}

	structorunionmembers.push_back({});

	std::cout << "begin scope finished" << std::endl;
}

void fixuplabels();

extern "C" void endscope() {
	// nonconstructable.mainmodule.
	// endexpression();
	//pcurrblock.pop_back();
	scopevar.pop_back();
	if (scopevar.size() > 1)
		splitbb("", 0);
	else pcurrblock.pop_back();
	currtypevectorbeingbuild.pop_back();
	structorunionmembers.pop_back();

	if (scopevar.size() == 1) //end of a function
		fixuplabels();
}

extern "C" void endexpression() { phndl->immidiates.clear(); }

std::list<llvm::BasicBlock*> dowhileloops;

std::list<std::list<llvm::BranchInst*>> breakbranches;

std::list<std::list<llvm::BranchInst*>> continuebranches;

void fixupcontinuebranches(), fixupbrakebranches();

extern "C" void startdowhileloop() {
	splitbb("", 0);

	dowhileloops.push_back(pcurrblock.back());
	breakbranches.push_back({});
	continuebranches.push_back({});
}

extern "C" void enddowhileloop() {
	fixupcontinuebranches();
	startifstatement();
	fixupbrakebranches();
	ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());
	ifstatements.back().first[0]->setSuccessor(0, dowhileloops.back());
	ifstatements.back().second->eraseFromParent();
	ifstatements.pop_back();
	dowhileloops.pop_back();
}

extern "C" void addbreak() {
	breakbranches.back().push_back(llvmbuilder.CreateBr(pcurrblock.back()));
}

extern "C" void addcontinue() {
	continuebranches.back().push_back(llvmbuilder.CreateBr(pcurrblock.back()));
}

void fixupbrakebranches() {
	for (auto branch : breakbranches.back())
		branch->setSuccessor(0, pcurrblock.back());

	breakbranches.pop_back();
}

void fixupcontinuebranches() {
	for (auto branch : continuebranches.back())
		branch->setSuccessor(0, pcurrblock.back());

	continuebranches.pop_back();
}

std::list<std::array<llvm::BasicBlock*, 2>> forloops;

extern "C" void startforloopcond() {
	std::array<llvm::BasicBlock*, 2> bb;
	splitbb("", 0);
	bb[0] = pcurrblock.back();

	forloops.push_back(bb);
	breakbranches.push_back({});
	continuebranches.push_back({});
}

extern "C" void endforloopcond() {
	startifstatement();
	endexpression();
	//insertinttoimm("0", sizeof "0" - 1, "", 0, 3);
	//startifstatement();
	auto lastblcok = pcurrblock.back();

	forloops.back()[1] = lastblcok;
}

extern "C" void addforloopiter() {
	llvmbuilder.CreateBr(forloops.back()[0]);
	splitbb("", 0);

	//endifstatement();
	ifstatements.back().first[0]->setSuccessor(0, pcurrblock.back());
}

extern "C" void endforloop() {
	fixupcontinuebranches();
	llvmbuilder.CreateBr(forloops.back()[1]);
	forloops.pop_back();
	//endifstatement();
	splitbb("", 0);
	fixupbrakebranches();
	ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());
	ifstatements.back().second->eraseFromParent();
	ifstatements.pop_back();
}

val decay(val lvalue) {
	auto& currtype = lvalue.type;
	if (currtype.front().uniontype == type::ARRAY) {
		::type ptrtype{ ::type::POINTER };

		currtype.erase(currtype.begin());
		currtype.push_back(ptrtype);
		std::rotate(currtype.rbegin(), currtype.rbegin() + 1,
			currtype.rend());

		assert(currtype.front().uniontype == ::type::POINTER);

		printvaltype(lvalue);

		lvalue.value = lvalue.lvalues.back().value;

		lvalue.lvalues.pop_back();

		if (lvalue.value->getType()->getPointerElementType()->isArrayTy())
			lvalue.value = llvmbuilder.CreateGEP(
				lvalue.value,
				{ llvmbuilder.getInt64(0), llvmbuilder.getInt64(0) });
	}
	return lvalue;
}

valandtype getrvalue(valandtype lvalue) {
	auto& currtype = lvalue.type;
	if (currtype.front().uniontype != type::FUNCTION) {
		if (lvalue.value->getType()->getPointerElementType()->isFunctionTy())
			return lvalue;
		std::cout << "rvalue" << std::endl;
		basehndl::val ret = {
			llvmbuilder.CreateLoad(
				/*gen_pointer_to_elem_if_ptr_to_array*/ (lvalue.value)),
			currtype };
		printvaltype(ret);
		return ret;
	}
	return lvalue;
}

extern "C" void startfunctioncall() {
	callees.push_back(--hndlbase.immidiates.end());
}

std::list<std::pair<llvm::BranchInst*, std::string>> branches;

extern "C" llvm::BranchInst * splitbb(const char* identifier, size_t szident) {
	bool bareweabrupt = bareweinabrupt();
	if (pcurrblock.size())
		pcurrblock.pop_back();
	else bareweabrupt = true;
	pcurrblock.push_back(llvm::BasicBlock::Create(
		llvmctx, std::string{ identifier, szident }, dyn_cast<llvm::Function> (currfunc->pValue)));
	llvm::BranchInst* preturn;
	if (!bareweabrupt)
		preturn = llvmbuilder.CreateBr(pcurrblock.back());
	llvmbuilder.SetInsertPoint(pcurrblock.back());
	return preturn;
}

extern "C" void gotolabel(const char* identifier, size_t szident) {
	branches.push_back({ llvmbuilder.CreateBr(pcurrblock.back()), std::string{identifier, szident} });
}

void fixuplabels() {
	for (auto& [branchinst, ident] : branches)
		for (auto& bb : dyn_cast<llvm::Function> (currfunc->pValue)->getBasicBlockList())
			if (bb.getName() == ident)
				branchinst->setSuccessor(0, &bb);
	branches.clear();
}

extern "C" void startswitch() {
	//pcurrblock.pop_back();
	//pcurrblock.push_back(llvm::BasicBlock::Create(llvmctx, "", dyn_cast<llvm::Function> (currfunc->pValue)));
	auto dummyblock = llvm::BasicBlock::Create(llvmctx, "", dyn_cast<llvm::Function> (currfunc->pValue));
	currswitch.push_back({ llvmbuilder.CreateSwitch(phndl->immidiates.back().value, dummyblock), dummyblock });
	breakbranches.push_back({});

	//llvmbuilder.SetInsertPoint(pcurrblock.back());
	phndl->immidiates.pop_back();
}

bool bareweinabrupt(bool barewe) {
	/*static bool bareweinabrupt;
	bool bareweinabruptlast = bareweinabrupt;
	bareweinabrupt = barewe;*/
	if (pcurrblock.size())
		if (pcurrblock.back()->getInstList().size())
		{
			auto lastinstropcode = pcurrblock.back()->back().getOpcode();
			return ranges::contains(std::array{ llvm::Instruction::Br, llvm::Instruction::Switch }, lastinstropcode);
		}
		else return false;
	return true;
}

extern "C" void endswitch() {
	splitbb("", 0);
	fixupbrakebranches();
	if (currswitch.back().first->getDefaultDest() == currswitch.back().second)
		currswitch.back().first->setDefaultDest(pcurrblock.back());
	currswitch.back().second->eraseFromParent();
	currswitch.pop_back();
}

extern "C" void addCase() {
	splitbb("", 0);
	currswitch.back().first->addCase(llvm::dyn_cast<llvm::ConstantInt>(phndl->immidiates.back().value), pcurrblock.back());
	phndl->immidiates.pop_back();
}

extern "C" void addDefaultCase() {
	splitbb("", 0);
	currswitch.back().first->setDefaultDest(pcurrblock.back());
}

std::vector<::type> getreturntype(std::vector<::type> type) {
	for (auto iter = type.begin(); iter < type.end(); ++iter)
		if (iter->uniontype == type::FUNCTION) {
			type.erase(type.begin(), iter + 1);
			return type;
		}
	return type;
}

/*val<> getfunctionptr(val<> fptr) {
	for(auto a = fptr.type.begin(); a != fptr.type.end(); ++a)
		if(a->uniontype == type::FUNCTION) break;
		else fptr.type.erase(a), fptr.value = llvmbuilder.CreatePointerCast
(value, type->getPointerTo ())
}*/

extern "C" void endfunctioncall() {
	auto lastblock = pcurrblock.back();

	auto argsiter = callees.back();

	auto calleevalntype = *argsiter;

	assert(calleevalntype.value->getType()->isPointerTy());

	/*std::string type_str;
	llvm::raw_string_ostream rso (type_str);
	calleevalntype.value->getType ()->print (rso);
	std::cout << calleevalntype.value->getName ().str () << " is " << rso.str ()
			  << std::endl;*/

	llvm::Value* callee = calleevalntype.value;

	auto functype = createllvmtype(calleevalntype.type);

	llvm::Value* pval;

	callees.pop_back();

	llvmbuilder.SetInsertPoint(lastblock);

	std::vector<llvm::Value*> immidiates;

	std::transform(
		++argsiter, hndlbase.immidiates.end(), std::back_inserter(immidiates),
		[](const basehndl::val& elem) { return decay(elem).value; });

	hndlbase.immidiates.erase(--argsiter, hndlbase.immidiates.end());

	printvaltype(calleevalntype);

	// if (functype->isPtrOrPtrVectorTy ())
	pval = llvmbuilder.CreateCall(
		llvm::dyn_cast<llvm::FunctionType> (
			callee->getType()->getPointerElementType()),
		callee, immidiates);
	// else
	// pval = llvmbuilder.CreateCall (
	// llvm::dyn_cast<llvm::Function> (callee)->getFunctionType (),
	// llvm::dyn_cast<llvm::Value> (callee), immidiates);

	phndl->immidiates.push_back(
		val{ pval, getreturntype(calleevalntype.type) });
}

extern "C" void endreturn() {
	//llvmbuilder.SetInsertPoint (pcurrblock.back ());
	llvmbuilder.CreateRet(hndlbase.immidiates.back().value);
}

extern "C" void endfunctionparamdecl(bool bisrest) {

	for (auto& a : *currtypevectorbeingbuild.back().p)
		a.pllvmtype = createllvmtype(a.type);

	currtypevectorbeingbuild.pop_back();

	auto& functype = currtypevectorbeingbuild.back().p->back().type.back();

	assert(functype.uniontype == type::FUNCTION);

	functype.spec.func.bisvariadic = bisrest;
}

extern "C" void continuedeclaration() {

	auto lastvar = currtypevectorbeingbuild.back().p->back();

	auto lastbasetype = lastvar.type.back();

	assert(lastbasetype.uniontype == type::BASIC);

	lastvar.type.clear();

	lastvar.type.push_back(lastbasetype);

	currtypevectorbeingbuild.back().p->push_back(lastvar);
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr);

extern "C" void startfunctionparamdecl() {

	if (currtypevectorbeingbuild.back().currdecltype ==
		currdecltypeenum::PARAMS) // if declaring a function inside another
								  // declaration
		addptrtotype("", 0);

	currtypevectorbeingbuild.back().p->back().type.push_back(
		{ type::FUNCTION });

	currtypevectorbeingbuild.push_back(
		{ currtypevectorbeingbuild.back()
			 .p->back()
			 .type.back()
			 .spec.func.parametertypes_list.begin(),
		 currdecltypeenum::PARAMS });
}

extern "C" void addsubtotype() {

	type arraytype{ type::ARRAY };

	auto res = llvm::dyn_cast<llvm::ConstantInt> (
		hndlcnstexpr.immidiates.back().value);

	arraytype.spec.arraysz = *res->getValue().getRawData();
	currtypevectorbeingbuild.back().p->back().type.push_back(arraytype);

	hndlcnstexpr.immidiates.pop_back();
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr) {

	type ptrtype{ type::POINTER };

	ptrtype.spec.ptrqualifiers =
		parsequalifiers(std::string{ quailifers, szstr });

	currtypevectorbeingbuild.back().p->back().type.push_back(ptrtype);
}

extern "C" void insertinttoimm(const char* str, size_t szstr, const char* suffix, size_t szstr1, int type) {
	phndl->insertinttoimm(str, szstr, suffix, szstr1, type);
}

extern "C" void insertfloattoimm(const char* postfix_arg, size_t szstr,
	const char* wholepart_arg, size_t szstr1,
	const char* fractionpart_arg, size_t szstr2,
	const char* exponent_arg, size_t szstr3,
	const char* exponentsign_arg, size_t szstr4) {
	std::string postfix{ postfix_arg, szstr }, wholepart{ wholepart_arg, szstr1 }, fractionpart{ fractionpart_arg, szstr2 },
		exponent{ exponent_arg, szstr3 }, exponent_sign{ exponentsign_arg, szstr4 };

	/*unsigned long long whole_number = std::stoull(wholepart, nullptr, 10),
		fractionpart_number = std::stoull(fractionpart, nullptr, 10),
		exponent_number = std::stoull(exponent, nullptr, 10);*/

	std::vector<::type> currtype = { 1, ::type::BASIC };

	llvm::Type* pllvmtype;

	const llvm::fltSemantics& floatsem = postfix.empty() ? currtype.back().spec.basicdeclspec.basic[1] = "double",
		pllvmtype = llvm::Type::getDoubleTy(llvmctx),
		llvm::APFloatBase::IEEEdouble() :
		ranges::contains(std::array{ "f", "F" }, postfix)
		? currtype.back().spec.basicdeclspec.basic[1] = "float",
		pllvmtype = llvm::Type::getFloatTy(llvmctx),
		llvm::APFloatBase::IEEEsingle() :
		(assert(ranges::contains(std::array{ "l", "L" }, postfix)),
			currtype.back().spec.basicdeclspec.basic[1] = "double",
			currtype.back().spec.basicdeclspec.longspecsn = 1,
			pllvmtype = llvm::Type::getFloatingPointTy(llvmctx, llvm::APFloatBase::IEEEquad()),
			llvm::APFloatBase::IEEEquad());

	llvm::APFloat floatlit{ floatsem };

	std::string finalnumber = wholepart + "." + fractionpart;

	if (!exponent.empty())
		finalnumber += "E" + exponent_sign + exponent;

	auto status = floatlit.convertFromString(finalnumber, llvm::APFloatBase::rmNearestTiesToEven);


	phndl->immidiates.push_back({ llvm::ConstantFP::get(pllvmtype, floatlit), currtype });
}

extern "C" void subscript() { phndl->subscripttwovalues(); }

extern "C" void beginconstantexpr() {
	//cnstexpriterstart = phndl->immidiates.end();
	szcnstexprinitial = phndl->immidiates.size();

	phndl = &hndlcnstexpr;
}

extern "C" void endconstantexpr() {
	assert(szcnstexprinitial == phndl->immidiates.size());

	// auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

	// hndlcnstexpr.immidiates.pop_back();

	// std::cout << "computed value: " << *res->getValue().getRawData() <<
	// std::endl;

	phndl = &hndlbase;
}

extern "C" void startmodule(const char* modulename, size_t szmodulename) {
	new (&nonconstructable.mainmodule)
		llvm::Module{ std::string{modulename, szmodulename}, llvmctx };

	pdatalayout = new llvm::DataLayout{ &nonconstructable.mainmodule };
}

extern "C" void endmodule() {
	std::error_code code{};
	llvm::raw_fd_ostream output{
		std::string{nonconstructable.mainmodule.getName()} + ".bc", code },
		outputll{ std::string{nonconstructable.mainmodule.getName()} + ".ll",
				 code };
	nonconstructable.mainmodule.print(outputll, nullptr);
	llvm::WriteBitcodeToFile(nonconstructable.mainmodule, output);
	nonconstructable.mainmodule.~Module();
	delete pdatalayout;
}

extern "C" void unary(const char* str, size_t szstr) {
	std::string imm;

	imm.assign(str, szstr);

	basehndl* phpriorhndl = phndl;

	switch (stringhash(imm.c_str())) {
	case "-"_h:
	case "+"_h:
	case "!"_h:
		if (bIsBasicFloat(phndl->immidiates.back().type.front()))
			phndl = &hndlfpexpr;
	}

	switch (stringhash(imm.c_str())) {
	case "&"_h: {
		phndl->getaddress();
		break;
	}
	case "*"_h:
		phndl->applyindirection();
		break;
	case "~"_h:
		phndl->getbitwisenot();
		break;
	case "-"_h:
		phndl->getnegative();
		break;
	case "+"_h:
		phndl->getpositive();
		break;
	case "!"_h:
		phndl->getlogicalnot();
		break;
	}

	phndl = phpriorhndl;
}

extern "C" void unaryincdec(const char* str, size_t szstr, bool postfix) {
	std::string imm;

	imm.assign(str, szstr);

	basehndl* phpriorhndl = phndl;

	if (bIsBasicFloat(phndl->immidiates.back().type.front()))
		phndl = &hndlfpexpr;

	unsigned int type = 3;// << 2 | 2;

	auto immlvalue = immidiates.back();

	immlvalue.originident.append("[[modified]]");

	insertinttoimm("1", sizeof "1" - 1, "", 0, type);

	if (immlvalue.type.front().uniontype != type::POINTER)

		immidiates.back() = convertTo(immidiates.back(), immlvalue.type);

	phndl->addlasttwovalues(imm == "--", false);

	immidiates.push_back(immlvalue);

	std::rotate(----immidiates.end(), --immidiates.end(), immidiates.end());

	phndl->assigntwovalues();

	if (postfix)
		immidiates.back() = immlvalue;

	phndl = phpriorhndl;
}

extern "C" void binary(const char* str, size_t szstr) {
	std::string imm;

	imm.assign(str, szstr);

	basehndl* phpriorhndl = phndl;

	auto ops = std::array{ *----phndl->immidiates.end(), phndl->immidiates.back() };

	//phndl->usualarithmeticconversions(ops);

	if (bIsBasicFloat(ops[0].type.front()) || bIsBasicFloat(ops[1].type.front()))
		phndl = &hndlfpexpr;

	/*switch (stringhash(imm.c_str())) {
	case "*"_h:
	case "/"_h:
	case "%"_h:
	case "+"_h:
	case "-"_h:
	case ">"_h:
	case "<"_h:
	case "<="_h:
	case ">="_h:
	case "=="_h:
	case "!="_h:
	case "&"_h:
	case "^"_h:
	case "|"_h:
	{
		auto ops = std::array{ *----phndl->immidiates.end(), phndl->immidiates.back() };

		//phndl->usualarithmeticconversions(ops);

		if (bIsBasicFloat(ops[0].type.front()) || bIsBasicFloat(ops[1].type.front()))
			phndl = &hndlfpexpr;

		//phndl->immidiates.back() = ops[1];
		//*----phndl->immidiates.end() = ops[0];
		break;
	}
	case ">>"_h:
	case "<<"_h:
	{
		auto ops = std::array{ *----phndl->immidiates.end(), phndl->immidiates.back() };

		ops[0] = phndl->integralpromotions(ops[0]);

		ops[1] = phndl->integralpromotions(ops[1]);

		if (ops[1].type.front().uniontype == type::BASIC &&
			ops[0].value->getType() != ops[1].value->getType())
			ops[1].type = ops[0].type,
			ops[1].value = phndl->CreateCastInst(
				ops[1].value, ops[0].value->getType(),
				ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned");

		phndl->immidiates.back() = ops[1];
		*----phndl->immidiates.end() = ops[0];
		break;
	}
	case "="_h:

		break;
	}*/

	switch (stringhash(imm.c_str())) {
	case "*"_h:
		phndl->mlutiplylasttwovalues();
		break;
	case "/"_h:
		phndl->dividelasttwovalues();
		break;
	case "%"_h:
		phndl->modulolasttwovalues();
		break;
	case "+"_h:
		phndl->addlasttwovalues(false);
		break;
	case "-"_h:
		phndl->addlasttwovalues(true);
		break;
	case "<<"_h:
		phndl->shifttwovalues(false);
		break;
	case ">>"_h:
		phndl->shifttwovalues(true);
		break;
	case ">"_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SGT);
		break;
	case "<"_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SLT);
		break;
	case "<="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SLE);
		break;
	case ">="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SGE);
		break;
	case "=="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_EQ);
		break;
	case "!="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_NE);
		break;
	case "&"_h:
		phndl->bitwisetwovalues(llvm::Instruction::And);
		break;
	case "^"_h:
		phndl->bitwisetwovalues(llvm::Instruction::Xor);
		break;
	case "|"_h:
		phndl->bitwisetwovalues(llvm::Instruction::Or);
		break;
	case "&&"_h:
		phndl->logictwovalues(true);
		break;
	case "||"_h:
		phndl->logictwovalues(false);
		break;
	case "="_h:
		phndl->assigntwovalues();
		break;
	}

	phndl = phpriorhndl;
}

void printtype(llvm::Type* ptype) { // debug purpose only
	printtype(ptype, "");
}

// llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());
// std::cout << "base type of " << currdeclidentifier << " is typedef " <<
// !bwhich << " :" << spec << ", is declaring a typedef :" << bistypedef <<
// std::endl;