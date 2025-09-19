/// @file BListHost.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <string>

#include "hif/NameTable.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{
class Object;
class TypedObject;

/// @brief List of Hif objects.
/// @details
/// This class represents a doubly linked list of Hif objects.
/// It consists of a pointer to the head element, and a set of primitives for
/// manipulation.
class BListHost
{
public:
    typedef bool (*CheckSuitableMethod)(Object *);

private:
    struct BLink;

protected:
    class iterator;

    /// @brief Constructs a BListHost with a check method.
    /// @param checkSuitableMethod The method to check if an object is suitable.
    BListHost(CheckSuitableMethod checkSuitableMethod);

    /// @brief Copy constructor.
    /// @param other The BListHost to copy.
    BListHost(const BListHost &other);

    BListHost &operator=(const BListHost &) = delete;

    virtual ~BListHost();

    /// @brief Swaps the contents with another BListHost.
    /// @param other The BListHost to swap with.
    void swap(BListHost &other);

    /// @brief Get the BList name.
    /// @return The BList name.
    std::string getName() const;

    /// @brief Returns an iterator pointing to the first element in the list.
    /// @return Iterator pointing to the first element in the list.
    iterator begin() const;

    /// @brief Returns an iterator referring to the past-the-end element in the list.
    /// @return Iterator referring to the past-the-end element in the list.
    iterator end() const;

    /// @brief Returns a reverse iterator pointing to the last element in the list.
    /// @return Reverse iterator pointing to the last element in the list.
    iterator rbegin() const;

    /// @brief Returns a reverse iterator pointing to the reverse end of the list
    /// (i.e., the theoretical element preceding the first element in the list).
    /// @return Reverse iterator pointing to the reverse end of the list.
    iterator rend() const;

    /// @brief Returns the first element in the list.
    /// @return The first element in the list.
    Object *front() const;

    /// @brief Returns the  last element in the list.
    /// @return The last element in the list.
    Object *back() const;

    /// @brief Inserts an element at the beginning of the list.
    /// @param a The element to insert.
    void push_front(Object *a);

    /// @brief Inserts an element at the end of the list.
    /// @param a The element to insert.
    void push_back(Object *a);

    /// @brief Deletes the first occurrence of element <tt>a</tt> from the list.
    /// @param a The element to be deleted.
    void erase(Object *a);

    /// @brief Removes the first occurrence of element <tt>a</tt> from the list
    /// without deleting it.
    /// @param a The element to be removed.
    void remove(Object *a);

    /// @brief Removes all elements from the list without deleting it.
    void removeAll();

    /// @brief Deletes the subtree which contains <tt>a</tt> from the list.
    /// @param a The element.
    void eraseSubTree(Object *a);

    /// @brief Removes without deleting the subtree which contains <tt>a</tt> from the list.
    /// @param a The element.
    void removeSubTree(Object *a);

    /// @brief Clears the list by removing all its elements.
    void clear();

    /// @brief Returns whether the list is empty.
    /// @return <tt>true</tt> if the list is empty, <tt>false</tt> otherwise.
    bool empty() const;

    /// @brief Computes the size of the list (i.e., the number of elements in the list).
    /// @return The number of elements in the list.
    size_t size() const;

    /// @brief Merges the current list with list @p x.
    /// All elements of @p x are added at the end of the current list.
    /// @param x The list to be merged.
    void merge(BListHost &x);

    /// @brief Function to remove duplicated elements from the current list.
    /// @param strict If <tt>true</tt>, remove duplicated elements if pointers
    /// to objects are the same.
    void remove_dopplegangers(bool strict = false);

    /// @brief Function to get the parent object of the list in the Hif tree.
    /// @return The parent object of the list in the Hif tree.
    Object *getParent();

    /// @brief Sets the parent object in the Hif tree for every element in the list.
    /// @param p The parent object to be set.
    void setParent(Object *p);

    /// @brief Sorts all the elements of the list according to a given comparator.
    /// The comparator shall accept two parameters, which are pointers of type of the elements
    /// of the BList.
    /// The comparator shall return a negative or positive interer as result of the comparison.
    /// Elements are ordered with a strict order criterion (less).
    /// Sorting algorithm is stable.
    /// @param c The comparator.
    /// @return <tt>True</tt> if at least sort has been performed, <tt>false</tt> otherwise.
    template <typename Comparator> bool sort(Comparator &c);

