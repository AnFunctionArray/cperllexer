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

static std::vector<llvm::Function> funcs{};

static std::vector<llvm::GlobalVariable> globals{};

static std::vector<llvm::Constant*> immidiates;

static llvm::LLVMContext llvmctx;

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
    }
};

auto getops()
{
    return std::array{ immidiates.front(), immidiates.back() };
}

struct handlecnstexpr : basehndl
{
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
};

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

static std::vector<std::string> currdeclspec;

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
}
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
        ret[0] = spec; break;
    case "int"_h:
    case "long"_h:
    case "short"_h:
    case "char"_h:
    case "float"_h:
    case "double"_h:
        ret[1] = spec; break;
    case "static"_h:
    case "extern"_h:
    case "auto"_h:
        ret[2] = spec; break;
    case "const"_h:
        ret.qualifiers[0] = 1; break;
    case "restrict"_h:
        ret.qualifiers[1] = 1; break;
    case "volatile"_h:
        ret.qualifiers[2] = 1;
        break;
    default:
        std::cerr << "invalid specifier: " << spec << std::endl;
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
    typedef std::vector<var> functype;

    type(const type& a) : spec(a.uniontype) {

        *this = a;
    }

    ~type()
    {
        std::array lambdas = { std::function{[&] {spec.basicdeclspec.~bascitypespec();}},
                                std::function{[&] {spec.ptrqualifiers.~bitset();}},
                                std::function{[&] {}},
                                std::function{[&] {spec.parametertypes.~vector();}} };
        lambdas[uniontype]();
    }

    type& operator=(const type& type)
    {
        std::array lambdas = { std::function{[&] {spec.basicdeclspec = type.spec.basicdeclspec;}},
                               std::function{[&] {spec.ptrqualifiers = type.spec.ptrqualifiers;}},
                               std::function{[&] {spec.arraysz = type.spec.arraysz;}},
                               std::function{[&] {spec.parametertypes = type.spec.parametertypes;}} };
        lambdas[uniontype]();

        return *this;
    }

    union typespec {
        typespec(typetype type)
        {
            std::array lambdas = { std::function{[&] {new (&basicdeclspec) bascitypespec{};}},
                                std::function{[&] {new (&ptrqualifiers) pointrtypequalifiers{};}},
                                std::function{[&] {arraysz = 0;}},
                                std::function{[&] {new (&parametertypes) functype{};}} };
            lambdas[type]();
        }
        ~typespec() {}
        bascitypespec basicdeclspec;
        pointrtypequalifiers ptrqualifiers;
        arraytype arraysz;
        functype parametertypes;
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

static std::vector<var&> currtypevectorbeingbuild{ 1, currdecltypevector };

void begindeclarationifnone()
{
    if (currdecltype != currdecltypeenum::NONE) return;

    if (qualifsandtypes.empty())
        currdeclspec.push_back("int");
    else currdeclspec = qualifsandtypes;

    if (currtypevectorbeingbuild.back().type.back().uniontype == type::FUNCTION)
    {
        currdecltype = currdecltypeenum::PARAMS;
        currtypevectorbeingbuild.back().type.back().spec.parametertypes.push_back({});
        currtypevectorbeingbuild.push_back(currtypevectorbeingbuild.back().type.back().spec.parametertypes.back());
    }
    else currdecltype = currdecltypeenum::UNKNOWN;
}

void addvar()
{
    scopevar.back().push_back(currdecltypevector);

    currdecltypevector.~var();

    new (&currdecltypevector) var{};

    const var& lastvar = scopevar.back().back();

    const char* lastvartypestoragespec = lastvartype.type.back().spec.basicdeclspec[2].c_str();

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

        switch (lastvartype[0].uniontype)
        {
        case type::POINTER:
        case type::ARRAY:
        case type::BASIC:
            globals.push_back(llvm::GlobalVariable{ lastvar.pllvmtype, false, linkagetype, nullptr, lastvar.identifier });
        case type::FUNCTION:
            funcs.push_back(llvm::Function{ lastvar.pllvmtype, linkagetype, static_cast<unsigned>(-1), lastvar.identifier, nonconstructable.mainmodule });
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
            switch (stringhash(type.spec.basicdeclspec[2].c_str()))
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
        std::function{[&](const type& type) {}} };

    std::reverse(decltypevector.begin(), decltypevector.end());

    for (const auto& type : decltypevector)
        lambdas[type.uniontype](type);

    return pcurrtype;
}

static llvm::BasicBlock* pcurrblock;

extern "C" void beginscope()
{
    pcurrblock = llvm::BasicBlock::Create(llvmctx, "", &funcs.back());
    scopevar.push_back({});
}

extern "C" void endscope()
{
    //nonconstructable.mainmodule.
    scopevar.pop_back();
}

extern "C" void endfunctiondef()
{
}

extern "C" void endfunctionparamdecl()
{
    currtypevectorbeingbuild.pop_back();
}

extern "C" void startfunctionparamdecl()
{
    begindeclarationifnone();

    if (currtypevectorbeingbuild.size() > 1) //if declaring a function inside another declaration
        addptrtotype("", 0);
    //currdecltype = currdecltypeenum::PARAMS;
    currtypevectorbeingbuild.back().type.push_back({ type::FUNCTION });
}

extern "C" void finalizedeclarationtypename()
{
    begindeclarationifnone();

    currtypevectorbeingbuild.back().type.push_back({ type::BASIC });
    currtypevectorbeingbuild.back().type.back().spec.basicdeclspec = parsebasictype(currdeclspec);
    currtypevectorbeingbuild.back().type.back().spec.basicdeclspec[3] = currdeclspectypedef;

    currtypevectorbeingbuild.back().pllvmtype = createllvmtype(currtypevectorbeingbuild.back().type);

    currdecltype = currdecltypeenum::NONE;
    currdeclspec = "";
    currdeclspectypedef = "";
}

extern "C" void addsubtotype()
{
    begindeclarationifnone();

    type arraytype{ type::ARRAY };

    auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

    arraytype.spec.arraysz = *res->getValue().getRawData();

    (currdecltype == currdecltypeenum::PARAMS ? currdecltypevector.back().spec.parametertypes.back().type : currdecltypevector).push_back(arraytype);
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr)
{
    begindeclarationifnone();

    type ptrtype{ type::POINTER };

    ptrtype = std::string{ quailifers, szstr };

    (currdecltype == currdecltypeenum::PARAMS ? currdecltypevector.back().spec.parametertypes.back().type : currdecltypevector).push_back(ptrtype);
}

static handlecnstexpr hndlcnstexpr{};

static basehndl hndlbase{};

static basehndl* phndl = &hndlbase;

extern "C" void insertinttoimm(const char* str, size_t szstr) { phndl->insertinttoimm(str, szstr); }

extern "C" void beginconstantexpr()
{
    phndl = &hndlcnstexpr;
}

extern "C" void endconstantexpr()
{
    assert(immidiates.size() == 1);

    //auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

    immidiates.pop_back();

    //std::cout << "computed value: " << *res->getValue().getRawData() << std::endl;

    phndl = &hndlbase;
}

extern "C" void startmodule(const char* modulename, size_t szmodulename)
{
    new (&nonconstructable.mainmodule) llvm::Module{ std::string{modulename, szmodulename}, llvmctx };
}

extern "C" void endmodule()
{

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