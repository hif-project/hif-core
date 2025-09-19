/// @file NodeVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "hif/backends/NodeVisitor.hpp"
#include "hif/backends/Session.hpp"
#include "hif/hif.hpp"

namespace hif
{
namespace backends
{
//
//  Class definition
//
int CNodeVisitor::visitCNode(CNode & /*rN*/) { return m_nR; }

CNodeVisitor::CNodeVisitor(int nR)
    : m_nR(nR)
{
    // ntd
}

CNodeVisitor::~CNodeVisitor() {}
//
// VisitList
int CNodeVisitor::visitList(std::list<CNode *> &rL)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CNodeVisitor VisitList" << endl;
#endif // HIFDIR_DBG
    std::list<CNode *>::iterator iElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        (*iElt)->acceptVisitor(*this);
    }

    return m_nR;
}
CFindVisitor::CFindVisitor()
    : CNodeVisitor()
    , m_nElt()
    , m_bInst()
    , m_pnRes()
{
}

CFindVisitor::~CFindVisitor() {}

// CFindVisitor
//
// CFindVisitor(CSession & rsTop, CNode & rnElt, bool bInst)
CFindVisitor::CFindVisitor(CSession &rsTop, CNode &rnElt, bool bInst)
    : CNodeVisitor()
    , m_nElt()
    , m_bInst()
    , m_pnRes()
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CFindVisitor CFindVisitor(CSession & rsTop, CNode & rnElt, bool bInst)" << endl;
#endif // HIFDIR_DBG
    m_nElt.SetAttributes(rnElt);
    m_bInst = bInst;
    m_pnRes = nullptr;
    if (!rsTop.m_lpHead.empty()) {
        visitList(rsTop.m_lpHead);
    }
}

//
// VisitList(list<CNode * > & rL)
int CFindVisitor::visitList(std::list<CNode *> &rL)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CFindVisitor VisitList" << endl;
#endif // HIFDIR_DBG
    int nRet = 1;
    std::list<CNode *>::iterator iElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        nRet = (*iElt)->acceptVisitor(*this);
        if (!nRet) {
            return nRet;
        }
    }
    return 1;
}

//
// VisitCNode(CNode & rN)
int CFindVisitor::visitCNode(CNode &rN)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CFindVisitor VisitCNode" << endl;
    INC_INDENT;
#endif // HIFDIR_DBG
    int nRet = 1;

    if (m_bInst) {
        if (m_nElt == rN) {
            m_pnRes = &rN;
#ifdef HIFDIR_DBG
            cout << INDENT << "Found exactly the instance" << endl;
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 0;
        }
    } else {
        if ((m_nElt.GetEntName() == rN.GetEntName()) &&
            ((m_nElt.GetInstName() != rN.GetInstName()) || (rN.GetInstName().size() == 0))) {
            m_pnRes = &rN;
#ifdef HIFDIR_DBG
            cout << INDENT << "Found an instance" << endl;
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 0;
        }
    }

    if (!(rN.m_lpChild).empty()) {
        nRet = visitList(rN.m_lpChild);
    }
#ifdef HIFDIR_DBG
    DEC_INDENT;
#endif // HIFDIR_DBG
    return nRet;
}
CUpdateVisitor::CUpdateVisitor()
    : CNodeVisitor()
    , m_eMode()
    , m_psTop()
{
}

// CUpdateVisitor
//
// CUpdateVisitor(CSession * rsTop, FMode_T eMode)
CUpdateVisitor::CUpdateVisitor(CSession *psTop, FMode_T eMode)
    : CNodeVisitor()
    , m_eMode()
    , m_psTop()
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CUpdateVisitor CUpdateVisitor : ";
    if (eMode == PRECHECK)
        cout << "PRECHECK";
    if (eMode == UPDATE)
        cout << "UPDATE";
    cout << endl;
#endif // HIFDIR_DBG
    m_psTop = psTop;
    m_eMode = eMode;
    if (!psTop->m_lpHead.empty()) {
        visitList(psTop->m_lpHead);
    }
    //
    //
    if (m_eMode == UPDATE) {
#ifdef HIFDIR_DBG
        cout << INDENT << "CUpdateVisitor constructor mode : REMOVED" << endl;
#endif // HIFDIR_DBG
        m_eMode = REMOVE;
        if (!psTop->m_lpHead.empty()) {
            visitList(psTop->m_lpHead);
        }
    }
}

CUpdateVisitor::~CUpdateVisitor()
{
    // ntd
}

//
// VisitList(list<CNode * > & rL)
int CUpdateVisitor::visitList(std::list<CNode *> &rL)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CUpdateVisitor VisitList" << endl;
#endif // HIFDIR_DBG
    int nRet = 1;
    std::list<CNode *>::iterator iElt;
    CNode *pnElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        nRet = (*iElt)->acceptVisitor(*this);
        if (!nRet) {
            return nRet;
        }
        if (nRet >= 2) {
            pnElt = *iElt;
            iElt  = rL.erase(iElt);
            iElt--;
            if (nRet == 3)
                delete pnElt;
        }
    }
    return 1;
}

