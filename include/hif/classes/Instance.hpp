/// @file Instance.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BList.hpp"
#include "hif/classes/Value.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

/// @brief Instance of a view of a design unit, or of a library.
///
/// @details
/// This class represents an instance of a view of a design unit, or
/// of a library.
/// In case it refers to a specific view of a design unit, it is used to
/// define an instance of sub-module. As such, it is possible to specify
/// its corresponding port bindings and template arguments.
/// Instead, if it refers to a library, it is used as a placeholder to
/// exploit its type )i.e., a sub-class of ReferencedType).
class Instance : public Value, public features::TemplateSymbolIf<Entity>, public features::INamedObject
{
public:
    /// @brief List of port bindings to the referenced view.
    BList<PortAssign> portAssigns;

    /// @brief Constructor.
    Instance();

    /// @brief Destructor.
    virtual ~Instance();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the view or library referenced by the instance.
    /// @return The view or library referenced by the instance.
    ReferencedType *getReferencedType() const;

    /// @brief Sets the view or library referenced by the instance.
    /// @param v The view or library referenced by the instance.
    /// @return The old view or library referenced if it is different
    /// from the new one, nullptr otherwise.
    ReferencedType *setReferencedType(ReferencedType *v);

    /// @brief Returns the initial value of the instance.
    /// @return The initial value of the instance.
    Value *getValue() const;

    /// @brief Sets the initial value of the instance.
    /// @param v The initial value of the instance to be set.
    /// @return The old initial value of the instance if it is
    /// different from the new one, nullptr otherwise.
    Value *setValue(Value *v);

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

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The view or library referenced by the instance.
    ReferencedType *_referencedType;

    /// @brief The initial value of the instance.
    Value *_initialValue;

    // K: disabled
    Instance(const Instance &);
    Instance &operator=(const Instance &);
};

} // namespace hif
