/// @file State.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <list>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BList.hpp"
#include "hif/classes/Declaration.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief State in an finite state machine.
///
/// @details
/// A state s of a state table is identified by a unique name, and has a list
/// of actions associated to it. This action list is executed sequentially
/// whenever the state machine is in state s and performs a transition.
///
/// The transition relation of the state machine is defined by the action
/// list using the @c NEXTSTATE statement: whenever an execution path of the
/// action list terminates with a @c NEXTSTATE statement, the state machine
/// will enter the state specified in the statement. If the last statement of
/// an execution path is not a @c NEXTSTATE statement, the state machine will
/// stay in the current state.
///
/// Note that state machines defined by state tables are necessarily
/// deterministic due to the semantics of sequential statement lists: if more
/// than one execution paths exist whose conditions are true, it is always
/// the first encountered path (where the order of the paths is given by the
/// order of the actions in the list) which is executed.
///
/// <b>Syntax</b>
///@verbatim
///state ::= (STATE state_name_id priority {action} {property} )
///@endverbatim
///
class State : public Declaration
{
public:
    /// @brief The state priority. See the formal model description.
    typedef unsigned long int priority_t;

    /// @brief List of edges.
    typedef std::list<Transition *> EdgeList_t;

    /// @brief The action list.
    BList<Action> actions;

    /// @brief The state invariant, expressed as a list of implicitly conjunct
    /// conditions.
    BList<Value> invariants;

    // @brief List of edges starting from this state.
    // See the formal model description.
    //BList< Transition > nextEdges;

    // @brief List of edges ending in this state.
    // See the formal model description.
    //BList< Transition > prevEdges;

    State();

    virtual ~State();

    /// @brief Returns a string representing the class name.
    ///
    /// @return The string.
    ///
    ClassId getClassId() const;

    virtual int acceptVisitor(HifVisitor &vis);

    /// @name Methods related to MoC state manipulation.
    //@{

    /// @brief Gets all edges with this state as destination.
    /// @return List of incoming edges.
    EdgeList_t getInEdges();

    /// @brief Gets all edges with this state as source.
    /// @return List of outgoing edges.
    EdgeList_t getOutEdges();

    /// @brief Sets the edge priority.
    /// @param p The priority value to set.
    void setPriority(const priority_t p);

    /// @brief Sets the edge priority.
    /// @return The current priority value.
    priority_t getPriority() const;

    /// @brief Sets the flag atomic.
    /// @param v The atomic flag value to set.
    void setAtomic(const bool v);

    /// @brief Gets the flag atomic.
    /// @return True if the state is atomic, false otherwise.
    bool isAtomic() const;

    //@}

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief This state priority. Zero means no priority. One means maximum priority.
    priority_t _priority;

    /// @brief Flag that identifies if state in atomic..
    bool _atomic;
};

} // namespace hif
