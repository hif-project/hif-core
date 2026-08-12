/// @file CHifDirStruct.hpp
/// @brief HIF backend directory structure.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/FileStructure.hpp"
#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace backends
{

class CSession;

/// @brief Directories management API according to an HIF structure.
/// @ingroup hif_dir
class CHifDirStruct
{
public:
    /// @brief The error codes when checking the directory structure.
    enum DirectoryStatus : char {
        DST_CNT_REM = -4,  ///< Cannot remove a directory.
        DST_CNT_MOVE,      ///< Cannot move a directory.
        DST_ALRDY_EXIST,   ///< Directory already exists.
        DST_NO_EXIST,      ///< Directory doesn't exist.
        DST_OK,            ///< Status Ok.
        DST_CNT_CREATE,    ///< Cannot create a directory.
        DST_CNT_CHANGE,    ///< Cannot change path to another.
        DST_NOT_A_DIR,     ///< File structure is not a directory.
        DST_CMD_NOT_EXIST, ///< The specified command doesn't exist.
        DST_CNT_APPLY_SYS  ///< Cannot apply the command in the HIF system.
    };

    /// @brief List of sub-directories (applied on a design unit or a library
    /// def directory).
    enum Suffix : unsigned char {
        SRC, ///< "src" source directory.
        INC, ///< "inc" include directory.
        DOC, ///< "doc" documentation directory.
        LIB, ///< "lib" library directory.
        EXE, ///< "exe" executable directory.
        OBJ, ///< "obj" object .o directory.
        NONE ///< "" parent directory.
    };

    /// @brief Return a string corresponding to the subdir identifier.
    /// @param eSuff subdir identifier.
    /// @return the string corresponding to the subdir identifier
    static std::string ReturnSuffix(Suffix eSuff);

    /// @brief Constructor build the directory structure
    /// @param rsoTop System to treat
    /// @param pcRoot root directory according to a session
    CHifDirStruct(hif::System &rsoTop, const char *pcRoot = nullptr);

    /// @brief Destructor
    ~CHifDirStruct();

    CHifDirStruct(const CHifDirStruct &)            = delete;
    CHifDirStruct &operator=(const CHifDirStruct &) = delete;

    /// @brief Return the status of the directory structure.
    /// @return the status of the directory structure.
    DirectoryStatus GetStatus() { return m_eStatus; }

    /// @brief Check the directories structure according to the current session.
    /// @return the status of the directory structure.
    DirectoryStatus Check();

    /// @brief Check the directories structure according to the new HIF system.
    /// @param rsoTop System to update.
    /// @return the status of the directory structure.
    DirectoryStatus Check(hif::System &rsoTop);

    /// @brief Update the DateFiles according to the directory structure.
    /// @return the status of the directory structure.
    DirectoryStatus UpdateDates();

    /// @brief Return the root directory.
    /// @return the root directory.
    /// @details
    /// if pcRoot is nullptr the returned value is given by the plugiprc variable
    /// SESSION_PATH. If the property doesn't exists, the returned value is
    /// $(IPNAME)_ENV.. If IPNAME cannot be determined then value = _ENV
    application_utils::FileStructure *GetHifOut();

    /// @brief Return the directory `GetHifOut()`/$(PLUGIP_TMP).
    /// @return the temporary directory.
    /// @details
    /// PLUGIP_TMP is fixed into the file plugiprc. If the variable is not
    /// fixed then the default directory is `GetHifOut()`/.plugip
    application_utils::FileStructure *GetHifTmp();

    /// @brief Following methods return the directory corresponding to the Hif
    /// object.
    /// @param rduTgt Hif object to treat.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return the directory corresponding to the Hif object.
    /// @details
    /// The Suffix parameter provide a subdir defined in the list {SRC, INC,
    /// DOC, LIB, EXE, OBJ}. A default value is fixed to SRC.
    /// The path returned is relative to the path returned by GetHifOut().
    application_utils::FileStructure *GetDir(hif::Object &rduTgt, Suffix rSuff = Suffix::SRC);

    /// @brief GetDir between two FileStructure
    /// @param fTgt target directory.
    /// @param fSrc source directory.
    /// @return the directory corresponding to the Hif object.
    application_utils::FileStructure *
    GetDir(application_utils::FileStructure &fTgt, application_utils::FileStructure &fSrc);

    /// @brief Following methods allow to perform a system command (pcLine) inside a specific
    /// diretory done by Hif hif::Object + Suffix rSuff

    /// @brief Apply a system command inside the temporary directory.
    /// @details Only applied inside GetHifTmp() directory.
    /// @param pcLine command to apply.
    /// @return the status of the directory structure.
    DirectoryStatus Apply(const char *pcLine);

    /// @brief Apply a system command inside the directory done by Hif hif::Object + Suffix rSuff
    /// @param rduTgt Hif object to treat.
    /// @param pcLine command to apply.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return the status of the directory structure.
    DirectoryStatus Apply(hif::DesignUnit &rduTgt, const char *pcLine, Suffix rSuff = Suffix::SRC);

    /// @brief Apply a system command inside the directory done by Hif hif::Object + Suffix rSuff
    /// @param rvTgt Hif object to treat.
    /// @param pcLine command to apply.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return the status of the directory structure.
    DirectoryStatus Apply(hif::View &rvTgt, const char *pcLine, Suffix rSuff = Suffix::SRC);

    /// @brief Apply a system command inside the directory done by Hif hif::Object + Suffix rSuff
    /// @param riTgt Hif object to treat.
    /// @param pcLine command to apply.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return the status of the directory structure.
    DirectoryStatus Apply(hif::Instance &riTgt, const char *pcLine, Suffix rSuff = Suffix::SRC);

    /// @brief Apply a system command inside the directory done by Hif hif::Object + Suffix rSuff
    /// @param rlTgt Hif object to treat.
    /// @param pcLine command to apply.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return the status of the directory structure.
    DirectoryStatus Apply(hif::LibraryDef &rlTgt, const char *pcLine, Suffix rSuff = Suffix::SRC);

    /// @brief Apply a system command inside the directory done by Hif hif::Object + Suffix rSuff
    /// @attention rduTgt corresponds to a DUBASE.
    /// @param rduTgt Hif object to treat.
    /// @param pcLine command to apply.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return the status of the directory structure.
    DirectoryStatus ApplyBase(hif::DesignUnit &rduTgt, const char *pcLine, Suffix rSuff = Suffix::SRC);

    /// @brief Return true if a modification in files has been performed on
    /// files inside the directory done by Hif hif::Object + Suffix rSuff.
    /// @warning cFile is filename without path
    /// @param rDU Hif object to treat.
    /// @param cFile file to check.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return true if a modification in files has been performed.
    bool IsModified(hif::DesignUnit &rDU, const char *cFile, Suffix rSuff = Suffix::SRC);

    /// @brief Return true if a modification in files has been performed on
    /// files inside the directory done by Hif hif::Object + Suffix rSuff.
    /// @warning cFile is filename without path
    /// @param rInst Hif object to treat.
    /// @param cFile file to check.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return true if a modification in files has been performed.
    bool IsModified(hif::Instance &rInst, const char *cFile, Suffix rSuff = Suffix::SRC);

    /// @brief Return true if a modification in files has been performed on
    /// files inside the directory done by Hif hif::Object + Suffix rSuff.
    /// @warning cFile is filename without path
    /// @param rDU Hif object to treat.
    /// @param cFile file to check.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return true if a modification in files has been performed.
    bool IsModified(hif::LibraryDef &rDU, const char *cFile, Suffix rSuff = Suffix::SRC);

    /// @brief Return true if a modification in files has been performed on
    /// files inside the directory done by Hif hif::Object + Suffix rSuff.
    /// @warning cFile is filename without path
    /// @param rDU Hif object to treat.
    /// @param cFile file to check.
    /// @param rSuff subdir defined in the list {SRC, INC, DOC, LIB, EXE, OBJ}.
    /// @return true if a modification in files has been performed.
    bool IsModifiedBase(hif::DesignUnit &rDU, const char *cFile, Suffix rSuff = Suffix::SRC);

    /// @brief Print error corresponding to the given status.
    /// @param eStatus status to print.
    void printError(DirectoryStatus eStatus);

private:
    /// @brief Default parent structure suffix
    static const char *const HifOut;

    /// @brief plugiprc parameter to get the temporary directory (.plugip by default)
    static const char *const HifTmp;

    /// @brief Default directory which the generated code (root directory)
    static const char *const DefaultHifOut;

    /// @brief Default temporary directory
    static const char *const DefaultHifTmp;

    /// @brief plugiprc parameter which identify the root directory for the generated code
    static const char *const CfgRoot;

    /// @brief plugiprc parameter which identify modified and user files
    static const char *const DateFileParam;

    /// @brief HIF property name to get the IP name to define the root directory
    static const char *const IPNameProp;

    /// @brief The name of the file which contain information of each generated file
    static const char *const DateFiles;

    /// @brief Status of the directory structure.
    DirectoryStatus m_eStatus;

    /// @brief The HIF system to treat.
    CSession *m_psSystem;

    /// @brief The root directory.
    application_utils::FileStructure *m_pfRoot;

    /// @brief Default constructor.
    CHifDirStruct();

    /// @brief Return true if the file checked is a user one (modified or not)
    /// @param rF directory which contains the file
    /// @param cFile file to check
    /// @return true if it is modified
    bool IsModified(application_utils::FileStructure &rF, const char *cFile);
};

} // namespace backends
} // namespace hif
