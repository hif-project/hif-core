/// @file hifEnums.hpp
/// @brief Enumerations and utility functions for HIF data structures.
/// @details Provides enums for range directions, port directions, bit constants,
/// and operators, along with helper functions to convert between enums and strings.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{

/// @brief Directions for ranges (e.g., array or bit ranges).
enum RangeDirection : unsigned char {
    dir_upto,  ///< Range from lower to higher index (e.g., `0 to 7`).
    dir_downto ///< Range from higher to lower index (e.g., `7 downto 0`).
};

/// @brief Converts a RangeDirection enum to its string representation.
/// @param t The range direction.
/// @return String representation of the range direction.

std::string rangeDirectionToString(const RangeDirection t);

/// @brief Parses a string to determine the corresponding RangeDirection.
/// @param s The string representation of the range direction.
/// @return The corresponding RangeDirection enum.

RangeDirection rangeDirectionFromString(const std::string &s);

/// @brief Directions for ports or parameters in hardware design.
enum PortDirection : unsigned char {
    dir_none, ///< No specific direction.
    dir_in,   ///< Input direction.
    dir_out,  ///< Output direction.
    dir_inout ///< Bidirectional (input and output) direction.
};

/// @brief Converts a PortDirection enum to its string representation.
/// @param t The port direction.
/// @return String representation of the port direction.

std::string portDirectionToString(const PortDirection t);

/// @brief Parses a string to determine the corresponding PortDirection.
/// @param s The string representation of the port direction.
/// @return The corresponding PortDirection enum.

PortDirection portDirectionFromString(const std::string &s);

/// @brief Possible bit values used in digital logic.
enum BitConstant : unsigned char {
    bit_u,       ///< Undefined bit.
    bit_x,       ///< Unknown value (X).
    bit_zero,    ///< Logical 0.
    bit_one,     ///< Logical 1.
    bit_z,       ///< High-impedance state (Z).
    bit_w,       ///< Weak unknown.
    bit_l,       ///< Weak 0.
    bit_h,       ///< Weak 1.
    bit_dontcare ///< Don't-care condition.
};

/// @brief Converts a BitConstant enum to its string representation.
/// @param t The bit constant.
/// @return String representation of the bit constant.

std::string bitConstantToString(const BitConstant t);

/// @brief Parses a string to determine the corresponding BitConstant.
/// @param s The string representation of the bit constant.
/// @return The corresponding BitConstant enum.

BitConstant bitConstantFromString(const std::string &s);

/// @brief List of all supported operators in HIF.
/// @details Operators include arithmetic, bitwise, logical, comparison, and
/// various specialized operations.
enum Operator : unsigned char {
    op_none,     ///< No operator.
    op_plus,     ///< Addition (`+`).
    op_minus,    ///< Subtraction (`-`).
    op_mult,     ///< Multiplication (`*`).
    op_div,      ///< Division (`/`).
    op_or,       ///< Logical OR (`||`).
    op_bor,      ///< Bitwise OR (`|`).
    op_and,      ///< Logical AND (`&&`).
    op_band,     ///< Bitwise AND (`&`).
    op_xor,      ///< Logical XOR.
    op_bxor,     ///< Bitwise XOR (`^`).
    op_not,      ///< Logical NOT (`!`).
    op_bnot,     ///< Bitwise NOT (`~`).
    op_eq,       ///< Equality (`==`).
    op_case_eq,  ///< Case-sensitive equality.
    op_neq,      ///< Inequality (`!=`).
    op_case_neq, ///< Case-sensitive inequality.
    op_le,       ///< Less than or equal to (`<=`).
    op_ge,       ///< Greater than or equal to (`>=`).
    op_lt,       ///< Less than (`<`).
    op_gt,       ///< Greater than (`>`).
    op_sll,      ///< Logical shift left (`<<`).
    op_srl,      ///< Logical shift right (`>>`).
    op_sla,      ///< Arithmetic shift left.
    op_sra,      ///< Arithmetic shift right.
    op_rol,      ///< Rotate left.
    op_ror,      ///< Rotate right.
    op_mod,      ///< Modulus.
    op_rem,      ///< Remainder.
    op_pow,      ///< Power (`**`).
    op_abs,      ///< Absolute value.
    op_concat,   ///< Concatenation.
    op_ref,      ///< Reference operation.
    op_deref,    ///< Dereference operation.
    op_andrd,    ///< Reduction AND.
    op_orrd,     ///< Reduction OR.
    op_xorrd,    ///< Reduction XOR.
    op_assign,   ///< Assignment (`=`).
    op_log,      ///< Logarithmic operation.
    op_conv,     ///< Conversion operation.
    op_bind,     ///< Binding operation.
    op_reverse,  ///< Reverse operation.
    op_size      ///< Size operation.
};

