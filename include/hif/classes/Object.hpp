/// @file Object.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <list>
#include <map>
#include <string>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/hifEnums.hpp"
#include "hif/manipulation/matchedInsertType.hpp"

namespace hif
{
class HifVisitor;

namespace semantics
{
class ILanguageSemantics;

Type *getBaseType(Type *type, bool consider_opacity, ILanguageSemantics *, bool compositeRecurse);

} // namespace semantics

namespace manipulation
{

bool matchedInsert(
    Object *newObj,
    Object *newParent,
    Object *oldObj,
    Object *oldParent,
    const MatchedInsertType::type type);

Object *matchedGet(Object *newParent, Object *oldObj, Object *oldParent);

} // namespace manipulation

/// @brief Abstract base class for all HIF objects.
///
/// @details
/// This class is the abstract base class for all HIF objects.
/// Every HIF class inherit from this class.
class Object
{
public:
    /// @brief Type for list of fields.
    typedef std::list<Object **> Fields;

    /// @brief Type for list of BLists.
    typedef std::list<BList<Object> *> BLists;

    /// @brief List of standard strings.
    typedef std::list<std::string> StringList;

    /// @brief Struct storing code info.
    struct CodeInfo {
        /// @brief Default constructor.
        CodeInfo();
        /// @brief Constructor with filename, line, and column.
        /// @param f The filename.
        /// @param l The line number.
        /// @param c The column number.
        CodeInfo(const std::string & f, unsigned int l, unsigned int c);
        /// @brief Destructor.
        ~CodeInfo();
        /// @brief Copy constructor.
        /// @param other The other CodeInfo to copy from.
        CodeInfo(const CodeInfo &other);
        /// @brief Assignment operator.
        /// @param other The other CodeInfo to assign from.
        /// @return Reference to this CodeInfo.
        CodeInfo &operator=(const CodeInfo &other);
        /// @brief Swap with another CodeInfo.
        /// @param other The other CodeInfo to swap with.
        void swap(CodeInfo &other);
        /// @brief Less than operator.
        /// @param o The other CodeInfo to compare with.
        /// @return true if this is less than o.
        bool operator<(const CodeInfo &o) const;

        /// @brief Returns the source file name concatenated with ':' and
        /// the corresponding line number.
        /// @return The source file name concatenated with ':' and
        /// the corresponding line number.
        std::string getSourceInfoString() const;

        /// @brief The name of the source code file.
        std::string filename;
        /// @brief Source code line number.
        unsigned int lineNumber;
        /// @brief Source code column number.
        unsigned int columnNumber;
    };

    /// @brief Map type for storing object properties.
    typedef std::map<std::string, TypedObject *> PropertyMap;

    /// @brief Iterator type for the property map.
    typedef PropertyMap::iterator PropertyMapIterator;

    /// @brief Destructor.
    virtual ~Object() = 0;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    virtual ClassId getClassId() const = 0;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis) = 0;

    /// @brief Returns whether the object is contained in a BList.
    /// @return <tt>true</tt> if the object is contained in a BList, <tt>false</tt> otherwise.
    bool isInBList() const;

    /// @brief Returns the BList containing the object (if any).
    /// @return The BList containing the object (if any).
    BList<Object> *getBList() const;

    /// @brief Returns the parent of the object in the HIF tree.
    /// @return The parent of the object in the HIF tree.
    Object *getParent() const;

    /// @brief Returns the parent of the object in the HIF tree, dynamically casted to given type.
    /// @return The parent of the object in the HIF tree.
    template <typename T> T *getParent() const;

    /// @brief Adds a property to the object.
    /// @param n The name of the property to be added to the object.
    /// @param v An optional value to be associated with the property.
    /// @return The previous value associated with the property if any.
    TypedObject *addProperty(const std::string &n, TypedObject *v = nullptr);

    /// @brief Adds a property to the object.
    /// @param n The name of the property to be added to the object.
    /// @param v An optional value to be associated with the property.
    /// @return The previous value associated with the property if any.
    TypedObject *addProperty(const PropertyId n, TypedObject *v = nullptr);

