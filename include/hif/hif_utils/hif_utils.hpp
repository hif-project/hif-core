/// @file hif_utils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"

#include "hif/hif_utils/apply.hpp"
#include "hif/hif_utils/compare.hpp"
#include "hif/hif_utils/copy.hpp"
#include "hif/hif_utils/declarationPropertyUtils.hpp"
#include "hif/hif_utils/equals.hpp"
#include "hif/hif_utils/getChildSkippingObjects.hpp"
#include "hif/hif_utils/getMatchingScope.hpp"
#include "hif/hif_utils/getNearestCommonParent.hpp"
#include "hif/hif_utils/getNearestParent.hpp"
#include "hif/hif_utils/getNearestScope.hpp"
#include "hif/hif_utils/getParentSkippingObjects.hpp"
#include "hif/hif_utils/isInTree.hpp"
#include "hif/hif_utils/isSubNode.hpp"
#include "hif/hif_utils/objectGetKey.hpp"
#include "hif/hif_utils/objectPropertyUtils.hpp"
#include "hif/hif_utils/operatorUtils.hpp"
#include "hif/hif_utils/rangePropertyUtils.hpp"
#include "hif/hif_utils/terminalInstanceUtils.hpp"
#include "hif/hif_utils/terminalPrefixUtils.hpp"
#include "hif/hif_utils/typePropertyUtils.hpp"

namespace hif
{
namespace semantics
{
class ILanguageSemantics;

}
} // namespace hif
