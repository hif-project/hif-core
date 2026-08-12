/// @file mapDeclarationsInTree.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @brief Re-compute all declarations of symbols in @p root, which is a
/// subnode of @p oldTree, by mapping them into @p newTree.
///
/// For example:
/// @p root is a function call to <tt>foo</tt> with parameter assign
/// <tt>param</tt>.
/// @p oldTree is the signature of the function call with the
/// <tt>param</tt> parameter inside.
/// The parameter assign <tt>param</tt> has the parameter <tt>param</tt> as
/// its declaration.
/// @p newTree is a copy of the <tt>foo</tt> signature.
/// After calling this function the declaration of the parameter assign
/// <tt>param</tt> will be set to parameter <tt>param</tt> in @p newTree.
///
/// @param root The starting root object.
/// @param oldTree The root of the tree containing the declarations to be remapped.
/// @param newTree The tree having the new declarations of root objects.
/// @param sem The reference semantics.
///

void mapDeclarationsInTree(
    Object *root,
    Object *oldTree,
    Object *newTree,
    ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance());

} // namespace semantics
} // namespace hif
