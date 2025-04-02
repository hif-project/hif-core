/// @file fixUnsupportedBits.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/fixUnsupportedBits.hpp"
#include "hif/GuideVisitor.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/transformConstant.hpp"
#include "hif/trash.hpp"

namespace hif
{
namespace manipulation
{

namespace /* anon */
{

/// @brief Performs all bits fixes.
class BadBitsVisitor : public hif::GuideVisitor
{
public:
    BadBitsVisitor(
        hif::semantics::ILanguageSemantics *sem,
        hif::semantics::ILanguageSemantics *checkSem,
        const FixUnsupportedBitsOptions &opts);
    ~BadBitsVisitor();

    /// @brief Returns true when at leas one fix has been performed.
    bool hasPerformedFixes() const;

    int visitBitValue(BitValue &o);
    int visitBitvectorValue(BitvectorValue &o);
    int visitExpression(Expression &o);
    int visitLibraryDef(LibraryDef &o);
    int visitSwitch(Switch &o);
    int visitView(View &o);
    int visitWith(With &o);

private:
    /// @brief Replaces in s the unsupported logic values.
    /// @param o The object (BitValue or BitvectorValue).
    /// @param s The object as string.
    void _fixLogicValues(Object *o, std::string &s);

    /// @brief Removes cases into Switch and With, having dontcare bits.
    /// @param o The Switch or With.
    template <typename T> void _removeDontcareCases(T *o);

    /// @brief Transform CASE_X and CASE_Z in CASE_LITERAL.
    template <typename T> void _fixCaseSemantics(T *o);

    /// @brief Simplify special expressions, which cannot be matched.
    /// @param o The expression.
    bool _simplifyEqualityExpressions(Expression *o);
    /// @brief Returns true if value is a constant containing logic bits.
    bool _isLogicConst(Value *o);
    /// @brief Checks whether given value is an initial value that must be skipped.
    /// @param v The value.
    /// @return The check result.
    bool _skipInitialValues(Value *v);

    hif::semantics::ILanguageSemantics *_sem;
    hif::semantics::ILanguageSemantics *_checkSem;
    const FixUnsupportedBitsOptions &_opts;
    const hif::semantics::ILanguageSemantics::SemanticOptions &_semOpts;
    bool _hasPerformedFixes;
    hif::Trash _trash;

