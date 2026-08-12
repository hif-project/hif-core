/// @file matchPatternInTree.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/matchPatternInTree.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{
class MatchPatternInTreeVisitor : public GuideVisitor
{
public:
    MatchPatternInTreeVisitor(Object *pattern, std::list<Object *> &resulList, const hif::EqualsOptions &opt);

    virtual ~MatchPatternInTreeVisitor();

    virtual int AfterVisit(Object &o);

private:
    Object *_pattern;
    std::list<Object *> *_resultList;
    hif::EqualsOptions _opt;

    MatchPatternInTreeVisitor(const MatchPatternInTreeVisitor &);
    const MatchPatternInTreeVisitor &operator=(MatchPatternInTreeVisitor &);
};

MatchPatternInTreeVisitor::MatchPatternInTreeVisitor(
    Object *pattern,
    std::list<Object *> &resulList,
    const hif::EqualsOptions &opt)
    : _pattern(pattern)
    , _resultList(&resulList)
    , _opt(opt)
{
    // nothing to do
}

MatchPatternInTreeVisitor::~MatchPatternInTreeVisitor()
{
    // nothing to do
}

int MatchPatternInTreeVisitor::AfterVisit(Object &o)
{
    if (!hif::equals(_pattern, &o, _opt))
        return 0;
    _resultList->push_back(&o);

    return 0;
}

} // namespace

void matchPatternInTree(Object *pattern, Object *tree, std::list<Object *> &resulList, const hif::EqualsOptions &opt)
{
    if (tree == nullptr)
        return;
    MatchPatternInTreeVisitor v(pattern, resulList, opt);
    tree->acceptVisitor(v);
}

} // namespace manipulation
} // namespace hif
