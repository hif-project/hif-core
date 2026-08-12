/// @file View.hpp
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

/// @brief View of a design unit.
///
/// @details
/// This class represents a view of a design unit (i.e., a specific
/// implementation of the design unit).
///
/// @see DesignUnit
class View : public Scope
{
public:
    /// @brief List of references to used libraries.
    BList<Library> libraries;

    /// @brief List of template parameters of the view.
    BList<Declaration> templateParameters;

    /// @brief List of declarations in the scope of the view.
    BList<Declaration> declarations;

    /// @brief List of references to views of design units from which the view inherits.
    BList<ViewReference> inheritances;

    /// @brief Constructor.
    View();

    /// @brief Destructor.
    virtual ~View();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the entity describing the interface of the view.
    /// @return The entity describing the interface of the view.
    Entity *getEntity() const;

    /// @brief Sets the entity describing the interface of the view.
    /// @param i The entity describing the interface of the view to be set.
    /// @return The entity describing the interface previously associated to
    /// the view.
    Entity *setEntity(Entity *i);

    /// @brief Returns the contents of the view.
    /// @return The contents of the view.
    Contents *getContents() const;

    /// @brief Sets the contents of the view.
    /// @param c The contents of the view to be set.
    /// @return The contents previously associated to the view.
    Contents *setContents(Contents *c);

    /// @brief Returns the language ID of the view.
    /// @return The language ID of the view.
    hif::LanguageID getLanguageID() const;

    /// @brief Sets the language ID of the view.
    /// @param languageID The language ID of the view to be set.
    /// @return The language ID previously associated to the view.
    hif::LanguageID setLanguageID(const hif::LanguageID languageID);

    /// @brief Returns whether this is a standard declaration.
    /// @return <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    bool isStandard() const;

    /// @brief Sets whether this is a standard declaration.
    /// @param standard <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    void setStandard(bool standard);

    /// @brief Returns the name of the file containing the view definition.
    /// @return The name of the file containing the view definition.
    const std::string &getFilename() const;

    /// @brief Sets the name of the file containing the view definition.
    /// @param v The name of the file containing the view definition.
    void setFilename(const std::string &v);

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
    /// @brief The contents of the view.
    Contents *_contents;

    /// @brief The entity describing the interface of the view.
    Entity *_entity;

    /// @brief The implementation language of this view. This field is set by
    /// front-end or manipulation tools, and it is used by back-end tools to
    /// determine how the view (or, more generally, the entire design unit) must
    /// be represented after the translation. This affects the whole contents
    /// of the view.
    hif::LanguageID _languageID;

    /// @brief Distinguishes between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

    /// @brief The name of the file containing the view definition.
    std::string _filename;

    // K: disabled
    View(const View &);
    View &operator=(const View &);
};

} // namespace hif
