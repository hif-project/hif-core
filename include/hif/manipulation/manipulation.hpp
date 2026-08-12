/// @file manipulation.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

namespace hif
{
/// @brief Wraps methods to manipulate trees.
namespace manipulation
{
} // namespace manipulation
} // namespace hif

// ///////////////////////////////////////////////////////////////////
// Main manipulation methods.
// ///////////////////////////////////////////////////////////////////

#include "hif/manipulation/addDeclarationInContext.hpp"
#include "hif/manipulation/addUniqueObject.hpp"
#include "hif/manipulation/aggregateUtils.hpp"
#include "hif/manipulation/assignUtils.hpp"
#include "hif/manipulation/assureSyntacticType.hpp"
#include "hif/manipulation/bindOpenPortAssigns.hpp"
#include "hif/manipulation/buildHierarchicalSymbol.hpp"
#include "hif/manipulation/checkConflictingName.hpp"
#include "hif/manipulation/collectLeftHandSideSymbols.hpp"
#include "hif/manipulation/createConcatFromSpans.hpp"
#include "hif/manipulation/expandAliases.hpp"
#include "hif/manipulation/expandGenerates.hpp"
#include "hif/manipulation/explicitCallsParameters.hpp"
#include "hif/manipulation/findScopeDependencies.hpp"
#include "hif/manipulation/findTopLevelModule.hpp"
#include "hif/manipulation/findViewDependencies.hpp"
#include "hif/manipulation/fixAssignments.hpp"
#include "hif/manipulation/fixInstanceBindings.hpp"
#include "hif/manipulation/fixMultipleSignalPortAssigns.hpp"
#include "hif/manipulation/fixNestedDeclarations.hpp"
#include "hif/manipulation/fixRangesDirection.hpp"
#include "hif/manipulation/fixTemplateParameters.hpp"
#include "hif/manipulation/fixUnsupportedBits.hpp"
#include "hif/manipulation/flattenDesign.hpp"
#include "hif/manipulation/getDeclarationTotalInstances.hpp"
#include "hif/manipulation/instanceUtils.hpp"
#include "hif/manipulation/isAbstractionSubProgram.hpp"
#include "hif/manipulation/mapToNative.hpp"
#include "hif/manipulation/matchObject.hpp"
#include "hif/manipulation/matchPatternInTree.hpp"
#include "hif/manipulation/matchTrees.hpp"
#include "hif/manipulation/matchedGet.hpp"
#include "hif/manipulation/matchedInsert.hpp"
#include "hif/manipulation/mergeTrees.hpp"
#include "hif/manipulation/moveDeclaration.hpp"
#include "hif/manipulation/moveToScope.hpp"
#include "hif/manipulation/narrowToCardinality.hpp"
#include "hif/manipulation/prefixTree.hpp"
#include "hif/manipulation/propagateSymbols.hpp"
#include "hif/manipulation/rangeGetIncremented.hpp"
#include "hif/manipulation/rangeGetShiftedToZero.hpp"
#include "hif/manipulation/rebaseTypeSpan.hpp"
#include "hif/manipulation/removeStandardMethods.hpp"
#include "hif/manipulation/removeUnusedDeclarations.hpp"
#include "hif/manipulation/renameConflictingDeclarations.hpp"
#include "hif/manipulation/renameForbiddenNames.hpp"
#include "hif/manipulation/renameInScope.hpp"
#include "hif/manipulation/replaceOccurrencesInScope.hpp"
#include "hif/manipulation/resolveHierarchicalSymbol.hpp"
#include "hif/manipulation/resolveTemplates.hpp"
#include "hif/manipulation/simplify.hpp"
#include "hif/manipulation/sort.hpp"
#include "hif/manipulation/sortParameters.hpp"
#include "hif/manipulation/splitAssignTargets.hpp"
#include "hif/manipulation/transformConstant.hpp"
#include "hif/manipulation/transformGlobalActions.hpp"
#include "hif/manipulation/transformIfToWhen.hpp"
#include "hif/manipulation/transformRangeToSpan.hpp"
#include "hif/manipulation/transformSpanToRange.hpp"
#include "hif/manipulation/transformSwitchToIf.hpp"
#include "hif/manipulation/transformWhenToIf.hpp"

// ///////////////////////////////////////////////////////////////////
// Semantics specific methods.
// ///////////////////////////////////////////////////////////////////

#include "hif/manipulation/systemCManipulation.hpp"
