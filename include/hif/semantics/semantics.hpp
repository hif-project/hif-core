/// @file semantics.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

// ///////////////////////////////////////////////////////////////////
// Semantics specific methods.
// ///////////////////////////////////////////////////////////////////
#include "hif/semantics/HIFSemantics.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"
#include "hif/semantics/SystemCSemantics.hpp"
#include "hif/semantics/VHDLSemantics.hpp"
#include "hif/semantics/VerilogSemantics.hpp"

// ///////////////////////////////////////////////////////////////////
// Main semantics methods and options.
// ///////////////////////////////////////////////////////////////////
#include "hif/semantics/DeclarationOptions.hpp"
#include "hif/semantics/DeclarationsStack.hpp"
#include "hif/semantics/analyzePrecisionType.hpp"
#include "hif/semantics/callingProcessesUtils.hpp"
#include "hif/semantics/canRemoveInternalCast.hpp"
#include "hif/semantics/checkHif.hpp"
#include "hif/semantics/collectSymbols.hpp"
#include "hif/semantics/compareValues.hpp"
#include "hif/semantics/declarationUtils.hpp"
#include "hif/semantics/getBaseType.hpp"
#include "hif/semantics/getInstantiatedType.hpp"
#include "hif/semantics/getOtherOperandType.hpp"
#include "hif/semantics/getSemanticType.hpp"
#include "hif/semantics/getVectorElementType.hpp"
#include "hif/semantics/mapDeclarationsInTree.hpp"
#include "hif/semantics/rangeGetBitwidth.hpp"
#include "hif/semantics/rangeGetMax.hpp"
#include "hif/semantics/rangeGetSum.hpp"
#include "hif/semantics/referencesUtils.hpp"
#include "hif/semantics/resetDeclarations.hpp"
#include "hif/semantics/resetTypes.hpp"
#include "hif/semantics/setDeclaration.hpp"
#include "hif/semantics/spanGetBitwidth.hpp"
#include "hif/semantics/spanGetSize.hpp"
#include "hif/semantics/standardizeDescription.hpp"
#include "hif/semantics/typeSemanticUtils.hpp"
#include "hif/semantics/updateDeclarations.hpp"

// ///////////////////////////////////////////////////////////////////
// Files grouping many methods.
// ///////////////////////////////////////////////////////////////////

namespace hif
{

/// @brief Wraps methods to perform semantic operations.
/// This namespace contains methods concerning visibility, types,
/// libraries, semantic checks etc.
/// This namespace define methods to check semantics of a system description,
///	get value of a specific type, get type from a value, get base type from a type,
///	get cardinality for a type, get size of a range, check and manipulate types,
///	add library definitions, simplify expressions.
namespace semantics
{
}

} // namespace hif
