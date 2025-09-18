/// @file equals.hpp
/// @brief Utilities for checking equality of HIF objects and structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

///	@name Hif comparison related functions.
///
/// @{

/// @brief Struct of options passed to the <tt>equals</tt> function.
/// @see hif::equals
struct EqualsOptions {
    EqualsOptions();
    ~EqualsOptions();
    /// @brief Copy constructor.
    /// @param o The object to copy from.
    EqualsOptions(const EqualsOptions &o);
    /// @brief Assignment operator.
    /// @param o The object to assign from.
    /// @return Reference to this object.
    EqualsOptions &operator=(EqualsOptions o);
    /// @brief Swap function.
    /// @param o The object to swap with.
    void swap(EqualsOptions &o);

    /// @name General options.
    /// @{

    /// Check only the class name of the objects. Default is false.
    bool checkOnlyTypes;

    /// Check only the name of the objects when exists. Default is false.
    bool checkOnlyNames;

    /// For symbols, checks whether their declarations are the same
    /// (same object pointer not equals on them) and in this case returns true without
    /// checks symbols children. If declaration is not the same, equals is
    /// determinated from standard visit.
    /// NOTE: declaration is not searched but only given when already set.
    /// Checks works only if both symbols have declarations set.
    bool checkOnlySymbolsDeclarations;

    /// @}

    /// @name Hif objects check options.
    /// @{

    /// Check spans within types. Default is true.
    bool checkSpans;
    /// Check inner type for compound types. Default is true.
    bool checkInnerTypeOfComposite;
    /// Check ranges within data declarations. Default is true.
    bool checkDeclarationRangeConstraint;
    /// Check Fields initial value. Default is true.
    bool checkFieldsInitialvalue;
    /// Check Instances of HIF objects. Default is true;
    bool checkReferencedInstance;

    /// @}

    /// @name Hif objects flags check options.
    /// @{

    /// Check constexpr flag within types. Default is true.
    bool checkConstexprFlag;
    /// Check logic flag within types. Default is true.
    bool checkLogicFlag;
    /// Check signed/unsigned flag within types. Default is true.
    bool checkSignedFlag;
    /// Check resolved flag within types. Default is true.
    bool checkResolvedFlag;
    /// Check type variant field withing types. Default is true;
    bool checkTypeVariantField;
    /// Check span of string types. Default is true;
    bool checkStringSpan;
    /// Check span direction. Default is true.
    bool checkSpanDirection;

    /// @}
    /// @name Special cases check options.
    /// @{

    /// If one of the types is constexpr, perform only minimal checks. Default is false.
    bool handleConstexprTypes;
    /// Check vectors characteristics regardless their actual type. Default is false.
    bool handleVectorTypes;
    /// For external typedefs, check only names. Default: false.
    bool handleExternalsTypedefs;

    /// @}

    /// @name Limit check options.
    /// @{

    /// Skip reference objects during check. Default is true.
    bool skipReferences;
    /// Check only the current object, skipping children. Default is false.
    bool skipChilden;
    /// If the first subtree has nullptr children and the second does not have, preserve equivalence.
    /// It can be used for pattern matching. Default is false.
    bool skipNullBranches;
    /// For methods, check only signatures (skip the body). Default: false.
    bool skipDeclarationBodies;
    /// For views skips check of template list and spans of types inside entity.
    /// Moreover does not check contents and other blists. Default: false.
    bool skipViewContents;

    /// @}

    /// @name Unrelated objects check options.
    /// @{

    /// Check that symbol declarations match. Default is false.
    bool assureSameSymbolDeclarations;
    /// Checks the list of properties and eventual objects.
    bool assureSameProperties;
    /// Check the list of additional keywords
    bool assureSameAdditionalKeywords;

    /// @}

    /// @brief Language semantics to use for comparison.
    hif::semantics::ILanguageSemantics *sem;
};

/// @brief Compares two Hif objects.
/// This function checks whether two objects are identical. It
/// checks the properties but does not compare code infos.
///
/// @param obj1 The first object.
/// @param obj2 The second object.
/// @param options The comparison options.
/// @return <tt>true</tt> if the objects are identical, <tt>false</tt> otherwise.
///

bool equals(Object *obj1, Object *obj2, const EqualsOptions &options = EqualsOptions());
///
/// @brief Compares two Hif lists.
/// This function checks whether two lists are identical.
///
/// @param list1 The first list.
/// @param list2 The second list.
/// @param options The comparison options.
/// @return <tt>true</tt> if the lists are identical, <tt>false</tt> otherwise.
///
template <class T>
bool equals(BList<T> &list1, BList<T> &list2, const EqualsOptions &options = EqualsOptions())
{
    typename BList<T>::iterator i, j;

    if (&list1 == &list2)
        return true;

    if (list1.size() != list2.size())
        return false;

    for (i = list1.begin(), j = list2.begin(); i != list1.end(); ++i, ++j) {
        if (!equals(*i, *j, options))
            return false;
    }

    return true;
}

/// @}

} // namespace hif
