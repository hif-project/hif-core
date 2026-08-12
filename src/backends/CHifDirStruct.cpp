/// @file CHifDirStruct.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "hif/backends/CHifDirStruct.hpp"
#include "hif/backends/NodeVisitor.hpp"
#include "hif/backends/Properties.hpp"
#include "hif/backends/Session.hpp"

namespace hif
{
namespace backends
{

//
//  Class definition
//

//
// Constants definition
const char *const CHifDirStruct::HifOut        = "_ENV"; // Default HifOut suffix
//#ifdef _MSC_VER
//const char * const CHifDirStruct::HifTmp = "C:\\workspace\\"; // HIF_TMP
//#else
//const char * const CHifDirStruct::HifTmp = "/tmp"; // HIF_TMP
//#endif
// To avoid permission problems, it is better to always create the tmp dir as a subdir:
const char *const CHifDirStruct::HifTmp        = ".hif_tmp";
const char *const CHifDirStruct::DefaultHifOut = "hif2hdl_out";
const char *const CHifDirStruct::DefaultHifTmp = ".hif_tmp";
const char *const CHifDirStruct::CfgRoot       = "SESSION_PATH";
const char *const CHifDirStruct::DateFileParam = "1"; // "DATEFILE_DISABLED";
const char *const CHifDirStruct::IPNameProp    = "IPNAME";
const char *const CHifDirStruct::DateFiles     = ".DateFiles";

//
// ReturnSuffix
std::string CHifDirStruct::ReturnSuffix(Suffix eSuff)
{
    std::string sRet(CSession::CSuffix[eSuff]);
    return sRet;
}

//
// CHifDirStruct()
CHifDirStruct::CHifDirStruct()
    : m_eStatus()
    , m_psSystem(nullptr)
    , m_pfRoot(nullptr)
{
}

//
// CHifDirStruct(System & rsoTop, char * pcRoot = nullptr)
//CHifDirStruct::CHifDirStruct(System & rsoTop, const char * pcRoot)
//{
//
//}
CHifDirStruct::CHifDirStruct(System &rsoTop, const char *pcRoot)
    : m_eStatus()
    , m_psSystem(nullptr)
    , m_pfRoot(nullptr)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct CHifDirStruct(System & rsoTop, char * pcRoot = nullptr)"
              << std::endl;
#endif // HIFDIR_DBG
    std::string sRoot;

    m_eStatus = DST_OK;
    if (pcRoot) {
        m_pfRoot = new hif::application_utils::FileStructure(pcRoot);
    } else {
        sRoot = ""; //RxCfg[CfgRoot];
        if (sRoot.size() != 0) {
            m_pfRoot = new hif::application_utils::FileStructure(sRoot);
        } else {
            std::string sHifOut = std::string(HifOut);
            if (rsoTop.checkProperty(std::string(IPNameProp))) {
                TypedObject *o = rsoTop.getProperty(std::string(IPNameProp));
                messageDebugAssert(o != nullptr, "Unexpected property", nullptr, nullptr);

                if (dynamic_cast<Identifier *>(o)) {
                    Identifier *t = static_cast<Identifier *>(o);
                    sHifOut       = t->getName() + sHifOut;
                } else if (dynamic_cast<StringValue *>(o)) {
                    StringValue *t = static_cast<StringValue *>(o);
                    sHifOut        = t->getValue() + sHifOut;
                } else if (dynamic_cast<IntValue *>(o)) {
                    IntValue *t = static_cast<IntValue *>(o);
                    std::ostringstream oss;
                    oss << t->getValue();
                    sHifOut = oss.str() + sHifOut;
                }
            } else {
                sHifOut = DefaultHifOut;
            }
            m_pfRoot = new hif::application_utils::FileStructure(sHifOut);
        }
    }
    //
    // Check PlugIP PLUGIP_TMP variable
    //if (RxCfg[HifTmp].empty()) {
    //  RxCfg.setProperty(HifTmp,DefaultHifTmp);
    //}
    //
    m_psSystem = new CSession();
#ifdef HIFDIR_DBG
    //CPrintVisitor cpPrint(*m_psSystem);
#endif // HIFDIR_DBG
}

//
// ~CHifDirStruct()
CHifDirStruct::~CHifDirStruct()
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct destructor" << endl;
#endif // HIFDIR_DBG
    if (m_pfRoot) {
        m_pfRoot->chdir();
        hif::application_utils::FileStructure fTmpOut(HifTmp);
        fTmpOut.remove();
        delete m_pfRoot;
    }
    if (m_psSystem)
        delete m_psSystem;
}

