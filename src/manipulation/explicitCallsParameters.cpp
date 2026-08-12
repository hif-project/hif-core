/// @file explicitCallsParameters.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/explicitCallsParameters.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/trash.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

}

void explicitCallsParameters(
    std::list<SubProgram *> &subprograms,
    hif::semantics::ReferencesMap &refMap,
    hif::semantics::ILanguageSemantics *sem,
    SortMissingKind kind)
{
    std::set<SubProgram *> set;
    set.insert(subprograms.begin(), subprograms.end());
    explicitCallsParameters(set, refMap, sem, kind);
}

void explicitCallsParameters(
    std::set<SubProgram *> &subprograms,
    hif::semantics::ReferencesMap &refMap,
    hif::semantics::ILanguageSemantics *sem,
    SortMissingKind kind)
{
    for (std::set<SubProgram *>::iterator i = subprograms.begin(); i != subprograms.end(); ++i) {
        SubProgram *sub = *i;

        for (hif::semantics::ReferencesSet::iterator j = refMap[sub].begin(); j != refMap[sub].end(); ++j) {
            Object *ref = *j;

            FunctionCall *fc  = dynamic_cast<FunctionCall *>(ref);
            ProcedureCall *pc = dynamic_cast<ProcedureCall *>(ref);
            messageAssert(fc != nullptr || pc != nullptr, "Unexpected method reference", ref, sem);
            BList<ParameterAssign> &actualParam = fc != nullptr ? fc->parameterAssigns : pc->parameterAssigns;

            SubProgram *inst = nullptr;
            if (fc != nullptr)
                inst = hif::manipulation::instantiate(fc, sem);
            else
                inst = hif::manipulation::instantiate(pc, sem);
            messageAssert(inst != nullptr, "Cannot instantiate.", ref, sem);

            hif::manipulation::sortParameters(actualParam, inst->parameters, true, kind, sem);
        }
    }
}

} // namespace manipulation
} // namespace hif
