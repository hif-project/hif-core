/// @file Signal.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Signal.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Signal::Signal()
    : _isStandard(false)
    , _isWrapper(false)
{
    // ntd
}

Signal::~Signal()
{
    // ntd
}

ClassId Signal::getClassId() const { return CLASSID_SIGNAL; }

int Signal::acceptVisitor(HifVisitor &vis) { return vis.visitSignal(*this); }

void Signal::_calculateFields() { DataDeclaration::_calculateFields(); }

bool Signal::isStandard() const { return _isStandard; }

void Signal::setStandard(bool standard) { _isStandard = standard; }

bool Signal::isWrapper() const { return _isWrapper; }

void Signal::setWrapper(bool wrapper) { _isWrapper = wrapper; }

} // namespace hif
