/// @file sortGraph.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{

namespace analysis
{

#ifndef NDEBUG
//#define DEBUG_SORT
#endif

namespace /* anon */
{

using List = Types<Object, Object>::List;
using Size = List::size_type;

auto _getPosition(const List &order, Object *o) -> Size
{
    Size ret = 0;
    for (auto i = order.begin(); i != order.end(); ++i, ++ret) {
        if (*i == o) {
            return ret;
        }
    }
    return static_cast<Size>(-1);
}

struct Comp {
    Comp(List *list, bool reverse);
    ~Comp();
    Comp(const Comp &other);
    auto operator=(const Comp &other) -> Comp &;

    auto operator()(Object *o1, Object *o2) const -> bool;

    List *_list;
    bool _reverse;
};

Comp::Comp(List *list, bool reverse)
    : _list(list)
    , _reverse(reverse)
{
    // ntd
}

Comp::~Comp()
{
    // ntd
}

Comp::Comp(const Comp &other)
    : _list(other._list)
    , _reverse(other._reverse)
{
    // ntd
}

auto Comp::operator=(const Comp &other) -> Comp &
{
    if (this == &other) {
        return *this;
    }

    _list    = other._list;
    _reverse = other._reverse;

    return *this;
}

auto Comp::operator()(Object *o1, Object *o2) const -> bool
{
    if (_list == nullptr) {
        if (_reverse) {
            return (o1 > o2);
        }
        return (o2 < o1);
    }

    Size s1 = _getPosition(*_list, o1);
    Size s2 = _getPosition(*_list, o2);
    if (_reverse) {
        return (s1 > s2);
    }
    return (s1 < s2);
}

} // namespace
void sortGraph(
    Types<Object, Object>::Graph &graph,
    Types<Object, Object>::List &list,
    bool fromLeaves,
    Types<Object, Object>::List *stableList)
{
    using ObjectsMap  = Types<Object, Object>::Map;
    using Cardinality = std::size_t;
    using StableSet   = std::set<Object *, Comp>;
    using EntriesMap  = std::map<Cardinality, Types<Object, Object>::Set>;
    using EntriesSet  = std::set<Cardinality>;

    EntriesMap entriesMap;
    EntriesSet entriesSet;

    ObjectsMap refsMap;
    if (fromLeaves) {
        refsMap = graph.second;
    } else {
        refsMap = graph.first;
    }

#ifdef DEBUG_SORT
    hif::PrintHifOptions opt;
    opt.printSummary = true;
    for (ObjectsMap::iterator i = refsMap.begin(); i != refsMap.end(); ++i) {
        Object *obj = i->first;
        std::clog << "# key = ";
        hif::writeFile(std::clog, obj, false, opt);
        std::clog << std::endl;
        for (ObjectsMap::mapped_type::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            std::clog << "#### value = ";
            hif::writeFile(std::clog, *j, false, opt);
            std::clog << std::endl;
        }
        std::clog << std::endl;
    }
#endif

    Cardinality c = 0;
    while (!refsMap.empty()) {
        bool infiniteLoop = true;
        for (auto i = refsMap.begin(); i != refsMap.end();) {
            if (!i->second.empty()) {
                ++i;
                continue;
            }
            entriesMap[c].insert(i->first);
            entriesSet.insert(c);
#ifdef DEBUG_SORT
            messageDebug("Inserting this object.", i->first, nullptr);
#endif
            refsMap.erase(i++);
            infiniteLoop = false;
        }

#ifndef NDEBUG
        if (infiniteLoop) {
            PrintHifOptions popt;
            popt.printSummary = true;
            std::clog << "--------------------------------------------------\n";
            messageDebug("Found infinite loop.", nullptr, nullptr);
            for (auto &i : refsMap) {
                std::clog << "--------------------------------------------------\n";
                Object *ref         = i.first;
                std::string refName = hif::objectGetName(ref);
                std::clog << "Reference object:\n";
                if (refName.empty()) {
                    hif::writeFile(std::clog, ref, false, popt);
                } else {
                    std::clog << refName << " (" << hif::classIDToString(ref->getClassId()) << ")" << '\n';
                }
                std::clog << "Dependencies:\n";
                for (auto dep : i.second) {
                    std::string depName = hif::objectGetName(dep);
                    if (depName.empty()) {
                        hif::writeFile(std::clog, dep, false, popt);
                        std::clog << '\n';
                    } else {
                        std::clog << depName << " (" << hif::classIDToString(ref->getClassId()) << ")" << '\n';
                    }
                }
            }
            std::clog << "--------------------------------------------------\n";
            messageDebug("End of infinite loop.", nullptr, nullptr);
            std::clog << "--------------------------------------------------\n";
        }
#endif
        messageAssert(!infiniteLoop, "Graph to be sorted is not a DAG.", nullptr, nullptr);

        for (auto &i : refsMap) {
            for (auto j = i.second.begin(); j != i.second.end();) {
                auto it = entriesMap[c].find(*j);
                if (it == entriesMap[c].end()) {
                    ++j;
                    continue;
                }
                i.second.erase(j++);
            }
        }

        ++c;
    }

    for (unsigned long i : entriesSet) {
        StableSet tmp(Comp(stableList, fromLeaves));

        for (auto j : entriesMap[i]) {
            tmp.insert(j);
        }

        for (auto *j : tmp) {
            list.push_back(j);
        }
    }
}

} // namespace analysis

} // namespace hif
