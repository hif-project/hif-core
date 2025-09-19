/// @file Library.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ReferencedType.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

///	@brief Reference to a library.
///
/// @details
/// This class represents a reference to a library, which is typically
/// included in order to use the components defined in it.
class Library : public ReferencedType, public features::TemplateSymbolIf<LibraryDef>
{
public:
    /// @brief Constructor.
    /// The name of the library is set to <tt>NAME_NONE</tt>
    /// and the name of the file to <tt>""</tt>.
    Library();

    /// @brief Destructor.
    virtual ~Library();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the name of the file containing the library definition.
    /// @return The name of the file containing the library definition.
    const std::string &getFilename() const;

    /// @brief Sets the name of the file containing the library definition.
    /// @param v The name of the file containing the library definition.
    void setFilename(const std::string &v);

    /// @brief Returns whether the referenced library is a standard library.
    /// @return <tt>true</tt> if the referenced library is a standard library.
    bool isStandard() const;

    /// @brief Sets whether the referenced library is a standard library.
    /// @param standard <tt>true</tt> if the referenced library is a standard library, <tt>false</tt> otherwise.
    void setStandard(bool standard);

    /// @brief Returns whether the referenced library is to be included as
    /// a system include or as a local include.
    /// @return <tt>true</tt> if the referenced library is to be included
    /// as a system include, <tt>false</tt> if it is to be included as a
    /// local include.
    bool isSystem() const;

    /// @brief Sets whehter the referenced library is to be included as
    /// a system include or as a local include.
    /// @param system <tt>true</tt> if the referenced library is to be included
    /// as a system include, <tt>false</tt> if it is to be included as a
    /// local include.
    void setSystem(bool system);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The name of the file containing the library definition.
    std::string _filename;

    /// @brief Distinguishes whether the referenced library is a standard
    /// library or if it is part of a design.
    bool _isStandard;

    /// @brief Distinguishes whether the referenced library is to be included
    /// as a system include or as a local include.
    bool _isSystem;

    // K: disabled
    Library(const Library &);
    Library &operator=(const Library &);
};

} // namespace hif
