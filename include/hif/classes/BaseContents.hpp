/// @file BaseContents.hpp
/// @brief Base class for contents of HIF composite objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BList.hpp"
#include "hif/classes/Scope.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief Abstract class for content classes (i.e., Contents and Generate).
/// @see: Contents, Generate.
class BaseContents : public Scope
{
public:
    /// @brief List of declarations.
    BList<Declaration> declarations;
    /// @brief List of state tables (i.e., processes).
    BList<StateTable> stateTables;
    /// @brief List of component instances.
    BList<Instance> instances;
    /// @brief List of generate constructs.
    BList<Generate> generates;

    /// @brief Constructor.
    BaseContents();

    /// @brief Destructor.
    virtual ~BaseContents();

    BaseContents(const BaseContents &)                     = delete;
    auto operator=(const BaseContents &) -> BaseContents & = delete;

    /// @brief Sets concurrent actions in the content.
    /// @param g The new concurrent actions to be set.
    /// @return The old concurrent actions if they differ from
    /// the new ones, nullptr otherwise.
    GlobalAction *setGlobalAction(GlobalAction *g);

    /// @brief Return concurrent actions in the content.
    /// @return The concurrent actions in the content.
    GlobalAction *getGlobalAction() const;

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of the given child relative to this object.
    /// @param child Pointer to the child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of the given BList relative to this object.
    /// @param list Reference to the BList of Object instances.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The concurrent actions in the content.
    GlobalAction *_globalAction;
};

} // namespace hif
