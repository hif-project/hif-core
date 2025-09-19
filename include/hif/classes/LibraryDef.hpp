/// @file LibraryDef.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Scope.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

/// @brief Library definition.
///
/// @details
/// This class represents a library definition.
/// A library definition could represent a VHDL <tt>package</tt>, or it can
/// be used to define C++ classes to be used by manipulation tools.
class LibraryDef : public Scope
{
public:
    /// @brief List of libraries used by the library definition.
    BList<Library> libraries;

    /// @brief List of declarations in the library definition.
    BList<Declaration> declarations;

    /// @brief Constructor.
    LibraryDef();

    /// @brief Destructor.
    virtual ~LibraryDef();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the language ID of the library definition.
    /// @return The language ID of the library definition.
    hif::LanguageID getLanguageID() const;

    /// @brief Sets the language ID of the library definition.
    /// @param languageID The language ID of the library definition to be set.
    /// @return The language ID previously associated to the library definition.
    hif::LanguageID setLanguageID(const hif::LanguageID languageID);

    /// @brief Returns whether this is the definition of a standard library.
    /// @return <tt>true</tt> if this is the definition of a standard library.
    bool isStandard() const;

    /// @brief Sets whether this is the definition of a standard library.
    /// @param standard <tt>true</tt> if this is the definition of a standard library, <tt>false</tt> otherwise.
    void setStandard(bool standard);

    /// @brief Sets whether the linkage of C++ code has C linkage.
    /// @param cLinkage The new value.
    void setCLinkage(bool cLinkage);

    /// @brief Sets whether the linkage of C++ code has C linkage.
    /// @return The current value.
    bool hasCLinkage() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given BList w.r.t. this.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The implementation language of this library definition. This field
    /// is set by front-end or manipulation tools, and it is used by back-end
    /// tools to determine how the library definition is represented after translation.
    hif::LanguageID _languageID;

    /// @brief Distinguishes between the definition of a standard library
    /// (e.g., TLM for SystemC or IEEE.numeric_std for VHDL) from a library
    /// definition which is part of a design.
    bool _isStandard;
    /// @brief Generates C++ code, wrapped by "extern C" linkage.
    bool _hasCLinkage;
};

} // namespace hif