//
// VisitCNode(CNode & rN)
int CUpdateVisitor::visitCNode(CNode &rN)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CUpdateVisitor VisitCNode" << endl;
    INC_INDENT;
#endif // HIFDIR_DBG
    int nRet = 1;
    CNode *pnTgt;
    int nRetSpc = 1;

    switch (m_eMode) {
    case PRECHECK:
        rN.SetMode(CNode::REMOVED);
        break;
    case UPDATE:
        if (rN.GetMode() == CNode::MOVED) {
            pnTgt = rN.MovedToParent();
            if (pnTgt) {
                (pnTgt->m_lpChild).push_back(&rN);
                rN.SetParent(pnTgt);
            }
            rN.SetMode(CNode::UNCHANGED);
            nRetSpc = 2;
        } else if (rN.GetMode() == CNode::NEW) {
            rN.SetMode(CNode::UNCHANGED);
        }
        break;
    case REMOVE:
        if (rN.GetMode() == CNode::REMOVED) {
#ifdef HIFDIR_DBG
            cout << INDENT << "Element removed" << endl;
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 3;
        }
        break;
    default:
        break;
    }

    if (!(rN.m_lpChild).empty()) {
        nRet = visitList(rN.m_lpChild);
    }
#ifdef HIFDIR_DBG
    DEC_INDENT;
#endif // HIFDIR_DBG
    if (nRetSpc == 2)
        nRet = 2;
    return nRet;
}
CApplyVisitor::CApplyVisitor()
    : CNodeVisitor()
    , m_nError()
    , m_pcLine()
{
}

// CApplyVisitor
//
// CApplyVisitor(CSession & rsTop, const char * pcLine)
CApplyVisitor::CApplyVisitor(CSession &rsTop, const char *pcLine)
    : CNodeVisitor()
    , m_nError()
    , m_pcLine()
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CApplyVisitor CApplyVisitor(CSession & rsTop, const char * pcLine)" << endl;
#endif // HIFDIR_DBG
    m_pcLine = new char[strlen(pcLine) + 1];
    strcpy(m_pcLine, pcLine);
    m_nError = 0;
    if (!rsTop.m_lpHead.empty()) {
        m_nError = visitList(rsTop.m_lpHead);
    }
}

CApplyVisitor::~CApplyVisitor()
{
    // ntd
}

//
// VisitList(list<CNode * > & rL)
int CApplyVisitor::visitList(std::list<CNode *> &rL)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CApplyVisitor VisitList" << std::endl;
#endif // HIFDIR_DBG
    int nRet = 0;
    std::list<CNode *>::iterator iElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        nRet = (*iElt)->acceptVisitor(*this);
        if (nRet) {
            return nRet;
        }
    }
    return nRet;
}

//
// VisitCNode(CNode & rN)
int CApplyVisitor::visitCNode(CNode &rN)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CApplyVisitor VisitCNode" << std::endl;
    INC_INDENT;
#endif // HIFDIR_DBG
    int nRet   = 0;
    int nEltNb = static_cast<int>(rN.GetPath().size());
    application_utils::FileStructure fCDir(rN.GetPath());
    application_utils::FileStructure fPDir("..");

    // Don't move if it isn't an original node
    if (rN.GetInstTag() != CNode::ANOTHERONE) {
        if (!(fCDir.chdir())) {
#ifdef HIFDIR_DBG
            cout << INDENT << "Cannot change directory" << endl;
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 2;
        }
        if (system(m_pcLine) == -1) {
#ifdef HIFDIR_DBG
            cout << INDENT << "Cannot apply the command line" << endl;
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 3;
        }
        if (!(rN.m_lpChild).empty()) {
            nRet = visitList(rN.m_lpChild);
        }
        for (int nNb = 0; nNb < nEltNb; nNb++) {
            fPDir.chdir();
        }
    }
#ifdef HIFDIR_DBG
    DEC_INDENT;
#endif // HIFDIR_DBG
    return nRet;
}
CCheckDirVisitor::CCheckDirVisitor()
    : CNodeVisitor()
    , m_nError()
    , m_bSecondPatch()
    , m_psTop()
{
}

