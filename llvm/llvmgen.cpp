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

struct basehndl
{
    virtual void mlutiplylasttwovalues()
    {
    }

    virtual void dividelasttwovalues()
    {
    }

    virtual void modulolasttwovalues()
    {
    }

    virtual void addlasttwovalues(bool bminus)
    {
    }

    virtual void shifttwovalues(bool bright)
    {
    }

    virtual void bitwisetwovalues(llvm::Constant* op(llvm::Constant*, llvm::Constant*))
    {
    }

    virtual void relatetwovalues(llvm::CmpInst::Predicate pred)
    {
    }

    virtual void logictwovalues(bool bisand)
    {
    }

    virtual void insertinttoimm(const char* str, size_t szstr)
    {
        std::string imm;

        imm.assign(str, szstr);

        std::istringstream in{ imm };

        uint64_t val;

        in >> val;

        immidiates.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvmctx), val));
    }

    std::vector<llvm::Value*> immidiates;
};

struct handlecnstexpr : basehndl
{
    auto getops()
    {
        return std::array{ immidiates.front(), immidiates.back() };
    }

    virtual void mlutiplylasttwovalues()
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getMul(ops[0], ops[1]));
    }

    virtual void dividelasttwovalues()
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getSDiv(ops[0], ops[1]));
    }

    virtual void modulolasttwovalues()
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getSRem(ops[0], ops[1]));
    }

    virtual void addlasttwovalues(bool bminus)
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getAdd(ops[0],
            !bminus ? ops[1] : llvm::ConstantExpr::getNeg(ops[1])));
    }

    virtual void shifttwovalues(bool bright)
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(
            (!bright ? llvm::ConstantExpr::getLShr
                : llvm::ConstantExpr::getAShr)(ops[0], ops[1], false));
    }

    virtual void relatetwovalues(llvm::CmpInst::Predicate pred)
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getICmp(pred, ops[0], ops[1]));
    }

    virtual void bitwisetwovalues(llvm::Constant* op(llvm::Constant*, llvm::Constant*))
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(op(ops[0], ops[1]));
    }

    virtual void logictwovalues(bool bisand)
    {
        std::array ops = getops();
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        const auto valzero = llvm::ConstantInt::getIntegerValue(llvm::Type::getInt64Ty(llvmctx), llvm::APInt{});

        immidiates.push_back(
            (!bisand ? llvm::ConstantExpr::getOr : llvm::ConstantExpr::getAnd)(
                llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[0], valzero),
                llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[1], valzero)));
    }

    virtual void insertinttoimm(const char* str, size_t szstr)
    {
        std::string imm;

        imm.assign(str, szstr);

        std::istringstream in{ imm };

        uint64_t val;

        in >> val;

        immidiates.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvmctx), val));
    }

    std::vector<llvm::Constant*> immidiates;
};

static handlecnstexpr hndlcnstexpr{};

static basehndl hndlbase{};

static basehndl* phndl = &hndlbase;

