/// @file addUniqueObject.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/addUniqueObject.hpp"

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

enum Result : unsigned char { RES_INSERTED, RES_DUPLICATED, RES_NOT_SUITABLE };

Result _addUniqueObject(Object *obj, BList<Object> &list, const AddUniqueObjectOptions &opt)
{
    if (!list.checkSuitable(obj)) {
        if (opt.deleteIfNotAdded)
            delete obj;
        return RES_NOT_SUITABLE;
    }

    for (BList<Object>::iterator i = list.begin(); i != list.end(); ++i) {
        Object *current = *i;
        if (!hif::equals(obj, current, opt.equalsOptions))
            continue;
        if (opt.deleteIfNotAdded)
            delete obj;
        return RES_DUPLICATED;
    }

    // not present
    if (opt.copyIfUnique)
        obj = hif::copy(obj);

    if (opt.position == static_cast<unsigned int>(-1)) {
        list.push_back(obj);
    } else {
        BList<Object>::iterator it = list.begin();
        it                         = it + opt.position;
        messageAssert(
            opt.position == 0 || it != list.end(), "Passing wrong position where to insert the object.", obj, nullptr);
        if (list.empty())
            list.push_back(obj);
        else
            it.insert_before(obj);
    }

    return RES_INSERTED;
}

} // namespace

// end anon

// ///////////////////////////////////////////////////////////////////
// AddUniqueObjectOptions
// ///////////////////////////////////////////////////////////////////
AddUniqueObjectOptions::AddUniqueObjectOptions()
    : position(static_cast<unsigned int>(-1))
    , copyIfUnique(false)
    , deleteIfNotAdded(false)
    , equalsOptions()
{
    // ntd
}

AddUniqueObjectOptions::~AddUniqueObjectOptions()
{
    // ntd
}

AddUniqueObjectOptions::AddUniqueObjectOptions(const AddUniqueObjectOptions &other)
    : position(other.position)
    , copyIfUnique(other.copyIfUnique)
    , deleteIfNotAdded(other.deleteIfNotAdded)
    , equalsOptions(other.equalsOptions)
{
    // ntd
}

AddUniqueObjectOptions &AddUniqueObjectOptions::operator=(AddUniqueObjectOptions other)
{
    swap(other);
    return *this;
}

void AddUniqueObjectOptions::swap(AddUniqueObjectOptions &other)
{
    std::swap(position, other.position);
    std::swap(copyIfUnique, other.copyIfUnique);
    std::swap(deleteIfNotAdded, other.deleteIfNotAdded);
    equalsOptions.swap(other.equalsOptions);
}
// ///////////////////////////////////////////////////////////////////
// addUniqueObject methods
// ///////////////////////////////////////////////////////////////////

bool addUniqueObject(Object *obj, BList<Object> &list, const AddUniqueObjectOptions &opt)
{
    Result res = _addUniqueObject(obj, list, opt);
    return (res == RES_INSERTED);
}

template <typename T> bool addUniqueObject(Object *obj, BList<T> &list, const AddUniqueObjectOptions &opt)
{
    return addUniqueObject(obj, list.template toOtherBList<Object>(), opt);
}

bool addUniqueObject(Object *obj, Object *startingObject, const AddUniqueObjectOptions &opt)
{
    if (obj == nullptr)
        return false;
    if (startingObject == nullptr) {
        if (opt.deleteIfNotAdded)
            delete obj;
        return false;
    }

    AddUniqueObjectOptions opt2(opt);
    opt2.deleteIfNotAdded = false;

    Object *current = startingObject;

    // special case
    if (dynamic_cast<DesignUnit *>(current) != nullptr) {
        DesignUnit *du = static_cast<DesignUnit *>(current);
        current        = du->views.front();
    }

    while (current != nullptr) {
        const Object::BLists &lists = current->getBLists();
        for (Object::BLists::const_iterator i = lists.begin(); i != lists.end(); ++i) {
            BList<Object> *list   = *i;
            const Result inserted = _addUniqueObject(obj, *list, opt2);
            if (inserted == RES_INSERTED)
                return true;
            if (inserted == RES_DUPLICATED) {
                if (opt.deleteIfNotAdded)
                    delete obj;
                return false;
            }
        }

        current = current->getParent();
    }

    if (opt.deleteIfNotAdded)
        delete obj;
    return false;
}

/// @brief Instantiates the `addUniqueObject` method template for all HIF object types.
#define HIF_TEMPLATE_METHOD(T) bool addUniqueObject<T>(Object *, BList<T> &, const AddUniqueObjectOptions &)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

} // namespace manipulation
} // namespace hif
