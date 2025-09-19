/// @file search.hpp
/// @brief Utilities for searching objects within HIF trees.
/// @details
/// Provides functionality to search for objects in HIF trees or subtrees based
/// on various customizable criteria using query objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/classes.hpp"
#include "hif/hif.hpp"

#include <functional>

namespace hif
{

/// @brief Base class for HIF tree queries.
/// @details
/// Provides a common interface and parameters for object queries in HIF trees.
class HifQueryBase
{
public:
    /// @brief Maximum search depth. Default is no limit.
    std::size_t depth;
    /// @brief The name of the object to search for.
    std::string name;
    /// @brief Set of object types to avoid during the search.
    std::set<ClassId> classToAvoid;
    /// @brief Custom method to filter objects. Default is nullptr.
    std::function<bool(Object *, const HifQueryBase *)> check_object_method;
    /// @brief Enables search within method call declarations.
    bool checkInsideCallsDeclarations;
    /// @brief If true, stops searching after the first match.
    bool onlyFirstMatch;
    /// @brief Skips standard scopes during the search. Default is false.
    bool skipStandardScopes;
    /// @brief Enables matching based on type variant.
    bool matchTypeVariant;
    /// @brief Desired type variant to match. Default is `Type::NATIVE_TYPE`.
    Type::TypeVariant typeVariant;
    /// @brief Language semantics for the query.
    hif::semantics::ILanguageSemantics *sem;
    /// @brief Checks if the given object matches the query type.
    /// @param o The object to check.
    /// @return True if the object matches, false otherwise.
    virtual bool isSameType(Object *o) const = 0;

protected:
    HifQueryBase();
    virtual ~HifQueryBase();

private:
    HifQueryBase(const HifQueryBase &);
    HifQueryBase &operator=(const HifQueryBase &);
};

/// @brief Typed query for objects in HIF trees.
/// @details
/// Extends the base query to support specific object types.
template <class T> class HifTypedQuery : public HifQueryBase
{
public:
    /// @brief Type for storing query results.
    using Results = std::list<T *>;

    HifTypedQuery()
        : HifQueryBase()
        , nextQueryType(nullptr)
    {
        // ntd
    }

    virtual ~HifTypedQuery()
    {
        // ntd
    }

    /// @brief Checks if the given object matches the query type.
    /// @param o The object to check.
    /// @return True if the object matches, false otherwise.
    virtual bool isSameType(Object *o) const
    {
        if (nextQueryType != nullptr && nextQueryType->isSameType(o))
            return true;
        if (matchTypeVariant && dynamic_cast<Type *>(o) != nullptr &&
            static_cast<Type *>(o)->getTypeVariant() != typeVariant)
            return false;
        return (dynamic_cast<T *>(o) != nullptr);
    }

    /// @brief Retrieves the next query type in a chain of queries.
    /// @return Pointer to the next query type, or nullptr if not set.
    HifQueryBase *getNextQueryType() const { return nextQueryType; }

    /// @brief Sets the next query type in a chain of queries.
    /// @tparam P The type of the next query.
    /// @param value Pointer to the next query type.
    template <typename P> void setNextQueryType(HifTypedQuery<P> *value) { nextQueryType = value; }

private:
    HifQueryBase *nextQueryType;

    HifTypedQuery(const HifTypedQuery &);
    HifTypedQuery &operator=(const HifTypedQuery &);
};

/// @brief Untyped query for objects in HIF trees.
/// @details
/// Provides a query mechanism without specifying a specific type.
class HifUntypedQuery : public HifQueryBase
{
public:
    /// @brief Type for storing query results.
    using Results = std::list<Object *>;

    HifUntypedQuery();
    virtual ~HifUntypedQuery();

    /// @brief Checks if the given object matches the query criteria.
    /// @param o The object to check.
    /// @return True if the object matches, false otherwise.
    virtual bool isSameType(Object *o) const;

private:
    HifUntypedQuery(const HifUntypedQuery &);
    HifUntypedQuery &operator=(const HifUntypedQuery &);
};

/// @brief Searches for objects matching specific criteria in a HIF tree.
/// @details
/// Traverses the tree starting from the given root object, storing matching objects in the result list.
/// @param result List to store the matching objects.
/// @param root Starting point for the search.
/// @param query Query object specifying search criteria.

void search(std::list<Object *> &result, Object *root, const HifQueryBase &query);

/// @brief Searches for objects of a specific type in a HIF tree.
/// @details
/// Traverses the tree starting from the given root object, storing matching objects in the result list.
/// @tparam T Type of objects to search for.
/// @param result List to store the matching objects.
/// @param root Starting point for the search.
/// @param query Query object specifying search criteria.
template <typename T> void search(std::list<T *> &result, Object *root, const HifQueryBase &query)
{
    auto *tmp = reinterpret_cast<std::list<Object *> *>(&result);
    search(*tmp, root, query);
}

/// @brief Searches for objects of a specific type in multiple HIF subtrees.
/// @details
/// Traverses each subtree in the root list, storing matching objects in the result list.
/// @tparam T1 Type of objects to search for.
/// @tparam T2 Type of root objects to traverse.
/// @param result List to store the matching objects.
/// @param root List of root objects to traverse.
/// @param query Query object specifying search criteria.
template <typename T1, typename T2> void search(std::list<T1 *> &result, BList<T2> &root, const HifQueryBase &query)
{
    for (auto iter = root.begin(); iter != root.end(); ++iter) {
        search(result, *iter, query);
    }
}

} // namespace hif
