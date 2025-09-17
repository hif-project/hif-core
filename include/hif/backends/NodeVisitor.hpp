/// @file NodeVisitor.hpp
/// @brief Visitor for traversing and processing backend node structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <list>

#include "hif/backends/Properties.hpp"
#include "hif/backends/Session.hpp"

namespace hif
{
namespace backends
{

/// @brief Node visitor class
/// @ingroup hif_dir
class CNodeVisitor
{
protected:
    /// @brief Returned value
    int m_nR;

public:
    /// @brief Constructor
    /// @param nR value to return
    CNodeVisitor(int nR = 0);
    /// @brief Destructor
    virtual ~CNodeVisitor();

    /// @brief Visit a nodes list.
    /// @param rL list of nodes.
    /// @return 0 if success
    int visitList(std::list<CNode *> &rL);

    /// @brief Visit a node.
    /// @param rN node to visit.
    /// @return 0 if success.
    virtual int visitCNode(CNode &rN);
};

/// @brief Find a node in a session.
/// @ingroup hif_dir
class CFindVisitor : public CNodeVisitor
{
private:
    CFindVisitor(const CFindVisitor &);
    CFindVisitor &operator=(const CFindVisitor &);

    /// @brief Element to find.
    CNode m_nElt;
    /// @brief Find the original instance.
    bool m_bInst;
    /// @brief Found node.
    CNode *m_pnRes;

    /// @brief Default constructor.
    CFindVisitor();

public:
    /// @brief Constructor visit the structure.
    /// @param rsTop Session to check.
    /// @param rnElt Element to find.
    /// @param bInst true => find the original node instance.
    CFindVisitor(CSession &rsTop, CNode &rnElt, bool bInst);

    /// @brief Destructor.
    virtual ~CFindVisitor();

    /// @brief Return the found node.
    /// @return Found node.
    inline CNode *GetNode() { return m_pnRes; }

    /// @brief Visit the list of nodes.
    /// @param rL list of nodes.
    /// @return 0 if success.
    int visitList(std::list<CNode *> &rL);

    /// @brief Visit a node.
    /// @param rN node to visit.
    /// @return 0 if success.
    virtual int visitCNode(CNode &rN);
};

/// @brief Update a session three modes PRECHECK, UPDATE and REMOVE.
/// @details
/// PRECHECK    : set all nodes status in REMOVED.
/// UPDATE      : move "MOVED "nodes and create "NEW" nodes.
/// REMOVE      : launched after an UPDATE stage.
/// @ingroup hif_dir
class CUpdateVisitor : public CNodeVisitor
{
public:
    /// @brief List of available operations.
    enum FMode_T : unsigned char {
        PRECHECK, ///< set all nodes status in REMOVED.
        UPDATE,   ///< move "MOVED "nodes and create "NEW" nodes.
        REMOVE,   ///< remove "REMOVED" tagged nodes.
    };

    /// @brief Constructor visit the structure.
    /// @param psTop Session to check.
    /// @param eMode chosen operation.
    CUpdateVisitor(CSession *psTop, FMode_T eMode);

    /// @brief Destructor.
    virtual ~CUpdateVisitor();

    CUpdateVisitor(const CUpdateVisitor &)            = delete;
    CUpdateVisitor &operator=(const CUpdateVisitor &) = delete;

    /// @brief Visit the list of nodes.
    /// @param rL list of nodes.
    /// @return 0 if success.
    int visitList(std::list<CNode *> &rL);

    /// @brief Visit a node.
    /// @param rN node to visit.
    /// @return 0 if success.
    virtual int visitCNode(CNode &rN);

private:
    /// @brief Default constructor
    CUpdateVisitor();

    /// @brief Class operation.
    FMode_T m_eMode;
    /// @brief Session to update.
    CSession *m_psTop;
};

/// @brief Apply a command line in a full session.
/// @ingroup hif_dir
class CApplyVisitor : public CNodeVisitor
{
public:
    /// @brief Constructor
    /// @param rsTop Target session
    /// @param pcLine command line to apply
    CApplyVisitor(CSession &rsTop, const char *pcLine);

    /// @brief Destructor.
    virtual ~CApplyVisitor();

    CApplyVisitor(const CApplyVisitor &)            = delete;
    CApplyVisitor &operator=(const CApplyVisitor &) = delete;

    /// @brief Return the error code 0 = Ok
    /// @return Error code.
    int GetErrorCode() { return m_nError; }

    /// @brief Visit the list of nodes.
    /// @param rL list of nodes.
    /// @return 0 if success.
    int visitList(std::list<CNode *> &rL);

    /// @brief Visit a node.
    /// @param rN node to visit.
    /// @return 0 if success.
    virtual int visitCNode(CNode &rN);

private:
    /// @brief Default constructor.
    CApplyVisitor();

    /// @brief Apply status error code.
    int m_nError;
    /// @brief Command line.
    char *m_pcLine;
};

/// @brief Create, move or remove directories according to a session.
/// @ingroup hif_dir
class CCheckDirVisitor : public CNodeVisitor
{
public:
    /// @brief Constructor.
    /// @param psTop Session to check.
    CCheckDirVisitor(CSession *psTop);

    /// @brief Destructor.
    virtual ~CCheckDirVisitor();

    CCheckDirVisitor(const CCheckDirVisitor &)            = delete;
    CCheckDirVisitor &operator=(const CCheckDirVisitor &) = delete;

    /// @brief Return the error code 0 = Ok.
    /// @return Error code.
    int GetErrorCode() { return m_nError; }

    /// @brief Visit the list of nodes.
    /// @param rL list of nodes.
    /// @return 0 if success.
    int visitList(std::list<CNode *> &rL);

    /// @brief Visit a node.
    /// @param rN node to visit.
    /// @return 0 if success.
    virtual int visitCNode(CNode &rN);

private:
    /// @brief Default constructor.
    CCheckDirVisitor();

    /// @brief Apply status error code.
    int m_nError;

    /// @brief Two visit must be performed :
    ///     The first removes all directories which node is tagged "REMOVED".
    ///     The second make all others operations create, move.
    bool m_bSecondPatch;

    /// @brief Session to treat
    CSession *m_psTop;
};

/// @brief Update the DateFile use a Properties class
/// @ingroup hif_dir
class CDateVisitor : public CNodeVisitor
{

public:
    /// @brief Constructor.
    /// @param rsTop Session to check
    /// @param sDates DateFile name
    CDateVisitor(CSession &rsTop, std::string &sDates);

    /// @brief Destructor.
    virtual ~CDateVisitor();

    CDateVisitor(const CDateVisitor &)            = delete;
    CDateVisitor &operator=(const CDateVisitor &) = delete;

    /// @brief Visit the list of nodes.
    /// @param rL list of nodes.
    /// @return 0 if success.
    int visitList(std::list<CNode *> &rL);

    /// @brief Visit a node.
    /// @param rN node to visit.
    /// @return 0 if success.
    virtual int visitCNode(CNode &rN);

    /// @brief Dump all files information into the DateFile sDates.
    /// @param sDates DateFile name.
    void DumpFile(std::string &sDates);

private:
    /// @brief Default constructor.
    CDateVisitor();

    /// @brief DateFile structure.
    Properties m_pDates;
};

#ifdef HIFDIR_DBG
class CPrintVisitor : public CNodeVisitor
{
private:
    CPrintVisitor(){};

public:
    CPrintVisitor(CSession &rsTop);
    virtual ~CPrintVisitor(){};

    int visitList(std::list<CNode *> &rL);
    virtual int visitCNode(CNode &rN);
};
#endif // HIFDIR_DBG

} // namespace backends
} // namespace hif
