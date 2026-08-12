/// @file transformIfToWhen.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/transformIfToWhen.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{

} // end namespace
When *transformIfToWhen(If *ifo, hif::semantics::ILanguageSemantics * /*sem*/)
{
    if (ifo == nullptr)
        return nullptr;

    When *ret = new When();

    for (BList<IfAlt>::iterator i = ifo->alts.begin(); i != ifo->alts.end(); ++i) {
        IfAlt *ifa = (*i);
        if (ifa->actions.empty())
            continue;
        if (ifa->actions.size() != 1) {
            delete ret;
            return nullptr;
        }

        Return *r = dynamic_cast<Return *>(ifa->actions.front());
        if (r == nullptr || r->getValue() == nullptr) {
            delete ret;
            return nullptr;
        }

        WhenAlt *wa = new WhenAlt();
        wa->setCondition(hif::copy(ifa->getCondition()));
        wa->setValue(hif::copy(r->getValue()));
        ret->alts.push_back(wa);
    }

    if (!ifo->defaults.empty()) {
        if (ifo->defaults.size() != 1) {
            delete ret;
            return nullptr;
        }

        Return *r = dynamic_cast<Return *>(ifo->defaults.front());
        if (r == nullptr || r->getValue() == nullptr) {
            delete ret;
            return nullptr;
        }
        ret->setDefault(hif::copy(r->getValue()));
    }

    return ret;
}

} // namespace manipulation
} // namespace hif
