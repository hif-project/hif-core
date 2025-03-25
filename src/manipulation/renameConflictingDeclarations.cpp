/// @file renameConflictingDeclarations.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/renameConflictingDeclarations.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif
namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

struct RenameInfos {
    std::string suffix;
    hif::semantics::ReferencesMap *refMap;
    bool result;
    hif::semantics::ILanguageSemantics *sem;
};

bool _renameConflictingDeclaration(Object *obj, RenameInfos &infos)
{
    // Fixing instances:
    if (dynamic_cast<Instance *>(obj) != nullptr) {
        Instance *inst   = dynamic_cast<Instance *>(obj);
        BaseContents *bp = dynamic_cast<BaseContents *>(inst->getParent());
        if (bp != nullptr && inst->isInBList() &&
            inst->getBList() == reinterpret_cast<hif::BList<hif::Object> *>(&bp->instances)) {
            // If instance is a sub-module instance...
            ViewReference *vr = dynamic_cast<ViewReference *>(inst->getReferencedType());
            messageDebugAssert(
                vr != nullptr, "Referenced type of sub-module instance must be a ViewReference.", inst, nullptr);

            if (vr->getDesignUnit() == inst->getName()) {
                // Instances cannot have the same name of their referenced DU: renaming.
                std::string freshName =
                    NameTable::getInstance()->getFreshName((inst->getName() + infos.suffix).c_str());
                inst->setName(freshName);
            }

            Identifier id(vr->getDesignUnit());
            hif::semantics::DeclarationOptions dopt;
            dopt.location           = bp;
            DataDeclaration *idDecl = hif::semantics::getDeclaration(&id, infos.sem, dopt);
            if (idDecl != nullptr) {
                // Bad case: singal or port (or other) with same name of a viewref
                // instantiated into the module.
                // Ref. design: vhdl/openCoresFunbase/hibi_udp

                View *view = hif::semantics::getDeclaration(vr, infos.sem);
                messageAssert(view != nullptr, "Declaration not found", vr, infos.sem);
                DesignUnit *du = dynamic_cast<DesignUnit *>(view->getParent());
                messageAssert(du != nullptr, "Declaration not found", view, infos.sem);

                // Get a fresh name
                std::string freshName = NameTable::getInstance()->getFreshName((du->getName() + infos.suffix).c_str());

                // Print info.
                std::string msg = std::string("Found design unit ") + du->getName() +
                                  ", instantiated into a context having a declaration with the same name." +
                                  du->getName() + " design unit has been renamed to " + freshName +
                                  " to avoid conflicts.";
                messageInfo(msg);

                // Renaming.
                du->setName(freshName);
                hif::semantics::ReferencesSet &set = (*infos.refMap)[view];
                for (hif::semantics::ReferencesSet::iterator it = set.begin(); it != set.end(); ++it) {
                    ViewReference *vv = dynamic_cast<ViewReference *>(*it);
                    if (vv == nullptr)
                        continue;
                    vv->setDesignUnit(freshName);
                }
            }
        }
        return true;
    }

    Declaration *decl = dynamic_cast<Declaration *>(obj);
    if (decl == nullptr)
        return true;
    DesignUnit *du = hif::getNearestParent<DesignUnit>(obj);
    if (du == nullptr)
        return true;

    std::string currentDuName = du->getName();
    if (decl->getName() != currentDuName)
        return true;

    infos.result = true;

    // Get a fresh name
    std::string freshName = NameTable::getInstance()->getFreshName((decl->getName() + infos.suffix).c_str());

    // Print info.
    std::string msg = std::string("Inside design unit ") + currentDuName + ", declaration " + decl->getName() +
                      " has been renamed to " + freshName + " to avoid conflicts.";
    messageInfo(msg);

    // Renaming.
    decl->setName(freshName);
    hif::semantics::ReferencesSet &set = (*infos.refMap)[decl];
    for (hif::semantics::ReferencesSet::iterator it = set.begin(); it != set.end(); ++it) {
        hif::objectSetName(*it, freshName);
    }

    return true;
}

} // namespace

bool renameConflictingDeclarations(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    std::map<Declaration *, std::set<Object *>> *refMap,
    const std::string &suffix)
{
    RenameInfos infos;
    infos.suffix = suffix;
    infos.sem    = sem;

    if (refMap == nullptr) {
        System *sys = dynamic_cast<System *>(root);
        if (sys == nullptr)
            sys = hif::getNearestParent<System>(root);
        messageAssert(sys != nullptr, "Unable to get System", root, sem);
        infos.refMap = new std::map<Declaration *, std::set<Object *>>();
        hif::semantics::GetReferencesOptions opt;
        opt.include_unreferenced = true;
        hif::semantics::getAllReferences(*infos.refMap, sem, sys, opt);
    } else {
        infos.refMap = refMap;
    }

    hif::apply::visit(root, _renameConflictingDeclaration, infos);

    if (refMap == nullptr) {
        delete infos.refMap;
    }

    return infos.result;
}

} // namespace manipulation
} // namespace hif
