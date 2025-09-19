/// @file Session.hpp
/// @brief Session management for HIF backend operations and state.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#ifdef HIFDIR_DBG
#    include <iomanip>
#    include <iostream>
#endif

#include "hif/classes/classes.hpp"

namespace hif
{
namespace backends
{

#ifdef HIFDIR_DBG

static int nIndent = 0;
char *PutnChar(int nI);

#    define INC_INDENT nIndent += 2
#    define DEC_INDENT nIndent -= 2
#    define INDENT     PutnChar(nIndent)

#    define METHOD "<<METHOD>> : "
#endif // HIFDIR_DBG
class CNodeVisitor;

/// @brief Describe a node of the model hierarchy
/// A node is dedicated to an instance or a library
/// @ingroup hif_dir
class CNode
{
public:
    //
    /// @brief Node status
    enum Mode_T : unsigned char {
        UNCHANGED, ///< Node is unchanged
        NEW,       ///< Is a new node
        MOVED,     ///< Node was moved
        REMOVED    ///< Node was removed
    };
    //
    /// @brief Instance tag : used for multi-instantiation
    /// Just one directory must exist.
    enum InstTag_T : unsigned char {
        ONLYONE,   ///< Just one instance
        ORIGINAL,  ///< Original instance
        ANOTHERONE ///< Another one
    };

private:
    CNode(const CNode &);
    auto operator=(const CNode &) -> CNode &;

    /// @brief Node status
    Mode_T m_eMode;

    /// @brief Instance flag
    InstTag_T m_eInstTag;

    /// @brief Path fixed by the property "path"
    std::vector<std::string> m_vPath;

    /// @brief Instance name
    std::string m_sInstName;

    /// @brief design unit name + view name
    std::string m_sEntName;

    /// @brief Link to the target parent node
    CNode *m_pnMovedTo;

    /// @brief If it the node is already instantiated :
    /// The pointer refers to the target node.
    CNode *m_pnFirstInst;

    /// @brief Parent node
    CNode *m_pnParent;

public:
    /// @brief List of child nodes
    std::list<CNode *> m_lpChild;

    /// @brief Default constructor
    CNode();

    /// @brief Destructor
    virtual ~CNode();

    /// @brief Set a parent node
    /// @param pnParent The parent node to set.
    void SetParent(CNode *pnParent);

    /// @brief Set a path (access to the node)
    /// @param sPath The path to set.
    void SetPath(std::string &sPath);

    /// @brief Set an instance name
    /// @param sName The instance name to set.
    void SetInstName(const std::string &sName);

    /// @brief Set an entity name.
    /// @param sName The entity name to set.
    void SetEntName(const std::string &sName);

    /// @brief Set an instance flag
    /// @param eTag The instance tag to set.

    void SetInstTag(InstTag_T eTag);

    /// @brief Set a status
    /// @param eMode The mode to set.
    void SetMode(Mode_T eMode);

    /// @brief Set all attributes of a node : status, path, name...
    /// @param rN The node to copy attributes from.
    void SetAttributes(CNode &rN);

    /// @brief Set m_pnFirstInst
    /// @param pnInst The instance node to set.
    void SetOrgPath(CNode *pnInst);

    /// @brief Set m_pnMovedTo
    /// @param pnParent The parent node to set.
    void SetMovedPath(CNode *pnParent);

    /// @brief Get the node name
    /// @return The node name.
    auto getName() -> std::string;

    /// @brief Get an abstract name which defining the node path
    /// @return The path vector.
    std::vector<std::string> GetPath();

    /// @brief Get an instance name
    /// @return The instance name.
    auto GetInstName() -> std::string { return m_sInstName; }
    /// @brief Get the entity name
    /// @return The entity name.
    auto GetEntName() -> std::string { return m_sEntName; }
    /// @brief Get the node status
    /// @return The mode.
    auto GetMode() -> Mode_T { return m_eMode; }
    /// @brief Get the instance flag
    /// @return The instance tag.
    auto GetInstTag() -> InstTag_T { return m_eInstTag; }

    /// @brief Get the first instance of a node
    /// @return The original node.
    auto GetOrgNode() -> CNode * { return m_pnFirstInst; }
    /// @brief Get the directory where the node must move to
    /// @return The moved to parent.
    auto MovedToParent() -> CNode * { return m_pnMovedTo; }

    /// @brief Compare two nodes
    /// @param rNr The node to compare with.
    /// @return True if equal.
    auto operator==(const CNode &rNr) -> bool;

    /// @brief Get a parent node of the current node
    /// @return The parent node.
    auto GetParent() -> CNode * { return m_pnParent; }

    /// @brief Define a visitor access on nodes
    /// @param rVis The visitor.
    /// @return The result of the visit.
    auto acceptVisitor(CNodeVisitor &rVis) -> int;

#ifdef HIFDIR_DBG
    const char *PrintMode();
    const char *PrintInstTag();
#endif // HIFDIR_DBG
};

//
/// @brief Define a session : set of nodes.
/// @ingroup hif_dir
class CSession
{
public:
    /// @brief List of sub-directories
    /// (applied on a design unit or a library def directory)
    enum Suffix : unsigned char {
        SRC, ///< "src" source directory
        INC, ///< "inc" include directory
        DOC, ///< "doc" documentation directory
        LIB, ///< "lib" library directory
        EXE, ///< "exe" executable directory
        OBJ  ///< "obj" object .o directory
    };

