/// @file Session.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <fstream>
#include <list>
#include <string>
#include <utility>
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

//
// CNode()
CNode::CNode()
    : m_eMode(NEW)
    , m_eInstTag(ONLYONE)
    , m_vPath()
    , m_sInstName()
    , m_sEntName()
    , m_pnMovedTo(nullptr)
    , m_pnFirstInst(nullptr)
    , m_pnParent(nullptr)
    , m_lpChild()
{
}

//
// ~CNode()
CNode::~CNode()
{
    std::list<CNode *>::iterator iElt;

    for (iElt = m_lpChild.begin(); iElt != m_lpChild.end(); iElt = m_lpChild.erase(iElt)) {
        delete *iElt;
    }
}

//
// SetParent(CNode * pnParent)
void CNode::SetParent(CNode *pnParent) { m_pnParent = pnParent; }

//
// SetPath(std::string& sPath)
void CNode::SetPath(std::string &sPath)
{
    if (!(sPath.empty())) {
        // FileStructure fPath(RxCfg.eval(sPath));
        messageInfo("MODIFIED   -> " + sPath);
        application_utils::FileStructure fPath(sPath);
        m_vPath = fPath.getAbstractName();
    }
}

//
// SetInstName(std::stringsName)
void CNode::SetInstName(const std::string &sName) { m_sInstName = sName; }

//
// SetEntName(std::stringsName)
void CNode::SetEntName(const std::string &sName)
{
    m_sEntName += (!m_sEntName.empty()) ? std::string("__") + sName : sName;
}

//
// SetInstTag(InstTag_T eTag)
void CNode::SetInstTag(InstTag_T eTag) { m_eInstTag = eTag; }

//
// SetMode(Mode_T eMode)
void CNode::SetMode(Mode_T eMode) { m_eMode = eMode; }

//
// SetAttributes(CNode & rN)
void CNode::SetAttributes(CNode &rN)
{
    m_sInstName   = rN.GetInstName();
    m_sEntName    = rN.GetEntName();
    m_eMode       = rN.GetMode();
    m_eInstTag    = rN.GetInstTag();
    m_pnMovedTo   = rN.MovedToParent();
    m_pnFirstInst = rN.GetOrgNode();
    m_vPath       = rN.GetPath();
}

//
// SetOrgPath(CNode * pnInst)
void CNode::SetOrgPath(CNode *pnInst) { m_pnFirstInst = pnInst; }

//
// SetMovedPath(CNode * rnParent)
void CNode::SetMovedPath(CNode *pnParent) { m_pnMovedTo = pnParent; }

//
// getName()
auto CNode::getName() -> std::string
{
    if (!m_sInstName.empty()) {
        return m_sInstName;
    }
    return m_sEntName;
}

//
// GetPath()
auto CNode::GetPath() -> std::vector<std::string>
{
    if (!(m_vPath.empty())) {
        return m_vPath;
    }
    std::vector<std::string> vsPath;
    vsPath.push_back(this->getName());
    CNode *pnCur = m_pnParent;
    while (pnCur != nullptr) {
        vsPath.insert(vsPath.begin(), pnCur->getName());
        pnCur = pnCur->GetParent();
    }
    return vsPath;
}

//
// operator ==(const CNode & rNr)
auto CNode::operator==(const CNode &rNr) -> bool
{
    if (this->m_sInstName != rNr.m_sInstName) {
        return false;
    }

    if (this->m_sEntName != rNr.m_sEntName) {
        return false;
    }

    return true;
}

//
// acceptVisitor(CNodeVisitor & rVis)
auto CNode::acceptVisitor(CNodeVisitor &rVis) -> int { return rVis.visitCNode(*this); }

//
// CSession constants definition
const char *const CSession::CSuffix[] = {"src", "inc", "doc", "lib", "exe", "obj", ""};

//
// CSession methods definition
//
// CSession()
CSession::CSession()
    : m_lpHead()
{
    // nothing to do
}

