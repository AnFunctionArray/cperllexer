//#include "llvmgen.h"
//#include "llvm/IR/Instructions.h"
//#include "llvm/IR/Value.h"
//#include "llvm/Support/Allocator.h"
#include <cstdlib>
#include <exception>
#include <iterator>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
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
#include <llvm/Support/Error.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <locale>
#include <ostream>
#include <queue>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/iterator/common_iterator.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/istream.hpp>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include <variant>
#include <unordered_map>
#include <fstream>
//#include <oniguruma.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC

#if !defined(_WIN32) & !defined(_WIN64)
//#include <pcre2.h>
#else
#include <pcre2/pcre2.h>
#endif

extern "C" {
#include "../src/main.h"
}

DLL_EXPORT void insertinttoimm(const char* str, size_t szstr, const char* suffix, size_t szstr1, int type);

DLL_EXPORT void constructstring();

llvm::BranchInst* splitbb(const char* identifier, size_t szident);

//static struct basehndl* phndl;

const std::list<struct var>::reverse_iterator obtainvalbyidentifier(std::string ident, bool push = true, bool bfindtypedef = false);

enum class currdecltypeenum {
	TYPEDEF,
	CAST,
	PARAMS,
	NORMAL,
	UNKNOWN,
	NONE
};

//std::string currdeclspectypedef;

std::list<std::pair<std::list<std::string>, bool>> qualifsandtypes{ 1 };

static std::list<std::list<var>> scopevar{ 1 };

static std::list<std::list<std::list<var>>> structorunionmembers{ 1 };

void startdeclaration(std::string typedefname);

struct currtypevectorbeingbuild_t {
	std::list<std::list<var>>::iterator p;
	currdecltypeenum currdecltype;

	decltype(p) endp;
};

std::list<currtypevectorbeingbuild_t> currtypevectorbeingbuild = {
	{scopevar.begin(), currdecltypeenum::NORMAL} };

std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>>::iterator startifstatement(bool pop);

static std::string currstring;

static bool bischarlit = false;

std::pair<std::string, std::string> currstruct;

std::string currenum;

typedef std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>>::iterator arrtwovals;

struct branch {
	std::list<arrtwovals> first;
	std::list<struct var>::iterator iterval;
	std::list<arrtwovals>::iterator itersecond;
	std::list<std::array<llvm::BranchInst*, 2>> second;
};

std::list<branch> nbranches;

struct enumdef {
	std::string ident;
	std::list<std::list<struct var>::iterator > memberconstants;
	int maxcount{};
};

std::list<std::list<enumdef>>  enums{ 1 };

extern std::list<std::pair<std::list<std::string>, bool>> qualifsandtypes;

#if 0

int getnameloc(const char* ntocompare, calloutinfo nametable) {
	return (int)getnameloc((long long)ntocompare, nametable);
}

int getnameloc2(const char* ntocompare, struct calloutinfo nametable, pcre2_callout_block* pcurrblock, int displ)
{
	return (int)getnameloc2((long long)ntocompare, nametable, pcurrblock, displ);
}

int getnameloc3(const char* ntocompare, calloutinfo nametable, pcre2_callout_block* pcurrblock, int displ, namelocopts opts) {
	return (int)getnameloc3((long long)ntocompare, nametable, pcurrblock, displ, opts);
}

DLL_EXPORT void startforloopcond();

struct bindings_payload {
	int getnameloc(const char* ntocompare, calloutinfo nametable) {
		return getnameloc3(ntocompare, nametable, a, 1, { .rev = 0, .dontsearchforclosest = 0,.printdispl = +1 });
	}
	struct calloutinfo* ptable;
	pcre2_callout_block* a;
	const char** pargs;
	size_t* szargs;
};

#endif

//static std::vector<std::vector<std::string>> typedefs{ 1 };

//bool bwastypedefmatched = false;

/*struct bindings_parsing : bindings_payload {

	virtual void cleanup() {}
	virtual std::string escape_1() { return ""; }
	virtual std::string text_2() { return ""; }
	virtual std::string unused_3() { return ""; };
	virtual std::string start_str_4() { return ""; }
	virtual std::string num_lit_5() { return ""; }
	virtual std::string ident_6() { return ""; }
	virtual std::string start_fn_call_7() { return ""; }
	virtual std::string member_access_op_8() { return ""; }
	virtual std::string postfix_arith_9() { return ""; }
	virtual std::string prefix_arith_10() { return ""; }
	virtual std::string decl_ptr_11() { return ""; }
	virtual std::string decl_subs_12() { return ""; }
	virtual std::string end_fn_call_13() { return ""; }
	virtual std::string unexplored_end_sizeof_14() { return ""; }
	virtual std::string begin_sizeof_15() { return ""; }
	virtual std::string unused_16() { return ""; };
	virtual std::string unused_17() { return ""; };
	virtual std::string unused_18() { return ""; };
	virtual std::string unary_op_19() { return ""; }
	virtual std::string add_op_20() { return ""; }
	virtual std::string mul_op_21() { return ""; }
	virtual std::string shift_op_22() { return ""; }
	virtual std::string rel_op_23() { return ""; }
	virtual std::string eq_op_24() { return ""; }
	virtual std::string and_op_25() { return ""; }
	virtual std::string xor_op_26() { return ""; }
	virtual std::string or_op_27() { return ""; }
	virtual std::string and_logic_op_28() { return ""; }
	virtual std::string or_logic_op_29() { return ""; }
	virtual std::string assign_op_30() { return ""; }
	virtual std::string unused_31() { return ""; };
	virtual std::string ternary_true_32() { return ""; }
	virtual std::string ternary_33() { return ""; }
	virtual std::string ternary_false_34() { return ""; }
	virtual std::string comma_op_35() { return ""; }
	virtual std::string unused_36() { return ""; };
	virtual std::string unused_37() { return ""; };
	virtual std::string identifier_typedef_38() {
		//std::string n = getnameloc3("typedefnm", *ptable, a, 0, { .rev = 0, .last = 0, .dontsearchforclosest = 0, });
		std::string ret = "";
		//auto ident = std::string{ (char*)GROUP_PTR_AND_SZ(n) };
		//if (ident.empty()) throw ident;
		for (auto& typedefscope : typedefs)
			for (auto& typedefident : typedefscope)
				ret += typedefident + "|";
		if (!ret.empty()) ret.pop_back();
		//ret = ret || bwastypedefmatched;
		//bwastypedefmatched = bwastypedefmatched || !ret;
		return ret;
	}
	virtual std::string identifier_decl_39() {
		/*if (qualifsandtypes.back().second)
		{
			//std::string n = getnameloc("ident", *ptable) + 1;
			typedefs.back().push_back({ FIRST_ARG_PTR_AND_SZ });
			qualifsandtypes.back().second = false;
		}*/
		/*	if (szargs[3]) typedefs.back().push_back({FIRST_ARG_PTR_AND_SZ});
			return "";
		}
		virtual std::string start_constant_expr_40() { return ""; }
		virtual std::string end_decl_sub_41() { return ""; }
		virtual std::string end_full_decl_42() { return ""; }
		virtual std::string extend_decl_43() { return ""; }
		virtual std::string start_new_scope_44() {
			typedefs.push_back({});
			return "";
		}
		virtual std::string end_of_scope_45() {
			typedefs.pop_back();
			return "";
		}
		virtual std::string unexplored_46() { return ""; }
		virtual std::string begin_param_list_47() { return ""; }
		virtual std::string end_param_list_48() { return ""; }
		virtual std::string add_type_or_qualifier_49() {
			//std::string n = getnameloc("typesandqualifiersmask", *ptable);
			qualifsandtypes.back().first.push_back(std::string{ FIRST_ARG_PTR_AND_SZ });

			return "";
		}
		virtual std::string unused_50() { return ""; };
		virtual std::string add_literal_51() { return ""; }
		virtual std::string finish_return_statement_52() { return ""; }
		virtual std::string finish_statement_53() { return ""; }
		virtual std::string subscript_op_54() { return ""; };
		virtual std::string decl_begin_55();
		virtual std::string end_of_sizeof_56() { return ""; }
		virtual std::string end_of_sizeof_tp_nm_57() { return ""; }
		virtual std::string ident_struc_58() { return ""; }
		virtual std::string struc_or_union_body_59() { qualifsandtypes.push_back({}); return ""; }
		virtual std::string struc_or_union_body_end_60() { qualifsandtypes.pop_back();  return ""; }
		virtual std::string perform_explicit_conversion_61() { return ""; }
		virtual std::string create_label_62() { return ""; }
		virtual std::string goto_stmt_63() { return ""; }
		virtual std::string switch_stmt_64() { return ""; }
		virtual std::string create_case_65() { return ""; }
		virtual std::string switch_stmt_end_66() { return ""; }
		virtual std::string create_default_case_67() { return ""; }
		virtual std::string collect_float_literal_68() { return ""; }
		virtual std::string finish_float_literal_69() { return ""; }
		virtual std::string begin_if_stm_70() { return ""; }
		virtual std::string cnt_if_stm_71() { return ""; }
		virtual std::string finish_if_stm_72() { return ""; }
		virtual std::string begin_loop_73() { return ""; }
		virtual std::string loop_iter_74() { return ""; }
		virtual std::string finish_loop_75() { return ""; }
		virtual std::string cond_loop_76() { return ""; }
		virtual std::string begin_do_while_77() { return ""; }
		virtual std::string finish_do_while_78() { return ""; }
		virtual std::string finish_continue_stm_79() { return ""; }
		virtual std::string finish_break_stm_80() { return ""; }
		virtual std::string register_calling_conv_81() { return ""; }
		virtual std::string unexplored_82() { return ""; }
		virtual std::string end_binary_83() { printf2("%s\n", __func__);  return ""; }
		virtual std::string begin_branch_84() { return ""; }
		virtual std::string begin_binary_85() { return ""; }
		virtual std::string unused_86() { return ""; }
		virtual std::string begin_nested_expr_87() { return ""; }
		virtual std::string end_nested_expr_88() { return ""; }
		virtual std::string add_ident_to_enum_def_89() { return ""; }
		virtual std::string begin_enumerator_def_90() { return ""; }
		virtual std::string begin_enumerator_decl_91() { return ""; }
		virtual std::string end_ass_to_enum_def_92() { return ""; }
		virtual std::string end_without_ass_to_enum_def_93() { return ""; }
		virtual std::string begin_unnamed_enum_def_94() { return ""; }
		virtual std::string end_expr_init_95() { return ""; }
	};

	struct destr_clear_qualifsandtypes : bindings_parsing {
		virtual std::string decl_begin_55() {
			qualifsandtypes.back().second = std::find(qualifsandtypes.back().first.begin(), qualifsandtypes.back().first.end(), "typedef") != qualifsandtypes.back().first.end();
			//bwastypedefmatched = false;
			return "";
		}
		void cleanup() override {
			qualifsandtypes.back().first.clear();
		}
	};

	std::string bindings_parsing::decl_begin_55() {
		this->~bindings_parsing();
		new (this)destr_clear_qualifsandtypes();
		return decl_begin_55();
	}*/

