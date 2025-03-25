/// @file BList.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <cassert>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BListHost.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-template-vtables"
#endif

namespace hif
{

/// @brief List of Hif objects.
/// @details
/// This class represents a doubly linked list of Hif objects. It consists of a
/// pointer to the head element, and a set of primitives for manipulation.
template <class T> class BList : private BListHost
{
public:
    class iterator;

    BList();

    /// @brief Deep copy of all objects.
    BList(const BList<T> &other);

    /// @brief Deep copy of all objects.
    BList<T> &operator=(BList<T> other);

    virtual ~BList();

    void swap(BList<T> &other);

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
    T *front() const;

    /// @brief Returns the  last element in the list.
    /// @return The last element in the list.
    T *back() const;

    /// @brief Inserts an element at the beginning of the list.
    /// @param a The element to insert.
    void push_front(T *a);

    /// @brief Inserts an element at the end of the list.
    /// @param a The element to insert.
    void push_back(T *a);

    /// @brief Deletes the first occurrence of element <tt>a</tt> from the list.
    /// @param a The element to be deleted.
    void erase(T *a);

    /// @brief Removes the first occurrence of element <tt>a</tt> from the list
    /// without deleting it.
    /// @param a The element to be removed.
    void remove(T *a);

    /// @brief Removes all elements from the list without deleting it.
    void removeAll();

    /// @brief Deletes the subtree which contains <tt>a</tt> from the list.
    /// @param a The element.
    void eraseSubTree(T *a);

    /// @brief Removes without deleting the subtree which contains <tt>a</tt> from the list.
    /// @param a The element.
    void removeSubTree(T *a);

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
    void merge(BList<T> &x);

    /// @brief Function to remove duplicated elements from the current list.
    /// @param strict If <tt>true</tt>, remove duplicated elements if pointers
    /// to objects are the same.
    void remove_dopplegangers(const bool strict = false);

    /// @brief Function to get the parent object of the list in the Hif tree.
    /// @return The parent object of the list in the Hif tree.
    Object *getParent();

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

    /// @brief Implicit cast operator to BList of type <tt>T1</tt>.
    template <typename T1> operator BList<T1> &();

    /// @brief Returns the position of a given element in the list.
    /// @param o The element to be found.
    /// @return The position of the given element in the list, or <tt>list.size()</tt>
    /// if the element is not found in the list.
    size_t getPosition(T *o) const;

    /// @brief Inserts an element at the given position, or at the end if the
    /// list has fewer elements than the given position.
    /// @param o The element to insert.
    /// @param pos The position.
    /// @param expand If <tt>false</tt>, replace the element ati the given position.
    /// @return The previous element at the given position (if any).
    T *insert(T *o, std::size_t pos, const bool expand);

    /// @brief Returns the element at the given position, or <tt>nullptr</tt> in case of error.
    /// @param pos The position.
    /// @return The element at the given position, or <tt>nullptr</tt> in case of error.
    T *at(std::size_t pos) const;

    /// @brief Checks whether a given element is contained in this list.
    /// @param o The element to be checked.
    /// @return <tt>true</tt> if the element is contained in the list, <tt>false</tt> otherwise.
    bool contains(T *o) const;

    /// @brief Returns the first Object in the list matching given name.
    /// @param n The name
    /// @return The first Object in the list matching given name.
    T *findByName(const std::string &n) const;

    /// @brief Check whether passed object can be inserted into current BList.
    bool checkSuitable(Object *o) const;

    /// @brief Converts this list to a list of different objects. No check is performed.
    /// @return A list of different objects.
    template <typename T1> BList<T1> &toOtherBList();

    /// @brief Converts this list to a list of different objects. No check is performed.
    /// @return A list of different objects.
    template <typename T1> const BList<T1> &toOtherBList() const;

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
    bool checkProperty(const std::string &n, const bool hasAll) const;

    /// @brief Checks whether the objects contains at least one property
    /// having @p n as its name.
    /// @param n The name of the property to be checked.
    /// @param hasAll If <tt>true</tt> checks that all objects has the property.
    /// @return <tt>true</tt> if the objects contains at least one property
    /// named @p n, <tt>false</tt> otherwise.
    bool checkProperty(const PropertyId n, const bool hasAll) const;

    /// @brief Clears all the properties from the objects.
    void clearProperties();

    /// @brief Check if has properties.
    /// @param hasAll If <tt>true</tt> checks that all objects has properties.
    /// @return true if has properties.
    bool hasProperties(const bool hasAll) const;

    /// @}

private:
    /// @brief Function to set the parent of the list to <tt>p</tt>.
    /// For every element of the list parent is set to <tt>p</tt>.
    ///
    /// @param p pointer to Object representing the parent node to set.
    ///
    void setParent(Object *p);

    static bool _checkSuitable(Object *o);

    friend class Object;

public:
    /// @brief Iterator class for the list.
    class iterator : private BListHost::iterator
    {
    public:
        /// @brief Constructor.
        explicit iterator(T *o = nullptr);

        /// @brief Copy constructor.
        iterator(const iterator &i);

        /// @brief BListHost::iterator constructor.
        iterator(const BListHost::iterator &i);

        /// @brief Destructor.
        virtual ~iterator();

        /// @brief Assignment operator.
        /// @param i Iterator to the new list element.
        /// @return Iterator to the new assigned value.
        iterator &operator=(const iterator &i);

        /// @brief Assignment operator.
        /// @param o The element to be assigned.
        /// @return This iterator.
        iterator &operator=(T *o);

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
        iterator insert_after(T *a);

        /// @brief Insert an element before the current element.
        /// @param a The element to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_before(T *a);

        /// @brief Inserts a list after the current element.
        /// @param a The list to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_after(BList<T> &a);

        /// @brief Insert a list before the current element.
        /// @param a The list to insert.
        /// @return Iterator to the newly inserted element in the list.
        iterator insert_before(BList<T> &a);

        /// @brief Returns true if the iterator is at end.
        /// @return The check result.
        bool isEnd() const;

        /// @brief Returns the current element.
        /// @return The current element.
        T *operator*() const;

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
        iterator next() const { return static_cast<iterator>(BListHost::iterator::next()); }

        /// @brief Returns the iterator to the previous element.
        /// @return the iterator to the previous element.
        iterator prev() const { return static_cast<iterator>(BListHost::iterator::prev()); }

        BListHost::iterator &toBaseClass();
    };
};

} // namespace hif

#include "BList.i.hpp"