    /// @brief List of sub-directories
    /// (applied on a design unit or a library def directory)
    static const char *const CSuffix[];

    /// @brief List of head nodes
    std::list<CNode *> m_lpHead;

    /// @brief Default constructor
    CSession();

    /// @brief Destructor
    ~CSession();

    /// @brief Get a related path between the two inputs : vsTgt - vsSrc
    /// @param vsTgt The target path.
    /// @param vsSrc The source path.
    /// @return The relative path.
    static std::vector<std::string> Find(std::vector<std::string> &vsTgt, std::vector<std::string> &vsSrc);

    /// @brief Return a node instance corresponding to rnElt
    /// @param rnElt The element to find instance for.
    /// @return The instance node.
    auto FindAnInstance(CNode &rnElt) -> CNode *;

    /// @brief Return the original node instance corresponding to rnElt
    /// @param rnElt The element to find instance for.
    /// @return The original instance node.
    auto FindTheInstance(CNode &rnElt) -> CNode *;

    /// @brief Update the session with the PRECHECK flag
    /// @return The result of the precheck.
    auto PreCheck() -> int;

    /// @brief Update the session with the UPDATE flag
    /// @return The result of the update.
    auto Update() -> int;

    /// @brief Absolute Find methods
    /// @param rlTgt The target library.
    /// @return The path to the library.
    std::vector<std::string> Find(hif::LibraryDef &rlTgt);
    /// @param rduTgt The target design unit.
    /// @return The path to the design unit.
    std::vector<std::string> Find(hif::DesignUnit &rduTgt);
    /// @param rvTgt The target view.
    /// @return The path to the view.
    std::vector<std::string> Find(hif::View &rvTgt);
    /// @param riTgt The target instance.
    /// @return The path to the instance.
    std::vector<std::string> Find(hif::Instance &riTgt);
    /// @param sBase The base name.
    /// @param sView The view name.
    /// @return The path.
    std::vector<std::string> Find(std::string &sBase, std::string &sView);

    /// @brief Relative Find methods
    /// @param rduTgt The target design unit.
    /// @param rduSrc The source design unit.
    /// @return The found paths.
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::DesignUnit &rduSrc);

    /// @brief Find paths from a design unit to a view.
    /// @param rduTgt The target design unit.
    /// @param rvSrc The source view.
    /// @return The found paths.
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::View &rvSrc);