    /// @brief Swaps the position of two elements.
    /// The iterators can belong to different lists, but the elements must be compatible.
    /// @param a Iterator pointing to the first element to be swapped.
    /// @param b Iterator pointing to the second element to be swapped.
    static void swap(iterator a, iterator b);

    /// @brief Returns the position of a given element in the list.
    /// @param o The element to be found.
    /// @return The position of the given element in the list, or <tt>list.size()</tt>
    /// if the element is not found in the list.
    size_t getPosition(Object *o) const;

    /// @brief Inserts an element at the given position, or at the end if the
    /// list has fewer elements than the given position.
    /// @param o The element to insert.
    /// @param pos The position.
    /// @param expand If <tt>false</tt>, replace the element ati the given position.
    /// @return The previous element at the given position (if any).
    Object *insert(Object *o, std::size_t pos, bool expand);

    /// @brief Returns the element at the given position, or <tt>nullptr</tt> in case of error.
    /// @param pos The position.
    /// @return The element at the given position, or <tt>nullptr</tt> in case of error.
    Object *at(std::size_t pos) const;

    /// @brief Checks whether a given element is contained in this list.
    /// @param o The element to be checked.
    /// @return <tt>true</tt> if the element is contained in the list, <tt>false</tt> otherwise.
    bool contains(Object *o) const;

    /// @brief Returns the first Object in the list matching given name.
    /// @param n The name
    /// @return The first Object in the list matching given name.
    Object *findByName(const std::string &n) const;

    /// @brief Check whether passed object can be inserted into current BList.
    /// @param o The object to check.
    /// @return true if the object can be inserted, false otherwise.
    bool checkSuitable(Object *o) const;

    /// @name Properties management methods.
    /// They works on all objects inside the BList.
    /// @{

    /// @brief Adds a property to the objects.
    /// @warning If property is already present, it does nothing.
    /// @param n The name of the property to be added to the objects.
    /// @param v An optional value to be associated with the property.
    void addProperty(const std::string &n, TypedObject *v = nullptr);

    /// @brief Adds a property to the objects.
    /// @warning If property is already present, it does nothing.
    /// @param n The name of the property to be added to the objects.
    /// @param v An optional value to be associated with the property.
    void addProperty(const PropertyId n, TypedObject *v = nullptr);

    /// @brief Removes a property from the objects.
    /// @param n The name of the property to be removed.
    void removeProperty(const std::string &n);

    /// @brief Removes a property from the objects.
    /// @param n The name of the property to be removed.
    void removeProperty(const PropertyId n);

    /// @brief Checks whether the objects contains at least one property
    /// having @p n as its name.
    /// @param n The name of the property to be checked.
    /// @param hasAll If <tt>true</tt> checks that all objects has the property.
    /// @return <tt>true</tt> if the objects contains at least one property
    /// named @p n, <tt>false</tt> otherwise.
    bool checkProperty(const std::string &n, bool hasAll) const;

    /// @brief Checks whether the objects contains at least one property
    /// having @p n as its name.
    /// @param n The name of the property to be checked.
    /// @param hasAll If <tt>true</tt> checks that all objects has the property.
    /// @return <tt>true</tt> if the objects contains at least one property
    /// named @p n, <tt>false</tt> otherwise.
    bool checkProperty(const PropertyId n, bool hasAll) const;

    /// @brief Clears all the properties from the objects.
    void clearProperties();

    /// @brief Check if has properties.
    /// @param hasAll If <tt>true</tt> checks that all objects has properties.
    /// @return true if has properties.
    bool hasProperties(bool hasAll) const;

    /// @}

private:
    static BLink *_toBLink(void *l);

    /// @brief The parent object of the list.
    Object *_parent;

    /// @brief The first element of the list.
    BLink *_head;

    /// @brief The last element of the list.
    BLink *_tail;

    /// @brief The method pointer to check suitable objects.
    CheckSuitableMethod _checkSuitableMethod;

    friend class Object;

protected:
    /// @brief Iterator class for the list.
    class iterator
    {
    public:
        /// @brief Constructor.
        /// @param o The object to initialize the iterator with.
        explicit iterator(Object *o = nullptr);

        /// @brief Copy constructor.
        /// @param i The iterator to copy from.
        iterator(const iterator &i);

        /// @brief Destructor.
        virtual ~iterator();

        /// @brief Assignment operator.
        /// @param i Iterator to the new list element.
        /// @return Iterator to the new assigned value.
        iterator &operator=(const iterator &i);

        /// @brief Assignment operator.
        /// @param o The element to be assigned.
        /// @return This iterator.
        iterator &operator=(Object *o);

