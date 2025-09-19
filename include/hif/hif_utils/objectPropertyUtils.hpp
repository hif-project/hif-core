/// @file objectPropertyUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Change the name of a generic object.
/// @param obj The object on which to operate.
/// @param n The new name to set.
///
void objectSetName(Object *obj, const std::string &n);

/// @brief Returns the name of a generic object.
/// @param obj The object on which to operate.
/// @return The name of the object if it has a <tt>name</tt> field, nullptr otherwise.
///
std::string objectGetName(Object *obj);

/// @brief Compares the object name with a string passed as parameter.
/// It is useful when you do not want to add a name in the name table.
/// @param obj The object whose name is to be compared.
/// @param nameToMatch The name to match.
/// @return <tt>true</tt> if the names match, <tt>false</tt> otherwise.
bool objectMatchName(Object *obj, const std::string &nameToMatch);

/// @brief Returns the <tt>type</tt> field of an object (if any).
/// In case object is a type, the object itself is returned.
/// All types are returned by aliasing.
/// @param obj The object on which to operate.
/// @return The <tt>type</tt> field of the object if any, nullptr otherwise.
///
Type *objectGetType(Object *obj);

/// @brief Sets the <tt>type</tt> of an object if any.
///
/// @param obj The object on which the type is to be set.
/// @param t The type to be set.
/// @return The old type.
///

Type *objectSetType(Object *obj, Type *t);

/// @brief Returns the list of template-parameter assignments (if any).
///
/// @param obj The object on which to operate.
/// @return The template-parameter assignments  list, or nullptr if the object does not have a template assignments list.
///

BList<TPAssign> *objectGetTemplateAssignList(Object *obj);
/// @brief Return the list of template parameters (if any).
///
/// @param obj The object on which to operate.
/// @return The template parameters list, or nullptr if object does not have a template parameters list.
///

BList<Declaration> *objectGetTemplateParameterList(Object *obj);
/// @brief Returns the list of parameters (if any).
///
/// @param obj The object on which to operate.
/// @return The parameters list, or nullptr if object does not have a parameters list.
///

BList<Parameter> *objectGetParameterList(Object *obj);
/// @brief Returns the list of declarations (if any).
///
/// @param obj The object on which to operate.
/// @return The declarations list, or nullptr if object does not have a declarations list.
///

BList<Declaration> *objectGetDeclarationList(Object *obj);
/// @brief Returns the list of libraries (if any).
///
/// @param obj The object on which to operate.
/// @return The libraries list, or nullptr if object does not have a libraries list.
///

BList<Library> *objectGetLibraryList(Object *obj);
/// @brief Returns the specified language in the object if present,
/// otherwise the language specified in the nearest parent having one.
/// @param obj The object on which to operate.
/// @return The specified language in the object, or in the nearest parent having one.
///

hif::LanguageID objectGetLanguage(Object *obj);

/// @brief Returns the specified process flavour in the object if present,
/// otherwise the process flavour specified in the nearest parent having one.
/// @param obj The object on which to operate.
/// @param flavour The output specified process flavour in the object, or in the nearest parent having one.
/// @return true if found.
///

bool objectGetProcessFlavour(Object *obj, ProcessFlavour &flavour);
/// @brief Sets the language ID of an object if any.
///
/// @param obj The object on which the language ID is to be set.
/// @param id The language ID to set.
/// @param recursive If recoursive in parents.
///

void objectSetLanguage(Object *obj, const hif::LanguageID id, bool recursive = false);

/// @brief Sets an object <tt>instance</tt> field.
/// @note If instance can be set and <tt>obj</tt> already has an instance,
/// the old intance is deleted.
/// @param obj The object on which to operate.
/// @param instance The new instance to be set.
/// @return <tt>true</tt> if instance is set, <tt>false</tt> otherwise.

bool objectSetInstance(Object *obj, Object *instance);
/// @brief Gets an object <tt>instance</tt> field.
/// @param obj The object on which to operate.
/// @return The <tt>instance</tt> field of the object.

Object *objectGetInstance(Object *obj);