    /// @brief Removes a property from the object deleting associated value if any.
    /// @param n The name of the property to be removed.
    void removeProperty(const std::string &n);

    /// @brief Removes a property from the object deleting associated value if any.
    /// @param n The name of the property to be removed.
    void removeProperty(const PropertyId n);

    /// @brief Checks whether the object contains at least one property
    /// having @p n as its name.
    /// @param n The name of the property to be checked.
    /// @return <tt>true</tt> if the object contains at least one property
    /// named @p n, <tt>false</tt> otherwise.
    bool checkProperty(const std::string &n) const;

    /// @brief Checks whether the object contains at least one property
    /// having @p n as its name.
    /// @param n The name of the property to be checked.
    /// @return <tt>true</tt> if the object contains at least one property
    /// named @p n, <tt>false</tt> otherwise.
    bool checkProperty(const PropertyId n) const;

    /// @brief Returns the values of the property named @p n in the object.
    /// @param n The name of the property to be searched for.
    /// @return The value corresponding to the searched property.
    TypedObject *getProperty(const std::string &n) const;

    /// @brief Returns the values of the property named @p n in the object.
    /// @param n The name of the property to be searched for.
    /// @return The value corresponding to the searched property.
    TypedObject *getProperty(const PropertyId n) const;

    /// @brief Clears all the properties from the object.
    void clearProperties();

    /// @brief Returns an iterator pointing to the beginning of the map of
    /// properties of the object.
    /// @return Iterator pointing to the beginning of the map of properties
    /// of the object.
    PropertyMapIterator getPropertyBeginIterator();

    /// @brief Returns an iterator pointing to the end of the map of properties
    /// of the object.
    /// @return Iterator pointing to the end of the map of properties of
    /// the object.
    PropertyMapIterator getPropertyEndIterator();

    /// @brief Check if has properties.
    /// @return true if has properties.
    bool hasProperties() const;

    /// @brief Sets the source code line number.
    /// @param i The source code line number to be set.
    void setSourceLineNumber(unsigned int i);

    /// @brief Sets the source code column number.
    /// @param i The source code column number to be set.
    void setSourceColumnNumber(unsigned int i);

    /// @brief Returns the source code line number.
    /// @return The source code line number.
    unsigned int getSourceLineNumber() const;

    /// @brief Returns the source code column number.
    /// @return The source code column number.
    unsigned int getSourceColumnNumber() const;

    /// @brief Sets the name of the source code file.
    /// @param f The name of the source code file.
    void setSourceFileName(const std::string &f);

    /// @brief Returns the name of the source code file.
    /// @return The name of the source code file.
    std::string getSourceFileName() const;

    /// @brief Gets all current codeinfos.
    /// @return The current code info.
    const CodeInfo &getCodeInfo() const;

    /// @brief Sets all current codeinfos.
    /// @param ci The code info to set.
    void setCodeInfo(const CodeInfo &ci);

    /// @brief Returns the source file name concatenated with ':' and
    /// the corresponding line number.
    /// @return The source file name concatenated with ':' and
    /// the corresponding line number.
    std::string getSourceInfoString() const;

    /// @ brief Returns if object has at least one comment.
    /// @return True if object has at least one comment
    bool hasComments() const;

    /// @brief Adds a comment to the object.
    /// @param c The comment to add.
    void addComment(const std::string &c);

    /// @brief Adds a list of comments to the object.
    /// @param cc The list of comments to add.
    void addComments(const StringList &cc);

    /// @brief Removes a comment from the object.
    /// @param c The comment to remove.
    void removeComment(const std::string &c);

    /// @brief Removes all the comments from the object.
    void clearComment();

    /// @brief Returns the list of comments of the object.
    /// @return The list of comments of the object.
    StringList &getComments();

    /// @brief Replaces this object with another object in the HIF tree.
    /// @param other The object to perform replacement with.
    /// @return <tt>true</tt> if replacement was successful, <tt>false</tt> otherwise.
    bool replace(Object *other);

