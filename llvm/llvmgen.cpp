#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/GlobalIFunc.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <vector>
#include <string>
#include <sstream>
#include <array>
#include <bitset>
#include <functional>
#include <iostream>
#include <sstream>

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
    NORMAL,
    NONE
};

static currdecltypeenum currdecltype{ currdecltypeenum::NONE };

static std::string currdeclspec;

static bool bcurrdeclspecistypedef;

std::bitset<3> parsequalifiers(const std::string& qualifs)
{
    std::bitset<3> ret;
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

std::array<std::string, 3> parsebasictype(const std::string& qualifs)
{
    std::array<std::string, 3> ret;
    std::stringstream ssqualifs{ qualifs };
    std::string spec{};
    while (ssqualifs >> spec) switch (stringhash(spec.c_str()))
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
    default:
        std::cerr << "invalid specifier: " << spec << std::endl;
        std::terminate();
    }
    return ret;
}

struct type {
    enum typetype {
        BASIC,
        POINTER,
        ARRAY,
        FUNCTION
    } uniontype;

    type(typetype a) : spec(a) {};

    type(const type& a) : spec(a.uniontype) {

        std::array lambdas = { std::function{[&] {spec.basicdeclspec = a.spec.basicdeclspec;}},
                                std::function{[&] {spec.ptrqualifiers = a.spec.ptrqualifiers;}},
                                std::function{[&] {spec.arraysz = a.spec.arraysz;}},
                                std::function{[&] {spec.parametertypes = a.spec.parametertypes;}} };
        lambdas[a.uniontype]();
    };

    ~type()
    {
        std::array lambdas = { std::function{[&] {spec.basicdeclspec.~array();}},
                                std::function{[&] {spec.ptrqualifiers.~bitset();}},
                                std::function{[&] {}},
                                std::function{[&] {spec.parametertypes.~vector();}} };
        lambdas[uniontype]();
    }

    type& operator=(const std::string& str)
    {
        std::array lambdas = { std::function{[&] {spec.basicdeclspec = parsebasictype(str);}},
                                std::function{[&] {spec.ptrqualifiers = parsequalifiers(str);}},
                                std::function{[&] {}},
                                std::function{[&] {}} };
        lambdas[uniontype]();

        return *this;
    }

    union typespec {
        typespec(typetype type)
        {
            std::array lambdas = { std::function{[&] {new (&basicdeclspec) std::array<std::string, 3>{};}},
                                std::function{[&] {new (&ptrqualifiers) std::bitset<3>{};}},
                                std::function{[&] {arraysz = 0;}},
                                std::function{[&] {new (&parametertypes) std::vector<::type>{};}} };
            lambdas[type]();
        }
        ~typespec() {}
        std::array<std::string, 3> basicdeclspec;
        std::bitset<3> ptrqualifiers;
        uint64_t arraysz;
        std::vector<type> parametertypes;
    } spec;
};

struct var {
    std::vector<type> type;
    bool bistypedef;
    std::string identifier;
};

static std::vector<std::vector<var>> scopevar{ 1 };

static std::vector<type> currdecltypevector{};

extern "C" void startdeclaration(const char* str, size_t szstr, int isinsidedecl, bool bistypedef, const char* typesandqualifiersortypedef, size_t szstr1, bool bwhich)
{
    currdeclidentifier.assign(str, szstr);

    if (isinsidedecl) return;

    currdecltype = bistypedef ? currdecltypeenum::TYPEDEF : currdecltypeenum::NORMAL;

    currdeclspec.assign(typesandqualifiersortypedef, szstr1);

    bcurrdeclspecistypedef = !bwhich;
}

extern "C" void enddeclaration()
{
    type basictype{ type::BASIC };

    basictype = currdeclspec;

    currdecltypevector.push_back(basictype);

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

    std::reverse(currdecltypevector.begin(), currdecltypevector.end());

    for (const auto& type : currdecltypevector)
        lambdas[type.uniontype](type);

    scopevar.back().push_back(var{
        currdecltypevector,
        currdecltype == currdecltypeenum::TYPEDEF,
            currdeclidentifier,
        });
    switch (stringhash(currdecltypevector.front().spec.basicdeclspec[2].c_str()))
    {
    case "extern"_h:
        new llvm::GlobalVariable{ pcurrtype, false, llvm::GlobalValue::LinkageTypes::PrivateLinkage };
        new llvm::GlobalIFunc{}
        break;
    case "static"_h:
        break;
    default:
        break;
    }
}

static llvm::BasicBlock *pcurrblock;

extern "C" void beginscope()
{
    pcurrblock = llvm::BasicBlock::Create(llvmctx);
    scopevar.push_back({});
}

extern "C" void endscope()
{
    nonconstructable.mainmodule.
    scopevar.pop_back();
}

extern "C" void startfunctionparamdecl()
{
    
}

extern "C" void finalizedeclaration()
{
    currdecltype = currdecltypeenum::NONE;
    currdeclspec = "";
    bcurrdeclspecistypedef = false;
    currdeclidentifier = "";
}

extern "C" void finalizedeclarationtypename()
{
    currdecltype = currdecltypeenum::NONE;
    currdeclspec = "";
    bcurrdeclspecistypedef = false;
}

extern "C" void addsubtotype()
{
    type arraytype{ type::ARRAY };

    auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

    arraytype.spec.arraysz = *res->getValue().getRawData();

    currdecltypevector.push_back(arraytype);
}

extern "C" void addptrtotype(const char* quailifers, size_t szstr)
{
    type ptrtype{ type::POINTER };

    ptrtype = std::string{ quailifers, szstr };

    currdecltypevector.push_back(ptrtype);
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
    new (&nonconstructable.mainmodule) llvm::Module{std::string{modulename, szmodulename}, llvmctx};
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