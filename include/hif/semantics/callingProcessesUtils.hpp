/// @file callingProcessesUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/referencesUtils.hpp"

namespace hif
{
namespace semantics
{

/// @brief The set of calling objects (processes and global actions).
typedef std::set<Object *> CallerSet;

/// @brief The map from sub programs to calling objects.
typedef std::map<SubProgram *, CallerSet> CallerMap;

/// @brief Given a sub program, returns a set of calling objects.
/// @param sub The sub program.
/// @param refMap The references map.
/// @param callerSet The resulting set.
/// @param sem The semantics.
/// @param root The possible sub tree for object search.

void findCallingProcesses(
    SubProgram *sub,
    ReferencesMap &refMap,
    CallerSet &callerSet,
    ILanguageSemantics *sem,
    Object *root = nullptr);

/// @brief Given a root, returns a map from sub programs to related calling objects.
/// @param root The root.
/// @param refMap The references map.
/// @param callerMap The resulting map.
/// @param sem The semantics.

void findAllCallingProcesses(Object *root, ReferencesMap &refMap, CallerMap &callerMap, ILanguageSemantics *sem);

} // namespace semantics
} // namespace hif
