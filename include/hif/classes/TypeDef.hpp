/// @file TypeDef.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/TypeDeclaration.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief User-defined type.
///
/// @details
/// This class represents the declaration of a user-defined type, thus
/// providing an alias for such a type.
class TypeDef : public TypeDeclaration
{
public:
    /// @brief List of template parameters of the type.
    BList<Declaration> templateParameters;

    /// @brief Constructor.
    TypeDef();

    /// @brief Destructor.
    virtual ~TypeDef();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the optional range constraint on the type.
    /// @return The range constraint on the type, nullptr if none.
    Range *getRange() const;

    /// @brief Sets the optional range constraint on the type.
    /// @param r The range constraint to be set on the type.
    /// @return The old range constraint, nullptr if none.
    Range *setRange(Range *r);

    /// @brief Returns the opacity of the type definition.
    /// @return The opacity of the type definition.
    bool isOpaque() const;

    /// @brief Sets the opacity of the type definition.
    /// @param is_opaque <tt>true</tt> if the type definition is opaque, <tt>false</tt> otherwise.
    void setOpaque(bool is_opaque);

    /// @brief Returns whether this is a standard declaration.
    /// @return <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    bool isStandard() const;

    /// @brief Sets whether this is a standard declaration.
    /// @param standard <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    void setStandard(bool standard);

    /// @brief Returns whether this typedef is a bookmark for a type defined elsewhenre.
    /// @return <tt>true</tt> if this is an external declaration, <tt>false</tt> otherwise.
    bool isExternal() const;

    /// @brief Sets whether this typedef is a bookmark for a type defined elsewhenre.
    /// @param external <tt>true</tt> if this is an external declaration, <tt>false</tt> otherwise.
    void setExternal(bool external);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The optional range constraint on the type.
    Range *_range;

    /// @brief Flag to store the opacity of the reference.
    bool _isOpaque;

    /// @brief Distinguishes between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

    /// @brief Sets the declaration as bookmark for an unknown actual implementation.
    bool _isExternal;

    // K: disabled
    TypeDef(const TypeDef &);
    TypeDef &operator=(const TypeDef &);
};

} // namespace hif
