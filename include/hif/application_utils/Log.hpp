/// @file Log.hpp
/// @brief Macros and functions for logging and debugging messages in HIF applications.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <sstream>
#include <string>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{
class ILanguageSemantics;
} // namespace semantics

namespace application_utils
{

/// @brief List of pointers to Object instances for warnings.
using WarningList = std::list<Object *>;

/// @brief Set of unique pointers to Object instances for warnings.
using WarningSet = std::set<Object *>;

/// @brief Set of unique warning description strings.
using WarningStringSet = std::set<std::string>;

/// @brief Structure to store detailed information about a warning.
struct WarningInfo {
    /// @brief Default constructor.
    WarningInfo();

    /// @brief Constructor initializing with an Object pointer.
    /// @param o Pointer to the associated Object.
    explicit WarningInfo(Object *o);

    /// @brief Destructor.
    ~WarningInfo();

    /// @brief Copy constructor.
    /// @param o Reference to the WarningInfo to copy.
    WarningInfo(const WarningInfo &o);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param o WarningInfo to assign from.
    /// @return Reference to the assigned WarningInfo.
    auto operator=(WarningInfo o) -> WarningInfo &;

    /// @brief Swaps the contents of two WarningInfo objects.
    /// @param o Reference to the WarningInfo to swap with.
    void swap(WarningInfo &o) noexcept;

    /// @brief Comparison operator for ordering WarningInfo objects.
    /// @param o Reference to the other WarningInfo for comparison.
    /// @return True if this object is less than the other.
    auto operator<(const WarningInfo &o) const -> bool;

    std::string name;          ///< The name associated with the warning.
    Object::CodeInfo codeInfo; ///< Code-related information for the warning.
    std::string description;   ///< Text description of the warning.
};

/// @brief List of WarningInfo objects for managing warning details.
using WarningInfoList = std::list<WarningInfo>;

/// @brief Set of unique WarningInfo objects.
using WarningInfoSet = std::set<WarningInfo>;

/// @brief Gets the application name (i.e., a specific front-end/back-end).
/// @return The name of application set (if any).
auto getApplicationName() -> std::string;

/// @brief Gets the component name (i.e., a part of the set front-end/back-end).
/// @return The name of component set (if any).
auto getComponentName() -> std::string;

/// @brief Initializes the log messages for a component, setting the application
/// name and component name. Current values are stored.
/// @param appName The name of the application.
/// @param compName The name of the component.
void initializeLogHeader(const std::string &appName, const std::string &compName);

/// @brief Restores the previous values for application and component name, if present.
void restoreLogHeader();

/// @brief Prints information message.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the actual message to print.
void _hif_internal_messageInfo(const std::string &file, unsigned int line, const std::string &message);

/// @brief Prints warning message.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the actual message to print.
/// @param involvedObject is the object related to the message (mainly if the
/// message concern an error). It includes also information about source
/// file name and line number originally related to the object.
/// @param sem The reference semantics.
void _hif_internal_messageWarning(
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject             = nullptr,
    hif::semantics::ILanguageSemantics *sem = nullptr);

/// @brief Collects a unique warning message. This kind of messages are
/// collected and not printed, until a call to printUniqueWarnings is raised.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the actual message to print.
void _hif_internal_raiseUniqueWarning(const std::string &file, unsigned int line, const std::string &message);

/// @brief Prints all the unique warning messages collected until now.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the optional message to print.
void _hif_internal_printUniqueWarnings(const std::string &file, unsigned int line, const std::string &message);

/// @brief Prints an error message, which causes exit.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the actual message to print.
/// @param involvedObject is the object related to the message (mainly if the
/// message concern an error). It includes also information about source
/// file name and line number originally related to the object.
/// @param sem The reference semantics.
void _hif_internal_messageError [[noreturn]] (
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject             = nullptr,
    hif::semantics::ILanguageSemantics *sem = nullptr);

/// @brief Prints a debug message.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the actual message to print.
/// @param involvedObject is the object related to the message (mainly if the
/// message concern an error). It includes also information about source
/// file name and line number originally related to the object.
/// @param sem The reference semantics.
/// @param dontPrintCondition indicates a boolean condition to suppress the print.
void _hif_internal_messageDebug(
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject             = nullptr,
    hif::semantics::ILanguageSemantics *sem = nullptr,
    bool dontPrintCondition                 = true);

/// @brief Prints a debug message that also validates a boolean
/// condition.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param message is the actual message to print.
/// @param involvedObject is the object related to the message (mainly if the
/// message concern an error). It includes also information about source
/// file name and line number originally related to the object.
/// @param sem The reference semantics.
void _hif_internal_messageAssert [[noreturn]] (
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject             = nullptr,
    hif::semantics::ILanguageSemantics *sem = nullptr);

/// @brief Prints a list of warnings.
/// The message is common for all passed objects.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param condition Warnings are printed only if condition holds.
/// @param message is the actual message to print.
/// @param objList The list of involved objects.
void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningList &objList);