//
// ~CSession()
CSession::~CSession()
{
    std::list<CNode *>::iterator iElt;

    for (iElt = m_lpHead.begin(); iElt != m_lpHead.end(); iElt = m_lpHead.erase(iElt)) {
        delete *iElt;
    }
}

//
// FindAnInstance(CNode & rnElt)
auto CSession::FindAnInstance(CNode &rnElt) -> CNode *
{
    CFindVisitor fvPath(*this, rnElt, false);
    CNode *pnElt = nullptr;

    pnElt = fvPath.GetNode();
    if (pnElt != nullptr) {
        pnElt->SetInstTag(CNode::ORIGINAL);
    }
    return pnElt;
}

//
// FindTheInstance(CNode & rnElt)
auto CSession::FindTheInstance(CNode &rnElt) -> CNode *
{
    CFindVisitor fvPath(*this, rnElt, true);

    return fvPath.GetNode();
}

//
// PreCheck()
auto CSession::PreCheck() -> int
{
    CUpdateVisitor uvPreCheck(this, CUpdateVisitor::PRECHECK);

    return 0;
}

//
// Update()
auto CSession::Update() -> int
{
    CUpdateVisitor uvUpdate(this, CUpdateVisitor::UPDATE);

    return 0;
}

//
// Find(LibraryDef & rlTgt)
auto CSession::Find(LibraryDef &rlTgt) -> std::vector<std::string>
{
    CNode nElt;
    CNode *pnRes;
    std::string nIdent;
    std::vector<std::string> vsNull;

    nIdent = rlTgt.getName();
    nElt.SetEntName(nIdent);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (pnRes == nullptr) {
        return vsNull;
    }
    //
    return pnRes->GetPath();
}

//
// Find(DesignUnit & rduTgt)
auto CSession::Find(DesignUnit &rduTgt) -> std::vector<std::string>
{
    CNode nElt;
    CNode *pnRes;
    messageDebugAssert(rduTgt.views.size() == 1, "Unexpected view list size", nullptr, nullptr);
    View *pvTgt = rduTgt.views.front();
    std::string nIdent;
    std::vector<std::string> vsNull;

    nIdent = rduTgt.getName();
    nElt.SetEntName(nIdent);
    if (pvTgt == nullptr) {
        return vsNull;
    }
    nIdent = pvTgt->getName();
    nElt.SetEntName(nIdent);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (pnRes == nullptr) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && ((pnRes->GetOrgNode()) != nullptr)) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(View & rvTgt)
auto CSession::Find(View &rvTgt) -> std::vector<std::string>
{
    CNode nElt;
    CNode *pnRes;
    auto *pduTgt = hif::getNearestParent<DesignUnit>(&rvTgt);
    std::string nIdent;
    std::vector<std::string> vsNull;

    if (pduTgt == nullptr) {
        return vsNull;
    }
    nIdent = pduTgt->getName();
    nElt.SetEntName(nIdent);
    nIdent = rvTgt.getName();
    nElt.SetEntName(nIdent);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (pnRes == nullptr) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && ((pnRes->GetOrgNode()) != nullptr)) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(Instance & riTgt)
auto CSession::Find(Instance &riTgt) -> std::vector<std::string>
{
    CNode nElt;
    CNode *pnRes;
    auto *pvrTgt = dynamic_cast<ViewReference *>(riTgt.getReferencedType());
    messageAssert(pvrTgt != nullptr, "Unexpected referenced type", riTgt.getReferencedType(), nullptr);
    std::string nIdent;
    std::vector<std::string> vsNull;

    if (pvrTgt == nullptr) {
        return vsNull;
    }
    nIdent = pvrTgt->getDesignUnit();
    nElt.SetEntName(nIdent);
    nIdent = pvrTgt->getName();
    nElt.SetEntName(nIdent);
    //
    nIdent = riTgt.getName();
    nElt.SetInstName(nIdent);
    //
    CFindVisitor fvPath(*this, nElt, true);
    pnRes = fvPath.GetNode();
    if (pnRes == nullptr) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && ((pnRes->GetOrgNode()) != nullptr)) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(std::string& sBase, std::string & sView)
