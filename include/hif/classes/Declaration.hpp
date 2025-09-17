/// @file Declaration.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <vector>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Object.hpp"
#include "hif/features/INamedObject.hpp"

namespace hif
{

/// @brief Abstract class for declarations.
class Declaration : public Object, public features::INamedObject
{
public:
    /// @brief A list of keywords.
    typedef std::vector<std::string> KeywordList;

    /// @brief Constructor.
    Declaration();

    /// @brief Destructor.
    virtual ~Declaration();

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

    /// @name Additional keywords handling methods.
    //@{

    /// @brief Adds a keyword to object list.
    /// @param kw The name of the keyword to be added.
    void addAdditionalKeyword(const std::string &kw);

    /// @brief Removes a keyword from object list.
    /// @param kw The name of the keyword to be removed.
    void removeAdditionalKeyword(const std::string &kw);

    /// @brief Check if has some additional keyword
    /// @return <tt>true</tt> if there is at least one additional keyword
    bool hasAdditionalKeywords();

    /// @brief Check whether the list contains the keyword @p kw.
    /// @param kw The name of the keyword to be checked.
    /// @return <tt>true</tt> if the list contains the keyword @p kw,
    /// <tt>false</tt> otherwise.
    bool checkAdditionalKeyword(const std::string &kw);

    /// @brief Clears all the keywords from the Declaration
    void clearAdditionalKeywords();

    /// @brief Returns an iterator to the beginning of the additional keywords list.
    /// @return Iterator to the beginning.
    KeywordList::iterator getAdditionalKeywordsBeginIterator();
    /// @brief Returns an iterator to the end of the additional keywords list.
    /// @return Iterator to the end.
    KeywordList::iterator getAdditionalKeywordsEndIterator();

    //@}

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief List of additional keywords (if any)
    KeywordList *_additionalKeywords;

private:
    // K: disabled.
    Declaration(const Declaration &);
    Declaration &operator=(const Declaration &);
};

} // namespace hif