CHifDirStruct::DirectoryStatus CHifDirStruct::Check()
{
    hif::application_utils::FileStructure fTmpOut(HifTmp);
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    // Check if the root directory exists
    if (m_pfRoot->exists()) {
        if (!(m_pfRoot->isDirectory())) {
            m_eStatus = DST_NOT_A_DIR;
            return m_eStatus;
        }
    } else {
        // Create the directory
        if (!(m_pfRoot->make_dirs())) {
            m_eStatus = DST_CNT_CREATE;
            return m_eStatus;
        }
    }

    m_pfRoot->chdir();
    // Check if the GetHifTmp exists
    if (!fTmpOut.exists()) {
        // Create the directory
        if (!(fTmpOut.make_dirs())) {
            m_eStatus = DST_CNT_CREATE;
            return m_eStatus;
        }
    } else if (!(fTmpOut.isDirectory())) {
        m_eStatus = DST_NOT_A_DIR;
        return m_eStatus;
    }

#ifdef HIFDIR_DBG
    //CPrintVisitor cpPrint(*m_psSystem);
#endif // HIFDIR_DBG
    m_psSystem->Update();
#ifdef HIFDIR_DBG
    //CPrintVisitor cpPrintn(*m_psSystem);
#endif // HIFDIR_DBG

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    //m_eStatus = (DirectoryStatus )(cvVisit.GetErrorCode());
    return m_eStatus;
}

//
// Check(System & rsoTop)
CHifDirStruct::DirectoryStatus CHifDirStruct::Check(System &)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct Check(System & rsoTop)" << endl;
#endif // HIFDIR_DBG
    m_psSystem->PreCheck();
#ifdef HIFDIR_DBG
    //CPrintVisitor cpPrintn(*m_psSystem);
#endif // HIFDIR_DBG

#ifdef HIFDIR_DBG
    //CPrintVisitor cpPrint(*m_psSystem);
#endif // HIFDIR_DBG
    return Check();
}

//
// UpdateDates()
CHifDirStruct::DirectoryStatus CHifDirStruct::UpdateDates()
{
    m_eStatus = DST_OK;

    int nIsDateFileDisabled = atoi(DateFileParam); // atoi(RxCfg[DateFileParam].c_str());
    if (nIsDateFileDisabled)
        return m_eStatus;

    //FileStructure fRx6Out(HifOut);
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    hif::application_utils::FileStructure fFile(*GetHifTmp(), DateFiles);
    std::string sFile = fFile.getAbsolutePath();
    fPADir            = fPDir.getAbsoluteFile();

    // Check if the root directory exists
    if (m_pfRoot->exists()) {
        if (!(m_pfRoot->isDirectory())) {
            m_eStatus = DST_NOT_A_DIR;
            return m_eStatus;
        }
    } else {
        // Create the directory
        if (!(m_pfRoot->make_dirs())) {
            m_eStatus = DST_CNT_CREATE;
            return m_eStatus;
        }
    }
    m_pfRoot->chdir();

    CDateVisitor cdDate(*m_psSystem, sFile);

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    cdDate.DumpFile(sFile);

    return m_eStatus;
}

//
// GetHifOut()
hif::application_utils::FileStructure *CHifDirStruct::GetHifOut()
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct GetHifOut" << m_pfRoot->toString() << endl;
#endif // HIFDIR_DBG

    m_eStatus = DST_OK;

    // Check if the root directory exists
    if (m_pfRoot->exists()) {
        if (!(m_pfRoot->isDirectory())) {
            m_eStatus = DST_NOT_A_DIR;
            return nullptr;
        }
    } else {
        // Create the directory
        if (!(m_pfRoot->make_dirs())) {
            m_eStatus = DST_CNT_CREATE;
            return nullptr;
        }
    }
    return m_pfRoot;
}