/// @brief Converts an Operator enum to its string representation.
/// @param t The operator.
/// @return String representation of the operator.

std::string operatorToString(const Operator t);

/// @brief Return the string representation of an operator.
/// @details Converts an operator enum to a plain string representation,
/// optionally adding a prefix or suffix. For example, `op_and` becomes `"op_and"`,
/// or with a prefix and suffix, `"<prefix>op_and<suffix>"`.
/// @param t The operator to convert.
/// @param prefix An optional prefix for the string.
/// @param suffix An optional suffix for the string.
/// @return The string representation of the operator.

std::string operatorToPlainString(const Operator t, const std::string &prefix = "", const std::string &suffix = "");

/// @brief Parse a string to determine the corresponding operator.
/// @details Matches the string exactly to an operator enum.
/// @param s The string to parse.
/// @return The corresponding operator enum.

Operator operatorFromString(const std::string &s);

/// @brief Parse a string with optional prefix or suffix to determine the operator.
/// @details For example, `"<prefix>op_and<suffix>"` will return `op_and`.
/// @param s The string to parse.
/// @param prefix An optional prefix to strip before parsing.
/// @param suffix An optional suffix to strip before parsing.
/// @return The corresponding operator enum.

Operator operatorFromPlainString(const std::string &s, const std::string &prefix = "", const std::string &suffix = "");

/// @brief Types of processes in a hardware description.
/// @details Represents different kinds of process semantics.
/// - `pf_method`: Similar to `sc_method` in SystemC. Local variables are automatic
///   and not preserved across awakenings.
/// - `pf_thread`: Similar to `sc_thread` in SystemC. Executes once at simulation start
///   and terminates when the method ends.
/// - `pf_hdl`: Similar to VHDL/Verilog processes. Sensitivity lists determine awakenings.
///   Local variable values are preserved.
/// - `pf_initial`: Verilog initial block, executed once at the start of simulation.
/// - `pf_analog`: Represents analog processes in mixed-signal designs (e.g., Verilog-AMS).
enum ProcessFlavour : unsigned char {
    pf_method,  ///< Similar to `sc_method`.
    pf_thread,  ///< Similar to `sc_thread`.
    pf_hdl,     ///< Similar to VHDL/Verilog processes.
    pf_initial, ///< Verilog initial block.
    pf_analog   ///< AMS analog processes.
};

/// @brief Converts a ProcessFlavour enum to its string representation.
/// @param t The process flavour.
/// @return String representation of the process flavour.

std::string processFlavourToString(const ProcessFlavour t);

/// @brief Parse a string to determine the corresponding process flavour.
/// @param s The string to parse.
/// @return The corresponding ProcessFlavour enum.

ProcessFlavour processFlavourFromString(const std::string &s);

/// @brief Languages supported in the design process.
/// @details Identifies the language or abstraction level of the design.
enum LanguageID : unsigned char {
    rtl, ///< Register Transfer Level (e.g., Verilog/VHDL).
    tlm, ///< Transaction Level Modeling (e.g., SystemC-TLM).
    cpp, ///< C++ codebase.
    c,   ///< C codebase.
    psl, ///< Property Specification Language.
    ams  ///< Analog/Mixed-Signal design.
};

/// @brief Converts a LanguageID enum to its string representation.
/// @param t The language ID.
/// @return String representation of the language ID.

std::string languageIDToString(const LanguageID t);

/// @brief Parse a string to determine the corresponding LanguageID.
/// @param s The string to parse.
/// @return The corresponding LanguageID enum.

LanguageID languageIDFromString(const std::string &s);

