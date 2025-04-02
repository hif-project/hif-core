/// @file StateTable.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BList.hpp"
#include "hif/classes/Scope.hpp"
#include "hif/classes/Transition.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

/// @brief A State table.
///
/// @details
/// A state table defines a process whose main control structure is a finite
/// state machine. The entry state of the machine can be specified
/// explicitly. Otherwise, the first state in the state list is considered as
/// entry state.
///
/// Additionally, a state table can have a sensitivity list which contains
/// the names of certain signals. The transitions of the state machine are
/// triggered by this sensitivity list: whenever an edge on any of the
/// signals in the list occurs, the state machine performs a transition.
///
/// A state table may have local declarations.
///
/// Note that state tables can describe sequential as well as combinational
/// processes: in the case of a combinational process, the sensitivity list
/// must contain all the signals which are read in the state table.
///
/// <b>Syntax</b>
///@verbatim
///(STATETABLE identifier
///            [entrystate]
///            [sensitivity_list]
///            {declaration}
///            {state}
///)
///@endverbatim
///
class StateTable : public Scope
{
public:
    /// @name Variable lists.
    //@{

    /// @brief
    /// The list of local declarations.
    /// Used also by the formal model as list of internal variables.
    BList<Declaration> declarations;

    // @brief List of input variables.
    // See the formal model description.
    //BList<Declaration> inputDeclarations;

    // @brief List of output variables.
    // See the formal model description.
    // BList<Declaration> outputDeclarations;

    //@}

    /// @name Event lists.
    //@{

    /// @brief The generic sensitivity list of the state table.
    /// Used also by the formal model as list of input events.
    BList<Value> sensitivity;

    /// @brief The positive sensitivity list of the state table.
    BList<Value> sensitivityPos;

    /// @brief The negative sensitivity list of the state table.
    BList<Value> sensitivityNeg;

    // @brief List of internal events.
    // See the formal model description.
    //BList<Value> internalSensitivity;

    // @brief List of output (generated) events.
    // See the formal model description.
    //BList<Value> outputSensitivity;

    //@}

    /// @brief
    /// The list of states of the statetable.
    ///
    BList<State> states;

    /// @brief List of edges.
    /// See the computational formal model.
    /// If the list is empty, then the ST is in the "old style" model, i.e.,
    /// many states, with actions inside them, and transitions embedded into the action list..
    BList<Transition> edges;

    StateTable();

    virtual ~StateTable();

    /// @brief Returns a string representing the class name.
    ///
    /// @return The string.
    ///
    ClassId getClassId() const;

    /// @brief
    /// Get the entry state, that is the initial state.
    ///
    /// @return The entry state.
    ///
    State *getEntryState();

    /// @brief
    /// Updates the entry state.
    ///
    /// @param s The new entry state.
    ///
    void setEntryState(State *s);

    /// @brief
    /// Rename the entry state.
    ///
    /// @param s The new name of the entry state.
    ///
    void setEntryStateName(const std::string &s);

    /// @brief
    /// Get the entry state name.
    ///
    /// @return The entry state name.
    ///
    std::string getEntryStateName();

    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Search for a state matching passed name.
    ///
    /// @param name The name f the state.
    /// @return The state, or nullptr in case of failure.
    ///
    State *findState(const std::string &name);

    /// @brief Set the process flavour.
    void setFlavour(ProcessFlavour f);

    /// @brief Returns the flavour.
    ProcessFlavour getFlavour() const;

    /// @brief Set whether perform initialization at time 0.
    void setDontInitialize(bool dontInitialize);

    /// @brief Get whether perform initialization at time 0.
    bool getDontInitialize() const;

    /// @brief Function to know if this is a standard declaration.
    /// @return True if this is a standard declaration.
    ///
    bool isStandard() const;

    /// @brief Function set if this is a standard declaration.
    /// @param standard The boolean that identifies if this is a standard
    /// declaration.
    ///
    void setStandard(bool standard);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given BList w.r.t. this.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    std::string _entryState;

    /// @brief The process type
    ProcessFlavour _flavour;

    /// @brief Avoid the initialization of process at time 0, which is usually
    /// performed by SystemC kernel simulation (but not by VHDL or Verilog).
    bool _dontInitialize;

    /// @brief Distinguish between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

    // K: disabled
    StateTable(const StateTable &);
    StateTable &operator=(const StateTable &);
};

} // namespace hif
