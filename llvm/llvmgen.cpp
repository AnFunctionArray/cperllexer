#include <array>
#include <bitset>
#include <functional>
#include <iostream>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
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
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <vector>

unsigned constexpr stringhash (char const *input) {
    return *input ? static_cast<unsigned int> (*input) +
                        33 * stringhash (input + 1)
                  : 5381;
}

constexpr inline auto operator"" _h (char const *p, size_t) {
    return stringhash (p);
}

static struct nonconstructable {
    nonconstructable () {}
    ~nonconstructable () {}
    union {
        llvm::Module mainmodule;
    };
} nonconstructable;

static std::vector<llvm::Function *> funcs{};

static llvm::Function *currfunc;

static std::vector<llvm::GlobalVariable *> globals{};

static llvm::LLVMContext llvmctx;

static llvm::IRBuilder<> llvmbuilder{llvmctx};

typedef std::bitset<3> pointrtypequalifiers;
/*
0 - const
1 - restrict
2 - volatile
*/

struct bascitypespec {
    std::array<std::string, 4> basic;
    pointrtypequalifiers qualifiers;
};
/*
0 - last signed/unsigned
1 - last basic type
2 - last storage specifier
3 - last typedef name
*/

pointrtypequalifiers parsequalifiers (const std::string &qualifs) {
    pointrtypequalifiers ret;
    std::stringstream ssqualifs{qualifs};
    std::string spec{};
    while (ssqualifs >> spec)
        switch (stringhash (spec.c_str ())) {
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
            std::terminate ();
        }
    return ret;
}

bascitypespec parsebasictype (const std::vector<std::string> &qualifs) {
    bascitypespec ret;

    for (const auto &a : qualifs)
        switch (stringhash (a.c_str ())) {
        case "unsigned"_h:
        case "signed"_h:
            ret.basic[0] = a;
            break;
        case "int"_h:
            if (ret.basic[1] != "")
                break;
        case "long"_h:
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
            std::terminate ();
        }
    return ret;
}
struct var;

struct type {
    enum typetype { BASIC, POINTER, ARRAY, FUNCTION } uniontype;

    type (typetype a) : spec{a}, uniontype{a} {};

    typedef uint64_t arraytype;
    struct functype {
        std::vector<var> parametertypes;
        bool bisvariadic;
    };

    type (const type &a) : spec{a.uniontype}, uniontype{a.uniontype} {
        *this = a;
    }

    ~type () {
        std::array lambdas = {
            std::function{[&] { spec.basicdeclspec.~bascitypespec (); }},
            std::function{[&] { spec.ptrqualifiers.~bitset (); }},
            std::function{[&] {}},
            std::function{[&] { spec.func.~functype (); }}};
        lambdas[uniontype]();
    }

    type &operator= (const type &type) {

        if (type.uniontype != uniontype)
            this->~type (), new (this)::type{type.uniontype};
        std::array lambdas = {
            std::function{
                [&] { spec.basicdeclspec = type.spec.basicdeclspec; }},
            std::function{
                [&] { spec.ptrqualifiers = type.spec.ptrqualifiers; }},
            std::function{[&] { spec.arraysz = type.spec.arraysz; }},
            std::function{[&] { spec.func = type.spec.func; }}};
        lambdas[uniontype]();

        return *this;
    }

    union typespec {
        typespec (typetype type) {
            std::array lambdas = {
                std::function{[&] { new (&basicdeclspec) bascitypespec{}; }},
                std::function{
                    [&] { new (&ptrqualifiers) pointrtypequalifiers{}; }},
                std::function{[&] { arraysz = 0; }},
                std::function{[&] { new (&func) functype{}; }}};
            lambdas[type]();
        }
        ~typespec () {}
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
    llvm::Type *pllvmtype;
};

static std::vector<llvm::BasicBlock *> pcurrblock;

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

struct valandtype {
    llvm::Value *value;
    std::vector<::type> currtype;
};

static std::vector<valandtype> callees{};

llvm::Type *createllvmtype (std::vector<type> decltypevector);

auto gen_pointer_to_elem_if_ptr_to_array (llvm::Value *value,
                                          llvm::Type **p_type = nullptr) {
    llvm::Type *type_tmp,
        *&type = p_type
                     ? *p_type
                     : (type_tmp = value->getType ()->getPointerElementType ());
    if (type->isArrayTy ()) {
        type = type->getArrayElementType ();
        value = llvmbuilder.CreatePointerCast (value, type->getPointerTo ());
    }
    return value;
}

struct basehndl {
    virtual llvm::Value *CreateCastInst (llvm::Instruction::CastOps op,
                                         llvm::Value *S, llvm::Type *Ty) {
        return llvm::CastInst::Create (op, S, Ty, "", pcurrblock.back ());
    }

    using valandtype = ::valandtype;

