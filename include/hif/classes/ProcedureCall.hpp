/// @file ProcedureCall.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

///	@brief Call to a procedure.
///
/// @details
/// This class represents a call to a procedure.
///
/// @see Procedure
class ProcedureCall : public Action, public features::TemplateSymbolIf<SubProgram>, public features::INamedObject
{
public:
    /// @brief Map to native call return type.
    typedef Action CallType;

    /// @brief List of template arguments (i.e., assignments to template formal procedure parameters).
    BList<TPAssign> templateParameterAssigns;

    /// @brief List of arguments (i.e., assignment to formal procedure parameters).
    BList<ParameterAssign> parameterAssigns;

    /// @brief Constructor.
    ProcedureCall();

    /// @brief Destructor.
    virtual ~ProcedureCall();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the calling object for the procedure call.
    /// @param v The calling object to set.
    /// @return The previous calling object.
    Value *setInstance(Value *v);

    /// @brief Returns the calling object for the procedure call.
    /// @return The calling object.
    Value *getInstance() const;

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
    ///@brief The calling object.
    Value *_instance;

    // K: disabled
    ProcedureCall(const ProcedureCall &);
    ProcedureCall &operator=(const ProcedureCall &);
};

} // namespace hif