    /// @brief Replaces this object with given list of objects in the HIF tree.
    /// @warning Works only if this object is in a BList.
    /// @param list The list of objects to perform replacement with.
    /// @return <tt>true</tt> if replacement was successful, <tt>false</tt> otherwise.
    bool replaceWithList(BList<Object> &list);

    /// @brief Gets the list of internal fields.
    /// @return The list of internal fields.
    const Fields &getFields();

    /// @brief Gets the list of internal blists.
    /// @return The list of internal blists.
    const BLists &getBLists();

    /// @brief Sets a field, also updating pointers to parent.
    /// @param field The field to be set.
    /// @param newObj The new object to be set into the field.
    /// @return The old field object.

    template <typename T> T *setChild(T *&field, T *newObj);

    /// @brief Gets the field name into which this object is set.
    /// @return The field name.
    std::string getFieldName() const;

    /// @brief Gets the given BList name w.r.t. this.
    /// @param list The BList.
    /// @return The BList name.
    std::string getBListName(const BList<Object> &list) const;

    /// @brief Gets the given BList name w.r.t. this.
    /// @tparam T The type of objects in the BList.
    /// @param list The BList.
    /// @return The BList name.
    template <typename T>
    std::string getBListName(const BList<T> &list) const;

protected:
    /// @brief Constructor.
    Object();

    /// @brief Sets the parent of the object in the HIF tree.
    /// @param p The parent of the object to be set.
    void _setParent(Object *p);

    /// @brief Sets the BList containing the object.
    /// @param p The BList containing the object to be set.
    void _setBListParent(BList<Object> &p);

    /// @brief Sets the BList containing the object.
    /// @param p The BList containing the object to be set.
    template <typename T> void _setBListParent(BList<T> &p);

    /// @brief Sets the parent BList link of the object.
    /// @param p The parent BList link of the object to be set.
    void _setParentLink(void *p);

    /// @brief Sets the parent BList link of the object (if the object
    /// is stored in a BList).
    /// @return The parent BList link of the object.
    void *_getParentLink();

    /// @brief List of string representing comments on the objects.
    StringList *_comments;

    /// @brief Adds a field.
    ///
    /// @tparam T The field type. Implicit.
    /// @param f The field.
    ///
    template <typename T> void _addField(T *&f);

    /// @brief Adds a BList.
    ///
    /// @tparam T The BList type. Implicit.
    /// @param l The BList.
    ///
    template <typename T> void _addBList(BList<T> &l);

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields() = 0;

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object to get the name for.
    /// @return The name of the child field.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

    /// @brief The parent BList link of the object (if the object is stored in a BList).
    void *_parentlink;

    /// @brief The parent of the object in the HIF tree.
    Object *_parent;

    /// @brief Code information for the object.
    CodeInfo *_codeInfo;

    /// @brief Map of properties related to the object.
    PropertyMap *_properties;

    /// @brief Pointer to the parent's field into which this object is stored
    /// (into the parent).
    Object **_field;

    /// @brief List of all child fields for current leaf object.
    Fields *_fields;

    /// @brief List of all child blists for current leaf object.
    BLists *_blists;

private:
    Object *_setChild(Object **field, Object *newObj);

    /// @brief Private copy constructor to prevent construction from copy.
    Object(const Object &o);

    /// @brief Private assignment operator to prevent assignment.
    Object &operator=(const Object &o);

    template <class T> friend class BList;

    friend class BListHost;

    friend Type *hif::semantics::getBaseType(
        Type *type,
        bool consider_opacity,
        hif::semantics::ILanguageSemantics *,
        bool compositeRecurse);

    friend bool hif::manipulation::matchedInsert(
        Object *newObj,
        Object *newParent,
        Object *oldObj,
        Object *oldParent,
        const hif::manipulation::MatchedInsertType::type type);

    friend Object *hif::manipulation::matchedGet(Object *newParent, Object *oldObj, Object *oldParent);
};

} // namespace hif

#include "BList.hpp"
