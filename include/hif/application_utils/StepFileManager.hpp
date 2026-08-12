/// @file StepFileManager.hpp
/// @brief Manages step files for simulation or processing steps.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/hifIOUtils.hpp"

namespace hif
{
namespace application_utils
{

/// @brief Manages step files, options, and related operations.
class StepFileManager
{
public:
    /// @brief Constructor.
    StepFileManager();

    /// @brief Deleted copy constructor.
    StepFileManager(const StepFileManager &) = delete;

    /// @brief Deleted copy assignment operator.
    auto operator=(const StepFileManager &) -> StepFileManager & = delete;

    /// @brief Destructor.
    virtual ~StepFileManager() = default;

    /// @brief Retrieves the current print options.
    /// @return Current print options.
    auto getPrintOpt() const -> hif::PrintHifOptions;

    /// @brief Sets new print options.
    /// @param opt New print options to set.
    void setPrintOpt(const hif::PrintHifOptions &opt);

    /// @brief Retrieves the prefix for step file names.
    /// @return Prefix string.
    auto getPrefix() const -> std::string;

    /// @brief Sets the prefix for step file names.
    /// @param prefix Prefix string to set.
    void setPrefix(const std::string &prefix);

    /// @brief Retrieves the suffix for step file names.
    /// @return Suffix string.
    auto getSuffix() const -> std::string;

    /// @brief Sets the suffix for step file names.
    /// @param suffix Suffix string to set.
    void setSuffix(const std::string &suffix);

    /// @brief Retrieves the current step number.
    /// @return Current step number.
    auto getStepNumber() const -> int;

    /// @brief Sets the current step number.
    /// @param stepNumber Step number to set.
    void setStepNumber(int stepNumber);

    /// @brief Retrieves the parent manager.
    /// @return Pointer to the parent manager.
    auto getParentManager() const -> StepFileManager *;

    /// @brief Sets the parent manager.
    /// @param parentManager Pointer to the new parent manager.
    void setParentManager(StepFileManager *parentManager);

    /// @brief Checks if printing is enabled.
    /// @return True if printing is enabled, false otherwise.
    auto getPrint() const -> bool;

    /// @brief Enables or disables printing.
    /// @param print True to enable printing, false to disable.
    void setPrint(bool print);

    /// @brief Prints information about a step.
    /// @param s Pointer to the system.
    /// @param stepName The name of the step to print.
    void printStep(System *s, const std::string &stepName);

    /// @brief Starts a new step.
    /// @param stepName The name of the step to start.
    void startStep(const std::string &stepName);

    /// @brief Ends the current step.
    /// @param s Pointer to the system.
    void endStep(System *s);

    /// @brief Retrieves the name of the current step.
    /// @return Reference to the current step name.
    auto getCurrentStepName() const -> const std::string &;

    /// @brief Retrieves information about the current step.
    /// @return String containing the current step information.
    auto getCurrentStepInfo() const -> std::string;

    /// @brief Sets the name of the auto-step file.
    /// @param autoStepFile The name of the auto-step file.
    void setAutoStepFile(const std::string &autoStepFile);

    /// @brief Retrieves the name of the auto-step file.
    /// @return Reference to the auto-step file name.
    auto getAutoStepFile() const -> const std::string &;

    /// @brief Checks if the current step name is valid.
    /// @return True if the step name is valid, false otherwise.
    auto checkStepName() -> bool;

private:
    hif::PrintHifOptions _opt;       ///< Print options.
    std::string _prefix;             ///< Prefix for step file names.
    std::string _suffix;             ///< Suffix for step file names.
    std::string _currentStep;        ///< The name of the current step.
    int _stepNumber;                 ///< Current step number.
    std::string _autoStepFile;       ///< The name of the auto-step file.
    int _autoStepNumber;             ///< Auto-step number.
    int _currentAutoStepNumber;      ///< Current auto-step number.
    StepFileManager *_parentManager; ///< Pointer to the parent manager.
    bool _print;                     ///< Print enabled flag.
};

} // namespace application_utils
} // namespace hif