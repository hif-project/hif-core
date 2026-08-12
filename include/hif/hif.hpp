/// @file hif.hpp
/// @brief Main include for the HIF library core definitions and utilities.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

// ///////////////////////////////////////////////////////////////////
// HIF local includes
// ///////////////////////////////////////////////////////////////////

#include "hif/AncestorVisitor.hpp"
#include "hif/BiVisitor.hpp"
#include "hif/DeclarationVisitor.hpp"
#include "hif/GuideVisitor.hpp"
#include "hif/HifFactory.hpp"
#include "hif/HifNodeRef.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/MapVisitor.hpp"
#include "hif/NameTable.hpp"
#include "hif/hifEnums.hpp"
#include "hif/search.hpp"
#include "hif/trash.hpp"

// ///////////////////////////////////////////////////////////////////
// Files grouping many methods.
// ///////////////////////////////////////////////////////////////////

#include "hif/hifIOUtils.hpp"

// ///////////////////////////////////////////////////////////////////
// Subdirs includes
// ///////////////////////////////////////////////////////////////////

#include "hif/analysis/analysis.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/backends/backends.hpp"
#include "hif/classes/classes.hpp"
#include "hif/features/features.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/univerCM/univerCM.hpp"

/// @brief Wraps all HIF library.
namespace hif
{

} // namespace hif
