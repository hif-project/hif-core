/// @file analyzePrecisionType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/analyzePrecisionType.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/copy.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"
#include "hif/semantics/getSemanticType.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

template <typename T> Type *_analyzePrecisionType(T *o, ILanguageSemantics *sem)
{
    std::list<Type *> altTypes;

    for (typename BList<typename T::AltType>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        typename T::AltType *wa = *i;
        for (BList<Value>::iterator j = wa->conditions.begin(); j != wa->conditions.end(); ++j) {
            Value *index = *j;
            if (dynamic_cast<Range *>(index) != nullptr) {
                Range *r = static_cast<Range *>(index);

                Type *altTypeLeft = getSemanticType(r->getLeftBound(), sem);
                messageDebugAssert(altTypeLeft != nullptr, "Cannot type alt left index", index, sem);
                if (altTypeLeft == nullptr)
                    return nullptr;
                altTypes.push_back(altTypeLeft);

                Type *altTypeRight = getSemanticType(r->getRightBound(), sem);
                messageDebugAssert(altTypeRight != nullptr, "Cannot type alt right index", index, sem);
                if (altTypeRight == nullptr)
                    return nullptr;
                altTypes.push_back(altTypeRight);
            } else {
                Type *altType = getSemanticType(index, sem);
                messageDebugAssert(altType != nullptr, "Cannot type alt index", index, sem);
                if (altType == nullptr)
                    return nullptr;
                altTypes.push_back(altType);
            }
        }
    }

    Type *condType = getSemanticType(o->getCondition(), sem);
    messageDebugAssert(condType != nullptr, "Cannot type condition", o, sem);
    if (condType == nullptr)
        return nullptr;
    altTypes.push_back(condType);

    AnalyzeParams altsParams;
    altsParams.operation    = op_case_eq;
    altsParams.operandsType = altTypes;
    altsParams.startingObj  = o;

    return analyzePrecisionType(altsParams, sem);
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// AnalyzeParams
// ///////////////////////////////////////////////////////////////////
AnalyzeParams::AnalyzeParams()
    : operation(op_none)
    , operandsType()
    , startingObj(nullptr)
{
    // ntd
}

AnalyzeParams::~AnalyzeParams()
{
    // ntd
}
AnalyzeParams::AnalyzeParams(const AnalyzeParams &t)
    : operation(t.operation)
    , operandsType(t.operandsType)
    , startingObj(t.startingObj)
{
    // ntd
}
AnalyzeParams &AnalyzeParams::operator=(const AnalyzeParams &t)
{
    if (&t == this)
        return *this;
    this->operation    = t.operation;
    this->operandsType = t.operandsType;
    this->startingObj  = t.startingObj;
    return *this;
}
// ///////////////////////////////////////////////////////////////////
// analyzePrecisionType
// ///////////////////////////////////////////////////////////////////

Type *analyzePrecisionType(const AnalyzeParams &params, ILanguageSemantics *sem)
{
    if (params.operandsType.empty())
        return nullptr;
    if (params.operandsType.size() == 1U)
        return hif::copy(params.operandsType.front());

    messageAssert(
        params.operation == op_eq || params.operation == op_case_eq,
        "analyzePrecisionType must be called with equality operator.", params.startingObj, sem);

    AnalyzeParams::TypeList::const_iterator it = params.operandsType.begin();
    Type *currOperator1                        = *it;
    Type *currOperator2                        = nullptr;

    ILanguageSemantics::ExpressionTypeInfo sourceExprAnalysis;

    // Establish result analyzing operand coupled (starting from second
    // element).

    ++it;
    for (; it != params.operandsType.end(); ++it) {
        currOperator2 = *it;

        sourceExprAnalysis = sem->getExprType(currOperator1, currOperator2, params.operation, params.startingObj);

        // the precision is used instead of the returned type
        if (currOperator1 != params.operandsType.front()) {
            delete currOperator1;
        }

        if (sourceExprAnalysis.returnedType == nullptr) {
            return nullptr;
        }

        currOperator1                         = sourceExprAnalysis.operationPrecision;
        sourceExprAnalysis.operationPrecision = nullptr;
        delete sourceExprAnalysis.returnedType;
        sourceExprAnalysis.returnedType = nullptr;
    }

    return currOperator1;
}

Type *analyzePrecisionType(With *o, ILanguageSemantics *sem) { return _analyzePrecisionType(o, sem); }

Type *analyzePrecisionType(Switch *o, ILanguageSemantics *sem) { return _analyzePrecisionType(o, sem); }

} // namespace semantics
} // namespace hif
