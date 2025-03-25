/// @file search.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"

namespace hif
{

namespace /* anon */
{
class HifSearchVisitor : public GuideVisitor
{
public:
    typedef std::set<Declaration *> CheckSet;

    HifSearchVisitor(std::list<Object *> &result, const HifQueryBase &query);
    ~HifSearchVisitor();

    bool BeforeVisit(Object &o);
    int AfterVisit(Object &o);

    int visitProcedureCall(ProcedureCall &o);
    int visitFunctionCall(FunctionCall &o);

    int visitLibraryDef(LibraryDef &o);
    int visitView(View &o);

private:
    std::list<Object *> &_result;
    const HifQueryBase &_query;
    std::size_t _currentDepth;
    CheckSet _checkedSet;

    HifSearchVisitor(const HifSearchVisitor &);
    HifSearchVisitor &operator=(const HifSearchVisitor &);
};

HifSearchVisitor::HifSearchVisitor(std::list<Object *> &result, const HifQueryBase &query)
    : GuideVisitor()
    , _result(result)
    , _query(query)
    , _currentDepth(0)
    , _checkedSet()
{
    // ntd
}

HifSearchVisitor::~HifSearchVisitor()
{
    // ntd
}

bool HifSearchVisitor::BeforeVisit(Object &o)
{
    if (_query.classToAvoid.find(o.getClassId()) != _query.classToAvoid.end())
        return true;

    ++_currentDepth;

    if (_query.depth != 0 && _currentDepth > _query.depth) {
        --_currentDepth;
        return true;
    }

    if (_query.onlyFirstMatch && !_result.empty())
        return true;

    return false;
}

int HifSearchVisitor::AfterVisit(Object &o)
{
    --_currentDepth;

    bool add = true;

    if (!_query.name.empty() && hif::objectGetName(&o) != _query.name) {
        add = false;
    }

    if (!_query.isSameType(&o)) {
        add = false;
    }

    if (_query.check_object_method && !_query.check_object_method(&o, &_query)) {
        add = false;
    }

    if (add)
        _result.push_back(&o);

    return 0;
}

int HifSearchVisitor::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);

    if (_query.sem == nullptr)
        return 0;
    if (_query.checkInsideCallsDeclarations == false)
        return 0;

    Declaration *d = hif::semantics::getDeclaration(&o, _query.sem);
    if (d == nullptr)
        return 0;

    if (_checkedSet.find(d) != _checkedSet.end())
        return 0;
    _checkedSet.insert(d);
    d->acceptVisitor(*this);

    return 0;
}

int HifSearchVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);

    if (_query.sem == nullptr)
        return 0;
    if (_query.checkInsideCallsDeclarations == false)
        return 0;

    Declaration *d = hif::semantics::getDeclaration(&o, _query.sem);
    if (d == nullptr)
        return 0;

    if (_checkedSet.find(d) != _checkedSet.end())
        return 0;
    _checkedSet.insert(d);
    d->acceptVisitor(*this);

    return 0;
}

int HifSearchVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard() && _query.skipStandardScopes)
        return 0;
    return GuideVisitor::visitLibraryDef(o);
}

int HifSearchVisitor::visitView(View &o)
{
    if (o.isStandard() && _query.skipStandardScopes)
        return 0;
    return GuideVisitor::visitView(o);
}

} // namespace

HifQueryBase::HifQueryBase()
    : depth(0)
    , name()
    , classToAvoid()
    , check_object_method(nullptr)
    , checkInsideCallsDeclarations(false)
    , onlyFirstMatch(false)
    , skipStandardScopes(false)
    , matchTypeVariant(false)
    , typeVariant(Type::NATIVE_TYPE)
    , sem(nullptr)
{
    // ntd
}

HifQueryBase::~HifQueryBase()
{
    // ntd
}

HifUntypedQuery::HifUntypedQuery()
    : HifQueryBase()
{
    // ntd
}

HifUntypedQuery::~HifUntypedQuery()
{
    // ntd
}

bool HifUntypedQuery::isSameType(Object *) const { return true; }

void search(std::list<Object *> &result, Object *root, const HifQueryBase &query)
{
    HifSearchVisitor v(result, query);
    root->acceptVisitor(v);
}
} // namespace hif