    valandtype integralpromotions (valandtype in) {
        assert (in.currtype.size () == 1);

        switch (stringhash (
            in.currtype.back ().spec.basicdeclspec.basic[1].c_str ())) {
        case "char"_h:
        case "short"_h:
            switch (stringhash (
                in.currtype.back ().spec.basicdeclspec.basic[0].c_str ())) {
            default:
            case "signed"_h:
                in.value =
                    CreateCastInst (llvm::Instruction::CastOps::SExt, in.value,
                                    llvm::Type::getInt32Ty (llvmctx));
                break;
            case "unsigned"_h:
                in.value =
                    CreateCastInst (llvm::Instruction::CastOps::ZExt, in.value,
                                    llvm::Type::getInt32Ty (llvmctx));
                break;
            }
            in.currtype.back ().spec.basicdeclspec.basic[0] = "";
            in.currtype.back ().spec.basicdeclspec.basic[1] = "int";
        }
        return in;
    }

    std::array<valandtype, 2>
    usualarithmeticconversions (std::array<valandtype, 2> ops) {
        for (auto i = 0; i < 2; ++i)
            if (ops[i].currtype.back ().spec.basicdeclspec.basic[1] == "long" &&
                ops[i].currtype.back ().spec.basicdeclspec.basic[0] ==
                    "unsigned")
                if (ops[!i].currtype.back ().spec.basicdeclspec.basic[1] !=
                        "long" ||
                    ops[!i].currtype.back ().spec.basicdeclspec.basic[0] !=
                        "unsigned")
                    return ops[!i]
                               .currtype.back ()
                               .spec.basicdeclspec
                               .basic[1] = "long",
          ops[!i].currtype.back ().spec.basicdeclspec.basic[0] = "unsigned",
          ops[!i].value = CreateCastInst (
              ops[!i].currtype.back ().spec.basicdeclspec.basic[0] == "unsigned"
                  ? llvm::Instruction::CastOps::ZExt
                  : llvm::Instruction::CastOps::SExt,
              ops[!i].value, llvm::Type::getInt64Ty (llvmctx)),
          ops;
                else
                    return ops;

        for (auto i = 0; i < 2; ++i)
            if (ops[i].currtype.back ().spec.basicdeclspec.basic[1] == "long" &&
                ops[i].currtype.back ().spec.basicdeclspec.basic[0] !=
                    "unsigned")
                if (ops[!i].currtype.back ().spec.basicdeclspec.basic[1] ==
                        "int" &&
                    ops[!i].currtype.back ().spec.basicdeclspec.basic[0] ==
                        "unsigned")
                    return ops[!i]
                               .currtype.back ()
                               .spec.basicdeclspec
                               .basic[1] = "long",
          ops[!i].currtype.back ().spec.basicdeclspec.basic[0] = "",
          ops[!i].value = CreateCastInst (llvm::Instruction::CastOps::ZExt,
                                          ops[!i].value,
                                          llvm::Type::getInt64Ty (llvmctx)),
          ops;

        for (auto i = 0; i < 2; ++i)
            if (ops[i].currtype.back ().spec.basicdeclspec.basic[1] == "long" &&
                ops[i].currtype.back ().spec.basicdeclspec.basic[0] !=
                    "unsigned")
                return ops[!i]
                           .currtype.back ()
                           .spec.basicdeclspec
                           .basic[1] = "long",
      ops[!i].currtype.back ().spec.basicdeclspec.basic[0] = "",
      ops[!i].value = CreateCastInst (
          ops[!i].currtype.back ().spec.basicdeclspec.basic[0] == "unsigned"
              ? llvm::Instruction::CastOps::ZExt
              : llvm::Instruction::CastOps::SExt,
          ops[!i].value, llvm::Type::getInt64Ty (llvmctx)),
      ops;

        for (auto i = 0; i < 2; ++i)
            if (ops[i].currtype.back ().spec.basicdeclspec.basic[1] == "int" &&
                ops[i].currtype.back ().spec.basicdeclspec.basic[0] ==
                    "unsigned")
                return ops[!i]
                           .currtype.back ()
                           .spec.basicdeclspec
                           .basic[1] = "int",
      ops[!i].currtype.back ().spec.basicdeclspec.basic[0] = "unsigned",
      ops[!i].value = CreateCastInst (
          ops[!i].currtype.back ().spec.basicdeclspec.basic[0] == "unsigned"
              ? llvm::Instruction::CastOps::ZExt
              : llvm::Instruction::CastOps::SExt,
          ops[!i].value, llvm::Type::getInt32Ty (llvmctx)),
      ops;

        return ops;
    }