static std::string currdeclidentifier{};

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

    type(typetype a) : spec(a) {};

    typedef uint64_t arraytype;
    struct functype {
        std::vector<var> parametertypes;
        bool bisvariadic;
    };

    type(const type& a) : spec(a.uniontype) {

        *this = a;
    }

    ~type()
    {
        std::array lambdas = { std::function{[&] {spec.basicdeclspec.~bascitypespec();}},
                                std::function{[&] {spec.ptrqualifiers.~bitset();}},
                                std::function{[&] {}},
                                std::function{[&] {spec.func.~functype();}} };
        lambdas[uniontype]();
    }

    type& operator=(const type& type)
    {
        std::array lambdas = { std::function{[&] {spec.basicdeclspec = type.spec.basicdeclspec;}},
                               std::function{[&] {spec.ptrqualifiers = type.spec.ptrqualifiers;}},
                               std::function{[&] {spec.arraysz = type.spec.arraysz;}},
                               std::function{[&] {spec.func = type.spec.func;}} };
        lambdas[uniontype]();

        return *this;
    }

    union typespec {
        typespec(typetype type)
        {
            std::array lambdas = { std::function{[&] {new (&basicdeclspec) bascitypespec{};}},
                                std::function{[&] {new (&ptrqualifiers) pointrtypequalifiers{};}},
                                std::function{[&] {arraysz = 0;}},
                                std::function{[&] {new (&func) functype{};}} };
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
static std::vector<std::vector<var>> scopevar{ 1 };

static var currdecltypevector{};

static std::vector<var*> currtypevectorbeingbuild{ 1, &currdecltypevector };

static std::string currstring;

extern "C" void obtainvalbyidentifier(const char* identifier, size_t szstr)
{
    std::string ident{ identifier, szstr };

    hndlbase.immidiates.push_back(nonconstructable.mainmodule.getFunction(identifier));
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
    hndlbase.immidiates.push_back(llvmbuilder.CreateGlobalStringPtr(currstring));
}

void begindeclarationifnone()
{
    if (currdecltype != currdecltypeenum::NONE) return;

    if (!currtypevectorbeingbuild.back()->type.size() ||
        currtypevectorbeingbuild.back()->type.back().uniontype != type::FUNCTION)
    {
        currdecltype = currdecltypeenum::UNKNOWN;
    add_basic_type:
        {
            type basic{ type::BASIC };

            if (qualifsandtypes.empty())
                basic.spec.basicdeclspec.basic[1] = "int";
            else basic.spec.basicdeclspec = parsebasictype(::qualifsandtypes);

            basic.spec.basicdeclspec.basic[3] = ::currdeclspectypedef;

            currtypevectorbeingbuild.back()->type.push_back(basic);
        }
    }
    else
    {
        currdecltype = currdecltypeenum::PARAMS;
        currtypevectorbeingbuild.back()->type.back().spec.func.parametertypes.push_back({});
        currtypevectorbeingbuild.push_back(&currtypevectorbeingbuild.back()->type.back().spec.func.parametertypes.back());

        goto add_basic_type;
    }
}

void addvar()
{
    scopevar.back().push_back(currdecltypevector);

    currdecltypevector.~var();

    new (&currdecltypevector) var{};

    const var& lastvar = scopevar.back().back();

    const char* lastvartypestoragespec = lastvar.type.back().spec.basicdeclspec.basic[2].c_str();

    switch (scopevar.size())
    {
    case 1:
        llvm::GlobalValue::LinkageTypes linkagetype;

        switch (stringhash(lastvartypestoragespec))
        {
        default:
        case "extern"_h:
            linkagetype = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
        case "static"_h:
            linkagetype = llvm::GlobalValue::LinkageTypes::InternalLinkage;
        }

        switch (lastvar.type[0].uniontype)
        {
        case type::POINTER:
        case type::ARRAY:
        case type::BASIC:
            globals.push_back(std::make_unique<llvm::GlobalVariable>(lastvar.pllvmtype, false, linkagetype, nullptr, lastvar.identifier));
        case type::FUNCTION:
            funcs.push_back(llvm::Function::Create(llvm::dyn_cast<llvm::FunctionType>(lastvar.pllvmtype), linkagetype, lastvar.identifier, nonconstructable.mainmodule));
            break;
        }
        break;
    default:
        break;
    }
}

extern "C" void startdeclaration(const char* str, size_t szstr, int isinsidedecl, bool bistypedef)
{
    begindeclarationifnone();

    currdeclidentifier.assign(str, szstr);

    if (!isinsidedecl && bistypedef) currdecltype = currdecltypeenum::TYPEDEF;
}

llvm::Type* createllvmtype(std::vector<type> decltypevector)
{
    llvm::Type* pcurrtype;

    std::array lambdas = { std::function{[&](const type& type) {
            switch (stringhash(type.spec.basicdeclspec.basic[2].c_str()))
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
            pcurrtype = dyn_cast<llvm::Type>(llvm::FunctionType::get(pcurrtype, paramtype, type.spec.func.bisvariadic ));
        }} };

    std::reverse(decltypevector.begin(), decltypevector.end());

    for (const auto& type : decltypevector)
        lambdas[type.uniontype](type);

    return pcurrtype;
}

static std::vector<llvm::BasicBlock*> pcurrblock;

extern "C" void beginscope()
{
    if (scopevar.size() == 1)
        currfunc = funcs.back();

    pcurrblock.push_back(llvm::BasicBlock::Create(llvmctx, "", currfunc));
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

    pcurrblock.push_back(llvm::BasicBlock::Create(llvmctx, "", currfunc));

    llvm::Value* callee = hndlbase.immidiates.front();

    hndlbase.immidiates.erase(hndlbase.immidiates.begin());

    lastblock->getInstList().push_back(
        llvm::dyn_cast<llvm::Instruction>
        (llvmbuilder.CreateInvoke(llvm::FunctionCallee{ dyn_cast<llvm::FunctionType>(hndlbase.immidiates.front()->getType()), hndlbase.immidiates.front() }, pcurrblock.back(), nullptr, hndlbase.immidiates)));
}

extern "C" void endfunctionparamdecl(bool bisrest)
{
    currtypevectorbeingbuild.pop_back();
    currtypevectorbeingbuild.back()->type.back().spec.func.bisvariadic = bisrest;
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr);

extern "C" void startfunctionparamdecl()
{
    begindeclarationifnone();

    if (currtypevectorbeingbuild.size() > 1) //if declaring a function inside another declaration
        addptrtotype("", 0);
    //currdecltype = currdecltypeenum::PARAMS;
    currtypevectorbeingbuild.back()->type.push_back({ type::FUNCTION });
}

extern "C" void finalizedeclarationtypename()
{
    begindeclarationifnone();

    currtypevectorbeingbuild.back()->pllvmtype = createllvmtype(currtypevectorbeingbuild.back()->type);

    currdecltype = currdecltypeenum::NONE;
    currdeclspectypedef = "";
}

extern "C" void addsubtotype()
{
    begindeclarationifnone();

    type arraytype{ type::ARRAY };

    auto res = llvm::dyn_cast<llvm::ConstantInt>(hndlcnstexpr.immidiates.back());

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

extern "C" void insertinttoimm(const char* str, size_t szstr) { phndl->insertinttoimm(str, szstr); }

extern "C" void beginconstantexpr()
{
    phndl = &hndlcnstexpr;
}

extern "C" void endconstantexpr()
{
    assert(hndlcnstexpr.immidiates.size() == 1);

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
    llvm::raw_fd_ostream output{std::string{nonconstructable.mainmodule.getName()} + ".bc", code};
    llvm::WriteBitcodeToFile(nonconstructable.mainmodule, output);
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
        phndl->bitwisetwovalues(llvm::ConstantExpr::getAnd); break;
    case "^"_h:
        phndl->bitwisetwovalues(llvm::ConstantExpr::getXor); break;
    case "|"_h:
        phndl->bitwisetwovalues(llvm::ConstantExpr::getOr); break;
    case "&&"_h:
        phndl->logictwovalues(true); break;
    case "||"_h:
        phndl->logictwovalues(false); break;
    }
}

//llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());
//std::cout << "base type of " << currdeclidentifier << " is typedef " << !bwhich << " :" << spec << ", is declaring a typedef :" << bistypedef << std::endl;