/// @brief Predefined properties for design elements.
/// @details Represents specific attributes or requirements associated with objects.
enum PropertyId : unsigned char {
    PROPERTY_CONFIGURATION_FLAG,         ///< Indicates an instance used only as a configuration flag.
    PROPERTY_TLM_FORCEARROW,             ///< Forces the use of the arrow operator (`->`) in C++.
    PROPERTY_REQUIRED_MACRO,             ///< Specifies a macro required by the object (e.g., `#define`).
    PROPERTY_REQUIRED_MACRO_HH,          ///< Specifies macros required in headers.
    PROPERTY_REQUIRED_MACRO_CC,          ///< Specifies macros required in implementation files.
    PROPERTY_UNSUPPORTED,                ///< Marks an unsupported construct.
    PROPERTY_CONSTEXPR,                  ///< Specifies a constexpr call.
    PROPERTY_METHOD_EXPLICIT_PARAMETERS, ///< Marks standard calls with internal array access.
    PROPERTY_TEMPORARY_OBJECT,           ///< Marks the object as temporary.
    PROPERTY_ORIGINAL_BITWIDTH           ///< Original bitwidth of a type before manipulations.
};

/// @brief Get the string name of a property.
/// @details Maps a `PropertyId` to its corresponding string representation.
/// Useful for debugging or reporting purposes.
/// @param id The property ID.
/// @return The string representation of the property.

const char *getPropertyName(const PropertyId id);

/// @brief Unique IDs for each class type.
/// @details This enum assigns a unique identifier to every leaf object type in the hierarchy.
/// These IDs can be used for classification, serialization, or type checking purposes.
enum ClassId : unsigned char {
    CLASSID_AGGREGATEALT,    ///< ID for AggregateAlt class.
    CLASSID_AGGREGATE,       ///< ID for Aggregate class.
    CLASSID_ALIAS,           ///< ID for Alias class.
    CLASSID_ARRAY,           ///< ID for Array class.
    CLASSID_ASSIGN,          ///< ID for Assign class.
    CLASSID_BIT,             ///< ID for Bit class.
    CLASSID_BITVALUE,        ///< ID for BitValue class.
    CLASSID_BITVECTOR,       ///< ID for Bitvector class.
    CLASSID_BITVECTORVALUE,  ///< ID for BitvectorValue class.
    CLASSID_BOOL,            ///< ID for Bool class.
    CLASSID_BOOLVALUE,       ///< ID for BoolValue class.
    CLASSID_BREAK,           ///< ID for Break class.
    CLASSID_CAST,            ///< ID for Cast class.
    CLASSID_CHAR,            ///< ID for Char class.
    CLASSID_CHARVALUE,       ///< ID for CharValue class.
    CLASSID_CONST,           ///< ID for Const class.
    CLASSID_CONTENTS,        ///< ID for Contents class.
    CLASSID_CONTINUE,        ///< ID for Continue class.
    CLASSID_DESIGNUNIT,      ///< ID for DesignUnit class.
    CLASSID_ENTITY,          ///< ID for Entity class.
    CLASSID_ENUM,            ///< ID for Enum class.
    CLASSID_ENUMVALUE,       ///< ID for EnumValue class.
    CLASSID_EVENT,           ///< ID for Event class.
    CLASSID_EXPRESSION,      ///< ID for Expression class.
    CLASSID_FIELD,           ///< ID for Field class.
    CLASSID_FIELDREFERENCE,  ///< ID for FieldReference class.
    CLASSID_FILE,            ///< ID for File class.
    CLASSID_FORGENERATE,     ///< ID for ForGenerate class.
    CLASSID_FOR,             ///< ID for For class.
    CLASSID_FUNCTIONCALL,    ///< ID for FunctionCall class.
    CLASSID_FUNCTION,        ///< ID for Function class.
    CLASSID_GLOBALACTION,    ///< ID for GlobalAction class.
    CLASSID_IDENTIFIER,      ///< ID for Identifier class.
    CLASSID_IFALT,           ///< ID for IfAlt class.
    CLASSID_IFGENERATE,      ///< ID for IfGenerate class.
    CLASSID_IF,              ///< ID for If class.
    CLASSID_INSTANCE,        ///< ID for Instance class.
    CLASSID_INT,             ///< ID for Int class.
    CLASSID_INTVALUE,        ///< ID for IntValue class.
    CLASSID_LIBRARYDEF,      ///< ID for LibraryDef class.
    CLASSID_LIBRARY,         ///< ID for Library class.
    CLASSID_MEMBER,          ///< ID for Member class.
    CLASSID_NULL,            ///< ID for Null class.
    CLASSID_PARAMETERASSIGN, ///< ID for ParameterAssign class.
    CLASSID_PARAMETER,       ///< ID for Parameter class.
    CLASSID_POINTER,         ///< ID for Pointer class.
    CLASSID_PORTASSIGN,      ///< ID for PortAssign class.
    CLASSID_PORT,            ///< ID for Port class.
    CLASSID_PROCEDURECALL,   ///< ID for ProcedureCall class.
    CLASSID_PROCEDURE,       ///< ID for Procedure class.
    CLASSID_RANGE,           ///< ID for Range class.
    CLASSID_REAL,            ///< ID for Real class.
    CLASSID_REALVALUE,       ///< ID for RealValue class.
    CLASSID_RECORD,          ///< ID for Record class.
    CLASSID_RECORDVALUEALT,  ///< ID for RecordValueAlt class.
    CLASSID_RECORDVALUE,     ///< ID for RecordValue class.
    CLASSID_REFERENCE,       ///< ID for Reference class.
    CLASSID_RETURN,          ///< ID for Return class.
    CLASSID_SIGNAL,          ///< ID for Signal class.
    CLASSID_SIGNED,          ///< ID for Signed class.
    CLASSID_SLICE,           ///< ID for Slice class.
    CLASSID_STATE,           ///< ID for State class.
    CLASSID_STATETABLE,      ///< ID for StateTable class.
    CLASSID_STRING,          ///< ID for String class.
    CLASSID_STRINGVALUE,     ///< ID for StringValue class.
    CLASSID_SWITCHALT,       ///< ID for SwitchAlt class.
    CLASSID_SWITCH,          ///< ID for Switch class.
    CLASSID_SYSTEM,          ///< ID for System class.
    CLASSID_TIME,            ///< ID for Time class.
    CLASSID_TIMEVALUE,       ///< ID for TimeValue class.
    CLASSID_TRANSITION,      ///< ID for Transition class.
    CLASSID_TYPEDEF,         ///< ID for TypeDef class.
    CLASSID_TYPEREFERENCE,   ///< ID for TypeReference class.
    CLASSID_TYPETPASSIGN,    ///< ID for TypeTPAssign class.
    CLASSID_TYPETP,          ///< ID for TypeTP class.
    CLASSID_UNSIGNED,        ///< ID for Unsigned class.
    CLASSID_VALUESTATEMENT,  ///< ID for ValueStatement class.
    CLASSID_VALUETPASSIGN,   ///< ID for ValueTPAssign class.
    CLASSID_VALUETP,         ///< ID for ValueTP class.
    CLASSID_VARIABLE,        ///< ID for Variable class.
    CLASSID_VIEW,            ///< ID for View class.
    CLASSID_VIEWREFERENCE,   ///< ID for ViewReference class.
    CLASSID_WAIT,            ///< ID for Wait class.
    CLASSID_WHENALT,         ///< ID for WhenAlt class.
    CLASSID_WHEN,            ///< ID for When class.
    CLASSID_WHILE,           ///< ID for While class.
    CLASSID_WITHALT,         ///< ID for WithAlt class.
    CLASSID_WITH             ///< ID for With class.
};