    auto getops (bool busual = true, bool bassign = false) {
        extern basehndl::valandtype getrvalue (basehndl::valandtype lvalue);

        auto ops =
            std::array{*(immidiates.end () - 2),
                       integralpromotions (getrvalue (immidiates.back ()))};

        auto op0type = ops[0].value->getType ();

        if (bassign)
            op0type = op0type->getPointerElementType ();
        else
            ops[0] = integralpromotions (getrvalue (ops[0]));

        ops[0].value =
            /*gen_pointer_to_elem_if_ptr_to_array*/ (ops[0].value);

        if (busual)
            ops = usualarithmeticconversions (ops);
        else
            ops[1].currtype = ops[0].currtype,
            ops[1].value = llvmbuilder.CreateIntCast (
                ops[1].value, op0type,
                ops[0].currtype.back ().spec.basicdeclspec.basic[0] !=
                    "unsigned");

        immidiates.erase (immidiates.end () - 2, immidiates.end ());

        return ops;
    }

    virtual void mlutiplylasttwovalues () {
        std::array ops = getops ();

        ops[0].value = llvmbuilder.CreateMul (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void dividelasttwovalues () {
        std::array ops = getops ();

        if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] != "unsigned")
            ops[0].value = llvmbuilder.CreateSDiv (ops[0].value, ops[1].value);
        else
            ops[0].value = llvmbuilder.CreateUDiv (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void modulolasttwovalues () {
        std::array ops = getops ();

        if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] != "unsigned")
            ops[0].value = llvmbuilder.CreateSRem (ops[0].value, ops[1].value);
        else
            ops[0].value = llvmbuilder.CreateURem (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void addlasttwovalues (bool bminus) {
        if (!bminus && subscripttwovalues ()) {
            getaddress ();
            return;
        }
        std::array ops = getops ();

        ops[0].value = llvmbuilder.CreateAdd (
            ops[0].value,
            !bminus ? ops[1].value : llvmbuilder.CreateNeg (ops[1].value));

        immidiates.push_back (ops[0]);
    }

    virtual void shifttwovalues (bool bright) {
        std::array ops = getops (false);

        if (!bright)
            ops[0].value = llvmbuilder.CreateShl (ops[0].value, ops[1].value);
        else if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] !=
                 "unsigned")
            ops[0].value = llvmbuilder.CreateAShr (ops[0].value, ops[1].value);
        else
            ops[0].value = llvmbuilder.CreateLShr (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void relatetwovalues (llvm::CmpInst::Predicate pred) {
        std::array ops = getops ();

        if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] != "unsigned")
            ops[0].value =
                llvmbuilder.CreateICmp (pred, ops[0].value, ops[1].value);
        else
            ops[0].value =
                llvmbuilder.CreateCmp (pred, ops[0].value, ops[1].value);

        ops[0].value =
            CreateCastInst (llvm::Instruction::CastOps::ZExt, ops[0].value,
                            llvm::Type::getInt32Ty (llvmctx));

        ops[0].currtype.back ().spec.basicdeclspec.basic[1] = "int",
                             ops[0]
                                 .currtype.back ()
                                 .spec.basicdeclspec.basic[0] = "";

        immidiates.push_back (ops[0]);
    }

