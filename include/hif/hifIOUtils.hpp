/// @file hifIOUtils.hpp
/// @brief Input/output utilities for HIF objects and data structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Options for printing
struct PrintHifOptions {
    /// @brief Whether to print comments.
    bool printComments;

    /// @brief Whether to print summary.
    bool printSummary;

    /// @brief Whether to print code infos.
    bool printCodeInfos;

    /// @brief Whether to print properties.
    bool printProperties;

    /// @brief Whether to print additional keywords.
    bool printAdditionalKeywords;

    /// @brief Whether to print HIF standard libraries.
    bool printHifStandardLibraries;

    /// @brief Whether to use append mode.
    bool appendMode;

    /// @brief The language semantics.
    hif::semantics::ILanguageSemantics *sem;

    PrintHifOptions();
    ~PrintHifOptions();

    /// @brief Copy constructor.
    /// @param other The PrintHifOptions to copy from.
    PrintHifOptions(const PrintHifOptions &other);

    /// @brief Copy assignment operator.
    /// @param other The PrintHifOptions to assign from.
    /// @return Reference to this PrintHifOptions.
    PrintHifOptions &operator=(const PrintHifOptions &other);
};

///	@brief Writes a Hif file.
///	This function try to save in filename file the obj hif description.
///
///	@param filename the name of the file to be created
///	@param obj the hif object to be saved
///	@param xml_format if true print in xml format (default false)
/// @param opt The printing options.
///

void writeFile(
    const std::string &filename,
    Object *obj,
    bool xml_format            = false,
    const PrintHifOptions &opt = PrintHifOptions());

///	@brief Writes a Hif file.
///	This function try to save in filename file the obj hif description.
///
///	@param outstream the output stream
///	@param obj the hif object to be saved
///	@param xml_format if true print in xml format (default false)
/// @param opt The printing options.
///

void writeFile(
    std::ostream &outstream,
    Object *obj,
    bool xml_format            = false,
    const PrintHifOptions &opt = PrintHifOptions());

///	@brief Writes a file.
///	This function try to save in filename file the obj hif description
/// This function append timestamp string to filename to make unique the
/// output file name.
/// If \p custom_path is not defined, function create an output folder,
/// called "debug_out_files" in the path where the executable is run,
/// otherwise file in saved in the path specified.
///	@param filename the name of the file to be created.
///	@param obj the hif object to be saved.
///	@param custom_path eventual custom path for out files
/// for example "/home/user/debug/".
///	@param xml_format if true print in xml format (default false)
/// @param opt The printing options.
///

void writeUniqueFile(
    const std::string &filename,
    Object *obj,
    const std::string &custom_path = "",
    bool xml_format                = false,
    const PrintHifOptions &opt     = PrintHifOptions());

///	@brief Print operator in textual form.
///
///	@param oper the operator to print
///	@param o the output stream
///

void printOperator(Operator oper, std::ostream &o);

/// @brief struct of options passed to readFile.
struct ReadHifOptions {
    /// @brief Default constructor.
    ReadHifOptions();

    /// @brief Default destructor.
    ~ReadHifOptions();

    /// @brief Whether to load HIF standard library.
    bool loadHifStandardLibrary;

    /// @brief The language semantics.
    hif::semantics::ILanguageSemantics *sem;

    /// @brief Copy constructor.
    /// @param other The ReadHifOptions to copy from.
    ReadHifOptions(const ReadHifOptions &other);

    /// @brief Copy assignment operator.
    /// @param other The ReadHifOptions to assign from.
    /// @return Reference to this ReadHifOptions.
    ReadHifOptions &operator=(const ReadHifOptions &other);
};

///	@brief Reads an hif.xml file.
///	This function opens and parses a file and returns the top Hif
///	object of the description.
///
///	@param filename the name of the file to be opened
/// @param opt The read file options.
///	@return the hif description
///

Object *readFile(const std::string &filename, const ReadHifOptions &opt = ReadHifOptions());

/// @brief Writes a Hif file printing all parents.
///
/// @param depth The number of parents to print, or zero to print all parents.
/// @param outstream the output stream
/// @param obj the hif object to be saved
/// @param opt The printing options.
///

void writeParents(
    unsigned int depth,
    std::ostream &outstream,
    Object *obj,
    const hif::PrintHifOptions &opt = hif::PrintHifOptions());

} // namespace hif
