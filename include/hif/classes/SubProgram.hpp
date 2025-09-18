/// @file SubProgram.hpp
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

/// @brief Abstract class for subprograms.
///
/// @details
/// This class is an abstract class for subprograms (i.e., functions
/// and procedures).
///
/// @see Function, Procedure
class SubProgram : public Scope
{
public:
    /// @name Traits.
    /// @{

    /// @brief List of parameters of the subprogram.
    BList<Parameter> parameters;

    /// @brief List of template parameters of the subprogram.
    BList<Declaration> templateParameters;

    /// @brief The kind of the subprogram.
    enum Kind : unsigned char {
        INSTANCE,
        VIRTUAL,
        STATIC,
        MACRO,
        IMPLICIT_INSTANCE,
    };

    /// @}

    /// @brief Constructor.
    SubProgram();

    /// @brief Destructor.
    virtual ~SubProgram();

    /// @brief Returns the state table of the subprogram.
    /// @return The state table of the subprogram.
    StateTable *getStateTable() const;

    /// @brief Sets the state table of the subprogram.
    /// @param s The state table of the subprogram to be set.
    /// @return The old state table of the subprogram, or nullptr if none.
    StateTable *setStateTable(StateTable *s);

    /// @brief Returns the kind of the subprogram.
    /// @return The kind of the subprogram.
    Kind getKind() const;

    /// @brief Sets the kind of the subprogram.
    /// @param k The kind of the subprogram to be set.
    void setKind(Kind k);

    /// @brief Returns whether this is a standard declaration.
    /// @return <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    bool isStandard() const;

    /// @brief Sets whether this is a standard declaration.
    /// @param standard <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    void setStandard(const bool standard);

    /// @brief Return string representation of given subprogram kind.
    /// @param t The subprogram kind.
    /// @return The string representation of given subprogram kind.
    static std::string kindToString(const Kind t);

    /// @brief Return the subprogram kind for given string.
    /// @param s The string.
    /// @return The subprogram kind for given string.
    static Kind kindFromString(const std::string &s);

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

protected:
    /// @brief The state table of the subprogram.
    StateTable *_statetable;

    /// @brief The kind of the subprogram.
    Kind _kind;

    /// @brief Distinguishes between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

private:
    // K: disabled.
    SubProgram(const SubProgram &);
    SubProgram &operator=(const SubProgram &);
};

} // namespace hif
