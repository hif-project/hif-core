/// @file fixMultipleSignalPortAssigns.hpp
/// @brief Handles issues related to multiple writes on signals or ports.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#endif

namespace hif
{
namespace manipulation
{

/// @brief Options for fixing multiple signal/port assignments.
struct FixMultipleSignalPortAssignsOptions {
    bool fixMultipleWritings; ///< Fix multiple signal/port writings. Default: true.
    bool fixPartialWritings;  ///< Fix partial signal/port writings. Default: true.
    bool
        fixSubrangesInSensitivity; ///< Replace members/slices in sensitivities with prefix expressions. Default: false.

    /// @brief Default constructor initializing options.
    FixMultipleSignalPortAssignsOptions();

    /// @brief Destructor.
    ~FixMultipleSignalPortAssignsOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    FixMultipleSignalPortAssignsOptions(const FixMultipleSignalPortAssignsOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    FixMultipleSignalPortAssignsOptions &operator=(FixMultipleSignalPortAssignsOptions other);

    /// @brief Swaps the contents of two options objects.
    /// @param other Options to swap with.
    void swap(FixMultipleSignalPortAssignsOptions &other);
};

/// @brief Manage the multiple write on signal or port.
/// @details
/// It is used for avoid problem of multiple write of single bits or slices on a
/// port/signal in the same duty cycle.
/// Here an example:
/// @code
/// // declarations
/// sc_signal<sc_bv<8> > sig;
/// sc_bv<4> sig1;
/// sc_bv<4> sig2;
///
/// // processes
/// void module::process1 () {
///     sig.range(3, 0) = sig1;
///     if (sig.read().range(7, 4) == "0000")
///         sig = "11111111";
/// }
/// @endcode
///
/// @n This code does not compile in SystemC, because you cannot write a slice
/// or a member of a signal. You can only write the whole signal. In HIF it
/// is allowed (because for example in VHDL and in Verilog you can do it).
/// So we have to handle this case in some way.
/// The idea is to use a temporary variable, write a slice of it and return
/// the value to the signal.
/// This is the resulting code:
/// @code
/// // declarations
/// sc_signal<sc_bv<8> > sig;
/// sc_bv<4> sig1;
/// sc_bv<4> sig2;
///
/// // processes
/// void module::process1 () {
///     sc_bv<8> sig_var;                       // declaration
///     sig_var = sig.read();                   // get the old value
///     sig_var.range(3, 0) = sig1;             // assign
///     if (sig.read().range(7, 4) == "0000")   // use the signal here
///         sig_var = "11111111";               // use the variable here
///     sig = sig_var;                          // update the whole signal
/// }
/// @endcode
///
/// @n In this way we get the old value from the signal, we update only the
/// slice needed and after we update the whole signal with the new value
/// (that is composed by the updated value in the selected range and the old
/// value in the other parts of the signal).
/// If the signal is assigned in other statements of the process we have to
/// use the variable instead. If the signal is read inside the process we
/// have to use the signal and not the variable, because the variable has the
/// new value, and not the old one! (signal values are updated in the next
/// delta cycle and not immediately).
///
/// Consider a change to the previous example:
/// @code
/// // declarations
/// sc_signal<sc_bv<8> > sig;
/// sc_bv<4> sig1;
/// sc_bv<4> sig2;
///
/// // processes
/// void module::process1 () {
///     sig.range(3, 0) = sig1;
/// }
/// void module::process2 () {
///     sig.range(7, 4) = sig2;
/// }
/// @endcode
///
/// @n Here we have two processes that writes a slice of the signal. The
/// previous fix does not work because the result is this:
/// @code
/// // declarations
/// sc_signal<sc_bv<8> > sig;
/// sc_bv<4> sig1;
/// sc_bv<4> sig2;
///
/// // processes
/// void module::process1 () {
///     sc_bv<8> sig_var;                       // declaration
///     sig_var = sig.read();                   // get the old value
///     sig_var.range(3, 0) = sig1;             // assign
///     sig = sig_var;                          // update the whole signal
/// }
/// void module::process2 () {
///     sc_bv<8> sig_var;                       // declaration
///     sig_var = sig.read();                   // get the old value
///     sig_var.range(7, 4) = sig2;             // assign
///     sig = sig_var;                          // update the whole signal
/// }
/// @endcode
///
/// @n In this case we have two conflicting assignments (sig = sig_var) that
/// updates the whole signal, while the original VHDL code updates only a
/// slice of the signal. To avoid this we add a new array of signals
/// representing the old signal. All the writes to the old signal must be
/// done to the new array. We add a new process sensitive to all the signals
/// of the array. This process updates the old signal.
/// Resulting code:
/// @code
/// // declarations
/// sc_signal<sc_bv<8> > sig;
/// sc_bv<4> sig1;
/// sc_bv<4> sig2;
/// sc_bit sig_mspw[8];             // new array of signals
///
/// // processes
/// void module::process1 () {
///     // support function in hif_utils.hpp
//      // it represents sig_mspw.range(3, 0) = sig1
///     hif_array_slice_assign(sig_mspw, sig1, 3, 0, 4);
/// }
/// void module::process2 () {
///     // support function in hif_utils.hpp
//      // it represents sig_mspw.range(7, 4) = sig2
///     hif_array_slice_assign(sig_mspw, sig2, 7, 4, 4);
/// }
/// // sensible to all the signals of sig_mspw
/// void module::sig_assign () {
///     sc_bv<8> sig_var;
///     for (int i = 0; i < 8; ++i)
///         sig_var[i] = sig_mspw[i];
///     sig = sig_var;
/// @endcode
///
/// In this way we have an unique process that updates the whole signal,
/// therefore no conflicts are present during the execution
/// @param o The system node to process.
/// @param sem The semantics interface for language-specific rules.
/// @param opts Options for fixing signal port assignments (optional, defaults to `FixMultipleSignalPortAssignsOptions()`).
/// @return `true` if the operation is successful; otherwise, `false`.

bool fixMultipleSignalPortAssigns(
    System *o,
    hif::semantics::ILanguageSemantics *sem,
    const FixMultipleSignalPortAssignsOptions &opts = FixMultipleSignalPortAssignsOptions());

} // namespace manipulation
} // namespace hif

#ifdef __clang__
#    pragma clang diagnostic pop
#endif