        /// @brief Erases the current element.
        /// @return Iterator pointing to the next element in the list.
        iterator &erase();

        /// @brief Erase the current element.
        /// @return Iterator pointing to the previous iterator in the list.
        iterator &rerase();

        /// @brief Removes the current element.
        /// @return Iterator pointing to the next element in the list.
        iterator &remove();

        /// @brief Remove the current element.
        /// @return Iterator pointing to the previous iterator in the list.
        iterator &rremove();

        /// @brief Inserts an element after the current element.
        /// @param a The element to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_after(Object *a);

        /// @brief Insert an element before the current element.
        /// @param a The element to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_before(Object *a);

        /// @brief Inserts a list after the current element.
        /// @param a The list to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_after(BListHost &a);

        /// @brief Insert a list before the current element.
        /// @param a The list to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_before(BListHost &a);

        /// @brief Returns true if the iterator is at end.
        /// @return The check result.
        bool isEnd() const;

        /// @brief Returns the current element.
        /// @return The current element.
        Object *operator*() const;

        /// @brief Moves to the next element.
        /// @return Iterator to the next element.
        iterator operator++(int);

        /// @brief Moves to next element.
        /// @return Iterator to the next element.
        iterator &operator++();

        /// @brief Moves to the previous element.
        /// @return Iterator to the previous element.
        iterator operator--(int);

        /// @brief Moves to the previous element.
        /// @return Iterator to the previous element.
        iterator &operator--();

        /// @brief Moves the iterator forward by @p s positions.
        /// @param s The number of positions to move forward.
        /// @return The resulting iterator.
        iterator operator+(std::size_t s) const;

        /// @brief Moves iterator backward of @p s positions.
        /// @param s The number of positions to move backward.
        /// @return The resulting iterator.
        iterator operator-(std::size_t s) const;

        /// @brief Equality operator.
        /// @param i Iterator to compare with the current iterator.
        /// @return <tt>true</tt> if the current element coincides with
        /// the element pointed by @p i, <tt>false</tt> otherwise.
        bool operator==(const iterator &i) const;

        /// @brief Inequality operator.
        /// @param i Iterator to compare with the current iterator.
        /// @return <tt>true</tt> if the current element does not coincide
        /// with the element pointed by @p i, <tt>false</tt> otherwise.
        bool operator!=(const iterator &i) const;

        /// @brief Returns the iterator to the next element.
        /// @return the iterator to the next element.
        iterator next() const;

        /// @brief Returns the iterator to the previous element.
        /// @return the iterator to the previous element.
        iterator prev() const;

    private:
        /// @brief The current element of the list.
        BLink *_link;
    };

private:
    /// @brief Link item in a BList.
    ///
    /// This class represents a bi-directional link for an element in a BList.
    struct BLink {
    public:
        /// @brief Pointer to the list the link belongs to.
        BListHost *parentlist;

        /// @brief Pointer to the next link in the list.
        BLink *next;

        /// @brief Pointer to the previous link in the list.
        BLink *prev;

        /// @brief Pointer to the element held by the link.
        Object *element;

        /// @brief Constructor.
        BLink();

        /// @brief Destructor.
        ~BLink();

        /// @brief Removes the current link from its list.
        void removeFromList();

        /// @brief Swaps the currently stored element with the given link.
        /// @param link The link to swap with.
        void swap(BLink *link);

    private:
        BLink(const BLink &);
        BLink &operator=(const BLink &);
    };
};

template <typename Comparator> bool BListHost::sort(Comparator &c)
{
    // Bubble sort is more than enough.

    // Bubble code for arrays (from wikibooks):
    // int i;
    // int tmp;
    // int high = size();
    //
    // while ( high > 1)
    // {
    //     for ( i = 0; i < high -1; ++i )
    //     {
    //         if ( array[ i ] > array[ i +1 ] )
    //         {
    //             tmp = array[i];
    //             array[ i ] = array[ i+1 ];
    //             array[ i+1 ] = tmp;
    //         }
    //     }
    //     -- high;
    // }

    // Translation for BListHosts:

    bool changed  = false;
    iterator high = rbegin();
    // empty?
    if (high == rend())
        return changed;

    // general case:
    // Order in growing mode
    while (high != begin()) {
        for (iterator i = begin(); i != high; ++i) {
            iterator next(i);
            ++next;
            if (c(*i, *next) > 0) {
                swap(next, i);
                changed = true;
            }
        }
        --high;
    }

    return changed;
}

} // namespace hif