    virtual void bitwisetwovalues (llvm::Instruction::BinaryOps op) {
        std::array ops = getops ();

        ops[0].value = llvmbuilder.CreateBinOp (op, ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void logictwovalues (bool bisand) {
        std::array ops = getops ();

        ops[0].value = llvmbuilder.CreateICmp (
            llvm::CmpInst::Predicate::ICMP_NE, ops[0].value,
            llvm::ConstantInt::getIntegerValue (ops[0].value->getType (),
                                                llvm::APInt{}));

        ops[1].value = llvmbuilder.CreateICmp (
            llvm::CmpInst::Predicate::ICMP_NE, ops[1].value,
            llvm::ConstantInt::getIntegerValue (ops[1].value->getType (),
                                                llvm::APInt{}));

        ops[0].value = !bisand
                           ? llvmbuilder.CreateOr (ops[0].value, ops[1].value)
                           : llvmbuilder.CreateAnd (ops[0].value, ops[1].value);

        ops[0].value = llvm::CastInst::Create (
            llvm::Instruction::CastOps::ZExt, ops[0].value,
            llvm::Type::getInt32Ty (llvmctx), "", pcurrblock.back ());

        ops[0].currtype.back ().spec.basicdeclspec.basic[1] = "int",
                             ops[0]
                                 .currtype.back ()
                                 .spec.basicdeclspec.basic[0] = "";

        immidiates.push_back (ops[0]);
    }

    virtual void assigntwovalues () {
        std::array ops = getops (false, true);

        llvmbuilder.CreateStore (ops[1].value, ops[0].value);

        immidiates.push_back (ops[1]);
    }

    virtual bool subscripttwovalues () {
        extern basehndl::valandtype getrvalue (basehndl::valandtype lvalue);

        auto ops = std::array{getrvalue (*(immidiates.end () - 2)),
                              getrvalue (immidiates.back ())};

        for (auto i = 0; i < 2; ++i)
            if (ops[i].currtype.front ().uniontype != type::BASIC) {
                if (ops[i].currtype.front ().uniontype == type::POINTER ||
                    ops[i].currtype.front ().uniontype == type::ARRAY) {
                    immidiates.erase (immidiates.end () - 2, immidiates.end ());
                    immidiates.push_back (
                        {llvmbuilder.CreateGEP (
                             ops[i].value,
                             {llvmbuilder.getInt64 (0),
                              llvmbuilder.CreateIntCast (
                                  ops[!i].value,
                                  llvm::Type::getInt64Ty (llvmctx), true)}),
                         ops[i].currtype});
                    immidiates.back ().currtype.erase (
                        immidiates.back ().currtype.begin ());
                    return true;
                }
            }
        return false;
    }

    virtual void getaddress () {
        auto op = immidiates.back ();

        if (op.value->getType ()->isArrayTy ())
            op.value = llvmbuilder.CreatePointerCast (
                op.value, op.value->getType ()->getPointerTo ());

        immidiates.pop_back ();

        op.currtype.push_back (type{type::POINTER});

        std::rotate (op.currtype.rbegin (), op.currtype.rbegin () + 1,
                     op.currtype.rend ());
        immidiates.push_back (op);
    }

    virtual void applyindirection () {
        unsigned int type = 3 << 2 | 2;

        if (immidiates.back ().currtype.front ().uniontype == ::type::POINTER)

            immidiates.back ().currtype.erase (
                immidiates.back ().currtype.begin ());

        insertinttoimm ("0", sizeof "0" - 1, type);

        subscripttwovalues ();
    }

    void insertinttoimm (const char *str, size_t szstr, unsigned int type) {
        std::string imm;

        imm.assign (str, szstr);

        std::vector<::type> currtype = {1, ::type::BASIC};

        const bool isunsigned = type & 1;

        const bool islong = type & 2;

        type >>= 2;

        const int base[] = {16, 2, 7, 10};

        const auto val = std::stoull (imm, nullptr, base[type]);

        if (type == 3 && !isunsigned && !islong)
            if (val <= INT_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "int";
            else if (val <= LLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long";
            else if (val <= ULLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long",
                              currtype.back ().spec.basicdeclspec.basic[0] =
                                  "unsigned";
            else
                ;
        else if (!isunsigned && !islong)
            if (val <= INT_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "int";
            else if (val <= UINT_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "int",
                              currtype.back ().spec.basicdeclspec.basic[0] =
                                  "unsigned";
            else if (val <= LLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long";
            else if (val <= ULLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long",
                              currtype.back ().spec.basicdeclspec.basic[0] =
                                  "unsigned";
            else
                ;
        else if (isunsigned && !islong)
            if (val <= UINT_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "int",
                              currtype.back ().spec.basicdeclspec.basic[0] =
                                  "unsigned";
            else if (val <= ULLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long",
                              currtype.back ().spec.basicdeclspec.basic[0] =
                                  "unsigned";
            else
                ;
        else if (!isunsigned && islong)
            if (val <= LLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long";
            else if (val <= ULLONG_MAX)
                currtype.back ().spec.basicdeclspec.basic[1] = "long",
                              currtype.back ().spec.basicdeclspec.basic[0] =
                                  "unsigned";
            else
                ;
        else if (isunsigned && islong)
            currtype.back ().spec.basicdeclspec.basic[1] = "long",
                          currtype.back ().spec.basicdeclspec.basic[0] =
                              "unsigned";

        auto valval = llvm::ConstantInt::get (createllvmtype (currtype), val);

        immidiates.push_back (valandtype{valval, currtype});
    }

    std::vector<valandtype> immidiates;
};

struct handlecnstexpr : basehndl {
    struct valandtype {
        llvm::Constant *value;
        std::vector<::type> currtype;
    };

    std::vector<valandtype> &immidiates =
        reinterpret_cast<std::vector<valandtype> &> (basehndl::immidiates);

    auto getops (bool busual = true) {
        auto ops = basehndl::getops ();

        return reinterpret_cast<std::array<valandtype, 2> &&> (ops);
    }

    virtual void mlutiplylasttwovalues () {
        std::array ops = getops ();

        ops[0].value = llvm::ConstantExpr::getMul (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void dividelasttwovalues () {
        std::array ops = getops ();

        if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] != "unsigned")
            ops[0].value =
                llvm::ConstantExpr::getSDiv (ops[0].value, ops[1].value);
        else
            ops[0].value =
                llvm::ConstantExpr::getUDiv (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void modulolasttwovalues () {
        std::array ops = getops ();

        if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] != "unsigned")
            ops[0].value =
                llvm::ConstantExpr::getSRem (ops[0].value, ops[1].value);
        else
            ops[0].value =
                llvm::ConstantExpr::getURem (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void addlasttwovalues (bool bminus) {
        std::array ops = getops ();

        ops[0].value = llvm::ConstantExpr::getAdd (
            ops[0].value,
            !bminus ? ops[1].value : llvm::ConstantExpr::getNeg (ops[1].value));

        immidiates.push_back (ops[0]);
    }

    virtual void shifttwovalues (bool bright) {
        std::array ops = getops (false);

        if (!bright)
            ops[0].value =
                llvm::ConstantExpr::getShl (ops[0].value, ops[1].value);
        else if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] !=
                 "unsigned")
            ops[0].value =
                llvm::ConstantExpr::getAShr (ops[0].value, ops[1].value);
        else
            ops[0].value =
                llvm::ConstantExpr::getLShr (ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void relatetwovalues (llvm::CmpInst::Predicate pred) {
        std::array ops = getops ();

        if (ops[0].currtype.back ().spec.basicdeclspec.basic[0] != "unsigned")
            ops[0].value =
                llvm::ConstantExpr::getICmp (pred, ops[0].value, ops[1].value);
        else
            ops[0].value = llvm::ConstantExpr::getCompare (pred, ops[0].value,
                                                           ops[1].value);

        ops[0].value =
            CreateCastInst (llvm::Instruction::CastOps::ZExt, ops[0].value,
                            llvm::Type::getInt32Ty (llvmctx));

        ops[0].currtype.back ().spec.basicdeclspec.basic[1] = "int",
                             ops[0]
                                 .currtype.back ()
                                 .spec.basicdeclspec.basic[0] = "";

        immidiates.push_back (ops[0]);
    }

    virtual void bitwisetwovalues (llvm::Instruction::BinaryOps op) {
        std::array ops = getops ();

        ops[0].value = llvm::ConstantExpr::get (op, ops[0].value, ops[1].value);

        immidiates.push_back (ops[0]);
    }

    virtual void logictwovalues (bool bisand) {
        std::array ops = getops ();

        ops[0].value = llvm::ConstantExpr::getICmp (
            llvm::CmpInst::Predicate::ICMP_NE, ops[0].value,
            llvm::ConstantInt::getIntegerValue (
                ((llvm::Constant *)ops[0].value)->getType (), llvm::APInt{}));

        ops[1].value = llvm::ConstantExpr::getICmp (
            llvm::CmpInst::Predicate::ICMP_NE, ops[1].value,
            llvm::ConstantInt::getIntegerValue (
                ((llvm::Constant *)ops[1].value)->getType (), llvm::APInt{}));

        ops[0].value =
            !bisand ? llvm::ConstantExpr::getOr (ops[0].value, ops[1].value)
                    : llvm::ConstantExpr::getAnd (ops[0].value, ops[1].value);

        ops[0].value =
            CreateCastInst (llvm::Instruction::CastOps::ZExt, ops[0].value,
                            llvm::Type::getInt32Ty (llvmctx));

        ops[0].currtype.back ().spec.basicdeclspec.basic[1] = "int",
                             ops[0]
                                 .currtype.back ()
                                 .spec.basicdeclspec.basic[0] = "";

        immidiates.push_back (ops[0]);
    }

    virtual void assigntwovalues () { assert (0); };

    virtual bool subscripttwovalues () { assert (0); }

    virtual llvm::Constant *CreateCastInst (llvm::Instruction::CastOps op,
                                            llvm::Constant *S, llvm::Type *Ty) {
        return llvm::ConstantExpr::getCast (op, S, Ty);
    }
};

static handlecnstexpr hndlcnstexpr{};

static basehndl hndlbase{};

static basehndl *phndl = &hndlbase;

static std::remove_reference<decltype (basehndl::immidiates)>::type::iterator
    cnstexpriterstart{};

// static std::string currdeclidentifier{};

enum class currdecltypeenum { TYPEDEF, CAST, PARAMS, NORMAL, UNKNOWN, NONE };

std::string currdeclspectypedef;

extern std::vector<std::string> qualifsandtypes;

static std::vector<std::vector<var>> scopevar{1};

struct currtypevectorbeingbuild {
    std::vector<var> *p;
    currdecltypeenum currdecltype;
};

std::vector<struct currtypevectorbeingbuild> currtypevectorbeingbuild = {
    {&scopevar.back (), currdecltypeenum::NORMAL}};

static std::string currstring;

extern "C" void obtainvalbyidentifier (const char *identifier, size_t szstr) {
    std::string ident{identifier, szstr};

    const ::var *var;

    llvm::GlobalVariable *pglobal;

    std::find_if (scopevar.begin (), scopevar.end (),
                  [&] (const std::vector<::var> &scope) {
                      auto iter =
                          std::find_if (scope.begin (), scope.end (),
                                        [&] (const ::var &scopevar) {
                                            return scopevar.identifier == ident;
                                        });

                      if (iter != scope.end ())
                          return (bool)(var = &*iter);
                      return false;
                  });

    if (var->type.front ().uniontype == ::type::FUNCTION)
        phndl->immidiates.push_back (
            {nonconstructable.mainmodule.getFunction (ident), var->type});
    else if (pglobal = nonconstructable.mainmodule.getGlobalVariable (ident))
        phndl->immidiates.push_back ({pglobal, var->type});
}

extern "C" void addplaintexttostring (const char *str, size_t szstr) {
    currstring += std::string{str, szstr};
}

extern "C" void addescapesequencetostring (const char *str, size_t szstr) {
    std::string escape{str, szstr};

    switch (stringhash (escape.c_str ())) {
    case "\\n"_h:
        currstring += '\n';
        break;
    }
}

extern "C" void constructstring () {
    std::vector<::type> stirngtype{1, ::type::ARRAY};

    stirngtype.back ().spec.arraysz = currstring.size () + 1;

    stirngtype.push_back ({::type::BASIC});

    stirngtype.back ().spec.basicdeclspec.basic[1] = "char";

    hndlbase.immidiates.push_back (
        {llvmbuilder.CreateGlobalStringPtr (currstring, "", 0,
                                            &nonconstructable.mainmodule),
         stirngtype});
    currstring = "";
}

void addvar (var &lastvar) {

    const char *lastvartypestoragespec =
        lastvar.type.back ().spec.basicdeclspec.basic[2].c_str ();

    switch (scopevar.size ()) {
    case 1:
        llvm::GlobalValue::LinkageTypes linkagetype;

        switch (stringhash (lastvartypestoragespec)) {
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
            if (scopevar.size () == 1)
                globals.push_back (new llvm::GlobalVariable (
                    nonconstructable.mainmodule, lastvar.pllvmtype, false,
                    linkagetype,
                    llvm::Constant::getNullValue (lastvar.pllvmtype),
                    lastvar.identifier));
            // scopevar.front ().push_back (lastvar);
            break;
        case type::FUNCTION:
            funcs.push_back (llvm::Function::Create (
                llvm::dyn_cast<llvm::FunctionType> (lastvar.pllvmtype),
                linkagetype, lastvar.identifier, nonconstructable.mainmodule));
            // scopevar.front ().push_back (lastvar);
            break;
        }
        break;
    default:

        break;
    }
}

extern "C" void startdeclaration () {

    var var;

    type basic{type::BASIC};

    if (qualifsandtypes.empty ())
        assert (!(scopevar.size () > 1)),
            basic.spec.basicdeclspec.basic[1] = "int";
    else
        basic.spec.basicdeclspec = parsebasictype (::qualifsandtypes);

    basic.spec.basicdeclspec.basic[3] = ::currdeclspectypedef;

    currdeclspectypedef = "";

    qualifsandtypes.clear ();

    var.type.push_back (basic);

    currtypevectorbeingbuild.back ().p->push_back (var);
}

void endpriordecl () {
    auto &currtype = currtypevectorbeingbuild.back ().p->back ().type;

    std::rotate (currtype.begin (), currtype.begin () + 1, currtype.end ());

    if (currtypevectorbeingbuild.back ().currdecltype !=
        currdecltypeenum::PARAMS)
        currtypevectorbeingbuild.back ()
            .p->back ()
            .pllvmtype = createllvmtype (currtype),
          addvar (currtypevectorbeingbuild.back ().p->back ());
}

extern "C" void adddeclarationident (const char *str, size_t szstr,
                                     bool bistypedef) {
    std::string ident{str, szstr};

    currtypevectorbeingbuild.back ().p->back ().identifier = ident;

    currtypevectorbeingbuild.back ().p->back ().bistypedef = bistypedef;
}

extern "C" void finalizedeclaration () { endpriordecl (); }

llvm::Type *createllvmtype (std::vector<type> decltypevector) {
    llvm::Type *pcurrtype;

    std::array lambdas = {
        std::function{[&] (const type &type) {
            switch (stringhash (type.spec.basicdeclspec.basic[1].c_str ())) {
            case "int"_h:
                pcurrtype =
                    dyn_cast<llvm::Type> (llvm::Type::getInt32Ty (llvmctx));
                break;
                break;
            case "short"_h:
                pcurrtype =
                    dyn_cast<llvm::Type> (llvm::Type::getInt16Ty (llvmctx));
                break;
                break;
            case "long"_h:
                pcurrtype =
                    dyn_cast<llvm::Type> (llvm::Type::getInt64Ty (llvmctx));
                break;
                break;
            case "char"_h:
                pcurrtype =
                    dyn_cast<llvm::Type> (llvm::Type::getInt8Ty (llvmctx));
                break;
                break;
            case "float"_h:
                pcurrtype =
                    dyn_cast<llvm::Type> (llvm::Type::getFloatTy (llvmctx));
                break;
                break;
            case "double"_h:
                pcurrtype =
                    dyn_cast<llvm::Type> (llvm::Type::getDoubleTy (llvmctx));
                break;
                break;
            }
        }},
        std::function{[&] (const type &type) {
            pcurrtype = dyn_cast<llvm::Type> (pcurrtype->getPointerTo ());
        }},
        std::function{[&] (const type &type) {
            pcurrtype = dyn_cast<llvm::Type> (
                llvm::ArrayType::get (pcurrtype, type.spec.arraysz));
        }},
        std::function{[&] (const type &type) {
            std::vector<llvm::Type *> paramtype;
            for (auto &a : type.spec.func.parametertypes)
                paramtype.push_back (a.pllvmtype);
            pcurrtype = dyn_cast<llvm::Type> (llvm::FunctionType::get (
                pcurrtype, paramtype, type.spec.func.bisvariadic));
        }}};

    std::reverse (decltypevector.begin (), decltypevector.end ());

    for (const auto &type : decltypevector)
        lambdas[type.uniontype](type);

    return pcurrtype;
}

extern std::vector<llvm::BasicBlock *> pcurrblock;

void finalizedecl ();

extern "C" void beginscope () {
    if (scopevar.size () == 1)
        currfunc = funcs.back ();

    pcurrblock.push_back (llvm::BasicBlock::Create (llvmctx, "", currfunc));
    llvmbuilder.SetInsertPoint (pcurrblock.back ());
    scopevar.push_back ({});
    currtypevectorbeingbuild.push_back (
        {&scopevar.back (), currdecltypeenum::NORMAL});
}

extern "C" void endscope () {
    // nonconstructable.mainmodule.
    // endexpression();
    scopevar.pop_back ();
    currtypevectorbeingbuild.pop_back ();
}

extern "C" void endexpression () { phndl->immidiates.clear (); }

basehndl::valandtype getrvalue (basehndl::valandtype lvalue) {
    if (lvalue.value->getType ()->isPtrOrPtrVectorTy () &&
        lvalue.currtype.front ().uniontype != type::POINTER &&
        lvalue.currtype.front ().uniontype != type::ARRAY) {
        std::cout << "rvalue" << std::endl;
        return {llvmbuilder.CreateLoad (
                    /*gen_pointer_to_elem_if_ptr_to_array*/ (lvalue.value)),
                lvalue.currtype};
    }
    return lvalue;
}

extern "C" void startfunctioncall () {
    callees.push_back (hndlbase.immidiates.back ());
    hndlbase.immidiates.erase (hndlbase.immidiates.end ());
}

extern "C" void endfunctioncall () {
    auto lastblock = pcurrblock.back ();

    valandtype calleevalntype = callees.back ();

    llvm::Function *callee =
        llvm::dyn_cast<llvm::Function> (calleevalntype.value);

    callees.pop_back ();

    llvmbuilder.SetInsertPoint (lastblock);

    std::vector<llvm::Value *> immidiates;

    std::transform (hndlbase.immidiates.begin (), hndlbase.immidiates.end (),
                    std::back_inserter (immidiates),
                    [] (const basehndl::valandtype &elem) {
                        return getrvalue (elem).value;
                    });

    phndl->immidiates.push_back (
        valandtype{llvmbuilder.CreateCall (callee->getFunctionType (),
                                           llvm::dyn_cast<llvm::Value> (callee),
                                           immidiates),
                   std::vector<::type>{calleevalntype.currtype.begin () + 1,
                                       calleevalntype.currtype.end ()}});
}

extern "C" void endreturn () {
    llvmbuilder.SetInsertPoint (pcurrblock.back ());
    llvmbuilder.CreateRet (hndlbase.immidiates.back ().value);
}

extern "C" void endfunctionparamdecl (bool bisrest) {

    for (auto &a : *currtypevectorbeingbuild.back ().p)
        std::rotate (a.type.begin (), a.type.begin () + 1, a.type.end ()),
            a.pllvmtype = createllvmtype (a.type);

    currtypevectorbeingbuild.pop_back ();

    auto &functype = currtypevectorbeingbuild.back ().p->back ().type.back ();

    assert (functype.uniontype == type::FUNCTION);

    functype.spec.func.bisvariadic = bisrest;
}

extern "C" void continuedeclaration () {

    auto lastvar = currtypevectorbeingbuild.back ().p->back ();

    auto lastbasetype = lastvar.type.back ();

    assert (lastbasetype.uniontype == type::BASIC);

    lastvar.type.clear ();

    lastvar.type.push_back (lastbasetype);

    currtypevectorbeingbuild.back ().p->push_back (lastvar);
}

extern "C" void addptrtotype (const char *quailifers, size_t szstr);

extern "C" void startfunctionparamdecl () {

    if (currtypevectorbeingbuild.back ().currdecltype ==
        currdecltypeenum::PARAMS) // if declaring a function inside another
                                  // declaration
        addptrtotype ("", 0);

    currtypevectorbeingbuild.back ().p->back ().type.push_back (
        {type::FUNCTION});

    currtypevectorbeingbuild.push_back ({&currtypevectorbeingbuild.back ()
                                              .p->back ()
                                              .type.back ()
                                              .spec.func.parametertypes,
                                         currdecltypeenum::PARAMS});
}

extern "C" void addsubtotype () {

    type arraytype{type::ARRAY};

    auto res = llvm::dyn_cast<llvm::ConstantInt> (
        hndlcnstexpr.immidiates.back ().value);

    arraytype.spec.arraysz = *res->getValue ().getRawData ();
    currtypevectorbeingbuild.back ().p->back ().type.push_back (arraytype);
}

extern "C" void addptrtotype (const char *quailifers, size_t szstr) {

    type ptrtype{type::POINTER};

    ptrtype.spec.ptrqualifiers =
        parsequalifiers (std::string{quailifers, szstr});

    currtypevectorbeingbuild.back ().p->back ().type.push_back (ptrtype);
}

extern "C" void insertinttoimm (const char *str, size_t szstr, int type) {
    phndl->insertinttoimm (str, szstr, type);
}

extern "C" void subscript () { phndl->subscripttwovalues (); }

extern "C" void beginconstantexpr () {
    cnstexpriterstart = phndl->immidiates.end ();

    phndl = &hndlcnstexpr;
}

extern "C" void endconstantexpr () {
    // assert(cnstexpriterstart == phndl->immidiates.end() - 1);

    // auto res = dyn_cast<llvm::ConstantInt>(immidiates.back());

    // hndlcnstexpr.immidiates.pop_back();

    // std::cout << "computed value: " << *res->getValue().getRawData() <<
    // std::endl;

    phndl = &hndlbase;
}

extern "C" void startmodule (const char *modulename, size_t szmodulename) {
    new (&nonconstructable.mainmodule)
        llvm::Module{std::string{modulename, szmodulename}, llvmctx};
}

extern "C" void endmodule () {
    std::error_code code{};
    llvm::raw_fd_ostream output{
        std::string{nonconstructable.mainmodule.getName ()} + ".bc", code};
    llvm::WriteBitcodeToFile (nonconstructable.mainmodule, output);
    nonconstructable.mainmodule.~Module ();
}

extern "C" void unary (const char *str, size_t szstr) {
    std::string imm;

    imm.assign (str, szstr);

    switch (stringhash (imm.c_str ())) {
    case "&"_h:
        phndl->getaddress ();
        break;
    case "*"_h:
        phndl->applyindirection ();
        break;
    }
}

extern "C" void binary (const char *str, size_t szstr) {
    std::string imm;

    imm.assign (str, szstr);

    switch (stringhash (imm.c_str ())) {
    case "*"_h:
        phndl->mlutiplylasttwovalues ();
        break;
    case "/"_h:
        phndl->dividelasttwovalues ();
        break;
    case "%"_h:
        phndl->modulolasttwovalues ();
        break;
    case "+"_h:
        phndl->addlasttwovalues (false);
        break;
    case "-"_h:
        phndl->addlasttwovalues (true);
        break;
    case "<<"_h:
        phndl->shifttwovalues (false);
        break;
    case ">>"_h:
        phndl->shifttwovalues (true);
        break;
    case ">"_h:
        phndl->relatetwovalues (llvm::CmpInst::Predicate::ICMP_SGT);
        break;
    case "<"_h:
        phndl->relatetwovalues (llvm::CmpInst::Predicate::ICMP_SLT);
        break;
    case "<="_h:
        phndl->relatetwovalues (llvm::CmpInst::Predicate::ICMP_SLE);
        break;
    case ">="_h:
        phndl->relatetwovalues (llvm::CmpInst::Predicate::ICMP_SGE);
        break;
    case "=="_h:
        phndl->relatetwovalues (llvm::CmpInst::Predicate::ICMP_EQ);
        break;
    case "!="_h:
        phndl->relatetwovalues (llvm::CmpInst::Predicate::ICMP_NE);
        break;
    case "&"_h:
        phndl->bitwisetwovalues (llvm::Instruction::And);
        break;
    case "^"_h:
        phndl->bitwisetwovalues (llvm::Instruction::Xor);
        break;
    case "|"_h:
        phndl->bitwisetwovalues (llvm::Instruction::Or);
        break;
    case "&&"_h:
        phndl->logictwovalues (true);
        break;
    case "||"_h:
        phndl->logictwovalues (false);
        break;
    case "="_h:
        phndl->assigntwovalues ();
        break;
    }
}

// llvm::BinaryOperator::CreateMul(*immidiates.begin(), *immidiates.end());
// std::cout << "base type of " << currdeclidentifier << " is typedef " <<
// !bwhich << " :" << spec << ", is declaring a typedef :" << bistypedef <<
// std::endl;