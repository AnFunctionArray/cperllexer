#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <vector>
#include <string>
#include <sstream>
#include <array>
#include <functional>
#include <iostream>

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
                llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[0],valzero),
                llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, ops[1],valzero)));
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

    auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

    immidiates.pop_back();

    std::cout << "computed value: " << *res->getValue().getRawData() << std::endl;

    phndl = &hndlbase;
}

unsigned constexpr stringhash(char const* input)
{
    return *input ? static_cast<unsigned int>(*input) + 33 * stringhash(input + 1) : 5381;
}

constexpr inline auto operator"" _h(char const* p, size_t)
{
    return stringhash(p);
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