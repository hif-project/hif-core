/// @file analyzeSpans.hpp
/// @brief Defines structures and functions for analyzing spans of indices.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <cstdint>
#include <map>

#include "hif/classes/forwards.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace analysis
{

/// @brief Struct representing an index for span analysis.
/// @details This structure is used to store information about a single index, a
/// range of indices, or a slice of indices. It supports comparisons to
/// facilitate its use in ordered containers.
struct IndexInfo {
    /// @brief Constructor.
    IndexInfo();

    /// @brief Destructor.
    ~IndexInfo();

    /// @brief Copy constructor.
    /// @param other The other instance to copy.
    IndexInfo(const IndexInfo &other);

    /// @brief Assignment operator.
    /// @param other The other instance to copy.
    /// @return a reference to this instance.
    auto operator=(const IndexInfo &other) -> IndexInfo &;

    /// @brief Comparison operator for ordered containers.
    /// @param other The other instance to compare against.
    /// @return true if this index is lower than other.
    auto operator<(const IndexInfo &other) const -> bool;

    /// @brief To be set only for single indices.
    Value *expression;
    /// @brief To be set to represent a range of indices, holding the same
    /// value.
    Range *range;
    /// @brief To be set for a span whose value is a span. Each index in the
    /// span will get the value at the matching index.
    Range *slice;
};

/// @brief Map from indices to values for span analysis.
using IndexMap = std::map<IndexInfo, Value *>;

/// @brief The result of span analysis.
/// @details Stores information about analyzed spans, including mappings of
/// indices to values, bounds, and coverage information.
struct AnalyzeSpansResult {
    /// @brief Enumeration of index kinds used in span analysis.
    enum IndexKind : unsigned char {
        INDEX_EXPRESSION, ///< A single index represented by an expression.
        INDEX_RANGE,      ///< A range of indices.
        INDEX_SLICE       ///< A slice of indices.
    };

    /// @brief Represents a value associated with an index in span analysis.
    /// @details Supports comparison and manipulation for ordered containers and
    /// mappings.
    class ValueIndex
    {
    public:
        /// @brief Constructor.
        ValueIndex();

        /// @brief Destructor.
        ~ValueIndex();

        /// @brief Copy constructor.
        /// @param other The other instance to copy.
        ValueIndex(const ValueIndex &other);

        /// @brief Assignment operator.
        /// @param other The other instance to copy.
        /// @return a reference to this instance.
        auto operator=(ValueIndex other) -> ValueIndex &;

        /// @brief Constructor with kind and range.
        /// @param kind The kind of the index.
        /// @param min The minimum bound of the range.
        /// @param max The maximum bound of the range.
        ValueIndex(IndexKind kind, std::uint64_t min, std::uint64_t max);

        /// @brief Swaps the contents of two ValueIndex objects.
        /// @param other The other instance to swap.
        void swap(ValueIndex &other) noexcept;

        /// @brief Comparison operator for ordered containers.
        /// @param other The other instance to compare against.
        /// @return true if this index is lower than other.
        auto operator<(const ValueIndex &other) const -> bool;

        /// @brief Gets the kind of the index.
        /// @return the kind of the index.
        auto getKind() const -> IndexKind;

        /// @brief Gets the maximum bound of the range or slice.
        /// @return the maximum bound of the range or slice.
        auto getMax() const -> std::uint64_t;

        /// @brief Gets the minimum bound of the range or slice.
        /// @return the minimum bound of the range or slice.
        auto getMin() const -> std::uint64_t;

        /// @brief Gets the size of the range or slice.
        /// @return the size of the range or slice.
        auto getSize() const -> std::uint64_t;

    private:
        IndexKind _kind;              ///< Kind of the index.
        std::uint64_t _index;         ///< Index value.
        std::uint64_t _minRangeIndex; ///< Minimum bound for ranges.
        std::uint64_t _maxRangeIndex; ///< Maximum bound for ranges.
        std::uint64_t _minSliceIndex; ///< Minimum bound for slices.
        std::uint64_t _maxSliceIndex; ///< Maximum bound for slices.
    };

    /// @brief The map storing indices and their associated values.
    using ValueMap = std::map<ValueIndex, Value *>;

    /// @brief Constructor.
    AnalyzeSpansResult();

    /// @brief Destructor.
    ~AnalyzeSpansResult();

    /// @brief Copy constructor.
    /// @param other The other instance to copy.
    AnalyzeSpansResult(const AnalyzeSpansResult &other);

    /// @brief Assignment operator.
    /// @param other The other instance to copy.
    /// @return a reference to this instance.
    auto operator=(AnalyzeSpansResult other) -> AnalyzeSpansResult &;

    /// @brief Swaps the contents of two AnalyzeSpansResult objects.
    /// @param other The other instance to swap with.
    void swap(AnalyzeSpansResult &other) noexcept;

    ValueMap resultMap;     ///< The result map from indices to values.
    std::uint64_t maxBound; ///< The maximum bound shifted to zero.
    bool allSpecified;      ///< True if indices fully cover the original span.
    bool allOthers;         ///< True if all index values match a given default.
};

/// @brief Analyzes a set of indices to unroll and pack their values.
/// @details This function processes indices to classify them as single values,
/// ranges, or slices, and stores the analysis results in the provided
/// `AnalyzeSpansResult` object.
/// @param spanType The type of the span represented by the given indices.
/// @param indexMap The input indices and their matching values.
/// @param sem The reference semantics used for analysis.
/// @param others Optional value for indices not present in `indexMap`.
/// @param result The object to store the analysis result.
/// @return False if an error occurs during analysis, true otherwise.

auto analyzeSpans(
    Type *spanType,
    const IndexMap &indexMap,
    hif::semantics::ILanguageSemantics *sem,
    Value *others,
    AnalyzeSpansResult &result) -> bool;

} // namespace analysis

} // namespace hif