DLL_EXPORT unsigned constexpr stringhash(char const* input) {
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

/*using iterenumtype = std::list<std::list<std::string>>::iterator;

using iterunorstrtype = std::list<std::list<struct var>>::iterator;

using itervartypedeftype = std::list<struct var>::reverse_iterator;

using variant_origin_type = std::variant<iterenumtype, iterunorstrtype, itervartypedeftype>;

struct basic_type_origin : variant_origin_type
{
	union {
		iterenumtype iterenum;
		iterunorstrtype iterunorstr;
		itervartypedeftype vartypedef;
	};

	enum ORIG_ENUM {
		ORIG_ENUM_TYPE,
		ORIG_STRUC_TYPE,
		ORIG_TYPEDEF,
		ORIG_PLAIN
	} orig;

	basic_type_origin(iterenumtype orig) : orig{ ORIG_ENUM_TYPE },
		variant_origin_type{ iterenum },
		iterenum{ std::get<iterenumtype>(*this) }
	{ }

	basic_type_origin(iterunorstrtype orig) : orig{ ORIG_STRUC_TYPE },
		variant_origin_type{ iterunorstr },
		iterunorstr{ std::get<iterunorstrtype>(*this) }
	{ }

	basic_type_origin(itervartypedeftype orig) : orig{ ORIG_TYPEDEF },
		variant_origin_type{ vartypedef },
		vartypedef{ std::get<itervartypedeftype>(*this) }
	{ }

	basic_type_origin() : orig{ ORIG_PLAIN } {}

	basic_type_origin& operator=(const basic_type_origin& val_origin_in) {
		this->~basic_type_origin();

		new (this)basic_type_origin{ val_origin_in };

		return *this;
	}

	basic_type_origin(const basic_type_origin& val_origin) {

		switch (orig) if (0)
		case ORIG_ENUM_TYPE: {
			if (val_origin.orig != orig)
				this->~basic_type_origin(), new (this)basic_type_origin{ iterenumtype{} };
			iterenum = val_origin.iterenum;
		}
		else if (0)
		case ORIG_STRUC_TYPE: {
			if (val_origin.orig != orig)
				this->~basic_type_origin(), new (this)basic_type_origin{ iterunorstrtype{} };
			iterunorstr = val_origin.iterunorstr;
		}
		else if (0)
		case ORIG_TYPEDEF: {
			if (val_origin.orig != orig)
				this->~basic_type_origin(), new (this)basic_type_origin{ itervartypedeftype{} };
			vartypedef = val_origin.vartypedef;
		}
	}

	~basic_type_origin() {
		switch (orig) if (0)
		case ORIG_ENUM_TYPE:
			iterenum.~decltype(iterenum)();
		else if (0)
		case ORIG_STRUC_TYPE:
			iterunorstr.~decltype(iterunorstr)();
		else if (0)
		case ORIG_TYPEDEF:
			vartypedef.~decltype(vartypedef)();
	}
};*/

/*
0 - const
1 - restrict
2 - volatile
3 - israwtype
*/
typedef std::bitset<5> pointrtypequalifiers;

struct basic_type_origin {
	/*
		0 - last signed/unsigned or struct/union/enum
		1 - last basic type
		2 - last storage specifier
		3 - last typedef or struct/union/enum name
	*/
	std::array<std::string, 4> basic;
	size_t longspecsn{}; //amount of long qualifiers
	std::bitset<4> qualifiers;

	void* pexternaldata = nullptr;

	bool operator==(const basic_type_origin&) const = default;
};

struct bascitypespec : basic_type_origin {


	bascitypespec& operator= (const bascitypespec& source) { // basic assignment
		basic_type_origin::operator=(source);
		basic[2].clear();
		return *this;
	}

	/*bool hasLinkage() {
		return ranges::contains(std::array{ "extern", "static" }, basic[2]);
	}*/

	bool operator== (const bascitypespec& comparer) {
		std::string ignoredmember = comparer.basic[2];
		std::swap(ignoredmember, basic[2]);
		bool bareequal = basic_type_origin::operator==(comparer);
		basic[2] = ignoredmember;
		return bareequal;
	}

	bool compareSign(const bascitypespec& comparer) {
		return ranges::contains(std::array{ "", "signed" }, comparer.basic[0]) &&
			ranges::contains(std::array{ "", "signed" }, basic[0]) ||
			comparer.basic[0] == basic[0];
	}

	bool istypedef() {
		std::string array [] = { [2] = basic[2], [3] = basic[3] };
		return basic == std::to_array(array) && !basic[3].empty();
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
		case "__ptr64"_h:
			ret[3] = 1;
			break;
		case "__ptr32"_h:
			ret[4] = 1;
			break;
		default:
			std::cerr << "invalid specifier: " << spec << std::endl;
			std::terminate();
		}
	return ret;
}

bascitypespec parsebasictype(const std::list<std::string>& qualifs, bascitypespec& ret) {
	//bascitypespec ret;

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
		case "void"_h:
		case "_Bool"_h:
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
		case "typedef"_h:
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
	enum typetype {
		BASIC,
		POINTER,
		ARRAY,
		FUNCTION
	} uniontype;

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
	std::list<::type> type;
};

struct val : valandtype {
	std::list<valandtype> lvalues = {};
	std::string originident = "[[immidiate]]";
};

llvm::Type* createllvmtype(std::list<type> &decltypevector,
	std::list<::var>* last_struct = nullptr,
	std::list<type> *refdecltypevector = nullptr);

llvm::Type* createllvmtype(const std::list<type> &decltypevector,
	std::list<::var>* last_struct = nullptr,
	std::list<type> *refdecltypevector = nullptr) {
		return createllvmtype(decltypevector, last_struct, refdecltypevector);
}

void addvar(var& lastvar, llvm::Constant* pInitializer = nullptr);

struct var {
	std::list<::type> type;
	//bool bistypedef;
	std::string identifier;
	llvm::Type* pllvmtype{};

	llvm::Type* requestType(std::list<::type> *pout = nullptr) {
		if (!pllvmtype) {
			pllvmtype = createllvmtype(type, nullptr, pout);
		}
		return pllvmtype;
	}
	std::list<::type> fixupTypeIfNeeded() {
		if (type.back().spec.basicdeclspec.istypedef()) {
			auto tmpident = identifier;
			identifier.clear();
			auto typedefval = obtainvalbyidentifier(type.back().spec.basicdeclspec.basic[3], false, true);
			type.pop_back();
			type.splice(type.end(), typedefval->fixupTypeIfNeeded());
			identifier = tmpident;
		}
		return type;
	}
	union {
		llvm::Value* pValue{};
		llvm::Constant* constant;
	};
	llvm::Value* requestValue() {
		if (pValue == nullptr && linkage != "typedef")
			addvar(*this);
		return pValue;
	}
	std::string linkage;

	size_t firstintroduced;

	operator val () { return { requestValue(), type, {}, identifier }; }
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

llvm::Type* createllvmtype(std::list<type> &decltypevector,
	std::list<::var>* last_struct,
	std::list<type> *refdecltypevector);

val convertTo(val target, std::list<::type> to);

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

static std::list<val> immidiates{ 1 };

bool bIsBasicInteger(const type& type);

using ifstatiter = std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>>::iterator;

struct opscopeinfo {
	std::list<val>::iterator immidiatesbegin;
	var currlogicopvar;

	ifstatiter lastifs;
	std::list<std::array<llvm::Value*, 2>> logicopswitches;
};

static std::list<opscopeinfo> opsscopeinfo;

//static std::list<std::pair<std::list<std::pair<llvm::BasicBlock *, std::array<llvm::Value *, 3>>>, val>> opbbs;

//static std::list<var> currlogicopval{};

struct basehndl /* : bindings_compiling*/ {
	//virtual llvm::Value* assigntwovalues() = 0;

	virtual void end_binary() {
		auto& currbranch = nbranches.back();

		var& ordinary = *currbranch.iterval;

		/*llvm::BranchInst* normalflow = nullptr;

		if (nbranches.back().first.size() > 1)
			normalflow = splitbb("", 0);*/

		val ordinary_imm = ordinary;

		//auto iternbranch = nbranches.begin();

		/*for (auto branch : nbranches.back().first | ranges::views::drop(1)) {
			//iternbranch = ++iternbranch;
			int fullindex = branch->first[0]->getSuccessor(1) != branch->second;
			branch->first[0]->setSuccessor(!fullindex, pcurrblock.back());
			branch->second->eraseFromParent();
			//ifstatements.erase(branch);

			ordinary_imm = ordinary;
			ordinary_imm.lvalues.push_back({ ordinary_imm.value, ordinary.type });

			immidiates.push_back(ordinary_imm);

			insertinttoimm((char*)branch->first[1], 1, "", 0, 3);

			phndl->assigntwovalues();

			immidiates.pop_back();

			branch->first[0] = splitbb("", 0);
		}*/

		llvm::BranchInst* lastsplit;

		if (!nbranches.back().second.empty()) {
			lastsplit = nbranches.back().second.back()[1];
			lastsplit->setSuccessor(0, pcurrblock.back());
			auto imm = immidiates.back();
			ordinary_imm.lvalues.push_back({ ordinary_imm.value, currbranch.iterval->type });
			immidiates.pop_back();
			immidiates.push_back(ordinary_imm);
			immidiates.push_back(imm);
			assigntwovalues();
			splitbb("", 0);
		}

		for (auto branch : nbranches.back().second)
			branch[0]->setSuccessor(0, pcurrblock.back());

		/*if (normalflow) {
			normalflow->setSuccessor(0, pcurrblock.back());*/
		if (!nbranches.back().second.empty()) {
			auto ordinaryval = ordinary.requestValue();
			ordinary_imm.value = llvmbuilder.CreateLoad(ordinaryval->getType()->getPointerElementType(), ordinaryval);

			immidiates.pop_back();

			immidiates.push_back(ordinary_imm);

		}
		//else //((llvm::AllocaInst*)ordinary.pValue)->replaceAllUsesWith(llvm::UndefValue::get(ordinary.pValue->getType())),
			//((llvm::AllocaInst*)ordinary.pValue)->eraseFromParent();
		//}

		scopevar.back().erase(currbranch.iterval);

		/**/

		nbranches.pop_back();
	}
	virtual void begin_branch() {
		auto& currbranch = nbranches.back();
		/*val ordinary_imm = *currbranch.iterval;
		auto imm = immidiates.back();
		ordinary_imm.lvalues.push_back({ ordinary_imm.value, currbranch.iterval->type });
		immidiates.pop_back();
		immidiates.push_back(ordinary_imm);
		immidiates.push_back(imm);
		phndl->assigntwovalues();*/
		currbranch.first.insert(++currbranch.first.begin(), startifstatement(false));
		//currbranch.second.push_back(currbranch.first.back());
		//ordinary_imm.value = llvmbuilder.CreateLoad(ordinary_imm.value);
		//immidiates.push_back(ordinary_imm);
	}
	virtual void begin_binary() {
		var ordinary; type basicint{ type::BASIC };
		basicint.spec.basicdeclspec.basic[1] = "int";
		ordinary.type = { basicint };
		scopevar.back().push_back(ordinary);
		scopevar.back().back().requestValue();
		nbranches.push_back({ std::list<arrtwovals>{1}, --scopevar.back().end() });
		nbranches.back().itersecond = nbranches.back().first.begin();

		/*val ordinary_imm = scopevar.back().back();
		ordinary_imm.lvalues.push_back({ ordinary_imm.value, ordinary.type });

		immidiates.push_back(ordinary_imm);

		insertinttoimm("1", sizeof "1" - 1, "", 0, 3);

		assigntwovalues();

		immidiates.pop_back();*/
	}

	static std::list<val>& immidiates;

	virtual basehndl* (*getrestorefn())(basehndl*) {
		return [](basehndl* pnhdl) -> basehndl* {
			return new (pnhdl) basehndl{};
		};
	}
	//friend void beginlogicalop(int blastorfirst);
	virtual llvm::Value* CreateCastInst(llvm::Value* C, llvm::Type* Ty, bool bissigned) {
		return llvmbuilder.CreateIntCast(C, Ty, bissigned);
	}

	virtual llvm::Constant* getValZero(val val) {
		llvm::APInt apint{};

		if (bIsBasicInteger(val.type.front()))
			apint = llvm::APInt{ dyn_cast<llvm::IntegerType> (val.value->getType())->getBitWidth(), 0 };

		return llvm::ConstantInt::getIntegerValue(val.value->getType(), apint);
	}

	using val = ::val;

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

	virtual llvm::Value* getlogicalnot() {
		auto& op = immidiates.back();

		llvm::Value* originstr;

		extern void printvaltype(val);

		printvaltype(op);

		if (std::list listtp = { type{type::BASIC} }; listtp.back().spec.basicdeclspec.basic[1] = "int", true)
			op = convertTo(op, listtp);

		originstr = op.value = llvmbuilder.CreateICmp(
			llvm::CmpInst::Predicate::ICMP_EQ, op.value,
			getValZero(op));
		op.type.erase(op.type.begin(), --op.type.end());

		op.value = CreateCastInst(op.value, llvm::Type::getInt32Ty(llvmctx), false);

		extern void printvaltype(::val val);

		printvaltype(op);

		op.type.back().spec.basicdeclspec.basic[1] = "int",
			op.type.back().spec.basicdeclspec.basic[0] = "";

		return originstr;
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
		switch (stringhash(basictyp.spec.basicdeclspec.basic[0].c_str())) {
		default:
		case "signed"_h:
		case "unsigned"_h:
			switch (stringhash(basictyp.spec.basicdeclspec.basic[1].c_str())) {
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

		throw std::out_of_range{ "not a integer" };
	}

	std::array<val, 2> usualarithmeticconversions(std::array<val, 2> ops_in) {
		std::array ops = { &ops_in[0], &ops_in[1] };

		std::array refspecops = { &ops[0]->type.back().spec.basicdeclspec, &ops[1]->type.back().spec.basicdeclspec };

		extern void printvaltype(val);

		printvaltype(*ops[0]);

		printvaltype(*ops[1]);

		for (auto i = 0; i < 2; ++i)
			if (std::list listtp = { type{type::BASIC} };
				listtp.back().spec.basicdeclspec.basic[0] = "unsigned",
				listtp.back().spec.basicdeclspec.basic[1] = "long",
				listtp.back().spec.basicdeclspec.longspecsn = 2,
				ops[i]->type.size() > 1)
				ops_in[i] = convertTo(*ops[!i], listtp);
			else if (ranges::contains(std::array{ "double", "float" }, refspecops[i]->basic[1]))
				for (ops_in[!i] = convertTo(*ops[!i], ops[i]->type);;)
					return ops_in;

		ops_in[0] = integralpromotions(*ops[0]);

		ops_in[1] = integralpromotions(*ops[1]);

		refspecops = { &ops[0]->type.back().spec.basicdeclspec, &ops[1]->type.back().spec.basicdeclspec };

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
				llvmbuilder.CreatePtrDiff(ops[0].value->getType()->getPointerElementType(), ops[0].value, ops[1].value);
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

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred, llvm::CmpInst::Predicate unsignedpred, llvm::CmpInst::Predicate fltpred) {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].value =
			llvmbuilder.CreateICmp(pred, ops[0].value, ops[1].value);
		else
			ops[0].value =
			llvmbuilder.CreateICmp(unsignedpred, ops[0].value, ops[1].value);

		ops[0].value =
			CreateCastInst(ops[0].value, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());
		;

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void bitwisetwovalues(llvm::Instruction::BinaryOps op) {
		std::array ops = getops();

		ops[0].value = llvmbuilder.CreateBinOp(op, ops[0].value, ops[1].value);

		immidiates.push_back(ops[0]);
	}

	/*void endlast (llvm::BasicBlock *bb, var value) {
		//llvm::Value *logicval = llvmbuilder.CreateICmp (
		//llvm::CmpInst::Predicate::ICMP_NE, immidiates.back().value,
		//getValZero (immidiates.back()));

		//llvmbuilder.CreateStore(logicval, pvalue);

		//immidiates.push_back()

		splitbb ("", 0);

		extern std::list<std::pair<std::array<llvm::BranchInst *, 2>, llvm::BasicBlock *>> ifstatements;
		auto &lastif = ifstatements.back ();

		auto dummysuccessor = lastif.first[0]->getSuccessor (0);

		if (dummysuccessor != lastif.second)
			lastif.first[0]->setSuccessor (1, bb);
		else
			lastif.first[0]->setSuccessor (0, bb);

		lastif.second->eraseFromParent ();
		ifstatements.pop_back ();
	}*/
	//private:
	virtual std::array<llvm::Value*, 2> logictwovalues(bool bisand) {

		std::array ops = getops(false);

		std::array<llvm::Value*, 2> instr;

		ops[0].value = llvmbuilder.CreateICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[0].value, getValZero(ops[0]));

		ops[1].value = llvmbuilder.CreateICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[1].value, getValZero(ops[1]));

		//if (bisand != 2)

		instr[0] = ops[0].value = !bisand
			? llvmbuilder.CreateOr(ops[0].value, ops[1].value)
			: llvmbuilder.CreateAnd(ops[0].value, ops[1].value);
		//else
		//	instr[0] = ops[0].value = llvmbuilder.CreateOr(ops[0].value, ops[1].value),
		//	instr[1] = ops[0].value = llvmbuilder.CreateAnd(ops[0].value, ops[1].value);

		ops[0].value = llvm::CastInst::Create(
			llvm::Instruction::CastOps::ZExt, ops[0].value,
			llvm::Type::getInt32Ty(llvmctx), "", pcurrblock.back());

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
		return instr;
	}

	/*public:*
		virtual void logictwovalues_seq(bool bisand) {

			/*auto plastopbb = opbbs.back ().first.back ();

			opbbs.back ().first.pop_back ();*/

			/*	auto lastlogicswitchinstr = opsscopeinfo.back().logicopswitches.back();

				opsscopeinfo.back().logicopswitches.pop_back();

				extern std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>> ifstatements;

				auto lastif = ifstatements.back();

				//lastif.first[0]->setSuccessor (1, plastopbb.first);

				lastif.second->eraseFromParent();
				ifstatements.pop_back();

				if (bisand)
					lastif.first[0]->swapSuccessors();
				else
					llvm::ReplaceInstWithInst(dyn_cast<llvm::Instruction> (lastlogicswitchinstr[1]), dyn_cast<llvm::Instruction> (lastlogicswitchinstr[0])),
					dyn_cast<llvm::Instruction> (lastlogicswitchinstr[0])->eraseFromParent();
				//dyn_cast<llvm::Instruction>(plastopbb.second[0])->eraseFromParent(),
				//else
				//dyn_cast<llvm::StoreInst> (plastopbb.second[2])->setOperand (0, plastopbb.second[1]);

				//immidiates.erase (--immidiates.end ());

				/*extern std::list<std::pair<std::array<llvm::BranchInst *, 2>, llvm::BasicBlock *>> ifstatements;
				auto &lastif = ifstatements.back ();

				if (opbbs.size () == 1)
					opbbs.push_back ({});

				//splitbb("", 0);

				startifstatement ();

				//lastif.first[0]->setSuccessor(0, pcurrblock.back());

				//if (bisand)
				//lastif.first[0]->swapSuccessors();*/
				//}

	virtual llvm::Value* assigntwovalues() {
		std::array ops = getops(false, true);

		extern ::val decay(::val lvalue);

		llvm::Value* instr;

		ops[1] = decay(ops[1]);

		ops[1] = convertTo(ops[1], ops[0].lvalues.back().type);

		printtype(ops[0].lvalues.back().value->getType(),
			ops[0].originident);
		printtype(ops[1].value->getType(), ops[1].originident);

		instr = llvmbuilder.CreateStore(ops[1].value, ops[0].lvalues.back().value);

		immidiates.push_back({ ops[1].value, ops[1].type, ops[1].lvalues });

		return instr;
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
					llvm::Value* lvalue = llvmbuilder.CreateGEP(ops[i].value->getType()->getPointerElementType(), ops[i].value,

						ops[!i].value);
					immidiates.push_back(
						{ ops[i].type.front().uniontype != type::FUNCTION && ops[i].type.front().uniontype != type::ARRAY
							 ? llvmbuilder.CreateLoad(lvalue->getType()->getPointerElementType(), lvalue)
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

		unsigned int type = 3; // << 2 | 2;

		auto immlvalue = immidiates.back();

		std::cout << "applying indirection" << std::endl;

		insertinttoimm("0", sizeof "0" - 1, "ul", sizeof "ul" - 1, type);

		subscripttwovalues();
	}

	void insertinttoimm(const char* str, size_t szstr, const char* suffix_in, size_t szstr1, int type) {
		std::string imm, suffix{ suffix_in, szstr1 };

		imm.assign(str, szstr);

		std::list<::type> currtype = { 1, ::type::BASIC };

		const bool isunsigned = suffix.find("u") != suffix.npos || suffix.find("U") != suffix.npos;

		const bool islonglong = suffix.find("ll") != suffix.npos || suffix.find("LL") != suffix.npos;

		const bool islong = !islonglong && (suffix.find("l") != suffix.npos || suffix.find("L") != suffix.npos);

		//type >>= 2;

		const int base[] = { 16, 2, 7, 10 };

		const unsigned long long val = isunsigned ? std::stoull(imm, nullptr, base[type]) : std::stoll(imm, nullptr, base[type]);

#define CHECK_VAL_RANGE(range) isunsigned ? val <= (range) : val <= ((long long)range)

		auto& currbasictype = currtype.back().spec.basicdeclspec;

		if (islonglong && isunsigned)
			goto ulonlong;
		else if (islonglong)
			goto longlong;
		else if (islong && isunsigned)
			goto ulong;
		else if (islong)
			goto slong;
		else if (isunsigned)
			goto uint;

		if (CHECK_VAL_RANGE(INT_MAX))
			currbasictype.basic[1] = "int";
		else
			uint:
		if (CHECK_VAL_RANGE(UINT_MAX) && (base[type] != 10 || isunsigned))
			currbasictype.basic[1] = "int",
			currbasictype.basic[0] = "unsigned";
		else
			slong :
			if (CHECK_VAL_RANGE(LONG_MAX))
				currbasictype.basic[1] = "long",
				currbasictype.longspecsn = 1;
			else
				ulong :
				if (CHECK_VAL_RANGE(ULONG_MAX))
					currbasictype.basic[0] = "unsigned",
					currbasictype.basic[1] = "long",
					currbasictype.longspecsn = 1;
				else
					longlong :
					if (CHECK_VAL_RANGE(LLONG_MAX))
						currbasictype.basic[1] = "long",
						currbasictype.longspecsn = 2;
					else
						ulonlong :
						if (CHECK_VAL_RANGE(ULLONG_MAX) && (base[type] != 10 || isunsigned))
							currbasictype.basic[0] = "unsigned",
							currbasictype.basic[1] = "long",
							currbasictype.longspecsn = 2;

		auto valval = llvm::ConstantInt::get(createllvmtype(currtype), val, !isunsigned);

		immidiates.push_back({ valval, currtype });
	}
};

std::list<val>& basehndl::immidiates = ::immidiates;

const llvm::fltSemantics& getfloatsembytype(val val) {
	if (val.type.front().spec.basicdeclspec.basic[1] == "float")
		return llvm::APFloatBase::IEEEsingle();
	else
		switch (val.type.front().spec.basicdeclspec.longspecsn)
		case 1:
			return llvm::APFloatBase::IEEEquad();
			return llvm::APFloatBase::IEEEdouble();
}

struct handlefpexpr : basehndl {

	virtual basehndl* (*getrestorefn())(basehndl*) {
		return [](basehndl* pnhdl) -> basehndl* {
			return new (pnhdl) handlefpexpr{};
		};
	}

	virtual void getnegative() override {
		//immidiates.back() = integralpromotions(immidiates.back());

		immidiates.back().value = llvmbuilder.CreateFNeg(immidiates.back().value);
	}

	virtual llvm::Value* getlogicalnot() override {
		auto& op = immidiates.back();

		llvm::Value* originstr;

		originstr = op.value = llvmbuilder.CreateFCmp(
			llvm::CmpInst::Predicate::FCMP_OEQ, op.value,
			llvm::ConstantFP::get(llvmctx, llvm::APFloat{ getfloatsembytype(op) }));

		op.type.erase(op.type.begin(), --op.type.end());

		op.value = CreateCastInst(op.value, llvm::Type::getInt32Ty(llvmctx), false);

		extern void printvaltype(::val val);

		printvaltype(op);

		op.type.back().spec.basicdeclspec.basic[1] = "int",
			op.type.back().spec.basicdeclspec.basic[0] = "";

		return originstr;
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

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred, llvm::CmpInst::Predicate unsignedpred, llvm::CmpInst::Predicate fltpred) override {
		std::array ops = getops();

		ops[0].value =
			llvmbuilder.CreateFCmp(fltpred, ops[0].value, ops[1].value);

		ops[0].value =
			CreateCastInst(ops[0].value, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual std::array<llvm::Value*, 2> logictwovalues(bool bisand) override {
		std::array ops = getops(false);

		std::array<llvm::Value*, 2> instr;

		ops[0].value = llvmbuilder.CreateFCmp(
			llvm::CmpInst::Predicate::FCMP_ONE, ops[0].value,
			llvm::ConstantFP::get(llvmctx, llvm::APFloat{ getfloatsembytype(ops[0]) }));

		ops[1].value = llvmbuilder.CreateFCmp(
			llvm::CmpInst::Predicate::FCMP_ONE, ops[1].value,
			llvm::ConstantFP::get(llvmctx, llvm::APFloat{ getfloatsembytype(ops[1]) }));

		//if (bisand != 2)

		instr[0] = ops[0].value = !bisand
			? llvmbuilder.CreateOr(ops[0].value, ops[1].value)
			: llvmbuilder.CreateAnd(ops[0].value, ops[1].value);
		//else
		//	instr[0] = ops[0].value = llvmbuilder.CreateOr(ops[0].value, ops[1].value),
		//	instr[1] = ops[0].value = llvmbuilder.CreateAnd(ops[0].value, ops[1].value);

		ops[0].value = CreateCastInst(ops[0].value, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
		return instr;
	}
};

struct handlecnstexpr : handlefpexpr {
	/*No branching needed for cnst expr*/
	virtual void begin_binary() { }
	virtual void begin_branch() { }
	virtual void end_binary() { }

	virtual basehndl* (*getrestorefn())(basehndl*) {
		return [](basehndl* pnhdl) -> basehndl* {
			return new (pnhdl) handlecnstexpr{};
		};
	}

	using val = ::val;

	//std::list<val>& immidiates = basehndl::immidiates;

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

	virtual void relatetwovalues(llvm::CmpInst::Predicate pred, llvm::CmpInst::Predicate unsignedpred, llvm::CmpInst::Predicate fltpred) override {
		std::array ops = getops();

		if (ops[0].type.back().spec.basicdeclspec.basic[0] != "unsigned")
			ops[0].constant =
			llvm::ConstantExpr::getICmp(pred, ops[0].constant, ops[1].constant);
		else
			ops[0].constant = llvm::ConstantExpr::getCompare(unsignedpred, ops[0].constant,
				ops[1].constant);

		ops[0].constant =
			CreateCastInst(ops[0].constant, llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());
		;

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);
	}

	virtual void bitwisetwovalues(llvm::Instruction::BinaryOps op) override {
		std::array ops = getops();

		ops[0].constant = llvm::ConstantExpr::get(op, ops[0].constant, ops[1].constant);

		immidiates.push_back(ops[0]);
	}

	virtual std::array<llvm::Value*, 2> logictwovalues(bool bisand) override {
		std::array ops = getops(false);

		llvm::Value* instr;

		ops[0].constant = llvm::ConstantExpr::getICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[0].constant, getValZero(ops[0]));

		ops[1].constant = llvm::ConstantExpr::getICmp(
			llvm::CmpInst::Predicate::ICMP_NE, ops[1].constant, getValZero(ops[1]));

		instr = ops[0].constant =
			!bisand ? llvm::ConstantExpr::getOr(ops[0].constant, ops[1].constant)
			: llvm::ConstantExpr::getAnd(ops[0].constant, ops[1].constant);

		ops[0].constant =
			CreateCastInst(ops[0].constant,
				llvm::Type::getInt32Ty(llvmctx), false);

		ops[0].type.erase(ops[0].type.begin(), --ops[0].type.end());
		;

		ops[0].type.back().spec.basicdeclspec.basic[1] = "int",
			ops[0].type.back().spec.basicdeclspec.basic[0] = "";

		immidiates.push_back(ops[0]);

		return {};
	}

	virtual llvm::Constant* CreateCastInst(llvm::Value* C, llvm::Type* Ty, bool bissigned) override {
		return llvm::ConstantExpr::getIntegerCast(llvm::dyn_cast<llvm::Constant> (C), Ty, bissigned);
	}
};

static handlecnstexpr hndlcnstexpr{};

static handlefpexpr hndlfpexpr{};

std::aligned_storage_t<sizeof(handlecnstexpr), alignof(handlecnstexpr)> unincompilingobj;

static basehndl* phndl = new (&unincompilingobj) basehndl{};

//static std::remove_reference<decltype (basehndl::immidiates)>::type::iterator
//cnstexpriterstart{};

size_t szcnstexprinitial;

// static std::string currdeclidentifier{};

extern std::string currstring;

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

DLL_EXPORT void obtainvalbyidentifier(std::unordered_map<unsigned, std::string>& hashmap) {
	obtainvalbyidentifier(hashmap["ident"_h]);
}

const std::list<::var>::reverse_iterator obtainvalbyidentifier(std::string ident, bool push, bool bfindtypedef) {

	std::list<::var>::reverse_iterator var{};

	(void)std::find_if(scopevar.rbegin(), scopevar.rend(),
		[&](std::list<::var>& scope) {
			auto iter =
				std::find_if(scope.rbegin(), scope.rend(),
					[&](const ::var& scopevar) {
						return scopevar.identifier == ident && (bfindtypedef == (scopevar.linkage == "typedef"));
					});

			if (iter != scope.rend())
				return var = iter, true;
			return false;
		});
	/*if (var->type.front ().uniontype == ::type::FUNCTION)
		phndl->immidiates.push_back (
			{pglobal = nonconstructable.mainmodule.getFunction (ident),
			 var->type, true});
	else if (pglobal = nonconstructable.mainmodule.getGlobalVariable (ident))
		phndl->immidiates.push_back ({pglobal, var->type, true});*/

	if (!push) return var;

	val immidiate;
	auto& currfunctype =
		currfunc->type.front().spec.func.parametertypes_list.front();

	if (auto findparam = std::find_if(
		currfunctype.rbegin(), currfunctype.rend(),
		[&](const ::var& param) { return param.identifier == ident; });
		findparam != currfunctype.rend())
		var = findparam;

	llvm::Value* pglobal;

	immidiate.type = var->type;

	pglobal = immidiate.value = var->requestValue();

	immidiate.originident = var->identifier;

	immidiate.lvalues.push_back(immidiate);

	printvaltype(immidiate);

	if (immidiate.value && !immidiate.value->getType()->getPointerElementType()->isFunctionTy())
		immidiate.value = llvmbuilder.CreateLoad(immidiate.value->getType()->getPointerElementType(), immidiate.value);

	phndl->immidiates.push_back(immidiate);

	printvaltype(immidiate);

	return var;
}

DLL_EXPORT void addplaintexttostring(std::unordered_map<unsigned, std::string>& hashes) {
	currstring += std::string{ hashes["textraw"_h] };
}

DLL_EXPORT void addescapesequencetostring(std::unordered_map<unsigned, std::string>& hashes) {
	std::string escape{ hashes["escaperaw"_h] };

	switch (stringhash(escape.c_str())) {
	case "\\n"_h:
		currstring += '\n';
		break;
	}
}

DLL_EXPORT void constructstring() {
	std::list<::type> stirngtype{ 1, ::type::ARRAY };

	stirngtype.back().spec.arraysz = currstring.size() + 1;

	stirngtype.push_back({ ::type::BASIC });

	stirngtype.back().spec.basicdeclspec.basic[1] = "char";

	auto lvalue = llvmbuilder.CreateGlobalStringPtr(
		currstring, "", 0);

	immidiates.push_back(
		{ llvmbuilder.CreateLoad(lvalue->getType()->getPointerElementType(), lvalue), stirngtype, {}, "[[stringliteral]]" });
	immidiates.back().lvalues.push_back(immidiates.back());
	immidiates.back().lvalues.back().value = lvalue;
	currstring = "";
}

void addvar(var& lastvar, llvm::Constant* pInitializer) {

	const char* lastvartypestoragespec = lastvar.linkage.c_str();

	assert (lastvar.linkage != "typedef");
	//	for (lastvar.pValue = nullptr /*(llvm::Value *)lastvar.pllvmtype*/;;)
	//		return;

	if(lastvar.type.front().uniontype == type::FUNCTION || !lastvar.linkage.empty())
		goto extrnl_decl;

	switch (lastvar.firstintroduced) {
	extrnl_decl:
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

		switch (lastvar.type.front().uniontype) {
		case type::POINTER:
		case type::ARRAY:
		case type::BASIC:
			lastvar.pValue = new llvm::GlobalVariable(
				nonconstructable.mainmodule, lastvar.requestType(), false,
				linkagetype,
				lastvar.firstintroduced == 1 ? 
				pInitializer ? pInitializer : llvm::Constant::getNullValue(lastvar.requestType())
				: nullptr,
				lastvar.identifier);
			// scopevar.front ().push_back (lastvar);
			break;
		case type::FUNCTION:
			printtype(lastvar.requestType(), lastvar.identifier);
			lastvar.pValue = llvm::Function::Create(
				llvm::dyn_cast<llvm::FunctionType> (lastvar.requestType()),
				linkagetype, lastvar.identifier, nonconstructable.mainmodule);
			// scopevar.front ().push_back (lastvar);
			break;
		}
		break;
	default:
		printtype(lastvar.requestType(), lastvar.identifier);
		lastvar.pValue = llvmbuilder.CreateAlloca(lastvar.requestType(), nullptr,
			lastvar.identifier);
		break;
	}
}

extern std::pair<std::string, std::string> currstruct;

DLL_EXPORT void endbuildingstructorunion() {
	auto& lastmembers = structorunionmembers.back().back();
	auto& structvar = lastmembers.front();

	//for (auto& a : lastmembers | ranges::views::drop(1))
		//a.pllvmtype = createllvmtype(a.type);

	/*std::vector<llvm::Type*> structtypes;

	std::transform(++lastmembers.begin(), lastmembers.end(),
		std::back_inserter(structtypes),
		[](const var& elem) { return elem.pllvmtype; });*/

	currtypevectorbeingbuild.pop_back();

	assert(structvar.type.back().spec.basicdeclspec.basic[0] == "struct");

	//dyn_cast<llvm::StructType> (structvar.pllvmtype)->setBody(structtypes);

	//currstruct.first = structvar.type.back().spec.basicdeclspec.basic[0];
	//currstruct.second = structvar.type.back().spec.basicdeclspec.basic[3];

	//static_cast<basic_type_origin&>(structvar.type.back().spec.basicdeclspec) = basic_type_origin{ iterunorstrtype{} };

	//structvar.type.back().spec.basicdeclspec.iterunorstr = structorunionmembers.back().end();
}

/*void startdeclaration(std::string typedefname) {
	//std::string typedefname = currdeclspectypedef;
	var var;

	type basic{ type::BASIC };

	if (qualifsandtypes.back().first.empty() && currstruct.first.empty() && typedefname.empty())
		assert(!(scopevar.size() > 1) || !currenum.empty()),
		basic.spec.basicdeclspec.basic[1] = "int";
	else {
		auto&& basictypefull = parsebasictype(::qualifsandtypes.back().first); //since ass will lose the storage spec
		basic.spec.basicdeclspec = basictypefull;
		var.linkage = basictypefull.basic[2];
	}

	if (!(basic.spec.basicdeclspec.basic[3] = typedefname).empty()) {
		//obtainvalbyidentifier(basic.spec.basicdeclspec.basic[3]),
		auto typetypedf = obtainvalbyidentifier(basic.spec.basicdeclspec.basic[3], false, true)->type;
		typetypedf.back().spec.basicdeclspec = typetypedf.back().spec.basicdeclspec;
		std::rotate(typetypedf.begin(), typetypedf.end() - 1, typetypedf.end());
		var.type = typetypedf;
	}
	//immidiates.pop_back();

	currenum /*= currdeclspectypedef/ = "";

	//qualifsandtypes.back().first.clear();

	if (basic.spec.basicdeclspec.basic[3].empty())
		if (!(basic.spec.basicdeclspec.basic[3] = currstruct.second).empty())
			basic.spec.basicdeclspec.basic[0] = currstruct.first;
		else if (!(basic.spec.basicdeclspec.basic[3] = currenum).empty())
			basic.spec.basicdeclspec.basic[0] = "enum";

	currstruct.first.clear();

	currstruct.second.clear();

	if (var.type.empty())
		var.type.push_back(basic);

	currtypevectorbeingbuild.back().p->push_back(var);
}*/

bool bIsBasicFloat(const type& type);

bool bIsBasicInteger(const type& type) {
	std::set integertraits{ "unsigned", "signed", "" };
	return type.uniontype == type::BASIC
		&& !type.spec.basicdeclspec.basic[1].empty()
		&& ranges::find(integertraits, type.spec.basicdeclspec.basic[0]) != integertraits.end() && !bIsBasicFloat(type);
}

bool bIsBasicFloat(const type& type) {
	return type.uniontype == type::BASIC
		&& ranges::contains(std::array{ "float", "double" }, type.spec.basicdeclspec.basic[1]);
}

val convertTo(val target, std::list<::type> to) {
	printvaltype(target);
	printtype(createllvmtype(to), "to");
	//if (to.front().spec.basicdeclspec.basic[3].empty())
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
		else if (target.type.front().uniontype == type::POINTER)
			target.value = llvmbuilder.CreatePtrToInt(target.value, createllvmtype(to));
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
		else
			;
	else if (to.front().uniontype == type::POINTER && bIsBasicInteger(target.type.front()))
		target.value = llvmbuilder.CreateIntToPtr(target.value, createllvmtype(to));
	else
		target.value = llvmbuilder.CreateBitCast(target.value, createllvmtype(to));

	target.type = to;

	target.lvalues.clear();

	return target;
}

DLL_EXPORT void applycast() {
	auto& lasttypevar = currtypevectorbeingbuild.back().p->back();

	//currtypevectorbeingbuild.back().p->pop_back();

	auto& currtype = lasttypevar.type;

	//std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	auto target = phndl->immidiates.back();

	phndl->immidiates.pop_back();

	target = convertTo(target, currtype);

	phndl->immidiates.push_back(target);

	currtypevectorbeingbuild.back().p->pop_back();
}

/*void endpriordecl() {
	auto& currtype = currtypevectorbeingbuild.back().p->back().type;

	std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	currtypevectorbeingbuild.back().p->back().firstintroduced = scopevar.size();

	if (currtypevectorbeingbuild.back().currdecltype !=
		currdecltypeenum::PARAMS
		&& currtypevectorbeingbuild.back().p->back().linkage.empty()
		&& currtypevectorbeingbuild.back().p->back().type.front().uniontype != type::FUNCTION
		&& scopevar.size() == 1)
		currtypevectorbeingbuild.back()
		.p->back()
		.pllvmtype = createllvmtype(currtype),
		addvar(currtypevectorbeingbuild.back().p->back());
}*/

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

void pushsizeoftype(std::list<type> type) {
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

extern const std::list<::var>* getstructorunion(bascitypespec& basic, std::list<::var>* last_struct=nullptr);

DLL_EXPORT void memberaccess(std::unordered_map<unsigned, std::string>&& hashentry) {
	if (hashentry["arrowordotraw"_h] == "->")
		phndl->applyindirection();

	auto& lastvar = phndl->immidiates.back();

	auto pliststruct = //lastvar.type.front().spec.basicdeclspec.iterunorstr;
		getstructorunion(lastvar.type.front().spec.basicdeclspec);

	auto imember = 0;

	auto listiter = ++pliststruct->begin();

	for (; listiter != pliststruct->end() &&
		listiter->identifier != hashentry["ident"_h];
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

	llvm::Value* rvalue = llvmbuilder.CreateLoad(lvalue->getType()->getPointerElementType(), lvalue);

	lastvar.type = member.type;

	lastvar.originident = member.identifier;

	lastvar.value = rvalue;

	printvaltype(lastvar);

	lastvar.lvalues.push_back({ lvalue, lastvar.type });

	return;
}

DLL_EXPORT void endsizeoftypename() {

	auto currtype = currtypevectorbeingbuild.back().p->back().type;

	//std::rotate(currtype.begin(), currtype.begin() + 1, currtype.end());

	currtypevectorbeingbuild.back().p->pop_back();

	pushsizeoftype(currtype);
}

std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>> ifstatements{ 1 };

llvm::BranchInst* splitbb(const char* identifier, size_t szident);

DLL_EXPORT void insertinttoimm(const char* str, size_t szstr, const char* suffix, size_t szstr1, int type);

DLL_EXPORT void startifstatement() {
	startifstatement(true);
}

std::list<std::pair<std::array<llvm::BranchInst*, 2>, llvm::BasicBlock*>>::iterator startifstatement(bool pop) {
	auto dummyblock = llvm::BasicBlock::Create(llvmctx, "", dyn_cast<llvm::Function> (currfunc->requestValue()));

	if (!immidiates.size())
		insertinttoimm("1", sizeof "1" - 1, "", 0, 3);

	auto firstnot = phndl->getlogicalnot();

	auto lastnot = phndl->getlogicalnot();

	splitbb("", 0);

	auto value = llvmbuilder.CreateIntCast(immidiates.back().value, llvm::IntegerType::get(llvmctx, 1), true);

	ifstatements.push_back({ {llvmbuilder.CreateCondBr(value, dummyblock, dummyblock)}, dummyblock });

	splitbb("", 0);

	ifstatements.back().first[0]->setSuccessor(0, pcurrblock.back());

	if (pop)
		immidiates.pop_back();

	return --ifstatements.end();
}

DLL_EXPORT void continueifstatement() {
	ifstatements.back().first[1] = splitbb("", 0);
	ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());
}

DLL_EXPORT void endifstatement() {
	splitbb("", 0);
	if (auto lastbranchif = ifstatements.back().first[1])
		lastbranchif->setSuccessor(0, pcurrblock.back());

	if (ifstatements.back().first[0]->getSuccessor(1) == ifstatements.back().second)
		ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());

	ifstatements.back().second->eraseFromParent();
	ifstatements.pop_back();
}

DLL_EXPORT void beginlogicalop(int blastorfirst) {

	/*if (!opsscopeinfo.back ().logicopswitches.empty ()) {

		val imm = opsscopeinfo.back ().currlogicopvar;

		imm.value = llvmbuilder.CreateLoad (imm.value);

		imm.lvalues.push_back (imm);

		imm.originident = "[[logicop]]";

		immidiates.push_back (imm);



	} else {*/

	val imm = opsscopeinfo.back().currlogicopvar;

	/*val imm = opsscopeinfo.back ().currlogicopvar;

	imm.lvalues.push_back (imm);

	imm.originident = "[[logicop]]";

	immidiates.push_back (imm);*/

	if (opsscopeinfo.back().lastifs != --ifstatements.end()) {

		imm.value = llvmbuilder.CreateLoad(imm.value->getType()->getPointerElementType(), imm.value);

		imm.lvalues.clear();

		//immidiates.pop_back ();

		immidiates.push_back(imm);

		opsscopeinfo.back().logicopswitches.push_back(phndl->logictwovalues(2));
	}

	std::rotate(----immidiates.end(), --immidiates.end(), immidiates.end());

	phndl->assigntwovalues();

	//immidiates.erase (++opsscopeinfo.back().immidiatesbegin, immidiates.end ());

	//}
	if (blastorfirst != 2) {
		if (opsscopeinfo.back().lastifs != --ifstatements.end())
			continueifstatement();

		startifstatement();
	}
	else {
		continueifstatement();

		imm.value = llvmbuilder.CreateLoad(imm.value->getType()->getPointerElementType(), imm.value);

		imm.lvalues.clear();

		immidiates.push_back(imm);
	}

	//opsscopeinfo.back ().logicopswitches

	/*std::array<llvm::Value *, 3> instrs;

		//instrs[1] = phndl->getlogicalnot ();
		////instrs[1] = immidiates.back ().value;
		//instrs[0] = phndl->getlogicalnot ();

	instrs[0] = phndl->logictwovalues (true);

	val imm = currlogicopval.back ();

	imm.originident = "[[logicop]]";

	imm.lvalues.push_back (imm);

	immidiates.push_back (imm);

	std::rotate (----immidiates.end (), --immidiates.end (), immidiates.end ());

	instrs[2] = phndl->assigntwovalues ();

	immidiates.pop_back ();

	//immidiates.erase (--immidiates.end ());

	splitbb ("", 0);

	imm.value = llvmbuilder.CreateLoad (imm.value);
	immidiates.push_back (imm);

	if (blastorfirst != 1) {

		opbbs.back ().first.push_back ({pcurrblock.back (), instrs});

		startifstatement ();
	} else {
		opbbs.back ().first.push_back ({pcurrblock.back (), instrs});

		//immidiates.push_back (imm);

		opbbs.back ().second = imm;
	}*/

	//opbbs.push_back (ifstatements.end());
}

DLL_EXPORT void beginbinary() {

	opsscopeinfo.push_back({ --immidiates.end(), {{type::BASIC}}, --ifstatements.end() });

	//std::pair<var, decltype (immidiates)::iterator> currlogicelem;

	opsscopeinfo.back().currlogicopvar.type.back().spec.basicdeclspec.basic[1] = "int";

	opsscopeinfo.back().currlogicopvar.pllvmtype = createllvmtype(opsscopeinfo.back().currlogicopvar.type);

	addvar(opsscopeinfo.back().currlogicopvar);

	/*var result{{type::BASIC}};

	result.type.back ().spec.basicdeclspec.basic[1] = "int";

	result.pllvmtype = createllvmtype (result.type);

	addvar (result);

	val imm = result;

	insertinttoimm ("1", sizeof "1", "", 0, 3);

	imm.value = llvmbuilder.CreateLoad (imm.value);
	immidiates.push_back (imm);

	phndl->assigntwovalues();

	currlogicopval.push_back (result);
	opbbs.push_back ({});
	beginlogicalop (1);*/
	//currlogicopval.back ().second = --immidiates.end ();
}

DLL_EXPORT void endbinarybeforerevlogicops() {
	if (opsscopeinfo.back().lastifs != --ifstatements.end())
		beginlogicalop(2);
	/*if (!opbbs.back ().first.empty ())
		insertinttoimm ("1", sizeof "1", "", 0, 3),
		beginlogicalop (1);*/
}

DLL_EXPORT void endbinary() {

	auto opscopinf = opsscopeinfo.back();

	opsscopeinfo.pop_back();

	auto lastimm = immidiates.back();

	immidiates.erase(++opscopinf.immidiatesbegin, immidiates.end());

	immidiates.push_back(lastimm);

	//if (immidiates.back ().originident == "[[logicop]]")
	//immidiates.back ().value = llvmbuilder.CreateLoad (immidiates.back ().value);

	/*if (opbbs.back ().second.value)
		immidiates.pop_back (),
			immidiates.push_back (opbbs.back ().second);

	currlogicopval.pop_back ();
	opbbs.pop_back ();*/
	//if(phndl->opbbs.size())
	//phndl->endlast(phndl->opbbs.back().first, phndl->opbbs.back().second);

	//opbbs.pop_back ();

	//if(currlogicopval.pValue)
	//immidiates.push_back(currlogicopval);

	//currlogicopval.pValue = nullptr;
}

DLL_EXPORT void endsizeofexpr() {
	auto lastimmtype = phndl->immidiates.back().type;
	printtype(createllvmtype(lastimmtype),
		phndl->immidiates.back().originident);
	phndl->immidiates.pop_back();
	pushsizeoftype(lastimmtype);
}

/*DLL_EXPORT void adddeclarationident(const char* str, size_t szstr,
	bool bistypedef) {
	std::string ident{ str, szstr };

	currtypevectorbeingbuild.back().p->back().identifier = ident;

	//currtypevectorbeingbuild.back().p->back().bistypedef = currtypevectorbeingbuild.back().p->back().type.back().spec.basicdeclspec.basic[2] == "typedef";
}*/

//DLL_EXPORT void finalizedeclaration() { endpriordecl(); }

void fixupstructype(std::list<::var>* var) {
	for (auto& a : *var | ranges::views::drop(1))
		a.pllvmtype = createllvmtype(a.type, var);

	auto& structvar = var->front();

	std::vector<llvm::Type*> structtypes;

	std::transform(++var->begin(), var->end(),
		std::back_inserter(structtypes),
		[](const ::var& elem) { return elem.pllvmtype; });

	dyn_cast<llvm::StructType> (structvar.pllvmtype)->setBody(structtypes);
}

const std::list<::var>* getstructorunion(bascitypespec& basic, std::list<::var>* last_struct) {
	std::list<::var>* var = nullptr;

	std::string ident = basic.basic[3];

	std::find_if(structorunionmembers.rbegin(), structorunionmembers.rend(),
		[&](std::list<std::list<::var>>& scope) {
			auto iter = std::find_if(
				scope.rbegin(), scope.rend(),
				[&](const std::list<::var>& scopevar) {
					return scopevar.front().identifier == ident;
				});

			if (iter != scope.rend())
				return var = &*iter, true;
			return false;
		});

	if (!var) var = static_cast<std::list<::var>*>(basic.pexternaldata);

	if (var) if (!var->back().pllvmtype)
		if (var != last_struct) fixupstructype(var);
		else var->back().pllvmtype = var->front().pllvmtype;

	return var;
}

llvm::Type* createllvmtype(std::list<type> &refdecltypevector, std::list<::var>* last_struct, std::list<type> * ptrdecltypevector) {
	llvm::Type* pcurrtype;

	auto decltypevector = refdecltypevector;

	//refdecltypevector.clear();

	//bool bjastypedef = false;

	std::array lambdas = {
		std::function{[&](type& type, std::list<::type>::iterator &iter) {
			switch (stringhash(type.spec.basicdeclspec.basic[1].c_str())) {
			case "short"_h:
				pcurrtype =
					dyn_cast<llvm::Type> (llvm::Type::getInt16Ty(llvmctx));
				break;
				break;
			label_int:
			case "int"_h:
			case "long"_h:
				if (type.spec.basicdeclspec.longspecsn > (LONG_MAX == INT_MAX))
				case "__int64"_h:
					pcurrtype = dyn_cast<llvm::Type> (llvm::Type::getInt64Ty(llvmctx));
				else
					pcurrtype = dyn_cast<llvm::Type> (llvm::Type::getInt32Ty(llvmctx));
				break;
				break;
			addvoid:
			case "void"_h: {
				/*pcurrtype =
					dyn_cast<llvm::Type> (llvm::Type::getVoidTy(llvmctx));
				break;
				break;*/
				/*pcurrtype = dyn_cast<llvm::Type> (llvm::Type::getInt32Ty(llvmctx));
				pcurrtype = dyn_cast<llvm::Type> (llvm::FunctionType::get(pcurrtype, {}, true));
				pcurrtype = dyn_cast<llvm::Type> (pcurrtype->getPointerTo());
				{
					::type typ{ ::type::BASIC};
					typ.spec.basicdeclspec.basic[0] = "int";
					refdecltypevector.insert(iter, typ);
				}
				{
					::type typ{ ::type::FUNCTION};
					typ.spec.func.bisvariadic = true;
					refdecltypevector.insert(iter, typ);
				}
				::type typ{ ::type::POINTER};
				refdecltypevector.insert(iter, typ);
				auto itertoerase = iter--;
				----iter;
				refdecltypevector.erase(itertoerase);*/
			}
			break;
		addchar:
		case "_Bool"_h:
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
			case "struct"_h: {
				auto currstruct = getstructorunion(type.spec.basicdeclspec, last_struct);
				if (!currstruct) {
					//bincompletetype = true;
					goto addchar;
				}
				pcurrtype =
					currstruct
					->front()
					.pllvmtype;
			}
				break;
			case "enum"_h:
				goto label_int;//throw std::exception{ "enum should have int type" };
			case "union"_h:
				break;
			default: { // typedef
				//bjastypedef = true;
				pcurrtype = obtainvalbyidentifier(type.spec.basicdeclspec.basic[3], false, true)->requestType(ptrdecltypevector);
				return false;
			}
			}
		}
			return true;
	}},
	std::function{[&](type& type, std::list<::type>::iterator &iter) {
		pcurrtype = dyn_cast<llvm::Type> (pcurrtype->getPointerTo());
		return true;
	}},
	std::function{[&](type& type, std::list<::type>::iterator &iter) {
		pcurrtype = dyn_cast<llvm::Type> (
			llvm::ArrayType::get(pcurrtype, type.spec.arraysz));
		return true;
	}},
	std::function{[&](type& type, std::list<::type>::iterator &iter) {
		std::vector<llvm::Type*> paramtype;
		for (auto& a : type.spec.func.parametertypes_list.front())
			paramtype.push_back(a.requestType());
		pcurrtype = dyn_cast<llvm::Type> (llvm::FunctionType::get(
			pcurrtype, paramtype, type.spec.func.bisvariadic));
		return true;
	}} };

	std::reverse(decltypevector.begin(), decltypevector.end());

	try {
		auto iterref = refdecltypevector.end();
		for (auto& type : decltypevector)
			lambdas[type.uniontype](type, iterref) &&
			ptrdecltypevector && (ptrdecltypevector->push_front(type), true),
			--iterref;
	}
	catch (std::nullptr_t exc) {
		return exc;
	}

	//bincompletetype = false;

	return pcurrtype;
}

extern std::vector<llvm::BasicBlock*> pcurrblock;

void finalizedecl();

static std::list<std::pair<llvm::SwitchInst*, llvm::BasicBlock*>> currswitch;

llvm::BranchInst* splitbb(const char* identifier, size_t szident);

DLL_EXPORT void beginscope() {
	bool beginofafnuc = scopevar.size() == 1;
	if (beginofafnuc) {
		var current_arg;
		currfunc = --scopevar.back().end();
		auto currfuncval = currfunc->requestValue();
		auto iter_params = currfunc->type.front()
			.spec.func.parametertypes_list.front()
			.begin();
		for (auto& arg : dyn_cast<llvm::Function> (currfuncval)->args())
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

	if (beginofafnuc)
		for (auto& param : currfunc->type.front().spec.func.parametertypes_list.front()) {
			auto origparamvalue = param.pValue;

			param.firstintroduced = 2;

			printvaltype(param);

			addvar(param);

			printvaltype(param);

			llvmbuilder.CreateStore(origparamvalue, param.pValue);
		}

	structorunionmembers.push_back({});

	enums.push_back({});

	std::cout << "begin scope finished" << std::endl;
}

void fixuplabels();

DLL_EXPORT void endscope() {
	// nonconstructable.mainmodule.
	// endexpression();
	//pcurrblock.pop_back();
	scopevar.pop_back();
	if (scopevar.size() > 1)
		splitbb("", 0);
	else
		pcurrblock.pop_back();
	currtypevectorbeingbuild.pop_back();
	structorunionmembers.pop_back();
	enums.pop_back();

	if (scopevar.size() == 1) //end of a function
		fixuplabels();
}

DLL_EXPORT void endexpression() { phndl->immidiates.clear(); }

std::list<llvm::BasicBlock*> dowhileloops;

std::list<std::list<llvm::BranchInst*>> breakbranches;

std::list<std::list<llvm::BranchInst*>> continuebranches;

void fixupcontinuebranches(), fixupbrakebranches();

DLL_EXPORT void startdowhileloop() {
	splitbb("", 0);

	dowhileloops.push_back(pcurrblock.back());
	breakbranches.push_back({});
	continuebranches.push_back({});
}

DLL_EXPORT void enddowhileloop() {
	fixupcontinuebranches();
	startifstatement();
	fixupbrakebranches();
	ifstatements.back().first[0]->setSuccessor(1, pcurrblock.back());
	ifstatements.back().first[0]->setSuccessor(0, dowhileloops.back());
	ifstatements.back().second->eraseFromParent();
	ifstatements.pop_back();
	dowhileloops.pop_back();
}

DLL_EXPORT void addbreak() {
	breakbranches.back().push_back(llvmbuilder.CreateBr(pcurrblock.back()));
}

DLL_EXPORT void addcontinue() {
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

DLL_EXPORT void startforloopcond() {
	std::array<llvm::BasicBlock*, 2> bb;
	splitbb("", 0);
	bb[0] = pcurrblock.back();

	forloops.push_back(bb);
	breakbranches.push_back({});
	continuebranches.push_back({});
}

DLL_EXPORT void endforloopcond() {
	startifstatement();
	endexpression();
	//insertinttoimm("0", sizeof "0" - 1, "", 0, 3);
	//startifstatement();
	auto lastblcok = pcurrblock.back();

	forloops.back()[1] = lastblcok;
}

DLL_EXPORT void addforloopiter() {
	llvmbuilder.CreateBr(forloops.back()[0]);
	splitbb("", 0);

	//endifstatement();
	ifstatements.back().first[0]->setSuccessor(0, pcurrblock.back());
}

DLL_EXPORT void endforloop() {
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
		std::rotate(currtype.rbegin(), ++currtype.rbegin(),
			currtype.rend());

		assert(currtype.front().uniontype == ::type::POINTER);

		printvaltype(lvalue);

		lvalue.value = lvalue.lvalues.back().value;

		lvalue.lvalues.pop_back();

		if (lvalue.value->getType()->getPointerElementType()->isArrayTy())
			lvalue.value = llvmbuilder.CreateGEP(lvalue.value->getType()->getPointerElementType(),
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
			llvmbuilder.CreateLoad(lvalue.value->getType()->getPointerElementType(),
				/*gen_pointer_to_elem_if_ptr_to_array*/ (lvalue.value)),
			currtype };
		printvaltype(ret);
		return ret;
	}
	return lvalue;
}

DLL_EXPORT void startfunctioncall() {
	callees.push_back(--immidiates.end());
}

std::list<std::pair<llvm::BranchInst*, std::string>> branches;

llvm::BranchInst* splitbb(const char* identifier, size_t szident) {
	bool bareweabrupt = bareweinabrupt();
	if (pcurrblock.size())
		pcurrblock.pop_back();
	else
		bareweabrupt = true;
	pcurrblock.push_back(llvm::BasicBlock::Create(
		llvmctx, std::string{ identifier, szident }, dyn_cast<llvm::Function> (currfunc->requestValue())));
	llvm::BranchInst* preturn;
	if (!bareweabrupt)
		preturn = llvmbuilder.CreateBr(pcurrblock.back());
	llvmbuilder.SetInsertPoint(pcurrblock.back());
	return preturn;
}

DLL_EXPORT void splitbb(std::unordered_map<unsigned, std::string>&& hashes) {
	splitbb(STRING_TO_PTR_AND_SZ(hashes["lbl"_h]));
}

DLL_EXPORT void gotolabel(std::unordered_map<unsigned, std::string>&& hashes) {
	branches.push_back({ llvmbuilder.CreateBr(pcurrblock.back()), hashes["gtid"_h] });
}

void fixuplabels() {
	for (auto& [branchinst, ident] : branches)
		for (auto& bb : dyn_cast<llvm::Function> (currfunc->requestValue())->getBasicBlockList())
			if (bb.getName() == ident)
				branchinst->setSuccessor(0, &bb);
	branches.clear();
}

DLL_EXPORT void startswitch() {
	//pcurrblock.pop_back();
	//pcurrblock.push_back(llvm::BasicBlock::Create(llvmctx, "", dyn_cast<llvm::Function> (currfunc->pValue)));
	auto dummyblock = llvm::BasicBlock::Create(llvmctx, "", dyn_cast<llvm::Function> (currfunc->requestValue()));
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
		if (pcurrblock.back()->getInstList().size()) {
			auto lastinstropcode = pcurrblock.back()->back().getOpcode();
			return ranges::contains(std::array{ llvm::Instruction::Br, llvm::Instruction::Switch,llvm::Instruction::Ret }, lastinstropcode);
		}
		else
			return false;
	return true;
}

DLL_EXPORT void endswitch() {
	splitbb("", 0);
	fixupbrakebranches();
	if (currswitch.back().first->getDefaultDest() == currswitch.back().second)
		currswitch.back().first->setDefaultDest(pcurrblock.back());
	currswitch.back().second->eraseFromParent();
	currswitch.pop_back();
}

DLL_EXPORT void addCase() {
	splitbb("", 0);
	currswitch.back().first->addCase(llvm::dyn_cast<llvm::ConstantInt> (phndl->immidiates.back().value), pcurrblock.back());
	phndl->immidiates.pop_back();
}

DLL_EXPORT void addDefaultCase() {
	splitbb("", 0);
	currswitch.back().first->setDefaultDest(pcurrblock.back());
}

/*val<> getfunctionptr(val<> fptr) {
	for(auto a = fptr.type.begin(); a != fptr.type.end(); ++a)
		if(a->uniontype == type::FUNCTION) break;
		else fptr.type.erase(a), fptr.value = llvmbuilder.CreatePointerCast
(value, type->getPointerTo ())
}*/

DLL_EXPORT void endfunctioncall() {
	auto lastblock = pcurrblock.back();

	auto argsiter = callees.back();

	auto calleevalntype = decay(*argsiter);

	assert(calleevalntype.value->getType()->isPointerTy());

	if (calleevalntype.type.front().uniontype == type::POINTER)
		calleevalntype.type.pop_front();

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

	auto& verylongthingy = calleevalntype.type.front().spec.func.parametertypes_list.front();

	auto iterparams = verylongthingy.begin();

	bool breached = false;

	std::transform(
		++argsiter, ::immidiates.end(), std::back_inserter(immidiates),
		[&](const basehndl::val& elem) { return !(breached = breached || iterparams == verylongthingy.end()) ? convertTo(decay(elem), iterparams++->type).value : decay(elem).value; });

	::immidiates.erase(--argsiter, ::immidiates.end());

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

	calleevalntype.type.pop_front();

	phndl->immidiates.push_back(
		val{ pval, calleevalntype.type });
}

DLL_EXPORT void endreturn(std::unordered_map<unsigned, std::string>&& hashes) {
	//llvmbuilder.SetInsertPoint (pcurrblock.back ());
	if(!hashes["returnval"_h].empty()) {
		auto currfunctype = currfunc->type;
		currfunctype.pop_front();
		auto op = convertTo(immidiates.back(), currfunctype);
		llvmbuilder.CreateRet(op.value);
	}
	else llvmbuilder.CreateRetVoid();
}

DLL_EXPORT void endfunctionparamdecl(std::unordered_map<unsigned, std::string>&& hashes) {

	//for (auto& a : *currtypevectorbeingbuild.back().p)
	//	a.pllvmtype = createllvmtype(a.type);

	currtypevectorbeingbuild.pop_back();

	auto& functype = currtypevectorbeingbuild.back().p->back().type.back();

	assert(functype.uniontype == type::FUNCTION);

	functype.spec.func.bisvariadic = !hashes["rest"_h].empty();
}

/*DLL_EXPORT void continuedeclaration() {

	auto lastvar = currtypevectorbeingbuild.back().p->back();

	auto lastbasetype = lastvar.type.back();

	assert(lastbasetype.uniontype == type::BASIC);

	lastvar.type.clear();

	lastvar.type.push_back(lastbasetype);

	currtypevectorbeingbuild.back().p->push_back(lastvar);
}*/

DLL_EXPORT void addptrtotype(std::unordered_map<unsigned, std::string>&& hashes);

DLL_EXPORT void endqualifs(std::unordered_map<unsigned, std::string>&& hashes) {
	auto& lastvar = currtypevectorbeingbuild.back().p->back();

	auto& refbasic = lastvar.type.back().spec.basicdeclspec.basic;

	std::string nontypedeflinkage = refbasic[2];

	if (!nontypedeflinkage.empty()) lastvar.linkage = nontypedeflinkage;

	if (std::all_of(refbasic.begin(), refbasic.end(), [](const std::string& elem) {return elem.empty(); }))
		if (lastvar.firstintroduced == 1) refbasic[1] = "int";
		else throw std::runtime_error{ "decl with no basic info"};

	if (ranges::contains(std::array{ "struct", "union", "enum" }, refbasic[0]) && refbasic[3].empty())
		switch (stringhash(refbasic[0].c_str()))
		if (0) case "struct"_h:
		case "union"_h:
		{
			auto* reflaststruc = &structorunionmembers.back().back();
			//if (!reflaststruc->back().pllvmtype) fixupstructype(reflaststruc);

			lastvar.type.back().spec.basicdeclspec.pexternaldata = reflaststruc;
		}
							  else;
	else
		lastvar.fixupTypeIfNeeded();
		/*					  else
		if (0) case "enum"_h:
	{
		lastvar.type.back().spec.basicdeclspec.pexternaldata = &enums.back().back();
	}*/
		//currtypevectorbeingbuild.pop_back();
}

DLL_EXPORT void startfunctionparamdecl() {

	if (currtypevectorbeingbuild.back().currdecltype ==
		currdecltypeenum::PARAMS) // if declaring a function inside another
								  // declaration
		addptrtotype(std::unordered_map<unsigned, std::string>{});

	currtypevectorbeingbuild.back().p->back().type.push_back(
		{ type::FUNCTION });

	currtypevectorbeingbuild.push_back(
		{ currtypevectorbeingbuild.back()
			 .p->back()
			 .type.back()
			 .spec.func.parametertypes_list.begin(),
		 currdecltypeenum::PARAMS });
}

DLL_EXPORT void addsubtotype() {

	type arraytype{ type::ARRAY };

	auto res = llvm::dyn_cast<llvm::ConstantInt> (
		hndlcnstexpr.immidiates.back().value);

	arraytype.spec.arraysz = *res->getValue().getRawData();
	currtypevectorbeingbuild.back().p->back().type.push_back(arraytype);

	hndlcnstexpr.immidiates.pop_back();
}

DLL_EXPORT void addptrtotype(std::unordered_map<unsigned, std::string>&& hashes) {

	type ptrtype{ type::POINTER };

	ptrtype.spec.ptrqualifiers =
		parsequalifiers(hashes["qualifptr"_h]);

	currtypevectorbeingbuild.back().p->back().type.push_back(ptrtype);
}

DLL_EXPORT void insertinttoimm(const char* str, size_t szstr, const char* suffix, size_t szstr1, int type) {
	phndl->insertinttoimm(str, szstr, suffix, szstr1, type);
}

DLL_EXPORT void subscript() { phndl->subscripttwovalues(); }

static basehndl* (*phpriorhndlfn_cnst_expr)(basehndl*);

DLL_EXPORT void beginconstantexpr() {
	//cnstexpriterstart = phndl->immidiates.end();
	szcnstexprinitial = phndl->immidiates.size();

	phpriorhndlfn_cnst_expr = phndl->getrestorefn();

	//phndl = &hndlcnstexpr;

	phndl->~basehndl();

	phndl = new (phndl) handlecnstexpr{};
}

DLL_EXPORT void endconstantexpr() {
	//assert(szcnstexprinitial == phndl->immidiates.size());

	// auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

	// hndlcnstexpr.immidiates.pop_back();

	// std::cout << "computed value: " << *res->getValue().getRawData() <<
	// std::endl;
		phndl->~basehndl(),
		phndl = phpriorhndlfn_cnst_expr(phndl);
}

DLL_EXPORT void docall(const char*, size_t, void*);

DLL_EXPORT void startmodule(const char* modulename, size_t szmodulename) {
	new (&nonconstructable.mainmodule)
		llvm::Module{ std::string{modulename, szmodulename}, llvmctx };

	pdatalayout = new llvm::DataLayout{ &nonconstructable.mainmodule };


	if(const char *preplaypath = getenv("REPLAY")) {
		std::ifstream replay{preplaypath, std::ifstream::binary};

		unsigned int hash;

		std::string value;

		std::unordered_map<unsigned, std::string> map;

		do {
			std::getline(replay, value, '\0');

			if(!value.empty()) {
				replay.read((char*)&hash, 4);
				map.insert({ hash, value });
			}
			else {
				std::getline(replay, value, '\0');
				docall(value.c_str(), value.length(), &map);
				map.clear();
			}
		} while(!replay.eof());
	}

	//system("PAUSE");
}

#include <condition_variable>
#include <mutex>
#include <pthread.h>

static std::condition_variable condwake;
static std::mutex mutexwork;

static bool endwork = false;

extern "C" pthread_t thread;

std::ofstream record{getenv("RECORD") ? getenv("RECORD") : "", std::ios::binary};

DLL_EXPORT void endmodule() {
	if(getenv("THREADING")) {
		mutexwork.lock();
		endwork = true;
		mutexwork.unlock();
		condwake.notify_one();
		pthread_join(thread, nullptr);
	}
	std::error_code code{};
	llvm::raw_fd_ostream output{
		std::string{nonconstructable.mainmodule.getName()} + ".bc", code },
		outputll{ std::string{nonconstructable.mainmodule.getName()} + ".ll",
				 code };
	if(!record.is_open()) {
		nonconstructable.mainmodule.print(outputll, nullptr);
		llvm::WriteBitcodeToFile(nonconstructable.mainmodule, output);
	}
	nonconstructable.mainmodule.~Module();
	delete pdatalayout;
}

DLL_EXPORT void unary(std::unordered_map<unsigned, std::string>&& hashes) {
	std::string imm;

	imm.assign(hashes["unop"_h]);

	auto phpriorhndlfn = phndl->getrestorefn();

	switch (stringhash(imm.c_str())) {
	case "-"_h:
	case "+"_h:
	case "!"_h:
		if (bIsBasicFloat(phndl->immidiates.back().type.front()))
			phndl->~basehndl(),
			phndl = dynamic_cast<basehndl*>(new (phndl) handlefpexpr{});
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

	phndl->~basehndl();

	phndl = phpriorhndlfn(phndl);
}

DLL_EXPORT void unaryincdec(std::unordered_map<unsigned, std::string>&& hashes) {
	std::string immpostfix = hashes["postfixarithraw"_h];

	std::string imm = !immpostfix.empty() ? immpostfix : hashes["prefixarithraw"_h];

	decltype(phndl->getrestorefn()) phpriorhndl = nullptr;//phndl->getrestorefn();

	if (bIsBasicFloat(phndl->immidiates.back().type.front()))
		phpriorhndl = phndl->getrestorefn(),
		phndl->~basehndl(),
		phndl = new (phndl) handlefpexpr{};

	unsigned int type = 3; // << 2 | 2;

	auto immlvalue = immidiates.back();

	immlvalue.originident.append("[[modified]]");

	insertinttoimm("1", sizeof "1" - 1, "", 0, type);

	if (immlvalue.type.front().uniontype != type::POINTER)

		immidiates.back() = convertTo(immidiates.back(), immlvalue.type);

	phndl->addlasttwovalues(imm == "--", false);

	immidiates.push_back(immlvalue);

	std::rotate(----immidiates.end(), --immidiates.end(), immidiates.end());

	phndl->assigntwovalues();

	if (!immpostfix.empty())
		immidiates.back() = immlvalue;
	if(phpriorhndl)
		phndl->~basehndl(),

		phndl = phpriorhndl(phndl);
}

DLL_EXPORT void binary(std::unordered_map<unsigned, std::string>&& hashes) {
	std::string imm;

	imm.assign(hashes["binoplast"_h]);

	auto phpriorhndl = phndl->getrestorefn();

	auto ops = std::array{ *----phndl->immidiates.end(), phndl->immidiates.back() };

	//phndl->usualarithmeticconversions(ops);

	if (bIsBasicFloat(ops[0].type.front()) || bIsBasicFloat(ops[1].type.front()))
		phndl->~basehndl(),
		phndl = dynamic_cast<basehndl*>(new (phndl) handlefpexpr{});

	//if(phndl->opbbs.back().second.pValue)
	//phndl->endlast(phndl->opbbs.back().first, phndl->opbbs.back().second);

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

	const char* inttoins = "";
	bool bislogicaland;

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
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SGT,
			 llvm::CmpInst::Predicate::ICMP_UGT, llvm::CmpInst::Predicate::FCMP_OGT);
		break;
	case "<"_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SLT,
			 llvm::CmpInst::Predicate::ICMP_ULT, llvm::CmpInst::Predicate::FCMP_OLT);
		break;
	case "<="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SLE,
			 llvm::CmpInst::Predicate::ICMP_ULE, llvm::CmpInst::Predicate::FCMP_OLE);
		break;
	case ">="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_SGE,
			 llvm::CmpInst::Predicate::ICMP_UGE, llvm::CmpInst::Predicate::FCMP_OGE);
		break;
	case "=="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_EQ,
			llvm::CmpInst::Predicate::ICMP_EQ, llvm::CmpInst::Predicate::FCMP_OEQ);
		break;
	case "!="_h:
		phndl->relatetwovalues(llvm::CmpInst::Predicate::ICMP_NE,
			llvm::CmpInst::Predicate::ICMP_NE, llvm::CmpInst::Predicate::FCMP_ONE);
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
		++nbranches.back().itersecond;
		inttoins = "0";
		bislogicaland = true;
		goto mainbranch;
	case "||"_h:
		++nbranches.back().itersecond;
		(*nbranches.back().itersecond)->first[0]->swapSuccessors();
		bislogicaland = false;
		inttoins = "1";
	mainbranch: {
		auto& currbranch = nbranches.back();
		auto refarr = *nbranches.back().itersecond;
		//refarr[1]->replaceAllUsesWith(refarr[0]);
		//refarr->first[0]->swapSuccessors();

		/*auto iter = nbranches.back().first.begin();

		std::advance(iter, nbranches.back().second.size());

		(*iter)->first[1] = (llvm::BranchInst*)"0";*/

		var& ordinary = *currbranch.iterval;

		//std::rotate(nbranches.back().second.begin(), --nbranches.back().second.end(), nbranches.back().second.end());
		//++nbranches.back().second;
		phndl->logictwovalues(bislogicaland);

		immidiates.back().lvalues.clear();

		immidiates.back().originident = "[[logicop]]";

		auto lastsplit = splitbb("", 0);

		int fullindex = refarr->first[0]->getSuccessor(1) != refarr->second;
		refarr->first[0]->setSuccessor(!fullindex, pcurrblock.back());
		refarr->second->eraseFromParent();
		//ifstatements.erase(branch);

		val ordinary_imm = ordinary;
		ordinary_imm.lvalues.push_back({ ordinary_imm.value, ordinary.type });

		immidiates.push_back(ordinary_imm);

		insertinttoimm(inttoins, 1, "", 0, 3);

		phndl->assigntwovalues();

		immidiates.pop_back();

		//auto iter = nbranches.back().first.begin();

		//if (nbranches.back().first.size() > 1)

			//std::advance(iter, nbranches.back().second.size());

		nbranches.back().second.push_back({ splitbb("", 0), lastsplit });

		lastsplit->setSuccessor(0, pcurrblock.back());

		auto itersecond = nbranches.back().itersecond;

		if (++itersecond == currbranch.first.end())
			currbranch.first.erase(++currbranch.first.begin(), currbranch.first.end()),
			currbranch.itersecond = currbranch.first.begin();
		//exhausted current logical ops
	//nbranches.back().second.pop_back();
		ifstatements.erase(refarr);
		}
	break;
	case "="_h:
		phndl->assigntwovalues();
		break;
	}

	//opbbs.pop_back ();
	phndl->~basehndl();
	phndl = phpriorhndl(phndl);
}

