/// @file transformWhenToIf.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/transformWhenToIf.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /* anon */
{

} // namespace

If *transformWhenToIf(When *wo, semantics::ILanguageSemantics *sem)
{
    Assign *ao     = dynamic_cast<Assign *>(hif::getParentSkippingCasts(wo));
    Return *ro     = dynamic_cast<Return *>(hif::getParentSkippingCasts(wo));
    Action *parent = ao;
    if (parent == nullptr)
        parent = ro;
    if (parent == nullptr)
        return nullptr;
    If *ifStm = new If();

    for (BList<WhenAlt>::iterator i = wo->alts.begin(); i != wo->alts.end(); ++i) {
        WhenAlt *wa = *i;

        Action *nao = hif::copy(parent);
        Object *mao = hif::manipulation::matchObject(wo, parent, nao, sem);
        messageAssert(mao != nullptr, "Unmatched when.", wo, sem);
        mao->replace(wa->setValue(nullptr));
        delete mao;

        IfAlt *ifa = new IfAlt();
        ifa->setCondition(wa->setCondition(nullptr));
        ifa->actions.push_back(nao);
        ifStm->alts.push_back(ifa);
    }

    Action *nao = hif::copy(parent);
    Object *mao = hif::manipulation::matchObject(wo, parent, nao, sem);
    messageAssert(mao != nullptr, "Unmatched when.", wo, sem);
    mao->replace(wo->setDefault(nullptr));
    delete mao;

    ifStm->defaults.push_back(nao);

    return ifStm;
}

} // namespace manipulation
} // namespace hif