//
// GetHifTmp()
hif::application_utils::FileStructure *CHifDirStruct::GetHifTmp()
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct GetHifTmp" << endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *pfRx6Tmp;

    m_eStatus = DST_OK;

    // Check if the root directory exists
    if (m_pfRoot->exists()) {
        if (!(m_pfRoot->isDirectory())) {
            m_eStatus = DST_NOT_A_DIR;
            return nullptr;
        }
    } else {
        // Create the directory
        if (!(m_pfRoot->make_dirs())) {
            m_eStatus = DST_CNT_CREATE;
            return nullptr;
        }
    }
    pfRx6Tmp = new hif::application_utils::FileStructure(*m_pfRoot, HifTmp); //RxCfg[HifTmp]);
    if (!(pfRx6Tmp->exists())) {
        // Create the directory
        if (!(pfRx6Tmp->make_dirs())) {
            m_eStatus = DST_CNT_CREATE;
            delete pfRx6Tmp;
            return nullptr;
        }
    } else if (!(pfRx6Tmp->isDirectory())) {
        m_eStatus = DST_NOT_A_DIR;
        delete pfRx6Tmp;
        return nullptr;
    }
    return pfRx6Tmp;
}

//
// GetDir(LibraryDef & rlTgt)
hif::application_utils::FileStructure *CHifDirStruct::GetDir(Object & /*rlTgt*/, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct GetDir(Object & rduTgt)" << endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *pfRes;
    std::vector<std::string> vsRes;

    m_eStatus = DST_OK;

    //vsRes = m_psSystem->Find(rlTgt);
    //if (vsRes.empty()) {
    //return nullptr;
    //}
    vsRes.push_back(CSession::CSuffix[rSuff]);
    pfRes = new hif::application_utils::FileStructure(vsRes);
    hif::application_utils::FileStructure fTest(*GetHifOut(), pfRes->toString());
    if (!(fTest.exists())) {
        fTest.make_dirs();
    }
    //
    return pfRes;
}

// GetDir(FileStructure & fTgt, FileStructure & fSrc)
hif::application_utils::FileStructure *
CHifDirStruct::GetDir(hif::application_utils::FileStructure &fTgt, hif::application_utils::FileStructure &fSrc)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct GetDir(FileStructure & fTgt, FileStructure & fSrc)" << endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *pfRes;
    std::vector<std::string> vsRes, vsTgt, vsSrc;
    vsTgt = fTgt.getAbstractName();
    vsSrc = fSrc.getAbstractName();
    vsRes = m_psSystem->Find(vsTgt, vsSrc);
    pfRes = new hif::application_utils::FileStructure(vsRes);

    return pfRes;
}

//
// Apply Methods
//

//
// Apply(const char * pcLine)
CHifDirStruct::DirectoryStatus CHifDirStruct::Apply(const char *pcLine)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct Apply(const char * pcLine)" << endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fTmpOut;
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    fTmpOut = GetHifTmp();
    if (!(fTmpOut->chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }
    if (system(pcLine) == -1) {
        m_eStatus = DST_CNT_APPLY_SYS;
        return m_eStatus;
    }

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    return m_eStatus;
}

