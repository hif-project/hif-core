/// @file Record.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ScopedType.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Record data structure declaration.
///
/// @details
/// This class represents the declaration of a record data structure
/// (i.e., a <tt>record</tt> in VHDL or a <tt>struct</tt> in C/C++).
/// A record contains the list of its fields and declares a composite type.
/// In case an object represents a C/C++ <tt>struct</tt>, the <tt>packed</tt>
/// attribute is used to prevent the compiler from padding when aligning memory
/// for the data structure, while the <tt>union</tt> declares the data
/// structure to be a <tt>union</tt> instead of a <tt>struct</tt>.
class Record : public ScopedType
{
public:
    /// @brief The list of fields of the record.
    BList<Field> fields;

    /// @brief Constructor.
    Record();

    /// @brief Destructor.
    virtual ~Record();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the <tt>packed</tt> attribute.
    /// @return The <tt>packed</tt> attribute.
    bool isPacked() const;

    /// @brief Sets the <tt>packed</tt> attribute.
    /// @param packed The <tt>packed</tt> attribute to be set.
    void setPacked(bool packed);

    /// @brief Returns the <tt>union</tt> attribute.
    /// @return The <tt>union</tt> attribute.
    bool isUnion() const;

    /// @brief Sets the <tt>union</tt> attribute.
    /// @param u The <tt>union</tt> attribute to be set.
    void setUnion(bool u);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns the composite base type.
    /// @param considerOpacity <tt>true</tt> if opacity is to be considered, <tt>false</tt> otherwise.
    /// @return The composite base type.
    Type *getBaseType(bool considerOpacity) const;

    /// @brief Sets the composite base type.
    /// @param t The new composite type to be set.
    /// @param considerOpacity <tt>true</tt> if opacity is to be considered, <tt>false</tt> otherwise.
    /// @return The old composite base type if it is different
    /// from the new one, nullptr otherwise.
    Type *setBaseType(Type *t, bool considerOpacity);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given BList w.r.t. this.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief Flag to store the <tt>packed</tt> attribute.
    bool _packed;

    /// @brief Flag to store the <tt>union</tt> attribute.
    bool _union;

    /// @brief The composite base type.
    Type *_baseType;

    /// @brief The composite base type considering opacity.
    Type *_baseOpaqueType;

    // K: disabled
    Record(const Record &);
    Record &operator=(const Record &);
};

} // namespace hif
