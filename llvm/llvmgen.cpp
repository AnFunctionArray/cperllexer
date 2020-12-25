#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <vector>
#include <string>
#include <sstream>
#include <array>

static std::vector<llvm::Constant *> immidiates;

static llvm::LLVMContext llvmctx;

enum class RELOP {
    LESSTHAN,
    GREATERTHAN,
    LESSTHANOREQUAL,
    GREATERTHANOREQUAL
};

struct basehndl
{
    virtual void mlutiplylasttwovalues()
    { }

    virtual void dividelasttwovalues()
    {  }

    virtual void modulolasttwovalues()
    { }

    virtual void insertinttoimm()
    { }

    virtual void addlasttwovalues(bool)
    { }

    virtual void shifttwovalues(bool)
    { }

    virtual void relatetwovalues(enum RELOP)
    { }
};

struct handlecnstexpr : basehndl
{
    virtual void mlutiplylasttwovalues()
    {
        std::array ops = {*immidiates.begin(), *immidiates.end()};
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getMul(ops[0], ops[1]));
    }

    virtual void dividelasttwovalues()
    {
        std::array ops = {*immidiates.begin(), *immidiates.end()};
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getSDiv(ops[0], ops[1]));
    }

    virtual void modulolasttwovalues()
    {
        std::array ops = {*immidiates.begin(), *immidiates.end()};
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getSRem(ops[0], ops[1]));
    }

    virtual void addlasttwovalues(bool bminus)
    {
        std::array ops = {*immidiates.begin(), *immidiates.end()};
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::getAdd(ops[0],
             !bminus ? ops[1] : llvm::ConstantExpr::getNeg(ops[1])));
    }

    virtual void shifttwovalues(bool bright)
    {
        std::array ops = {*immidiates.begin(), *immidiates.end()};
        immidiates.erase(immidiates.end() - 2, immidiates.end());

        immidiates.push_back(llvm::ConstantExpr::get(
            !bright ? llvm::BinaryOperator::LShr : llvm::BinaryOperator::AShr,
             ops[0], ops[1]));
    }

    virtual void relatetwovalues(enum RELOP)
    {
        llvm::ConstantExpr::
    }

    virtual void insertinttoimm(const char *str, size_t szstr)
    {
        std::string imm;

        imm.assign(str, szstr);

        std::istringstream in{imm};

        uint64_t val;

        in >> val;

        immidiates.push_back(llvm::ConstantInt::get(llvm::IntegerType::get(llvmctx, 64), val));
    }
};

static handlecnstexpr hndlcnstexpr{};

static basehndl hndlbase{};

static basehndl *phndl = &hndlbase;

extern "C" void insertinttoimm(const char *str, size_t szstr) { phndl->insertinttoimm(); }

extern "C" void beginconstantexpr()
{
    phndl = &hndlcnstexpr;
}

extern "C" void endconstantexpr()
{
    phndl = &hndlbase;
}

extern "C" void binary(const char *str, size_t szstr)
{
    std::string imm;

    imm.assign(str, szstr);

    if (imm == "*")
        phndl->mlutiplylasttwovalues();
    else if (imm == "/")
        phndl->dividelasttwovalues();
    else if (imm == "%")
        phndl->modulolasttwovalues();
}

//llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());