void printtype(llvm::Type* ptype) { // debug purpose only
	printtype(ptype, "");
}

// llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());
// std::cout << "base type of " << currdeclidentifier << " is typedef " <<
// !bwhich << " :" << spec << ", is declaring a typedef :" << bistypedef <<
// std::endl;

//bindings_parsing parsing;

//bindings_compiling compiling;

std::list<std::pair<std::string, void*>> recursion_list;

int lasttop = -1;

void* lastoffsetvector;

void print_tabs(int n) {
	while (n--) printf("\t");
}

void do_print_layour() {
	int n = 0;
	for (auto patt : recursion_list)
		print_tabs(n++), printf(" %s\n", patt.first.c_str());
}

//std::aligned_storage_t<sizeof(destr_clear_qualifsandtypes), alignof(destr_clear_qualifsandtypes)> unincompilingobj;

//auto pcompiling = new (&unincompilingobj) bindings_compiling{};
#if 0
DLL_EXPORT const char* callout_test(const char** pargs, size_t * szargs) {
	bindings_payload paylod = { .pargs = pargs, .szargs = szargs }; //= { (calloutinfo*)b, a };
	//struct calloutinfo* ptable = (calloutinfo*)b;

	static std::string res = "";

	//std::aligned_storage_t<sizeof(destr_clear_qualifsandtypes), alignof(destr_clear_qualifsandtypes)> uninparsingobj;

	//auto pbindings = new (&uninparsingobj) bindings_parsing{};

	//(bindings_payload&)*pbindings = paylod;
	(bindings_payload&)*phndl = paylod;

	int calloutnum = strtoull(std::string{ pargs[0], szargs[0] }.c_str(), nullptr, 10);

	//printf2("callout %d\n", a->callout_number);

#ifdef _WIN32
	__try {
#else
	try {
#endif
		//if (a->callout_number != 255)
		//(((parsing_plain&)*pbindings).lpVtbl->vtbl[calloutnum](pbindings, res)),
		(((compiling_plain&)*phndl).lpVtbl->vtbl[calloutnum - 1](phndl), 0);
	}
#ifdef _WIN32
	__except (EXCEPTION_EXECUTE_HANDLER) {
		//goto dump;
	}
#else
	catch (...) {
		goto dump;
	}
#endif
	//pbindings->cleanup();

	//if(a->capture_top > 1)
	//	printf2("%.*s\n", GROUP_SZ_AND_PTR_TRUNC(a->capture_top - 1));
	return res.c_str();
#if 0
	if (PATTERN_FLAGS & PCRE2_AUTO_CALLOUT) dump: {
		//do_print_layour();
		printf("%.*s @ %zu\n", a->next_item_length, ptable->pattern + a->pattern_position, a->pattern_position);

		std::string next_item{ (char*)ptable->pattern + a->pattern_position, a->next_item_length };

		/*if (lastoffsetvector != a->offset_vector)
			if (!recursion_list.empty() && recursion_list.back().second == a->offset_vector)
				recursion_list.pop_back();
			else
				recursion_list.push_back({ next_item, a->offset_vector });
		//else if (next_item == (")"))
			//recursion_list.pop_back();
			/*for (auto pattiter = recursion_list.begin(); pattiter != recursion_list.end(); ++pattiter)
			if (pattiter->second == a->capture_top) {
				recursion_list.erase(pattiter, recursion_list.end());
				break;
			}*/

			//lasttop = a->capture_top;

		lastoffsetvector = a->offset_vector;

		std::string subjtoprint{ (char*)a->subject + a->current_position, (a->subject_length - a->current_position) & 0xFF };

		std::replace_if(subjtoprint.begin(), subjtoprint.end(), isspace, ' ');

		printf("%.*s\n", (unsigned int)subjtoprint.length(), subjtoprint.c_str());

#if PATTERN_FLAGS & PCRE2_AUTO_CALLOUT
		static unsigned long long currregion;
		printf1p3("#region %llu capture %d\n", currregion, a->callout_number);
		currregion++;
		for (int n = 0; n < a->capture_top; ++n)
			subjtoprint.assign((char*)a->subject + a->offset_vector[2 * n],
				(unsigned int)(a->offset_vector[2 * n + 1] - a->offset_vector[2 * n])),
			std::replace_if(subjtoprint.begin(), subjtoprint.end(), isspace, ' '),
			printf3(
				"%s %d - %s\n",
				getnameloc(-n, *ptable),
				n,
				subjtoprint.c_str());
		printf1p3("#endregion\n", 0);
#else
		__debugbreak();
#endif
	}
	return res;
#endif
	}
#endif

#if 0
virtual void escape_1() {
	//int n = getnameloc("escaperaw", *ptable);
	addescapesequencetostring(FIRST_ARG_PTR_AND_SZ);

}
virtual void text_2() {
	//int n = getnameloc("textraw", *ptable);
	addplaintexttostring(FIRST_ARG_PTR_AND_SZ);

}
virtual void unused_3() { };
virtual void start_str_4() {
	//int n = getnameloc2("begincharliteral", *ptable, a, 0);
	bischarlit = this->szargs[1];//n != -1 && a->offset_vector[n * 2] != -1;
	//a->offset_vector[n * 2] = a->offset_vector[n * 2 + 1] = -1;
}
#endif
DLL_EXPORT void num_lit(std::unordered_map<unsigned, std::string> &hashes) {
	unsigned int type;
	//int n = getnameloc("numberliteralraw", *ptable);

	//ntoprint[0] = getnameloc("uns", *ptable);

	//int ntoprint[2], ntoclear;

	//ntoprint[1] = getnameloc2("lng", *ptable,a,0);

	std::string groups[] = { "hex", "bin", "oct", "dec" };

	//for (const char** pgroup = groups; pgroup != 1[&groups]; ++pgroup)
	for (int i = 0; i < std::extent< decltype( groups ) >::value; ++i)
	{
		//ntoclear = getnameloc3(*pgroup, *ptable, a, 0, { .dontsearchforclosest = 0 });
		//if (ntoclear != -1 && a->offset_vector[2 * ntoclear] != -1)
		std::string curr = hashes[stringhash(groups[i].c_str())];
		if (!curr.empty())
		{
			type = i; //pgroup - groups; //<< 2;//| (a->offset_vector[2 * ntoprint[0]] != -1) | (a->offset_vector[2 * ntoprint[1]] != -1) << 1;

			insertinttoimm(STRING_TO_PTR_AND_SZ(curr), STRING_TO_PTR_AND_SZ(hashes["lng"_h]), type);

			//a->offset_vector[2 * ntoclear] = a->offset_vector[2 * ntoclear + 1] = -1;

			break;
		}
	}

}
#if 0
virtual void ident_6() {
	//int n = getnameloc("ident", *ptable);
	obtainvalbyidentifier({ FIRST_ARG_PTR_AND_SZ });

}
virtual void start_fn_call_7() { startfunctioncall(); }
virtual void member_access_op_8() {
	//int n = getnameloc("ident", *ptable), ntoprint[2];
	//ntoprint[1] = getnameloc("arrowordotraw", *ptable) + 1;
	memberaccess(PTR_AND_SZ_N(2), PTR_AND_SZ_N(1));//(char*)GROUP_PTR_AND_SZ(ntoprint[1]), (char*)GROUP_PTR_AND_SZ(n + 1));

}
virtual void postfix_arith_9() {
	//int n = getnameloc("postfixarithraw", *ptable);
	unaryincdec(FIRST_ARG_PTR_AND_SZ, true);

}
virtual void prefix_arith_10() {
	//int n = getnameloc("prefixarithraw", *ptable);
	unaryincdec(FIRST_ARG_PTR_AND_SZ, false);

}
virtual void decl_ptr_11() {
	//int n = (getnameloc2("qualif", *ptable, a, 1));

	//n { };

	addptrtotype(FIRST_ARG_PTR_AND_SZ);

}
virtual void decl_subs_12() {  }
virtual void end_fn_call_13() { endfunctioncall(); }
virtual void unexplored_end_sizeof_14() {  }
virtual void begin_sizeof_15() {  }
virtual void unused_16() { };
virtual void unused_17() { };
virtual void unused_18() { };
virtual void unary_op_19() {
	//int n = getnameloc("unop", *ptable);
	unary(FIRST_ARG_PTR_AND_SZ);//(char*)GROUP_PTR_AND_SZ(n + 1));

}

virtual void or_logic_op_29() {
	//int n = getnameloc3("binoplast", *ptable, a, 1, { .rev = 0, .last = 0, .dontsearchforclosest = 0, });
	//if (n != -1 && a->offset_vector[2 * (n + 1)] != -1)
	binary(FIRST_ARG_PTR_AND_SZ);//(char*)GROUP_PTR_AND_SZ(n + 1));
	//ntoclearauto[0] = n + 1;

}
virtual void assign_op_30() {
	//BINARY_OP("assignopraw");
	binary(FIRST_ARG_PTR_AND_SZ);//(char*)GROUP_PTR_AND_SZ(n + 1));

}
virtual void unused_31() { };
virtual void ternary_true_32() {  }
virtual void ternary_33() {  }
virtual void ternary_false_34() {  }
virtual void comma_op_35() {  }
virtual void unused_36() { };
virtual void unused_37() { };
/*void handledeclident(const std::string contentstr) {[
	if (contentstr.empty()) return;
	//int n = getnameloc2("typedefkeyword", *ptable, a, 0);

	auto c_strcontentstr = contentstr.c_str();

	if (auto pvar = obtainvalbyidentifier(contentstr, false))
		if (pvar->type.back().spec.basicdeclspec.basic[2] == "typedef")
			if (currdeclspectypedef.empty()) currdeclspectypedef = contentstr; else goto adddecl;
		else;
	else adddecl: adddeclarationident(contentstr.c_str(), contentstr.size(), false);//a->offset_vector[2 * n] != -1);
}*/
virtual void identifier_typedef_38() {
	//int n = getnameloc("identifierminetypedef", *ptable) + 1;

	//currdeclspectypedef = { (char*)GROUP_PTR_AND_SZ(n) };

	//handledeclident({ (char*)GROUP_PTR_AND_SZ(n) });
}
#endif
void expandtype(std::list<::type> typein, std::list<::type> &typeout) {
	if (typein.back().spec.basicdeclspec.istypedef()) {
		auto var = obtainvalbyidentifier(typein.back().spec.basicdeclspec.basic[3], false, true);
		expandtype(var->type, typeout);
		typeout.insert(typeout.begin(), typein.begin(), --typein.end());
	} else typeout.insert(typeout.begin(), typein.begin(), typein.end());
}

/*DLL_EXPORT void add_typedef_to_decl(std::unordered_map<unsigned, std::string>&& hashes) {
	currtypevectorbeingbuild.back().p->back().type.front().spec.basicdeclspec.basic[3] = hashes["typedefnmmatched"_h];
}*/

DLL_EXPORT void identifier_decl(std::unordered_map<unsigned, std::string> && hashes) {
	//int n = getnameloc("ident", *ptable) + 1;

	//handledeclident({ (char*)GROUP_PTR_AND_SZ(n) });
	//adddeclarationident((char*)GROUP_PTR_AND_SZ(n), false);

	//currtypevectorbeingbuild.back().p->back().identifier = { FIRST_ARG_PTR_AND_SZ };//(char*)GROUP_PTR_AND_SZ(n) };

	var var;

	type basic{ type::BASIC };

	basic.spec.basicdeclspec.basic[3] = hashes["typedefnmmatched"_h];;

	//basic.spec.basicdeclspec.basic[0] = hashes["structorunionlast"_h];

	//basic.spec.basicdeclspec.basic[3] = hashes["identlasttag"_h];

	//if (basic.spec.basicdeclspec.basic[3].empty())

	var.identifier = hashes["ident"_h];

	var.type = { basic };

	var.linkage = hashes["typedefkey"_h];

	var.firstintroduced = scopevar.size();

	/*basic.spec.basicdeclspec.basic[3] = hashes["typedefnmmatched"_h];

	

	if (const char* pidentraw = var.identifier.c_str())
	{
	}*/

	currtypevectorbeingbuild.back().p->push_back(var);
}
#if 0
virtual void start_constant_expr_40() {
	beginconstantexpr();

}
virtual void end_decl_sub_41() {
	addsubtotype();
	endconstantexpr();

}
virtual void end_full_decl_42() {
	//isinsidedecl = false;
	//typedefname[0] = typedefname[1] = -1;
	finalizedeclaration();

}
virtual void extend_decl_43() {
	continuedeclaration();

}
virtual void start_new_scope_44() {
	//szscopeleveltoadd++;
	//addtypedefsscope();
	beginscope();

}
virtual void end_of_scope_45() {
	endscope();

}
virtual void unexplored_46() {  }
virtual void begin_param_list_47() {
	startfunctionparamdecl();

}
virtual void end_param_list_48() {
	//int n = getnameloc3("rest", *ptable, a, 0, { .dontsearchforclosest = 0 });
	endfunctionparamdecl(this->szargs[1]);

}
#endif
DLL_EXPORT void add_type(std::unordered_map<unsigned, std::string>&hashes) {
	auto& lastvar = currtypevectorbeingbuild.back().p->back();
	parsebasictype({ hashes["ident"_h] }, lastvar.type.back().spec.basicdeclspec);
}

DLL_EXPORT void add_qualif(std::unordered_map<unsigned, std::string>&hashes) {
	auto& lastvar = currtypevectorbeingbuild.back().p->back();
	lastvar.linkage = hashes["storageclass"_h];
}

DLL_EXPORT void add_tag(std::unordered_map<unsigned, std::string>&hashes) {
	auto& lastvar = currtypevectorbeingbuild.back().p->back();
	lastvar.type.back().spec.basicdeclspec.basic[0] = hashes["structorunionlast"_h];
	if (hashes["lasttag"_h].empty())
		lastvar.type.back().spec.basicdeclspec.pexternaldata = &structorunionmembers.back().back();
	else lastvar.type.back().spec.basicdeclspec.basic[3] = hashes["lasttag"_h];
}

DLL_EXPORT void enddeclaration(std::unordered_map<unsigned, std::string>&hashes) {
	auto& lastvar = currtypevectorbeingbuild.back().p->back();

	std::rotate(lastvar.type.begin(), ++lastvar.type.begin(), lastvar.type.end());

	auto lasttype = lastvar.type;

	//lastvar.type.clear();

	//expandtype(lasttype, lastvar.type);

	//lastvar.firstintroduced = scopevar.size();

	//currtypevectorbeingbuild.pop_back();
}
//virtual void unused_50() { };
DLL_EXPORT void add_literal(std::unordered_map<unsigned, std::string> &hashes) {
	if (hashes["begincharliteral"_h] == "\"") constructstring();
	else {
		std::stringstream ssstream;
		ssstream << (int)currstring[0];
		insertinttoimm(ssstream.str().c_str(), ssstream.str().size(), "", 0, 3);
		currstring = "";
	}
}
#if 0
virtual void finish_return_statement_52() {
	endreturn();

}
virtual void finish_statement_53() {
	endexpression();

}
virtual void subscript_op_54() {
	subscript();

}
virtual void decl_begin_55() {
	//int n = getnameloc3("typedefnmmatched", *ptable, a, 0, { .rev = 0, .last = 0, .dontsearchforclosest = 0, });

	//std::string identtypedef{ (char*)GROUP_PTR_AND_SZ(n) };
	startdeclaration({ FIRST_ARG_PTR_AND_SZ });

}
virtual void end_of_sizeof_56() {
	endsizeofexpr();

}
virtual void end_of_sizeof_tp_nm_57() {
	endsizeoftypename();

}
virtual void ident_struc_58() {
	const char* namedcapture;
	int ntoprint[2];
	//int n = getnameloc(namedcapture = "ident", *ptable);
	//ntoprint[1] = getnameloc(namedcapture = "structorunionlast", *ptable) + 1;
	currstruct.first = { PTR_AND_SZ_N(1) };
	currstruct.second = { PTR_AND_SZ_N(2) };


}
#endif
DLL_EXPORT void struc_or_union_body(std::unordered_map<unsigned, std::string> &hashes) {
	//auto& lastvar = currtypevectorbeingbuild.back().p->back();

	var tmp;
	type typestruct{ type::BASIC };
	typestruct.spec.basicdeclspec.basic[3] = hashes["lasttag"_h];
	typestruct.spec.basicdeclspec.basic[0] = hashes["structorunionlast"_h];
	tmp.type.push_back(typestruct);
	tmp.identifier = hashes["lasttag"_h];
	tmp.pllvmtype = llvm::StructType::create(llvmctx);
	structorunionmembers.back().push_back({ tmp });
	currtypevectorbeingbuild.push_back(
		{ --structorunionmembers.back().end(), currdecltypeenum::PARAMS });
}
#if 0
virtual void struc_or_union_body_end_60() {
	endbuildingstructorunion();

}
virtual void perform_explicit_conversion_61() {
	applycast();

}
virtual void create_label_62() {
	//const char* namedcapture;
	//int n = getnameloc(namedcapture = "lbl", *ptable);
	splitbb(FIRST_ARG_PTR_AND_SZ);//(char*)GROUP_PTR_AND_SZ(n + 1));

}
virtual void goto_stmt_63() {
	//const char* namedcapture;
	//int n = getnameloc(namedcapture = "gtid", *ptable);
	gotolabel(FIRST_ARG_PTR_AND_SZ);//(char*)GROUP_PTR_AND_SZ(n + 1));

}
virtual void switch_stmt_64() { startswitch(); }
virtual void create_case_65() {
	addCase();
	endconstantexpr();
}
virtual void switch_stmt_end_66() { endswitch(); }
virtual void create_default_case_67() { addDefaultCase(); }
#endif
DLL_EXPORT void collect_float_literal(std::unordered_map<unsigned, std::string> &hashes) {
	std::string wholepart, fractionpart, exponent, exponent_sign;
	std::string ntoclear;
	if ((ntoclear = hashes["wholeopt"_h]).empty())
		if ((ntoclear = hashes["whole"_h]).empty())
			if ((ntoclear = hashes["wholenodot"_h]).empty())
				goto rest;

	if (wholepart.empty())
		wholepart = ntoclear;
rest:
	ntoclear = hashes["fraction"_h];//getnameloc2("fraction", *ptable, a, 0);

	if (fractionpart.empty())
		fractionpart = ntoclear;

	ntoclear = hashes["signexp"_h];

	if (exponent_sign.empty())
		exponent_sign = ntoclear;

	ntoclear = hashes["exp"_h];

	if (exponent.empty())
		exponent = ntoclear;
	std::list<::type> currtype = { 1, ::type::BASIC };

	llvm::Type* pllvmtype;

	std::string postfix = hashes["suffixflt"_h];

	const llvm::fltSemantics& floatsem = postfix.empty() ? currtype.back().spec.basicdeclspec.basic[1] = "double",
		pllvmtype = llvm::Type::getDoubleTy(llvmctx),
		llvm::APFloatBase::IEEEdouble() : ranges::contains(std::array{ "f", "F" }, postfix) ? currtype.back().spec.basicdeclspec.basic[1] = "float",
		pllvmtype = llvm::Type::getFloatTy(llvmctx),
		llvm::APFloatBase::IEEEsingle() : (assert(ranges::contains(std::array{ "l", "L" }, postfix)), currtype.back().spec.basicdeclspec.basic[1] = "double", currtype.back().spec.basicdeclspec.longspecsn = 1, pllvmtype = llvm::Type::getFP128Ty(llvmctx), llvm::APFloatBase::IEEEquad());

	llvm::APFloat floatlit{ floatsem };

	std::string finalnumber = wholepart + "." + fractionpart;

	if (!exponent.empty())
		finalnumber += "E" + exponent_sign + exponent;

	auto status = floatlit.convertFromString(finalnumber, llvm::APFloatBase::rmNearestTiesToEven);

	immidiates.push_back({ llvm::ConstantFP::get(pllvmtype, floatlit), currtype });
}
#if 0
virtual void finish_float_literal_69() {
	//int ntoclear = getnameloc2("flt", *ptable, a, 0);

	insertfloattoimm(FIRST_ARG_PTR_AND_SZ, (char*)STRING_TO_PTR_AND_SZ(fltlitctx.wholepart),
		(char*)STRING_TO_PTR_AND_SZ(fltlitctx.fractionpart),
		(char*)STRING_TO_PTR_AND_SZ(fltlitctx.exponent),
		(char*)STRING_TO_PTR_AND_SZ(fltlitctx.exponent_sign));

	fltlitctx.~decltype(fltlitctx)();
	new (&fltlitctx) decltype(fltlitctx)();
}
virtual void begin_if_stm_70() { startifstatement(); }
virtual void cnt_if_stm_71() { continueifstatement(); }
virtual void finish_if_stm_72() { endifstatement(); }
virtual void begin_loop_73() { startforloopcond(); }
virtual void loop_iter_74() { addforloopiter(); }
virtual void finish_loop_75() { endforloop(); }
virtual void cond_loop_76() { endforloopcond(); }
virtual void begin_do_while_77() { startdowhileloop(); }
virtual void finish_do_while_78() { enddowhileloop(); }
virtual void finish_continue_stm_79() { addcontinue(); }
virtual void finish_break_stm_80() { addbreak(); }
virtual void register_calling_conv_81() { }
virtual void unexplored_82() { }
#endif
DLL_EXPORT void end_binary() {
	phndl->end_binary();
}
DLL_EXPORT void begin_branch() {
	phndl->begin_branch();
}
DLL_EXPORT void begin_binary() {
	phndl->begin_binary();
}
#if 0
virtual void unused_86() { }
virtual void begin_nested_expr_87() { }
virtual void end_nested_expr_88() { }
#endif
DLL_EXPORT void add_ident_to_enum_def(std::unordered_map<unsigned, std::string> &hashes) {
	var tmp;
	type enumtype{ type::BASIC };
	enumtype.spec.basicdeclspec.basic = { "enum", "int", "", enums.back().back().ident };

	tmp.type.push_back(enumtype);

	//tmp.pllvmtype = createllvmtype(tmp.type);

	//int n = getnameloc3("identlast", *ptable, a, 1, { .dontsearchforclosest = 0 }) + 1;

	tmp.identifier = hashes["identlasttag"_h];//(char*)GROUP_PTR_AND_SZ(n) };

	scopevar.back().push_back(tmp);

	enums.back().back().memberconstants.push_back(--scopevar.back().end());
}
DLL_EXPORT void begin_enumerator_def(std::unordered_map<unsigned, std::string> && hashes) {
	//begin_enumerator_decl(pargs, szargs);
	enums.back().push_back({ hashes["identlasttag"_h], {} });
	//add_tag(hashes);
}
/*DLL_EXPORT void begin_enumerator_decl(const char** pargs, size_t* szargs) {
	//int n = getnameloc3("identlast", *ptable, a, 1, { .dontsearchforclosest = 0 }) + 1;
	currenum = { FIRST_ARG_PTR_AND_SZ };
}*/
DLL_EXPORT void end_ass_to_enum_def() {
	enums.back().back().memberconstants.back()->constant = immidiates.back().constant;
	enums.back().back().maxcount = dyn_cast<llvm::ConstantInt>(enums.back().back().memberconstants.back()->constant)->getSExtValue() + 1;
	immidiates.pop_back();
	endconstantexpr();
}
DLL_EXPORT void end_without_ass_to_enum_def() {
	enums.back().back().memberconstants.back()->constant = llvm::ConstantInt::get(llvmctx, llvm::APInt(32, enums.back().back().maxcount++));
}
#if 0
virtual void begin_unnamed_enum_def_94() {
	enums.back().push_back({ {}, {} });
}
virtual void end_expr_init_95() {
	auto val = immidiates.back();
	immidiates.pop_back();
	obtainvalbyidentifier(scopevar.back().back().identifier);
	immidiates.push_back(val);
	assigntwovalues();
	immidiates.pop_back();
}
virtual llvm::Value* assigntwovalues() = 0;

static std::list<val>& immidiates;

std::list<val>& bindings_compiling::immidiates = ::immidiates;

#endif


extern "C" {
#include <EXTERN.h> /* from the Perl distribution     */
#include <perl.h>	/* from the Perl distribution     */
#include <XSUB.h>
}

DLL_EXPORT void docall(const char*, size_t, void*);

void call(std::unordered_map<unsigned, std::string> hash, std::string callname) {
	//SvREFCNT_dec(hash);

	docall(callname.c_str(), callname.length(), &hash);

	//SvREFCNT_dec(hash);
}

std::queue<std::pair<std::unordered_map<unsigned, std::string>, std::string>> callstack;

extern "C" PerlInterpreter* my_perl;

extern "C" void *wait_for_call(void*) {
	do {
		std::unique_lock lock{mutexwork};
		condwake.wait(lock, []{return !callstack.empty() || endwork;});

		while(!callstack.empty()) {
			auto callinfo = callstack.front();
			callstack.pop();
			lock.unlock();
			call(callinfo.first, callinfo.second);
			lock.lock();
		}

	} while(!endwork);
	return nullptr;
}

static std::list<std::pair<std::unordered_map<unsigned, std::string>, std::string>> recordstack;

DLL_EXPORT void do_callout(SV * in, HV * hash)
{

	char* key, *pinstr;
	I32 key_length;
	STRLEN inlen;
	SV* value;
	hv_iterinit(hash);
	const char nill[1] = {'\0'};
	std::unordered_map<unsigned, std::string> map;
	while (value = hv_iternextsv(hash, &key, &key_length)) {
		pinstr = SvPVutf8(value, inlen);
		if(!inlen) continue;
		if(record.is_open() && recordstack.empty()) {
			record.write(pinstr, inlen);
			record.write(nill, 1);
			record.write(reinterpret_cast<const char(&)[4]>(std::move(stringhash(std::string{key, (size_t)key_length}.c_str()))), 4);
		} else {
			map.insert({ stringhash(std::string{key, (size_t)key_length}.c_str()), std::string{pinstr, inlen} });
		}
	}

	pinstr = SvPVutf8(in, inlen);

	if(!recordstack.empty()) {
		recordstack.push_back({map, std::string{pinstr, inlen}});
	} else if(record.is_open()) {

		record.write(nill, 1);
		record.write(pinstr, inlen);
		record.write(nill, 1);
	} else {
		if(getenv("THREADING")) {
			mutexwork.lock();

			callstack.push({map, std::string{pinstr, inlen}});

			mutexwork.unlock();

			condwake.notify_one();
		} else {
			docall(pinstr, inlen, &map);
		}
	}
}

using keys = std::unordered_map<unsigned, std::string>;

std::list<std::pair<unsigned, std::variant<keys, struct metatypeiter>>> regexmeta(1);

struct metatypeiter : decltype(regexmeta)::iterator {};

DLL_EXPORT void dostartmetaregex(SV* in, AV* hashes) {
	STRLEN inlen;
	const char *pmodulenmae = SvPVutf8(in, inlen);
	startmodule(pmodulenmae, inlen);
	HV *pavelem;

	size_t arraycount = av_len(hashes);

	auto iterregexmeta = regexmeta.begin();

	for(size_t i : ranges::views::iota(0uz, arraycount)) {
		hv_iterinit(pavelem = (HV*)SvRV(*av_fetch(hashes, i, 1)));
		SV* value;
		char* key;
		I32 key_length;
		const char*pinstr;

		decltype(regexmeta)::value_type val;

		value = hv_iternextsv(pavelem, &key, &key_length);

		val.first = stringhash(std::string{key, (size_t)key_length}.c_str());

		decltype(val)::second_type valmap;

		if(std::string{sv_reftype(value, 0)} != "HASH") {
			pinstr = SvPVutf8(value, inlen);
			long index = atoll(std::string{pinstr, inlen}.c_str());

			regexmeta.resize(index + 1);

			valmap = metatypeiter{regexmeta.begin()};

			std::advance(std::get<metatypeiter>(valmap), index);
			
		} else {

			SV* valueinner;

			hv_iterinit(pavelem = (HV*)SvRV(value));

			valmap = keys{};

			while(valueinner = hv_iternextsv(pavelem, &key, &key_length)) {
				pinstr = SvPVutf8(valueinner, inlen);
				std::get<keys>(valmap).insert({stringhash(std::string{key, (size_t)key_length}.c_str()), std::string{pinstr, inlen}});
			}
		}

		val.second = valmap;

		if(regexmeta.size() < i + 1)
			regexmeta.resize(i + 2);

		*iterregexmeta++ = val;
	}
}

DLL_EXPORT void startrecording() {
	recordstack.push_back({});
}

DLL_EXPORT void stoprecording() {
	recordstack.pop_back();
}

struct reggroup {
	std::string name;
};

static std::list<reggroup> groups;

static void parsegroup(decltype(recordstack.front()) groupbegin) {
	groups.push_back({});
}

extern "C" int secondmain(char *subject, size_t szsubject, char *pattern,
	 size_t szpattern, char *modulename, size_t szmodulename,
	 char *entrypoint, size_t szentrypoint) {
	
	std::string entry{entrypoint, szentrypoint};

	auto iterentry = std::find_if(recordstack.begin(), recordstack.end(), [&](decltype(recordstack.back()) &elem) {
		return elem.second == entry;
	});


	


	//startmodule(modulename, szmodulename);
	return 0;
}