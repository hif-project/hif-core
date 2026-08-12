/// @file standardizeDescription.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>

#include "hif/application_utils/application_utils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/semantics/standardization.hpp"
#include "hif/semantics/standardizeDescription.hpp"

/////////////////////////////////////////
// Debug macros
/////////////////////////////////////////
//#define HIF_PRINT_DEBUG_FILES

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

void clearCastMap(CastMap &castMap)
{
    for (CastMap::iterator i = castMap.begin(); i != castMap.end(); ++i) {
        delete i->second;
    }

    castMap.clear();
}

void printInfo(const std::string &message, int &stepNr)
{
    ++stepNr;
    std::stringstream ssName;
    ssName << "   - STD ";
    if (stepNr < 10)
        ssName << "0";
    ssName << stepNr << ": " << message;

    messageInfo(ssName.str());
}

} // namespace

void standardizeDescription(
    System *&root,
    ILanguageSemantics *sourceSem,
    ILanguageSemantics *destSem,
    hif::application_utils::StepFileManager *parentFileManager)
{
    // Keep count of current step number.
    int stepNr = 0;

    hif::application_utils::StepFileManager _stepFileManager;
    _stepFileManager.setParentManager(parentFileManager);
#ifdef HIF_PRINT_DEBUG_FILES
    _stepFileManager.setPrint(true);
#endif

    hif::manipulation::SimplifyOptions sopt;
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.forceRefresh = true;
    dopt.error        = true;
    CastMap castMap;

    if (root == nullptr)
        return;

    printInfo("Simplifying source tree", stepNr);
    // Reset types and update declarations:
    // this is necessary for set declaration of
    // objects for avoid problem of getDeclaration of objects not
    // related to the tree and that have not member declaration set.
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    resetTypes(root);
    updateDeclarations(root, sourceSem, dopt);
    typeTree(root, sourceSem, true);

    // simplify source tree
    hif::manipulation::simplify(root, sourceSem, sopt);
    _stepFileManager.printStep(root, "post_first_simplify");

    // ///////////////////////////////////////////////////////////////////
    // Prefix source tree
    // ///////////////////////////////////////////////////////////////////
    printInfo("Prefixing source tree symbols", stepNr);
    hif::manipulation::prefixTree(root, sourceSem);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    resetTypes(root);
    updateDeclarations(root, sourceSem, dopt);
    typeTree(root, sourceSem, true);
    _stepFileManager.printStep(root, "post_prefix_tree");

    // ///////////////////////////////////////////////////////////////////
    // Standardization
    // ///////////////////////////////////////////////////////////////////
    printInfo("Perform tree standardization", stepNr);
    System *dstRoot = standardizeHif(root, sourceSem, destSem, castMap);
    delete root;
    root = dstRoot;
    _stepFileManager.printStep(root, "post_standardizeHif");

    // ///////////////////////////////////////////////////////////////////
    // Mapping standard symbols.
    // ///////////////////////////////////////////////////////////////////
    printInfo("Mapping destination tree standard symbols", stepNr);
    mapStandardSymbols(root, sourceSem, destSem);
    _stepFileManager.printStep(root, "post_standard_symbols_mapping");

    // simplify destination tree
    printInfo("Simplifying destination tree -- 1", stepNr);
    sopt.behavior = hif::manipulation::SimplifyOptions::BEHAVIOR_CONSERVATIVE;
    hif::manipulation::simplify(root, destSem, sopt);
    sopt.behavior = hif::manipulation::SimplifyOptions::BEHAVIOR_NORMAL;
    _stepFileManager.printStep(root, "post_second_simplify");

    // ///////////////////////////////////////////////////////////////////
    // Manage of casts
    // ///////////////////////////////////////////////////////////////////
    printInfo("Managing destination tree casts", stepNr);
    manageCasts(root, sourceSem, destSem, castMap);
    clearCastMap(castMap);
    _stepFileManager.printStep(root, "post_manageCast");

    // simplify destination tree
    printInfo("Simplifying destination tree -- 2", stepNr);
    hif::manipulation::simplify(root, destSem, sopt);
    _stepFileManager.printStep(root, "post_third_simplify");

    // avoid forbidden names in destination tree
    printInfo("Renaming destination tree forbidden names", stepNr);
    hif::manipulation::renameForbiddenNames(root, destSem);
    _stepFileManager.printStep(root, "post_renameForbiddenNames");

    // Try to minimize memory usage.
    printInfo("Final cleanup", stepNr);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    hif::semantics::resetDeclarations(root);
    hif::semantics::resetTypes(root);
}

} // namespace semantics
} // namespace hif