/// @brief Converts a ClassId enum to its string representation.
/// @details Useful for debugging and reporting purposes.
/// @param id The ClassId to convert.
/// @return The string representation of the ClassId.

std::string classIDToString(const ClassId id);

/// @brief Matching semantics for `Switch` and `With` constructs.
/// @details Determines how cases are matched within these constructs.
/// - `CASE_LITERAL`: Matches strictly on each bit.
/// - `CASE_Z`: Matches similarly to `casez` in Verilog (ignores `z` bits).
/// - `CASE_X`: Matches similarly to `casex` in Verilog (ignores `x` bits).
enum CaseSemantics : unsigned char {
    CASE_LITERAL, ///< Strict matching on each bit.
    CASE_Z,       ///< Matches with `z` bits ignored (Verilog `casez`).
    CASE_X        ///< Matches with `x` bits ignored (Verilog `casex`).
};

/// @brief Converts a CaseSemantics enum to its string representation.
/// @param c The CaseSemantics value.
/// @return The string representation of the CaseSemantics.

std::string caseSemanticsToString(const CaseSemantics c);

/// @brief Parses a string to determine the corresponding CaseSemantics.
/// @param c The string to parse.
/// @return The corresponding CaseSemantics value.

CaseSemantics caseSemanticsFromString(const std::string &c);

} // namespace hif
