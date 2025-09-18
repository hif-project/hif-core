/// @file System.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Scope.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief System description.
///
/// @details
/// This class represents a HIF description of a system.
/// A System object is the root node of the HIF tree representing a
/// system description.
/// It may contain library definitions which define new data types,
/// subprograms and constants.
/// It then contains a number of design units describing the actual
/// components of the system.
///
/// @see LibraryDef, DesignUnit
class System : public Scope
{
public:
    /// @brief The list of library definitions of the system.
    BList<LibraryDef> libraryDefs;

    /// @brief The list of design units of the system.
    BList<DesignUnit> designUnits;

    /// @brief The list of global declarations.
    BList<Declaration> declarations;

    /// @brief The list of libraries used by global declarations.
    BList<Library> libraries;

    /// @brief The list of global actions.
    BList<Action> actions;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Struct containing information about the current version of HIF.
    struct VersionInfo {
        /// @brief Type for version numbers.
        typedef unsigned long long VersionNumber;

        VersionInfo();
        ~VersionInfo();
        /// @brief Copy constructor.
        /// @param other The object to copy from.
        VersionInfo(const VersionInfo &other);
        /// @brief Assignment operator.
        /// @param other The object to assign from.
        /// @return Reference to this object.
        VersionInfo &operator=(VersionInfo other);
        /// @brief Swap function.
        /// @param other The object to swap with.
        void swap(VersionInfo &other);

        /// @brief The HIF release version.
        std::string release;

        /// @brief The tool that generated the HIF description.
        std::string tool;

        /// @brief The generation date.
        std::string generationDate;

        /// @brief The HIF tree format version major number.
        VersionNumber formatVersionMajor;

        /// @brief The HIF tree format version minor number.
        VersionNumber formatVersionMinor;
    };

    /// @brief Sets the version information structure.
    /// @param info The version information structure to be set.
    void setVersionInfo(const VersionInfo &info);

    /// @brief Returns the version information structure.
    /// @return The version information structure.
    VersionInfo getVersionInfo() const;

    /// @brief Returns the language ID of the system description.
    /// @return The language ID of the system description.
    hif::LanguageID getLanguageID() const;

    /// @brief Sets the language ID of the system description.
    /// @param languageID The language ID of the system description to be set.
    /// @returns The language ID previously associated to the system description.
    hif::LanguageID setLanguageID(hif::LanguageID languageID);

    /// @brief Constructor.
    System();

    /// @brief Destructor.
    virtual ~System();

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Version information structure.
    VersionInfo _version;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The implementation language. This field is set by front-end or
    /// manipulation tools, and it is used by back-end tools to determine how
    /// global declarations (e.g., functions, procedures, design units, etc.)
    /// must be represented after the translation.
    hif::LanguageID _languageID;
};

} // namespace hif
