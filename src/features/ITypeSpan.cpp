/// @file ITypeSpan.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/features/ITypeSpan.hpp"
#include "hif/classes/classes.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{
namespace features
{

ITypeSpan::ITypeSpan()
    : _span(nullptr)
{
    // ntd
}

ITypeSpan::~ITypeSpan() { delete _span; }

ITypeSpan::ITypeSpan(const ITypeSpan &other)
    : IFeature(other)
    , _span(nullptr)
{
    // Copy of span must be performed by child since setSpan must be called.
}

ITypeSpan &ITypeSpan::operator=(const ITypeSpan &other)
{
    if (this == &other)
        return *this;

    IFeature::operator=(other);
    delete setSpan(hif::copy(other._span));

    return *this;
}

Range *ITypeSpan::getSpan() const { return _span; }

} // namespace features
} // namespace hif