auto CSession::Find(std::string &sBase, std::string &sView) -> std::vector<std::string>
{
    CNode nElt;
    CNode *pnRes;
    std::vector<std::string> vsNull;

    nElt.SetEntName(sBase);
    nElt.SetEntName(sView);
    CFindVisitor fvPath(*this, nElt, false);
    pnRes = fvPath.GetNode();
    if (pnRes == nullptr) {
        return vsNull;
    }
    // If is a multi-instantiation node
    // get the original node
    if ((pnRes->GetInstTag() == CNode::ANOTHERONE) && ((pnRes->GetOrgNode()) != nullptr)) {
        pnRes = pnRes->GetOrgNode();
    }
    //
    return pnRes->GetPath();
}

//
// Find(std::vector<std::string> & vsTgt, std::vector<std::string> & vsSrc)
auto CSession::Find(std::vector<std::string> &vsTgt, std::vector<std::string> &vsSrc) -> std::vector<std::string>
{
    std::vector<std::string>::iterator iTgt;
    std::vector<std::string>::iterator iSrc;
    std::vector<std::string> vsRes;
    std::vector<std::string> vsNull;

    if (vsTgt.empty()) {
        return vsNull;
    }
    if (vsSrc.empty()) {
        return vsTgt;
    }
    iTgt = vsTgt.begin();
    iSrc = vsSrc.begin();
    while ((iTgt != vsTgt.end()) && (iSrc != vsSrc.end()) && (*iTgt == *iSrc)) {
        iTgt++;
        iSrc++;
    }
    while (iSrc != vsSrc.end()) {
        vsRes.emplace_back("..");
        iSrc++;
    }
    while (iTgt != vsTgt.end()) {
        vsRes.push_back(*iTgt);
        iTgt++;
    }
    if (vsRes.empty()) {
        vsRes.emplace_back(".");
    }
    return vsRes;
}

//
// Find related to DESIGNUNITS
//

