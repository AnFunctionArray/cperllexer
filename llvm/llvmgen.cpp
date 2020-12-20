#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/InstrTypes.h>
#include <vector>
#include <string>
#include <sstream>

static std::vector<llvm::Constant*> immidiates;

static llvm::LLVMContext llvmctx;

static bool constantexpr = false;

extern "C" void beginconstantexpr()
{
    constantexpr = true;
}

extern "C" void endconstantexpr()
{
    constantexpr = false;
}


extern "C" void insertinttoimm(const char *str, size_t szstr)
{
    std::string imm;

    imm.assign(str, szstr);

    std::istringstream in{imm};

    uint64_t val;

    in >> val;

    immidiates.push_back(llvm::ConstantInt::get(llvm::IntegerType::get (llvmctx, 64), val));
}

void mlutiplylasttwovalues()
{
    if(constantexpr) 
        immidiates.push_back(llvm::ConstantExpr::getMul(*immidiates.begin(), *immidiates.end())), immidiates.erase(immidiates.begin());
}

void dividelasttwovalues()
{
    if(constantexpr) 
        immidiates.push_back(llvm::ConstantExpr::getMul(*immidiates.begin(), *immidiates.end())), immidiates.erase(immidiates.begin());
}

void modulolasttwovalues()
{

}

extern "C" void mul(const char *str, size_t szstr)
{
    std::string imm;

    imm.assign(str, szstr);

    if(imm == "*")
        mlutiplylasttwovalues();
    else if(imm == "/")
        dividelasttwovalues();
    else if(imm == "%")
        modulolasttwovalues();
}

//llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());