/// @brief Prints a list of warnings.
/// The message is common for all passed objects.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param condition Warnings are printed only if condition holds.
/// @param message is the actual message to print.
/// @param objSet The set of involved objects.
void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningSet &objSet);

/// @brief Prints a list of warnings.
/// The message is common for all passed objects.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param condition Warnings are printed only if condition holds.
/// @param message is the actual message to print.
/// @param objSet The set of involved objects.
void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningStringSet &objSet);

/// @brief Prints a list of warnings.
/// The message is common for all passed objects.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param condition Warnings are printed only if condition holds.
/// @param message is the actual message to print.
/// @param objList The list of involved objects.
void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningInfoList &objList);

/// @brief Prints a list of warnings.
/// The message is common for all passed objects.
/// @param file is expanded by macro to the raising point of the message.
/// @param line is expanded by macro to the raising point of the message.
/// @param condition Warnings are printed only if condition holds.
/// @param message is the actual message to print.
/// @param objSet The set of involved objects.
void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningInfoSet &objSet);

/// @brief Sets the verbose printing flag.
/// @param isVerbose The value.
void setVerboseLog(bool isVerbose);

/// @brief Gets the verbose printing flag.
/// @return The value.
auto isVerboseLog() -> bool;

/// @name Logging and Debugging Macros
/// @brief These macros are used for logging and debugging messages.
/// @details
/// Each macro automatically includes `__FILE__` and `__LINE__` to ensure accurate
/// tracking of where the log or debug message originated.
/// @{

/// @brief Logs an informational message.
/// @param message The message to log.
#define messageInfo(message)                                                                                           \
    do {                                                                                                               \
        hif::application_utils::_hif_internal_messageInfo(__FILE__, __LINE__, (message));                              \
    } while (0)

/// @brief Logs a warning message.
/// @param message The warning message to log.
/// @param involvedObject The object related to the warning.
/// @param semantics The semantics involved in the warning.
#define messageWarning(message, involvedObject, semantics)                                                             \
    do {                                                                                                               \
        hif::application_utils::_hif_internal_messageWarning(                                                          \
            __FILE__, __LINE__, (message), (involvedObject), (semantics));                                             \
    } while (0)

/// @brief Raises a unique warning message to prevent repeated logging of the same warning.
/// @param message The warning message to raise.
#define raiseUniqueWarning(message)                                                                                    \
    do {                                                                                                               \
        hif::application_utils::_hif_internal_raiseUniqueWarning(__FILE__, __LINE__, (message));                       \
    } while (0)

/// @brief Prints all unique warnings that have been raised.
/// @param message The context or description of the warnings being printed.
#define printUniqueWarnings(message)                                                                                   \
    do {                                                                                                               \
        hif::application_utils::_hif_internal_printUniqueWarnings(__FILE__, __LINE__, (message));                      \
    } while (0)

/// @brief Logs an error message.
/// @param message The error message to log.
/// @param involvedObject The object related to the error.
/// @param semantics The semantics involved in the error.
#define messageError(message, involvedObject, semantics)                                                               \
    do {                                                                                                               \
        hif::application_utils::_hif_internal_messageError(                                                            \
            __FILE__, __LINE__, (message), (involvedObject), (semantics));                                             \
    } while (0)