// CCheckDirVisitor
//
// CCheckDirVisitor(CSession * psTop)
CCheckDirVisitor::CCheckDirVisitor(CSession *psTop)
    : CNodeVisitor()
    , m_nError()
    , m_bSecondPatch()
    , m_psTop()
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CCheckDirVisitor CCheckDirVisitor(CSession * psTop) First STAGE" << std::endl;
#endif // HIFDIR_DBG
    m_psTop        = psTop;
    m_nError       = 0;
    m_bSecondPatch = false;
    if (!psTop->m_lpHead.empty()) {
        m_nError = visitList(psTop->m_lpHead);
    }
    if (!m_nError) {
        m_bSecondPatch = true;
#ifdef HIFDIR_DBG
        std::cout << INDENT << METHOD << "CCheckDirVisitor CCheckDirVisitor(CSession * psTop) Second STAGE"
                  << std::endl;
#endif // HIFDIR_DBG
        if (!psTop->m_lpHead.empty()) {
            m_nError = visitList(psTop->m_lpHead);
        }
    }
}

CCheckDirVisitor::~CCheckDirVisitor()
{
    // ntd
}

//
// VisitList(list<CNode * > & rL)
int CCheckDirVisitor::visitList(std::list<CNode *> &rL)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CCheckDirVisitor VisitList" << std::endl;
#endif // HIFDIR_DBG
    int nRet = 0;
    std::list<CNode *>::iterator iElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        nRet = (*iElt)->acceptVisitor(*this);
        if (nRet > 0) {
            return nRet;
        }
    }
    return nRet;
}

//
// VisitCNode(CNode & rN)
int CCheckDirVisitor::visitCNode(CNode &rN)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CCheckDirVisitor VisitCNode : " << std::endl;
    INC_INDENT;
#endif // HIFDIR_DBG
    int nRet = 0;
    std::vector<std::string> vsPSrc, vsPTgt, vsPRel;
    application_utils::FileStructure fCDir(rN.GetPath());
    application_utils::FileStructure fPDir(".."), fCurr(".");
    application_utils::FileStructure *pfPMoveTo; //, * pfPInstLnk;

    int nEltNb = static_cast<int>(rN.GetPath().size());
    for (int nNb = 1; nNb < nEltNb; nNb++) {
        fPDir.addChild("..");
    }

    if (m_bSecondPatch && (rN.GetMode() == CNode::REMOVED)) {
        if (!(fCDir.rmdir())) {
            nRet = -4;
        }
        return nRet;
    } else if (!m_bSecondPatch) {
        switch (rN.GetMode()) {
        case CNode::UNCHANGED:
            if (!(fCDir.exists())) {
                nRet = -1;
            }
            break;
        case CNode::NEW:
            if (fCDir.exists()) {
                nRet = -2;
            } else {
                switch (rN.GetInstTag()) {
                case CNode::ONLYONE:
                case CNode::ORIGINAL:
                    if (!(fCDir.make_dirs())) {
                        nRet = 1;
                    }
                    // Create CSession::Suffix sub-directories
                    for (int nSuff = CSession::SRC; nSuff <= CSession::DOC; nSuff++) {
                        application_utils::FileStructure fSDir(fCDir, std::string(CSession::CSuffix[nSuff]));
                        if (!(fSDir.make_dirs())) {
                            nRet = 1;
                        }
                    }
                    break;
                case CNode::ANOTHERONE:
                    // No action
                    break;
                default:
                    break;
                }
            }
            break;
        case CNode::MOVED:
            vsPTgt = (rN.MovedToParent())->GetPath();
            if (rN.GetParent()) {
                vsPSrc = (rN.GetParent())->GetPath();
            } else {
                vsPSrc.push_back(std::string("."));
            }
            vsPRel    = m_psTop->Find(vsPTgt, vsPSrc);
            pfPMoveTo = new application_utils::FileStructure(vsPRel);
            pfPMoveTo->addChild(rN.getName());
            if (!(fCDir.renameTo(*pfPMoveTo))) {
                nRet = -3;
            }
            //
            fPDir = fCurr.getAbsoluteFile();
            fCDir.renameFile(*pfPMoveTo);
            delete pfPMoveTo;
            break;

        case CNode::REMOVED:
        default:
            break;
        }
    }

    if (nRet > 0) {
#ifdef HIFDIR_DBG
        DEC_INDENT;
#endif // HIFDIR_DBG
        return nRet;
    }
    if (!(rN.m_lpChild).empty()) {
        if (!(fCDir.chdir())) {
#ifdef HIFDIR_DBG
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 2;
        }
        nRet = visitList(rN.m_lpChild);
        if (!(fPDir.chdir())) {
#ifdef HIFDIR_DBG
            DEC_INDENT;
#endif // HIFDIR_DBG
            return 2;
        }
    }
#ifdef HIFDIR_DBG
    DEC_INDENT;
#endif // HIFDIR_DBG
    return nRet;
}
CDateVisitor::CDateVisitor()
    : m_pDates()
{
}

// CDateVisitor