    /// @brief Find paths from a design unit to an instance.
    /// @param rduTgt The target design unit.
    /// @param riSrc The source instance.
    /// @return The found paths.
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::Instance &riSrc);

    /// @brief Find paths from a design unit to a library definition.
    /// @param rduTgt The target design unit.
    /// @param rlSrc The source library definition.
    /// @return The found paths.
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, hif::LibraryDef &rlSrc);

    /// @brief Find paths from a design unit to a base and view string.
    /// @param rduTgt The target design unit.
    /// @param sbSrc The source base string.
    /// @param svSrc The source view string.
    /// @return The found paths.
    std::vector<std::string> Find(hif::DesignUnit &rduTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods
    /// @brief Find paths from a view to a design unit.
    /// @param rvTgt The target view.
    /// @param rduSrc The source design unit.
    /// @return The found paths.
    std::vector<std::string> Find(hif::View &rvTgt, hif::DesignUnit &rduSrc);

    /// @brief Find paths from a view to a view.
    /// @param rvTgt The target view.
    /// @param rvSrc The source view.
    /// @return The found paths.
    std::vector<std::string> Find(hif::View &rvTgt, hif::View &rvSrc);

    /// @brief Find paths from a view to an instance.
    /// @param rvTgt The target view.
    /// @param riSrc The source instance.
    /// @return The found paths.
    std::vector<std::string> Find(hif::View &rvTgt, hif::Instance &riSrc);

    /// @brief Find paths from a view to a library definition.
    /// @param rvTgt The target view.
    /// @param rlSrc The source library definition.
    /// @return The found paths.
    std::vector<std::string> Find(hif::View &rvTgt, hif::LibraryDef &rlSrc);

    /// @brief Find paths from a view to a base and view string.
    /// @param rvTgt The target view.
    /// @param sbSrc The source base string.
    /// @param svSrc The source view string.
    /// @return The found paths.
    std::vector<std::string> Find(hif::View &rvTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods
    /// @brief Find paths from an instance to a design unit.
    /// @param riTgt The target instance.
    /// @param rduSrc The source design unit.
    /// @return The found paths.
    std::vector<std::string> Find(hif::Instance &riTgt, hif::DesignUnit &rduSrc);

    /// @brief Find paths from an instance to a view.
    /// @param riTgt The target instance.
    /// @param rvSrc The source view.
    /// @return The found paths.
    std::vector<std::string> Find(hif::Instance &riTgt, hif::View &rvSrc);

    /// @brief Find paths from an instance to an instance.
    /// @param riTgt The target instance.
    /// @param riSrc The source instance.
    /// @return The found paths.
    std::vector<std::string> Find(hif::Instance &riTgt, hif::Instance &riSrc);

    /// @brief Find paths from an instance to a library definition.
    /// @param riTgt The target instance.
    /// @param rlSrc The source library definition.
    /// @return The found paths.
    std::vector<std::string> Find(hif::Instance &riTgt, hif::LibraryDef &rlSrc);

    /// @brief Find paths from an instance to a base and view string.
    /// @param riTgt The target instance.
    /// @param sbSrc The source base string.
    /// @param svSrc The source view string.
    /// @return The found paths.
    std::vector<std::string> Find(hif::Instance &riTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods
    /// @brief Find paths from a library definition to a design unit.
    /// @param rlTgt The target library definition.
    /// @param rduSrc The source design unit.
    /// @return The found paths.
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::DesignUnit &rduSrc);

    /// @brief Find paths from a library definition to a view.
    /// @param rlTgt The target library definition.
    /// @param rvSrc The source view.
    /// @return The found paths.
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::View &rvSrc);

    /// @brief Find paths from a library definition to an instance.
    /// @param rlTgt The target library definition.
    /// @param riSrc The source instance.
    /// @return The found paths.
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::Instance &riSrc);

    /// @brief Find paths from a library definition to a library definition.
    /// @param rlTgt The target library definition.
    /// @param rlSrc The source library definition.
    /// @return The found paths.
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, hif::LibraryDef &rlSrc);

    /// @brief Find paths from a library definition to a base and view string.
    /// @param rlTgt The target library definition.
    /// @param sbSrc The source base string.
    /// @param svSrc The source view string.
    /// @return The found paths.
    std::vector<std::string> Find(hif::LibraryDef &rlTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Relative Find methods : target node = du base + view
    /// @brief Find paths from a base and view string to a design unit.
    /// @param sbTgt The target base string.
    /// @param svTgt The target view string.
    /// @param rduSrc The source design unit.
    /// @return The found paths.
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::DesignUnit &rduSrc);

    /// @brief Find paths from a base and view string to a view.
    /// @param sbTgt The target base string.
    /// @param svTgt The target view string.
    /// @param rvSrc The source view.
    /// @return The found paths.
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::View &rvSrc);

    /// @brief Find paths from a base and view string to an instance.
    /// @param sbTgt The target base string.
    /// @param svTgt The target view string.
    /// @param riSrc The source instance.
    /// @return The found paths.
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::Instance &riSrc);

    /// @brief Find paths from a base and view string to a library definition.
    /// @param sbTgt The target base string.
    /// @param svTgt The target view string.
    /// @param rlSrc The source library definition.
    /// @return The found paths.
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, hif::LibraryDef &rlSrc);

    /// @brief Find paths from a base and view string to a base and view string.
    /// @param sbTgt The target base string.
    /// @param svTgt The target view string.
    /// @param sbSrc The source base string.
    /// @param svSrc The source view string.
    /// @return The found paths.
    std::vector<std::string> Find(std::string &sbTgt, std::string &svTgt, std::string &sbSrc, std::string &svSrc);

    /// @brief Apply the command line pcLine
    /// @param pcLine The command line to apply.
    /// @return The result of the apply.
    auto Apply(const char *pcLine) -> int;

    /// @brief Apply methods on a specific node
    /// @param rduTgt The target design unit.
    /// @param pcLine The command line.
    /// @param pcSuffix The suffix.
    /// @return The result of the apply.
    auto Apply(hif::DesignUnit &rduTgt, const char *pcLine, const char *pcSuffix) -> int;
    /// @param rvTgt The target view.
    /// @param pcLine The command line.
    /// @param pcSuffix The suffix.
    /// @return The result of the apply.
    auto Apply(hif::View &rvTgt, const char *pcLine, const char *pcSuffix) -> int;
    /// @param riTgt The target instance.
    /// @param pcLine The command line.
    /// @param pcSuffix The suffix.
    /// @return The result of the apply.
    auto Apply(hif::Instance &riTgt, const char *pcLine, const char *pcSuffix) -> int;
    /// @param rlTgt The target library.
    /// @param pcLine The command line.
    /// @param pcSuffix The suffix.
    /// @return The result of the apply.
    auto Apply(hif::LibraryDef &rlTgt, const char *pcLine, const char *pcSuffix) -> int;
    /// \attention Target = base + view
    /// @param sbTgt The base target.
    /// @param svTgt The view target.
    /// @param pcLine The command line.
    /// @param pcSuffix The suffix.
    /// @return The result of the apply.
    auto Apply(std::string &sbTgt, std::string &svTgt, const char *pcLine, const char *pcSuffix) -> int;
};

#ifdef HIFDIR_DBG
const char *PrintSVector(std::vector<std::string> vsVec);
#endif // HIFDIR_DBG

} // namespace backends
} // namespace hif