/// @brief Asserts a condition, logging an error message if the condition fails.
/// @param assertCondition The condition to check.
/// @param message The error message to log if the assertion fails.
/// @param involvedObject The object related to the assertion failure.
/// @param semantics The semantics involved in the assertion failure.
#define messageAssert(assertCondition, message, involvedObject, semantics)                                             \
    do {                                                                                                               \
        if (!(assertCondition)) {                                                                                      \
            hif::application_utils::_hif_internal_messageAssert(                                                       \
                __FILE__, __LINE__, (message), (involvedObject), (semantics));                                         \
        }                                                                                                              \
    } while (0)

/// @brief Logs a warning message based on a condition and a list of objects.
/// @param cond The condition to evaluate.
/// @param message The warning message to log.
/// @param objList The list of objects related to the warning.
#define messageWarningList(cond, message, objList)                                                                     \
    do {                                                                                                               \
        hif::application_utils::_hif_internal_messageWarningList(__FILE__, __LINE__, (cond), (message), (objList));    \
    } while (0)

#ifdef NDEBUG // Debugging disabled in release builds.

/// @brief Logs a debug message. Disabled in release builds.
#    define messageDebug(...)

/// @brief Logs a debug error message. Disabled in release builds.
#    define messageDebugError(...)

/// @brief Logs a debug message if a condition fails. Disabled in release builds.
#    define messageDebugIfFails(...)

/// @brief Asserts a debug condition. Disabled in release builds.
#    define messageDebugAssert(...)

/// @brief Logs a debug message if debugging is enabled. Disabled in release builds.
#    define messageDebugIfEnabled(...)

#else // Debugging enabled in debug builds.

/// @brief External flag indicating if debug logging is active.

extern bool hifLogDebugIsActive;

/// @brief Logs a debug message.
/// @param message The debug message to log.
/// @param involvedObject The object related to the debug message.
/// @param semantics The semantics involved in the debug message.
#    define messageDebug(message, involvedObject, semantics)                                                           \
        do {                                                                                                           \
            hif::application_utils::_hif_internal_messageDebug(                                                        \
                __FILE__, __LINE__, (message), (involvedObject), (semantics), false);                                  \
        } while (0)

/// @brief Logs a debug error message using the messageError macro.
/// This macro expands its arguments to ensure proper handling by certain preprocessors.
#    define _messageExpandHelper(X) X

/// @brief Logs a debug error message using the messageError macro.
/// This macro expands its arguments to ensure proper handling by certain preprocessors.
#    define messageDebugError(...)  _messageExpandHelper(messageError(__VA_ARGS__))

/// @brief Logs a debug message if a condition fails.
/// @param condition The condition to evaluate.
/// @param message The debug message to log if the condition fails.
/// @param involvedObject The object related to the debug message.
/// @param semantics The semantics involved in the debug message.
#    define messageDebugIfFails(condition, message, involvedObject, semantics)                                         \
        do {                                                                                                           \
            hif::application_utils::_hif_internal_messageDebug(                                                        \
                __FILE__, __LINE__, (message), (involvedObject), (semantics), (condition));                            \
        } while (0)

/// @brief Asserts a debug condition, logging an error message if the condition fails.
/// @param assertCondition The condition to check.
/// @param message The error message to log if the assertion fails.
/// @param involvedObject The object related to the assertion failure.
/// @param semantics The semantics involved in the assertion failure.
#    define messageDebugAssert(assertCondition, message, involvedObject, semantics)                                    \
        do {                                                                                                           \
            if (!(assertCondition)) {                                                                                  \
                hif::application_utils::_hif_internal_messageAssert(                                                   \
                    __FILE__, __LINE__, (message), (involvedObject), (semantics));                                     \
            }                                                                                                          \
        } while (0)

/// @brief Logs a debug message if debugging is enabled.
/// @param message The debug message to log.
/// @param involvedObject The object related to the debug message.
/// @param semantics The semantics involved in the debug message.
#    define messageDebugIfEnabled(message, involvedObject, semantics)                                                  \
        do {                                                                                                           \
            hif::application_utils::_hif_internal_messageDebug(                                                        \
                __FILE__, __LINE__, (message), (involvedObject), (semantics), !hif::hifLogDebugIsActive);              \
        } while (0)

#endif // NDEBUG

/// @}

} // namespace application_utils

} // namespace hif