//
// Find(DesignUnit & rduTgt, DesignUnit & rduSrc)
auto CSession::Find(DesignUnit &rduTgt, DesignUnit &rduSrc) -> std::vector<std::string>
{
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(DesignUnit & rduTgt, View & rvSrc)
auto CSession::Find(DesignUnit &rduTgt, View &rvSrc) -> std::vector<std::string>
{
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(DesignUnit & rduTgt, Instance & riSrc)
auto CSession::Find(DesignUnit &rduTgt, Instance &riSrc) -> std::vector<std::string>
{
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(DesignUnit & rduTgt, LibraryDef & rlSrc)
auto CSession::Find(DesignUnit &rduTgt, LibraryDef &rlSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(DesignUnit & rduTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(DesignUnit & rduTgt, std::string & sbSrc, std::string svSrc)
auto CSession::Find(DesignUnit &rduTgt, std::string &sbSrc, std::string &svSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(DesignUnit & rduTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rduTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to VIEWS
//

//
// Find(View & rvTgt, DesignUnit & rduSrc)
auto CSession::Find(View &rvTgt, DesignUnit &rduSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(View & rvTgt, View & rvSrc)
auto CSession::Find(View &rvTgt, View &rvSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(View & rvTgt, Instance & riSrc)
auto CSession::Find(View &rvTgt, Instance &riSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(View & rvTgt, LibraryDef & rlSrc)
auto CSession::Find(View &rvTgt, LibraryDef &rlSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(View & rvTgt, std::string & sbSrc, std::string svSrc)
auto CSession::Find(View &rvTgt, std::string &sbSrc, std::string &svSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(View & rvTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rvTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to INSTANCES
//

//
// Find(Instance & riTgt, DesignUnit & rduSrc)
auto CSession::Find(Instance &riTgt, DesignUnit &rduSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(Instance & riTgt, View & rvSrc)
auto CSession::Find(Instance &riTgt, View &rvSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(Instance & riTgt, Instance & riSrc)
auto CSession::Find(Instance &riTgt, Instance &riSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(Instance & riTgt, LibraryDef & rlSrc)
auto CSession::Find(Instance &riTgt, LibraryDef &rlSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(Instance & riTgt, std::string & sbSrc, std::string svSrc)
auto CSession::Find(Instance &riTgt, std::string &sbSrc, std::string &svSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(Instance & riTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(riTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to LIBRARYDEFS
//

//
// Find(LibraryDef & rlTgt, DesignUnit & rduSrc)
auto CSession::Find(LibraryDef &rlTgt, DesignUnit &rduSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(LibraryDef & rlTgt, View & rvSrc)
auto CSession::Find(LibraryDef &rlTgt, View &rvSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find(LibraryDef & rlTgt, Instance & riSrc)
auto CSession::Find(LibraryDef &rlTgt, Instance &riSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(LibraryDef & rlTgt, LibraryDef & rlSrc)
auto CSession::Find(LibraryDef &rlTgt, LibraryDef &rlSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(LibraryDef & rlTgt, std::string & sbSrc, std::string svSrc)
auto CSession::Find(LibraryDef &rlTgt, std::string &sbSrc, std::string &svSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(LibraryDef & rlTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(rlTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Find related to STRINGS (DUBASE)
//

// Find(std::string& sbTgt, std::string & svTgt, DesignUnit & rduSrc)
auto CSession::Find(std::string &sbTgt, std::string &svTgt, DesignUnit &rduSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, DesignUnit & rduSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(rduSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, View & rvSrc)
auto CSession::Find(std::string &sbTgt, std::string &svTgt, View &rvSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, View & rvSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(rvSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, Instance & riSrc)
auto CSession::Find(std::string &sbTgt, std::string &svTgt, Instance &riSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, Instance & riSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(riSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, LibraryDef & rlSrc)
auto CSession::Find(std::string &sbTgt, std::string &svTgt, LibraryDef &rlSrc) -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Find(std::string& sbTgt, std::string & svTgt, LibraryDef & rlSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(rlSrc);

    return Find(vsTgt, vsSrc);
}

// Find(std::string& sbTgt, std::string & svTgt, std::string & sbSrc, std::string svSrc)
auto CSession::Find(std::string &sbTgt, std::string &svTgt, std::string &sbSrc, std::string &svSrc)
    -> std::vector<std::string>
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD
         << "CSession Find(std::string& sbTgt, std::string & svTgt, std::string & sbSrc, std::string & svSrc)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsTgt = Find(sbTgt, svTgt);
    std::vector<std::string> vsSrc = Find(sbSrc, svSrc);

    return Find(vsTgt, vsSrc);
}

//
// Apply Methods
//

//
// Apply(const char * pcLine)
auto CSession::Apply(const char *pcLine) -> int
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(const char * pcLine)" << endl;
#endif // HIFDIR_DBG
    if (pcLine == nullptr) {
        return 4;
    }

    CApplyVisitor avSys(*this, pcLine);
    return avSys.GetErrorCode();
}

//
// Apply(DesignUnit & rduTgt, const char * pcLine, const char * pcSuffix)
auto CSession::Apply(DesignUnit &rduTgt, const char *pcLine, const char *pcSuffix) -> int
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(DesignUnit & rduTgt, const char * pcLine, const char * pcSuffix)"
         << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;
    BList<View>::iterator iElt;
    application_utils::FileStructure fPDir(".");
    application_utils::FileStructure fPADir;

    fPADir = fPDir.getAbsoluteFile();

    if (pcLine == nullptr) {
        return 4;
    }

    for (iElt = rduTgt.views.begin(); iElt != rduTgt.views.end(); iElt++) {
        vsPath = Find(**iElt);
        if (!vsPath.empty()) {
            if (pcSuffix != nullptr) {
                vsPath.emplace_back(pcSuffix);
            }
            application_utils::FileStructure fCDir(vsPath);
            if (!(fCDir.chdir())) {
                return 2;
            }
            if (system(pcLine) == -1) {
                return 5;
            }
            if (!(fPADir.chdir())) {
                return 2;
            }
        }
    }
    return 0;
}

//
// Apply(View & rvTgt, const char * pcLine, const char * pcSuffix)
auto CSession::Apply(View &rvTgt, const char *pcLine, const char *pcSuffix) -> int
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(View & rvTgt, const char * pcLine, const char * pcSuffix)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;

    if (pcLine == nullptr) {
        return 4;
    }

    vsPath = Find(rvTgt);
    if (pcSuffix != nullptr) {
        vsPath.emplace_back(pcSuffix);
    }
    application_utils::FileStructure fCDir(vsPath);
    if (!(fCDir.chdir())) {
        return 2;
    }
    if (system(pcLine) == -1) {
        return 5;
    }
    return 0;
}

//
// Apply(Instance & riTgt, const char * pcLine, const char * pcSuffix)
auto CSession::Apply(Instance &riTgt, const char *pcLine, const char *pcSuffix) -> int
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(Instance & riTgt, const char * pcLine, const char * pcSuffix)" << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;

    if (pcLine == nullptr) {
        return 4;
    }

    vsPath = Find(riTgt);
    if (pcSuffix != nullptr) {
        vsPath.emplace_back(pcSuffix);
    }
    application_utils::FileStructure fCDir(vsPath);
    if (!(fCDir.chdir())) {
        return 2;
    }
    if (system(pcLine) == -1) {
        return 5;
    }
    return 0;
}

//
// Apply(LibraryDef & rlTgt, const char * pcLine, const char * pcSuffix)
auto CSession::Apply(LibraryDef &rlTgt, const char *pcLine, const char *pcSuffix) -> int
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CSession Apply(LibraryDef & rlTgt, const char * pcLine, const char * pcSuffix)"
         << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;
    BList<View>::iterator iElt;
    application_utils::FileStructure fPDir(".");
    application_utils::FileStructure fPADir;

    fPADir = fPDir.getAbsoluteFile();

    if (pcLine == nullptr) {
        return 4;
    }

    vsPath = Find(rlTgt);
    if (!vsPath.empty()) {
        if (pcSuffix != nullptr) {
            vsPath.emplace_back(pcSuffix);
        }
        application_utils::FileStructure fCDir(vsPath);
        if (!(fCDir.chdir())) {
            return 2;
        }
        if (system(pcLine) == -1) {
            return 5;
        }
        if (!(fPADir.chdir())) {
            return 2;
        }
    }
    return 0;
}

//
// Apply(std::string& sbTgt, std::string & svTgt, const char * pcLine, const char * pcSuffix)
auto CSession::Apply(std::string &sbTgt, std::string &svTgt, const char *pcLine, const char *pcSuffix) -> int
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD
         << "CSession Apply(std::string& sbTgt, std::string & svTgt, const char * pcLine, const char * pcSuffix)"
         << endl;
#endif // HIFDIR_DBG
    std::vector<std::string> vsPath;
    BList<View>::iterator iElt;
    application_utils::FileStructure fPDir(".");
    application_utils::FileStructure fPADir;

    fPADir = fPDir.getAbsoluteFile();

    if (pcLine == nullptr) {
        return 4;
    }

    vsPath = Find(sbTgt, svTgt);
    if (!vsPath.empty()) {
        if (pcSuffix != nullptr) {
            vsPath.emplace_back(pcSuffix);
        }
        application_utils::FileStructure fCDir(vsPath);
        if (!(fCDir.chdir())) {
            return 2;
        }
        if (system(pcLine) == -1) {
            return 5;
        }
        if (!(fPADir.chdir())) {
            return 2;
        }
    }
    return 0;
}

//
// Debug Methods
//

#ifdef HIFDIR_DBG
char *PutnChar(int nI)
{
    char *pStr = new char[nI + 1];
    for (int i = 0; i < nI; i++) {
        pStr[i] = ' ';
    }
    pStr[nI] = '\0';
    return pStr;
}

const char *PrintSVector(std::vector<std::string> vsVec)
{
    std::vector<std::string>::iterator iElt;
    std::string sRes;

    for (iElt = vsVec.begin(); iElt != vsVec.end(); iElt++)
        sRes += *iElt + '#';
    return sRes.c_str();
}
#endif // HIFDIR_DBG

} // namespace backends
} // namespace hif
