/// @file Transition.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/NameTable.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief Next state.
///
/// @details
/// This action specifies the next state and terminates the transition.
class Transition : public Action
{
public:
    /// @name Traits.
    //@{

    /// @brief The edge priority. See the formal model description.
    typedef unsigned long int priority_t;

    //@}

    /// @brief Constructor
    ///
    /// Create a new Transition.
    ///
    Transition();

    /// @brief Destructor
    ///
    virtual ~Transition();

    /// @brief Returns a string representing the class name.
    ///
    /// @return The string.
    ///
    ClassId getClassId() const;

    /// @brief Function to change the name of next state.
    ///
    /// @param n The name representing next state's new name.
    ///
    void setName(const std::string &n);

    /// @brief Function to get the name of next state.
    ///
    /// @return The name representing next state's name.
    ///
    std::string getName() const;

    /// @brief Function to change the name of prev state.
    ///
    /// @param n The name representing prev state's new name.
    ///
    void setPrevName(const std::string &n);

    /// @brief Function to get the name of prev state.
    ///
    /// @return The name representing prev state's name.
    ///
    std::string getPrevName() const;

    /// @brief Function to visit the current object.
    ///
    /// @param vis visitor of type HifVisitor.
    ///
    /// @return Integer representing visit result. Default 0.
    ///
    virtual int acceptVisitor(HifVisitor &vis);

    /// @name Computational model related methods and variables.
    //@{

    /// @brief Sets the edge priority.
    /// @param p The priority value to set.
    void setPriority(const priority_t p);

    /// @brief Gets the edge priority.
    /// @return The current priority value.
    priority_t getPriority() const;

    /// @brief Composes all enablingList expressions into a single expression.
    /// The resulting expression will be the only object into the enablingList.
    void enablingListToExpression();

    /// @brief List of conditions on events (labels).
    /// The labels are considered in AND.
    BList<Value> enablingLabelList;

    /// @brief Function to set the value of the enablingLabelOrMode flag.
    ///
    /// @param flag boolean value of the enablingLabelOrMode flag.
    ///
    void setEnablingOrCondition(bool flag);

    /// @brief Function to get the value of the enablingLabelOrMode flag.
    ///
    /// @return boolean value of the enablingLabelOrMode flag.
    ///
    bool getEnablingOrCondition() const;

    /// @brief List of conditions on variables (enabling function).
    /// The expressions are considered in AND.
    ///
    BList<Value> enablingList;

    /// @brief List of labels (events) to be fired when the edge is traversed (update label function).
    ///
    BList<Value> updateLabelList;

    /// @brief List of actions to be performed when the edge is traversed (update function).
    /// The actions are considered sequential.
    ///
    BList<Action> updateList;

    //@}

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief Reference to the next state.
    ///
    std::string _name;

    /// @brief Reference to the previous state.
    std::string _prevName;

    /// @brief This edge priority. Zero means no priority. One means maximum priority.
    priority_t _priority;

    /// @brief Flag to indicate if the enabling labels have to be considered in
    /// in OR. Default value is false, to consider enabling labels in AND.
    ///
    bool _enablingLabelOrMode;

    // K: disabled
    Transition(const Transition &);
    Transition &operator=(const Transition &);
};

} // namespace hif
