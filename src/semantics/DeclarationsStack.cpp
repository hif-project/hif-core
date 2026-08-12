/// @file DeclarationsStack.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/DeclarationsStack.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

} // namespace

DeclarationsStack::DeclarationsStack(Object *root)
    : _stack()
    , _root(root)
{
    // ntd
}

DeclarationsStack::~DeclarationsStack()
{
    messageDebugAssert(_stack.empty(), "Stack should be empty!", nullptr, nullptr);
}

void DeclarationsStack::push()
{
    typedef std::list<Object *> List;
    List list;
    collectSymbols(list, _root);

    DeclarationsMap map;
    hif::semantics::DeclarationOptions dopt;
    dopt.dontSearch = true;
    for (List::iterator i = list.begin(); i != list.end(); ++i) {
        map[*i] = getDeclaration(*i, nullptr, dopt);
    }

    _stack.push_back(map);
}

void DeclarationsStack::pop()
{
    typedef std::list<Object *> List;
    List list;
    collectSymbols(list, _root);
    messageAssert(!_stack.empty(), "Unexpected empty stack", nullptr, nullptr);

    DeclarationsMap map(_stack.back());
    for (List::iterator i = list.begin(); i != list.end(); ++i) {
        DeclarationsMap::iterator found = map.find(*i);
        if (found == map.end())
            continue;
        setDeclaration(*i, found->second);
    }

    _stack.pop_back();
}

} // namespace semantics
} // namespace hif