//
// Apply(DesignUnit & rduTgt, const char * pcLine, Suffix rSuff = SRC)
CHifDirStruct::DirectoryStatus CHifDirStruct::Apply(DesignUnit &rduTgt, const char *pcLine, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct Apply(DesignUnit & rduTgt, const char * pcLine, Suffix rSuff = SRC)"
         << endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fRx6Out;
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    fRx6Out = GetHifOut();
    if (!(fRx6Out->chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    m_eStatus = static_cast<DirectoryStatus>(m_psSystem->Apply(rduTgt, pcLine, CSession::CSuffix[rSuff]));

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    return m_eStatus;
}

//
// Apply(View & rduTgt, const char * pcLine, Suffix rSuff = SRC)
CHifDirStruct::DirectoryStatus CHifDirStruct::Apply(View &rvTgt, const char *pcLine, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct Apply(View & rvTgt, const char * pcLine, Suffix rSuff = SRC)"
              << std::endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fRx6Out;
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    fRx6Out = GetHifOut();
    if (!(fRx6Out->chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    m_eStatus = static_cast<DirectoryStatus>(m_psSystem->Apply(rvTgt, pcLine, CSession::CSuffix[rSuff]));

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    return m_eStatus;
}

//
// Apply(Instance & rduTgt, const char * pcLine, Suffix rSuff = SRC)
CHifDirStruct::DirectoryStatus CHifDirStruct::Apply(Instance &riTgt, const char *pcLine, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct Apply(Instance & riTgt, const char * pcLine, Suffix rSuff = SRC)"
              << std::endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fRx6Out;
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    fRx6Out = GetHifOut();
    if (!(fRx6Out->chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    m_eStatus = static_cast<DirectoryStatus>(m_psSystem->Apply(riTgt, pcLine, CSession::CSuffix[rSuff]));

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    return m_eStatus;
}

//
// Apply(LibraryDef & rlTgt, const char * pcLine, Suffix rSuff = SRC)
CHifDirStruct::DirectoryStatus CHifDirStruct::Apply(LibraryDef &rlTgt, const char *pcLine, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct Apply(LibraryDef & rlTgt, const char * pcLine, Suffix rSuff = SRC)"
              << std::endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fRx6Out;
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    fRx6Out = GetHifOut();
    if (!(fRx6Out->chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    m_eStatus = static_cast<DirectoryStatus>(m_psSystem->Apply(rlTgt, pcLine, CSession::CSuffix[rSuff]));

    if (!(fPADir.chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    return m_eStatus;
}

//
// ApplyBase(DesignUnit & rduTgt, const char * pcLine, Suffix rSuff = SRC)
CHifDirStruct::DirectoryStatus CHifDirStruct::ApplyBase(DesignUnit &rduTgt, const char *, Suffix)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD
              << "CHifDirStruct ApplyBase(DesignUnit & rduTgt, const char * pcLine, Suffix rSuff = SRC)" << std::endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fRx6Out;
    hif::application_utils::FileStructure fPDir(".");
    hif::application_utils::FileStructure fPADir;

    m_eStatus = DST_OK;

    fPADir = fPDir.getAbsoluteFile();

    fRx6Out = GetHifOut();
    if (!(fRx6Out->chdir())) {
        m_eStatus = DST_CNT_CHANGE;
        return m_eStatus;
    }

    messageDebugAssert(rduTgt.views.size() == 1, "Unexpected view list size", nullptr, nullptr);
    View *pvTgt = rduTgt.views.front();
    if (!pvTgt) {
        return DST_CNT_APPLY_SYS;
    }
    std::string svTgt(pvTgt->getName());
    std::string sbTgt;

    return DST_CNT_APPLY_SYS;
}

//
// Date Methods
//

//
// IsModified(FileStructure & rF, const char * cFile)
bool CHifDirStruct::IsModified(hif::application_utils::FileStructure &rF, const char *cFile)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD
              << "CHifDirStruct IsModified(hif::application_utils::FileStructure & rF, char * cFile)" << std::endl;
#endif // HIFDIR_DBG
    int nIsDateFileDisabled = atoi(DateFileParam);
    if (nIsDateFileDisabled)
        return false;

    hif::application_utils::FileStructure *fHifOut = GetHifOut();
    if (!fHifOut)
        return false;
    hif::application_utils::FileStructure *fHifTmp = GetHifTmp();
    if (!fHifTmp)
        return false;
    if (!cFile)
        return false;
    // DateFiles FILE
    hif::application_utils::FileStructure fFilet(rF.toString(), std::string(cFile));
    hif::application_utils::FileStructure fDateFile(*fHifTmp, std::string(DateFiles));
    Properties ppDateFile;
    ppDateFile.load(fDateFile.toString(), 1);
    long nSaveDate = atol(ppDateFile[fFilet.toString()].c_str());
    //
    hif::application_utils::FileStructure fFile(*fHifOut, fFilet.toString());
    std::ofstream ofDateFile;
    if (!(fFile.exists())) {
        // Change date to Zero : erased file
        std::string sZero("0");
        ppDateFile.setProperty(fFilet.toString(), sZero);
        // Dump dates file
        ofDateFile.open(fDateFile.toString().c_str());
        if (ofDateFile.is_open()) {
            ppDateFile.dump(ofDateFile);
        }
        return false;
    }
    unsigned long nNewDate = static_cast<unsigned long>(fFile.lastModified());

    if ((nSaveDate == 1) && (!(ppDateFile[fFilet.toString()].empty())))
        return true;
    if (nNewDate == static_cast<unsigned long>(nSaveDate))
        return false;
    // Change date to One : user file
    std::string sOne("1");
    ppDateFile.setProperty(fFilet.toString(), sOne);
    // Dump dates file
    ofDateFile.open(fDateFile.toString().c_str());
    if (ofDateFile.is_open()) {
        ppDateFile.dump(ofDateFile);
    }
    return true;
}

//
// IsModified(DesignUnit & rDU, const char * cFile, Suffix rSuff = SRC)
bool CHifDirStruct::IsModified(DesignUnit &rDU, const char *cFile, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct IsModified(DesignUnit & rDU, char * cFile, Suffix rSuff = SRC)"
              << std::endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fDir = GetDir(rDU, rSuff);
    if (!fDir)
        return false;
    return IsModified(*fDir, cFile);
}

//
// IsModified(Instance & rInst, const char * cFile, Suffix rSuff = SRC)
bool CHifDirStruct::IsModified(Instance &rInst, const char *cFile, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    cout << INDENT << METHOD << "CHifDirStruct IsModified(Instance & rInst, char * cFile, Suffix rSuff = SRC)" << endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fDir = GetDir(rInst, rSuff);
    if (!fDir)
        return false;
    return IsModified(*fDir, cFile);
}

//
// IsModified(LibraryDef & rLd, const char * cFile, Suffix rSuff = SRC)
bool CHifDirStruct::IsModified(LibraryDef &rLd, const char *cFile, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct IsModified(LibraryDef & rLd, char * cFile, Suffix rSuff = SRC)"
              << std::endl;
#endif // HIFDIR_DBG
    hif::application_utils::FileStructure *fDir = GetDir(rLd, rSuff);
    if (!fDir)
        return false;
    return IsModified(*fDir, cFile);
}

//
// IsModifiedBase(DesignUnit & rDU, const char * cFile, Suffix rSuff = SRC)
bool CHifDirStruct::IsModifiedBase(DesignUnit &rDU, const char *cFile, Suffix rSuff)
{
#ifdef HIFDIR_DBG
    std::cout << INDENT << METHOD << "CHifDirStruct IsModifiedBase(DesignUnit & rDU, char * cFile, Suffix rSuff = SRC)"
              << std::endl;
#endif // HIFDIR_DBG
    //hif::application_utils::FileStructure * fDir = GetDirBase(rDU,rSuff);
    hif::application_utils::FileStructure *fDir = GetDir(rDU, rSuff);
    if (!fDir)
        return false;
    return IsModified(*fDir, cFile);
}
void CHifDirStruct::printError(DirectoryStatus eStatus)
{
    switch (eStatus) {
    case hif::backends::CHifDirStruct::DST_CNT_REM:
        messageError("HIF directory : Cannot remove a directory.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_CNT_MOVE:
        messageError("HIF directory :Cannot move the contents of a directory.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_ALRDY_EXIST:
        messageError("HIF directory : Directory already exists.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_NO_EXIST:
        messageError("HIF directory : Directory doesn't exist.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_CNT_CREATE:
        messageError("HIF directory : Cannot create the directory.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_CNT_CHANGE:
        messageError("HIF directory : Cannot change directory.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_NOT_A_DIR:
        messageError("HIF directory : Is not a directory.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_CMD_NOT_EXIST:
        messageError("HIF directory : System executable command doesn't exist.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_CNT_APPLY_SYS:
        messageError("HIF directory : Cannot apply a system executable command.", nullptr, nullptr);
    case hif::backends::CHifDirStruct::DST_OK:
    default:
        break;
    }
}
} // namespace backends
} // namespace hif