    BadBitsVisitor(const BadBitsVisitor &);
    BadBitsVisitor &operator=(const BadBitsVisitor &);
};

BadBitsVisitor::BadBitsVisitor(
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem,
    const FixUnsupportedBitsOptions &opts)
    : GuideVisitor()
    , _sem(sem)
    , _checkSem(checkSem)
    , _opts(opts)
    , _semOpts(checkSem->getSemanticsOptions())
    , _hasPerformedFixes(false)
    , _trash()
{
    // ntd
}

BadBitsVisitor::~BadBitsVisitor() { _trash.clear(); }

bool BadBitsVisitor::hasPerformedFixes() const { return _hasPerformedFixes; }

int BadBitsVisitor::visitBitValue(BitValue &o)
{
    GuideVisitor::visitBitValue(o);
    if (_skipInitialValues(&o))
        return 0;

    std::string s(o.toString());
    _fixLogicValues(&o, s);
    o.setValue(s[0]);

    return 0;
}

int BadBitsVisitor::visitBitvectorValue(BitvectorValue &o)
{
    GuideVisitor::visitBitvectorValue(o);
    if (_skipInitialValues(&o))
        return 0;

    std::string s(o.getValue());
    _fixLogicValues(&o, s);
    o.setValue(s);

    return 0;
}

int BadBitsVisitor::visitExpression(Expression &o)
{
    if (_simplifyEqualityExpressions(&o))
        return 0;

    GuideVisitor::visitExpression(o);
    return 0;
}

int BadBitsVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    return GuideVisitor::visitLibraryDef(o);
}

int BadBitsVisitor::visitSwitch(Switch &o)
{
    _removeDontcareCases(&o);
    if (o.alts.empty()) {
        if (!o.defaults.empty())
            o.replaceWithList(o.defaults);
        _trash.insert(&o);
        return 0;
    }

    GuideVisitor::visitSwitch(o);

    _fixCaseSemantics(&o);

    return 0;
}

int BadBitsVisitor::visitView(View &o)
{
    if (o.isStandard())
        return 0;
    return GuideVisitor::visitView(o);
}

int BadBitsVisitor::visitWith(With &o)
{
    _removeDontcareCases(&o);
    if (o.alts.empty()) {
        if (o.getDefault() != nullptr)
            o.replace(o.setDefault(nullptr));
        _trash.insert(&o);
        return 0;
    }

    GuideVisitor::visitWith(o);

    _fixCaseSemantics(&o);

    return 0;
}
void BadBitsVisitor::_fixLogicValues(Object *o, std::string &s)
{
    for (std::string::size_type i = 0; i != s.size(); ++i) {
        switch (s[i]) {
        case '0':
        case '1':
            break;

        case '-':
            if (_semOpts.lang_hasDontCare && !_opts.onlyBinaryBits)
                break;
            if (!_opts.onlyBinaryBits) {
                raiseUniqueWarning("Found at least one bit value '-' in logic or logic vector. "
                                   "They are replaced with 'X'.");
                s[i] = 'X'; // TODO bit_z
            } else {
                raiseUniqueWarning(
                    "Found at least one bit value '-' in logic or logic vector. "
                    "They are replaced with " +
                    std::string(1, _opts.xzReplaceValue) + ".");
                s[i] = _opts.xzReplaceValue;
            }
            _hasPerformedFixes = true;
            break;

        case 'X':
            if (!_opts.onlyBinaryBits)
                break;
            raiseUniqueWarning(
                "Found at least one bit value 'X' in logic or logic vector. "
                "They are replaced with " +
                std::string(1, _opts.xzReplaceValue) + ".");
            s[i]               = _opts.xzReplaceValue;
            _hasPerformedFixes = true;
            break;

        case 'Z':
            if (!_opts.onlyBinaryBits)
                break;
            raiseUniqueWarning(
                "Found at least one bit value 'Z' in logic or logic vector. "
                "They are replaced with " +
                std::string(1, _opts.xzReplaceValue) + ".");
            s[i]               = _opts.xzReplaceValue;
            _hasPerformedFixes = true;
            break;

        case 'L':
            if (_semOpts.lang_has9logic && !_opts.onlyBinaryBits)
                break;
            raiseUniqueWarning("Found at least one bit value 'L' in logic or logic vector. "
                               "They are replaced with '0'.");
            s[i]               = '0';
            _hasPerformedFixes = true;
            break;

        case 'H':
            if (_semOpts.lang_has9logic && !_opts.onlyBinaryBits)
                break;
            raiseUniqueWarning("Found at least one bit value 'H' in logic or logic vector. "
                               "They are replaced with '1'.");
            s[i]               = '1';
            _hasPerformedFixes = true;
            break;

        case 'W':
            if (_semOpts.lang_has9logic && !_opts.onlyBinaryBits)
                break;
            if (_opts.onlyBinaryBits) {
                raiseUniqueWarning(
                    "Found at least one bit value 'W' in logic or logic vector. "
                    "They are replaced with " +
                    std::string(1, _opts.xzReplaceValue) + ".");
                s[i] = _opts.xzReplaceValue;
            } else {
                raiseUniqueWarning("Found at least one bit value 'W' in logic or logic vector. "
                                   "They are replaced with 'X'.");
                s[i] = 'X';
            }
            _hasPerformedFixes = true;
            break;

        case 'U':
            if (_semOpts.lang_has9logic && !_opts.onlyBinaryBits)
                break;
            if (_opts.onlyBinaryBits) {
                raiseUniqueWarning(
                    "Found at least one bit value 'U' in logic or logic vector. "
                    "They are replaced with " +
                    std::string(1, _opts.xzReplaceValue) + ".");
                s[i] = _opts.xzReplaceValue;
            } else {
                raiseUniqueWarning("Found at least one bit value 'W' in logic or logic vector. "
                                   "They are replaced with 'X'.");
                s[i] = 'X';
            }
            _hasPerformedFixes = true;
            break;

        default:
            messageError("Internal error.", o, _sem);
        }
    }
}

bool BadBitsVisitor::_simplifyEqualityExpressions(Expression *o)
{
    const hif::Operator op = o->getOperator();
    if (!hif::operatorIsEquality(op))
        return false;
    if (_semOpts.lang_has9logic && !_opts.onlyBinaryBits)
        return false;

    // Equality on 4 values will always be false when mapped to 2 values!
    // Eg. XXX01 == a --> false
    // Ref design: m6502_original + ddt
    bool isLogic1 = _isLogicConst(o->getValue1());
    bool isLogic2 = _isLogicConst(o->getValue2());
    if (!isLogic1 && !isLogic2)
        return false;

    Type *type1 = hif::semantics::getSemanticType(o->getValue1(), _sem);
    messageAssert(type1 != nullptr, "Cannot type expression", o, _sem);
    Type *type2 = hif::semantics::getSemanticType(o->getValue2(), _sem);
    messageAssert(type2 != nullptr, "Cannot type expression", o, _sem);
    Type *type = hif::semantics::getSemanticType(o, _sem);
    messageAssert(type != nullptr, "Cannot type expression", o, _sem);

    // VHDL Signed/Unsigned need special care
    // Ref design: vhdl/custom/test_library_equality
    Signed *sign1     = dynamic_cast<Signed *>(type1);
    Unsigned *unsign1 = dynamic_cast<Unsigned *>(type1);
    Signed *sign2     = dynamic_cast<Signed *>(type2);
    Unsigned *unsign2 = dynamic_cast<Unsigned *>(type2);
    if (sign1 != nullptr || sign2 != nullptr || unsign1 != nullptr || unsign2 != nullptr) {
        if (!_opts.onlyBinaryBits)
            return false;
    }

    raiseUniqueWarning("Found at least one equality expression involving logic bits."
                       "It will be replaced with a constant boolean.");

    // Literal comparison is always not-matched:
    // - case_eq --> False,
    // - case_neq --> True,
    // Logic comparison --> False
    BoolValue b(op == hif::op_case_neq);
    ConstValue *cv = hif::manipulation::transformConstant(&b, type, _sem);
    messageAssert(cv != nullptr, "Cannot transform boolean constant to type", type, _sem);
    o->replace(cv);
    delete o;
    cv->acceptVisitor(*this);

    _hasPerformedFixes = true;
    return true;
}

bool BadBitsVisitor::_isLogicConst(Value *o)
{
    BitvectorValue *bv = dynamic_cast<BitvectorValue *>(o);
    BitValue *bit      = dynamic_cast<BitValue *>(o);

    if (bv != nullptr) {
        std::string s = bv->getValue();
        bool isLogic  = false;
        if (_opts.onlyBinaryBits)
            isLogic = !bv->is01();
        else if (_semOpts.lang_hasDontCare) // assuming 4 logic
            isLogic = (s.find_first_of("UWLH") != std::string::npos);
        else // if (!_semOpts.lang_hasDontCare)
            isLogic = (s.find_first_of("-UWLH") != std::string::npos);
        return isLogic;
    } else if (bit != nullptr) {
        bool isLogic = false;
        if (_opts.onlyBinaryBits)
            isLogic = !bit->is01();
        else if (_semOpts.lang_hasDontCare) // assuming 4 logic
            isLogic = bit->getValue() == hif::bit_w || bit->getValue() == hif::bit_l || bit->getValue() == hif::bit_h ||
                      bit->getValue() == hif::bit_u;
        else // if (!_semOpts.lang_hasDontCare)
            isLogic = bit->getValue() == hif::bit_w || bit->getValue() == hif::bit_l ||
                      bit->getValue() == hif::bit_dontcare || bit->getValue() == hif::bit_h ||
                      bit->getValue() == hif::bit_u;
        return isLogic;
    }

    return false;
}

bool BadBitsVisitor::_skipInitialValues(Value *v)
{
    if (!_opts.skipInitialValues)
        return false;
    DataDeclaration *ddecl = dynamic_cast<DataDeclaration *>(v->getParent());
    return ddecl != nullptr && ddecl->getValue() == v;
}
template <typename T> void BadBitsVisitor::_removeDontcareCases(T *o)
{
    if (_semOpts.lang_hasDontCare && !_opts.onlyBinaryBits)
        return;

    typedef typename T::AltType AltType;
    hif::Trash condTrash;
    hif::Trash altTrash;
    for (typename BList<AltType>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        AltType *alt = *i;
        for (BList<Value>::iterator j = alt->conditions.begin(); j != alt->conditions.end(); ++j) {
            Value *cond         = *j;
            BitValue *bv        = dynamic_cast<BitValue *>(cond);
            BitvectorValue *bvv = dynamic_cast<BitvectorValue *>(cond);
            if (bvv == nullptr && bv == nullptr)
                continue;
            if (bv != nullptr && bv->getValue() != bit_dontcare)
                continue;
            if (bvv != nullptr && bvv->getValue().find('-') == std::string::npos)
                continue;

            // has dontcare.. can never matched (in systemc)! remove it!
            condTrash.insert(cond);
            _hasPerformedFixes = true;
        }

        condTrash.clear();
        if (alt->conditions.empty())
            altTrash.insert(alt);
    }

    altTrash.clear();
}
template <typename T> void BadBitsVisitor::_fixCaseSemantics(T *o)
{
    if (o->getCaseSemantics() == hif::CASE_LITERAL)
        return;

    Type *condType = hif::semantics::getSemanticType(o->getCondition(), _sem);
    messageAssert(condType != nullptr, "Cannot type condition", o, _sem);
    Type *condBaseType = hif::semantics::getBaseType(condType, false, _sem);
    messageAssert(condBaseType != nullptr, "Cannot type condition (2)", condType, _sem);

    // Check type of condition.
    // It may be different from logic vector or logic bit in case of manipulation
    // (e.g. DDT). In this case CASE_X and CASE_Z are mapped in CASE_LITERAL.
    bool isLogic = hif::typeIsLogic(condBaseType, _sem);
    if (isLogic)
        return;

    o->setCaseSemantics(hif::CASE_LITERAL);
    _hasPerformedFixes = true;
}

} // namespace

// /////////////////////////////////////////////////////////////////////////////
// FixUnsupportedBits options
// /////////////////////////////////////////////////////////////////////////////

FixUnsupportedBitsOptions::FixUnsupportedBitsOptions()
    : onlyBinaryBits(false)
    , xzReplaceValue('0')
    , skipInitialValues(false)
{
    // ntd
}

FixUnsupportedBitsOptions::~FixUnsupportedBitsOptions()
{
    // ntd
}

FixUnsupportedBitsOptions::FixUnsupportedBitsOptions(const FixUnsupportedBitsOptions &other)
    : onlyBinaryBits(other.onlyBinaryBits)
    , xzReplaceValue(other.xzReplaceValue)
    , skipInitialValues(other.skipInitialValues)
{
    // ntd
}

FixUnsupportedBitsOptions &FixUnsupportedBitsOptions::operator=(FixUnsupportedBitsOptions other)
{
    swap(other);
    return *this;
}

void FixUnsupportedBitsOptions::swap(FixUnsupportedBitsOptions &other)
{
    std::swap(onlyBinaryBits, other.onlyBinaryBits);
    std::swap(xzReplaceValue, other.xzReplaceValue);
    std::swap(skipInitialValues, other.skipInitialValues);
}

// /////////////////////////////////////////////////////////////////////////////
// fixUnsupportedBits()
// /////////////////////////////////////////////////////////////////////////////

bool fixUnsupportedBits(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem,
    const FixUnsupportedBitsOptions &opts)
{
    if (root == nullptr || sem == nullptr || checkSem == nullptr)
        return false;

    BadBitsVisitor bbv(sem, checkSem, opts);
    root->acceptVisitor(bbv);
    return bbv.hasPerformedFixes();
}

} // namespace manipulation
} // namespace hif
