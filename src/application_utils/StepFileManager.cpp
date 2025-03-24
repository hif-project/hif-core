/// @file StepFileManager.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/application_utils/StepFileManager.hpp"

#include "hif/application_utils/Log.hpp"

namespace hif
{
namespace application_utils
{

StepFileManager::StepFileManager()
    : _opt()
    , _prefix()
    , _suffix()
    , _currentStep()
    , _stepNumber(1)
    , _autoStepFile()
    , _autoStepNumber(0)
    , _currentAutoStepNumber(1)
    , _parentManager(nullptr)
    , _print(false)
{
    // ntd
}

auto StepFileManager::getPrintOpt() const -> hif::PrintHifOptions { return _opt; }

void StepFileManager::setPrintOpt(const hif::PrintHifOptions &opt) { _opt = opt; }

auto StepFileManager::getPrefix() const -> std::string { return _prefix; }

void StepFileManager::setPrefix(const std::string &prefix) { _prefix = prefix; }

auto StepFileManager::getSuffix() const -> std::string { return _suffix; }

void StepFileManager::setSuffix(const std::string &suffix) { _suffix = suffix; }

auto StepFileManager::getStepNumber() const -> int { return _stepNumber; }

void StepFileManager::setStepNumber(int stepNumber) { _stepNumber = stepNumber; }

auto StepFileManager::getParentManager() const -> StepFileManager * { return _parentManager; }

void StepFileManager::setParentManager(StepFileManager *parentManager) { _parentManager = parentManager; }

auto StepFileManager::getPrint() const -> bool { return _print; }

void StepFileManager::setPrint(const bool print) { _print = print; }

void StepFileManager::printStep(System *s, const std::string &stepName)
{
    if (!_print) {
        ++_stepNumber;
        return;
    }

    std::stringstream ssName;
    ssName << getApplicationName() << "_";

    // print parent steps info:
    if (_parentManager != nullptr) {
        ssName << _parentManager->getCurrentStepInfo() << "_";
    }

    // print current step info:
    if (_stepNumber < 10) {
        ssName << '0';
    }
    ssName << _stepNumber << "_" << _prefix << stepName << _suffix;
    ++_stepNumber;

    hif::PrintHifOptions opt;
    opt.printHifStandardLibraries = true;
    hif::writeFile(ssName.str(), s, true, opt);
    hif::writeFile(ssName.str(), s, false, opt);
}

void StepFileManager::startStep(const std::string &stepName) { _currentStep = stepName; }

void StepFileManager::endStep(System *s) { printStep(s, _currentStep); }

auto StepFileManager::getCurrentStepName() const -> const std::string & { return _currentStep; }

auto StepFileManager::getCurrentStepInfo() const -> std::string
{
    std::stringstream ssName;
    if (_parentManager != nullptr) {
        ssName << _parentManager->getCurrentStepInfo() << "_";
    }

    // print current step info:
    if (_stepNumber < 10) {
        ssName << '0';
    }
    ssName << _stepNumber << "_" << _prefix << _currentStep << _suffix;

    return ssName.str();
}

void StepFileManager::setAutoStepFile(const std::string &autoStepFile)
{
    if (_parentManager == nullptr) {
        _autoStepFile             = autoStepFile;
        // Removing: .hif.xml
        _autoStepFile             = _autoStepFile.substr(0, _autoStepFile.size() - 8);
        std::size_t lastSlashIdx = _autoStepFile.find_last_of("\\/");
        if (std::string::npos != lastSlashIdx) {
            _autoStepFile.erase(0, lastSlashIdx + 1);
        }

        std::string::size_type size = hif::application_utils::getApplicationName().size();
        _autoStepFile               = _autoStepFile.substr(size + 1);

        std::stringstream s;
        s << _autoStepFile;
        s >> _autoStepNumber;

        return;
    }

    _autoStepFile          = _parentManager->getAutoStepFile();
    std::string parentStep = _parentManager->getCurrentStepName();
    const std::size_t size = 3 + parentStep.size() + 1;
    if (size >= _autoStepFile.size()) {
        _autoStepFile = "";
    } else {
        _autoStepFile = _autoStepFile.substr(size);

        std::stringstream s;
        s << _autoStepFile;
        s >> _autoStepNumber;
    }
}

auto StepFileManager::getAutoStepFile() const -> const std::string & { return _autoStepFile; }

auto StepFileManager::checkStepName() -> bool
{
    if (_autoStepFile.empty()) {
        return true;
    }
    const bool ret = (_currentAutoStepNumber > _autoStepNumber);
    ++_currentAutoStepNumber;
    if (!ret) {
        ++_stepNumber;
    }
    return ret;
}

} // namespace application_utils
} // namespace hif
