/// @file matchTrees.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/matchTrees.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{

typedef std::map<Object *, Object *> MatchMap;

struct Data {
    MatchMap &matched;
    MatchMap &unmatched;
    Object *matchedTop;
};

bool matchingMethod(Object *current, const Data &data)
{
    Object *currentParent = current->getParent();
    Object *matchedObject = nullptr;
    Object *matchedParent = nullptr;

    MatchMap::iterator it = data.matched.find(currentParent);
    if (it != data.matched.end()) {
        matchedParent = it->second;
    } else {
        it = data.unmatched.find(currentParent);
        if (it != data.unmatched.end()) {
            matchedParent = it->second;
        }
    }

    if (currentParent == nullptr) {
        messageDebugAssert(matchedParent == nullptr, "Parent not found.", current, nullptr);
        matchedObject = data.matchedTop;
    } else {
        matchedObject = matchedGet(matchedParent, current, currentParent);
    }

    bool ret = false;
    EqualsOptions opt;
    opt.skipChilden = true;
    if (matchedObject == nullptr) {
        data.unmatched[current] = data.matchedTop; // ????
        ret                     = false;
    } else if (!hif::equals(current, matchedObject, opt)) {
        data.unmatched[current] = matchedObject;
        ret                     = false;
    } else {
        data.matched[current] = matchedObject;
        ret                   = true;
    }

    return ret;
}

} // end namespace

void matchTrees(
    Object *referenceTree,
    Object *matchedTree,
    std::map<Object *, Object *> &matched,
    std::map<Object *, Object *> &unmatched)
{
    Data data{matched, unmatched, nullptr};
    data.matchedTop = matchedTree;

    hif::apply::visit(referenceTree, matchingMethod, data);
}

} // namespace manipulation
} // namespace hif