/// @brief Options for objectIsInSensitivityList() and objectGetSensitivityList().
struct ObjectSensitivityOptions {
    ObjectSensitivityOptions();
    ~ObjectSensitivityOptions();
    /// @brief Copy constructor.
    /// @param other The object to copy from.
    ObjectSensitivityOptions(const ObjectSensitivityOptions &other);
    /// @brief Assignment operator.
    /// @param other The object to assign from.
    /// @return Reference to this object.
    ObjectSensitivityOptions &operator=(const ObjectSensitivityOptions &other);

    /// @brief Check all objects with sensitivity (e.g. Wait). Default is false.
    bool checkAll;
    /// @brief Check only in case of direct parent. Default is false.
    bool directParent;
    /// Considers function calls as valid if appears into sensitivity lists. Default is true.
    bool considerFunctionCalls;
};

/// @brief Returns whether an object is an item in a sensitivity list of a state table.
/// @param obj The object to be checked.
/// @param opts The options.
/// @return <tt>true</tt> if the object belongs to a sensitivity list, <tt>false</tt> otherwise.

bool objectIsInSensitivityList(Object *obj, const ObjectSensitivityOptions &opts = ObjectSensitivityOptions());
/// @brief Returns the nearest parent sensitivity of an object.
/// @param obj The object to be checked.
/// @param opts The options.
/// @return The sensitivity list if the object belongs to a sensitivity list, nullptr otherwise.

BList<Value> *objectGetSensitivityList(Object *obj, const ObjectSensitivityOptions &opts = ObjectSensitivityOptions());
/// @brief Returns true of given object is a process.
/// In case of StateTables and Assigns without parent, returns true.
///
/// @param o The object to be checked.
/// @return True if given object is a process.
///

bool objectIsProcess(Object *o);

/// @brief Returns true of given object is a process.
/// In case of StateTables and Assigns without parent, returns true.
///
/// @param o The object to be checked.
/// @param matchAssigns True if also globact assings must be considered. Default is true.
/// @param matchStarting True if o can be directly a process. Default is true.
/// @return True if given object is into process.
///

bool objectIsInProcess(Object *o, bool matchAssigns = true, bool matchStarting = true);

/// @brief Returns the wrapping process.
/// In case of StateTables and Assigns without parent, returns them.
///
/// @param o The object to be checked.
/// @param matchAssigns True if also globact assings must be considered. Default is true.
/// @param matchStarting True if o can be directly a process. Default is true.
/// @return True if given object is into process.
///

Object *objectGetProcess(Object *o, bool matchAssigns = true, bool matchStarting = true);
/// @brief Returns true if given object is standard.
/// In case of has no standard flag it returns false.
///
/// @param o The object to be checked.
/// @return True if given object is a standard.
///

bool objectIsStandard(Object *o);

/// @brief Sets the given object standard flag.
/// In case of has no standard flag it does nothing.
///
/// @param o The object to be modified.
/// @param isStandard The new value.
///

void objectSetStandard(Object *o, bool isStandard);

/// @brief Returns true if given object is external.
/// In case of has no external flag it returns false.
///
/// @param o The object to be checked.
/// @return True if given object is external.
///

bool objectIsExternal(Object *o);

/// @brief Sets the given object external flag.
/// In case of has no external flag it does nothing.
///
/// @param o The object to be modified.
/// @param isExternal The new value.
///

void objectSetExternal(Object *o, bool isExternal);
/// @brief Checks whether given object is a nullptr pointer.
///
/// @param o The object to be checked.
/// @param sem The language semantics.
/// @return True when it is a nullptr pointer representation.
///

bool objectIsNUllPointer(Object *o, semantics::ILanguageSemantics *sem);
/// @brief Return the object port direction field.
/// If no direction field is present, dir_none is returned.
/// @param o The object.
/// @return The direction.

hif::PortDirection objectGetPortDirection(Object *o);

/// @brief Set the object port direction field.
/// If no direction field is present, does nothing.
/// @param o The object.
/// @param dir The direction.

void objectSetPortDirection(Object *o, const hif::PortDirection dir);

} // namespace hif