//
// CDateVisitor(...)
CDateVisitor::CDateVisitor(CSession &rsTop, std::string &sDates)
    : m_pDates()
{
    m_pDates.load(sDates, 1);
    if (!rsTop.m_lpHead.empty()) {
        visitList(rsTop.m_lpHead);
    }
}

/// @brief Destructor
CDateVisitor::~CDateVisitor() {}

//
// VisitList(list<CNode * > & rL)
int CDateVisitor::visitList(std::list<CNode *> &rL)
{
    int nRet = 1;
    std::list<CNode *>::iterator iElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        nRet = (*iElt)->acceptVisitor(*this);
        if (!nRet) {
            return nRet;
        }
    }
    return 1;
}

//
// VisitCNode(CNode & rN)
/// @brief Maximum buffer size for CNode visitor.
#define BUFF_MAX 32
int CDateVisitor::visitCNode(CNode &rN)
{
    int nRet = 1;
    char cBuff[BUFF_MAX];
    std::string sBuff;
    std::list<std::string> lSuff;
    lSuff.push_back(CSession::CSuffix[CSession::SRC]);
    lSuff.push_back(CSession::CSuffix[CSession::INC]);

    std::list<std::string>::iterator iSuff;
    for (iSuff = lSuff.begin(); iSuff != lSuff.end(); iSuff++) {
        application_utils::FileStructure fNode(rN.GetPath());
        fNode.addChild(*iSuff);
        std::vector<application_utils::FileStructure> vfList = fNode.listFiles();
        std::vector<application_utils::FileStructure>::iterator ifList;

        for (ifList = vfList.begin(); ifList != vfList.end(); ifList++) {
            if ((*ifList).isFile()) {
                long nSaveDate = atol(m_pDates[(*ifList).toString()].c_str());
                if ((m_pDates[(*ifList).toString()].empty()) || (nSaveDate == 0)) {
                    sprintf(cBuff, "%lu", static_cast<unsigned long>((*ifList).lastModified()));
                    sBuff = std::string(cBuff);
                    m_pDates.setProperty((*ifList).toString(), sBuff);
                } else if (nSaveDate != 1) {
                    // Else is a User File : keep it
                    if (static_cast<unsigned long>((*ifList).lastModified()) == static_cast<unsigned long>(nSaveDate)) {
                        sprintf(cBuff, "%lu", static_cast<unsigned long>((*ifList).lastModified()));
                    } else {
                        sprintf(cBuff, "1");
                    }
                    sBuff = std::string(cBuff);
                    m_pDates.setProperty((*ifList).toString(), sBuff);
                }
            }
        }
    }

    if (!(rN.m_lpChild).empty()) {
        nRet = visitList(rN.m_lpChild);
    }

    return nRet;
}

//
// DumpFile
void CDateVisitor::DumpFile(std::string &sDates)
{
    // Open DateFiles
    std::ofstream *fDates;
    fDates = new std::ofstream(sDates.c_str());
    if (!(fDates->is_open())) {
        delete fDates;
        return;
    }
    m_pDates.dump(*fDates);
    delete fDates;
}
#ifdef HIFDIR_DBG
// CPrintVisitor
//
// CPrintVisitor(CSession & rsTop)
CPrintVisitor::CPrintVisitor(CSession &rsTop)
{
    std::cout << "<#> TREE STRUCTURE :" << std::endl;
    if (rsTop.m_lpHead.size() != 0) {
        VisitList(rsTop.m_lpHead);
    }
}

//
// VisitList(list<CNode * > & rL)
int CPrintVisitor::visitList(list<CNode *> &rL)
{
    int nRet = 1;
    std::list<CNode *>::iterator iElt;

    for (iElt = rL.begin(); iElt != rL.end(); iElt++) {
        nRet = (*iElt)->acceptVisitor(*this);
        if (!nRet) {
            return nRet;
        }
    }
    return 1;
}

//
// VisitCNode(CNode & rN)
int CPrintVisitor::visitCNode(CNode &rN)
{
    int nRet = 1;

    std::cout << "<#>" << INDENT << "BEGIN NODE" << std::endl;
    INC_INDENT;
    std::cout << "<#>" << INDENT << "- InstName : " << (rN.GetInstName()).c_str() << std::endl;
    std::cout << "<#>" << INDENT << "- EntName  : " << (rN.GetEntName()).c_str() << std::endl;
    std::cout << "<#>" << INDENT << "- Mode     : " << rN.PrintMode() << std::endl;
    std::cout << "<#>" << INDENT << "- InstTag  : " << rN.PrintInstTag() << std::endl;
    DEC_INDENT;
    std::cout << "<#>" << INDENT << "END NODE" << std::endl;
    INC_INDENT;

    if ((rN.m_lpChild).size() != 0) {
        nRet = VisitList(rN.m_lpChild);
    }

    DEC_INDENT;
    return nRet;
}
#endif // HIFDIR_DBG

} // namespace backends
} // namespace hif
