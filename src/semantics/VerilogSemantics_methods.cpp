/// @file VerilogSemantics_methods.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cmath>
#include <iostream>

#include "hif/HifFactory.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/HIFSemantics.hpp"
#include "hif/semantics/VerilogSemantics.hpp"

namespace hif
{
namespace semantics
{
LibraryDef *VerilogSemantics::getStandardPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("standard", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Bitvector *templateBV =
        factory.bitvector(factory.range(new Identifier("left"), dir_downto, new Identifier("right")));

    Type *integerUnsBV = nullptr;
    Type *integerBV    = nullptr;
    Value *integerBVal = nullptr;
    if (hifFormat) {
        integerUnsBV = factory.integer(nullptr, false);
        integerBV    = factory.integer();
        integerBVal  = factory.intval(0, hif::copy(integerBV));
    } else {
        integerUnsBV = factory.bitvector(factory.range(31, 0), true, true);
        integerBV    = factory.bitvector(factory.range(31, 0), true, true, false, true);
        integerBVal =
            factory.bitvectorval("00000000000000000000000000000000", static_cast<Bitvector *>(hif::copy(integerBV)));
    }

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // template <int times, int size>
    // sc_dt::sc_lv<times * size> hif_verilog_iterated_concat(sc_dt::sc_lv<size> expression);
    {
        Range *retRange = factory.range(
            factory.expression(
                factory.expression(
                    factory.expression(new Identifier("size"), op_plus, factory.intval(1)), op_mult,
                    new Identifier("times")),
                op_minus, factory.intval(1)),
            dir_downto, new IntValue(0));

        Range *paramRange = factory.range(new Identifier("size"), dir_downto, new IntValue(0));

        ld->declarations.push_back(factory.subprogram(
            factory.bitvector(retRange, true, true), _makeHifName("iterated_concat", hifFormat).c_str(),
            (factory.templateValueParameter(hif::copy(integerUnsBV), "times", nullptr),
             factory.templateValueParameter(hif::copy(integerUnsBV), "size", nullptr)),
            (factory.parameter(factory.bitvector(paramRange, true, true), "expression", nullptr))));
    }

    // ///////////////////////////////////////////////////////////////////
    // System tasks (Clause 17)
    // ///////////////////////////////////////////////////////////////////

    // Display tasks
    //$display
    _addMultiparamFunction(ld, "_system_display", factory, hifFormat, nullptr);
    //$displayb // bin??
    //$displayh // hex??
    //$displayo // oct??
    //$strobe
    _addMultiparamFunction(ld, "_system_strobe", factory, hifFormat, nullptr);
    //$strobeb
    //$strobeh
    //$strobeo
    //$write
    _addMultiparamFunction(ld, "_system_write", factory, hifFormat, nullptr);
    //$writeb
    //$writeh
    //$writeo
    //$monitor
    _addMultiparamFunction(ld, "_system_monitor", factory, hifFormat, nullptr);

    //$monitorb
    _addMultiparamFunction(ld, "_system_monitorb", factory, hifFormat, nullptr);

    //$monitorh
    _addMultiparamFunction(ld, "_system_monitorh", factory, hifFormat, nullptr);

    //$monitoro
    _addMultiparamFunction(ld, "_system_monitoro", factory, hifFormat, nullptr);

    //$monitoroff
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("_system_monitoroff", hifFormat).c_str(), factory.noTemplates(), factory.noParameters()));

    //$monitoron
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("_system_monitoron", hifFormat).c_str(), factory.noTemplates(), factory.noParameters()));

    // File I/O tasks
    //$fclose
    {
        SubProgram *p = factory.subprogram(
            nullptr, _makeHifName("_system_fclose", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(hif::copy(integerBV), "param1"));

        ld->declarations.push_back(p);
    }

    //$fdisplay
    _addMultiparamFunction(ld, "_system_fdisplay", factory, hifFormat, nullptr);
    //$fdisplayb
    //$fdisplayh
    //$fdisplayo
    //$fstrobe
    _addMultiparamFunction(ld, "_system_fstrobe", factory, hifFormat, nullptr);
    //$fstrobeb
    //$fstrobeh
    //$fstrobeo
    //$swrite
    //$swriteb
    //$swriteh
    //$swriteo
    //$fscanf
    _addMultiparamFunction(ld, "_system_fscanf", factory, hifFormat, nullptr);
    //$fread
    _addMultiparamFunction(ld, "_system_fread", factory, hifFormat, nullptr);
    //$fseek
    //$fflush
    {
        SubProgram *p = factory.subprogram(
            nullptr, _makeHifName("_system_fflush", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(hif::copy(integerBV), "param1", hif::copy(integerBVal)));

        ld->declarations.push_back(p);
    }

    //$feof
    {
        SubProgram *p = factory.subprogram(
            hif::copy(integerBV), _makeHifName("_system_feof", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(hif::copy(integerBV), "param1"));

        ld->declarations.push_back(p);
    }

    //$sdf_annotate
    //$fopen
    {
        SubProgram *f = factory.subprogram(
            hif::copy(integerBV), _makeHifName("_system_fopen", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.string(false), "param1"),
             factory.parameter(factory.string(false), "param2", factory.stringval("w+"))));

        ld->declarations.push_back(f);
    }

    //$fwrite
    _addMultiparamFunction(ld, "_system_fwrite", factory, hifFormat, nullptr);

    //$fwriteb
    //$fwriteh
    //$fwriteo
    //$fmonitor
    _addMultiparamFunction(ld, "_system_fmonitor", factory, hifFormat, nullptr);

    //$fmonitorb
    _addMultiparamFunction(ld, "_system_fmonitorb", factory, hifFormat, nullptr);

    //$fmonitorh
    _addMultiparamFunction(ld, "_system_fmonitorh", factory, hifFormat, nullptr);

    //$fmonitoro
    _addMultiparamFunction(ld, "_system_fmonitoro", factory, hifFormat, nullptr);

    //$sformat
    //$fgetc
    //$ungetc
    //$fgets
    //$sscanf
    //$rewind
    //$ftell
    //$ferror

    //$readmemb
    {
        SubProgram *f = factory.subprogram(
            nullptr, _makeHifName("_system_readmemb", hifFormat).c_str(),
            (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(hif::copy(integerBV), "left"),
             factory.templateValueParameter(hif::copy(integerBV), "right")),
            (factory.parameter(factory.string(false), "param1"),
             factory.parameter(
                 factory.array(
                     factory.range(factory.identifier("left"), hif::dir_downto, factory.identifier("right")),
                     factory.typeRef("T")),
                 "param2"),
             factory.parameter(hif::copy(integerBV), "param3", hif::copy(integerBVal)),
             factory.parameter(hif::copy(integerBV), "param4", factory.identifier("left")),
             factory.parameter(
                 hif::copy(integerBV), "param5",
                 factory.when(
                     (factory.whenalt(
                         factory.expression(factory.identifier("left"), hif::op_lt, factory.identifier("right")),
                         factory.identifier("left"))),
                     factory.identifier("right"))),
             factory.parameter(
                 hif::copy(integerBV), "param6",
                 factory.expression(factory.identifier("left"), hif::op_minus, factory.identifier("right")))));

        f->addProperty(PROPERTY_METHOD_EXPLICIT_PARAMETERS);
        ld->declarations.push_back(f);
    }

    //$readmemh
    {
        SubProgram *f = factory.subprogram(
            nullptr, _makeHifName("_system_readmemh", hifFormat).c_str(),
            (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(hif::copy(integerBV), "left"),
             factory.templateValueParameter(hif::copy(integerBV), "right")),
            (factory.parameter(factory.string(false), "param1"),
             factory.parameter(
                 factory.array(
                     factory.range(factory.identifier("left"), hif::dir_downto, factory.identifier("right")),
                     factory.typeRef("T")),
                 "param2"),
             factory.parameter(hif::copy(integerBV), "param3", hif::copy(integerBVal)),
             factory.parameter(hif::copy(integerBV), "param4", factory.identifier("left")),
             factory.parameter(
                 hif::copy(integerBV), "param5",
                 factory.when(
                     (factory.whenalt(
                         factory.expression(factory.identifier("left"), hif::op_lt, factory.identifier("right")),
                         factory.identifier("left"))),
                     factory.identifier("right"))),
             factory.parameter(
                 hif::copy(integerBV), "param6",
                 factory.expression(factory.identifier("left"), hif::op_minus, factory.identifier("right")))));

        f->addProperty(PROPERTY_METHOD_EXPLICIT_PARAMETERS);
        ld->declarations.push_back(f);
    }

    // Timescale tasks
    //$printtimescale
    //$timeformat

    // Simulation control tasks
    //$finish
    {
        SubProgram *p = factory.subprogram(
            nullptr, _makeHifName("_system_finish", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(
                factory.bitvector(factory.range(31, 0), true, true), "param1",
                factory.bitvectorval(
                    "00000000000000000000000000000001", factory.bitvector(factory.range(31, 0), true, true))));
        ld->declarations.push_back(p);

        SubProgram *p_hdtlib = getSuffixedCopy(p, "_hdtlib");
        ld->declarations.push_back(p_hdtlib);
    }

    //$stop
    {
        SubProgram *p = factory.subprogram(
            nullptr, _makeHifName("_system_stop", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(
                factory.bitvector(factory.range(31, 0), true, true), "param1",
                factory.bitvectorval(
                    "00000000000000000000000000000001", factory.bitvector(factory.range(31, 0), true, true))));
        ld->declarations.push_back(p);

        SubProgram *p_hdtlib = getSuffixedCopy(p, "_hdtlib");
        ld->declarations.push_back(p_hdtlib);
    }

    // PLA modeling tasks
    //$async$and$array
    //$async$nand$array
    //$async$or$array
    //$async$and$plane
    //$async$nand$plane
    //$async$or$plane
    //$async$nor$array
    //$sync$and$array
    //$sync$nand$array
    //$sync$or$array
    //$sync$nor$array
    //$async$nor$plane
    //$sync$and$plane
    //$sync$nand$plane
    //$sync$or$plane
    //$sync$nor$plane

    // Stochastic analysis tasks
    //$q_initialize
    //$q_remove
    //$q_exam
    //$q_add
    //$q_full

    // Simulation time functions
    //$realtime
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("_system_realtime", hifFormat).c_str(), factory.noTemplates(),
        factory.noParameters()));

    //$time
    {
        SubProgram *p = factory.subprogram(
            factory.bitvector(factory.range(63, 0), true, true), _makeHifName("_system_time", hifFormat).c_str(),
            factory.noTemplates(), factory.noParameters());
        ld->declarations.push_back(p);

        SubProgram *p_hdtlib = getSuffixedCopy(p, "_hdtlib");
        ld->declarations.push_back(p_hdtlib);
    }

    //$stime
    {
        SubProgram *p = factory.subprogram(
            factory.bitvector(factory.range(31, 0), true, true, false, true),
            _makeHifName("_system_stime", hifFormat).c_str(), factory.noTemplates(), factory.noParameters());
        ld->declarations.push_back(p);

        SubProgram *p_hdtlib = getSuffixedCopy(p, "_hdtlib");
        ld->declarations.push_back(p_hdtlib);
    }

    // Conversion functions
    //$bitstoreal
    //$itor
    //$signed
    //$realtobits
    //$rtoi
    //$unsigned

    // Probabilistic distribution functions
    //$random
    {
        SubProgram *p = factory.subprogram(
            hif::copy(integerBV), _makeHifName("_system_random", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(hif::copy(integerBV), "param1", hif::copy(integerBVal)));
        ld->declarations.push_back(p);
    }
    //$dist_erlang
    //$dist_normal
    //$dist_t
    //$dist_chi_square
    //$dist_exponential
    //$dist_poisson
    //$dist_uniform

    // Command line input
    //$test$plusargs
    //$value$plusargs

    // Math functions
    //$clog2
    //$ln
    //$log10
    //$exp
    //$sqrt
    //$pow
    //$floor
    //$ceil
    //$sin
    //$cos
    //$tan
    //$asin
    //$acos
    //$atan
    //$atan2
    //$hypot
    //$sinh
    //$cosh
    //$tanh
    //$asinh
    //$acosh
    //$atanh

    // The following are not part of the standard, and thus not supported (Annex C):
    // $countdrivers, $getpattern, $incsave, $input, $key, $list, $log, $nokey
    // $nolog, $reset, $reset_count, $reset_value, $restart, $save, $scale
    // $scope, $showscopes, $showvars, $sreadmemb, $sreadmemh

    delete templateBV;
    delete integerUnsBV;
    delete integerBV;
    delete integerBVal;

    return ld;
}

LibraryDef *VerilogSemantics::getVAMSStandardPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("vams_standard", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::ams);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Type *integerBV = nullptr;
    if (hifFormat)
        integerBV = factory.integer();
    else
        integerBV = factory.bitvector(factory.range(31, 0), true, true, false, true);
    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // 3.6.5

    // ground
    ld->declarations.push_back(factory.typeDef(
        _makeHifName("ground", hifFormat).c_str(), factory.typeRef("T"), false,
        factory.templateTypeParameter(nullptr, "T")));

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // Note: vams_contribute_statement has been defined on disciplines only
    // e.g., electrical <+ electrical --> vams_contribute_statement(electrical&, electrical)
    // generalized using templates
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("vams_contribution_statement", hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"))));

    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("vams_indirect_contribution_statement", hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"))));

    // VAMS branch() statement is mapped as an Alias plus a branch() method.
    // It is overloaded to assure correct typing.
    {
        // Port parameter:
        ld->declarations.push_back(factory.subprogram(
            factory.typeRef("T"), _makeHifName("vams_branch_port", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"), factory.parameter(factory.typeRef("T"), "param1")));
        // Single parameter:
        ld->declarations.push_back(factory.subprogram(
            factory.typeRef("T"), _makeHifName("vams_branch", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"), factory.parameter(factory.typeRef("T"), "param1")));
        // Two scalar parameters:
        ld->declarations.push_back(factory.subprogram(
            factory.typeRef("T"), _makeHifName("vams_branch", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(factory.typeRef("T"), "param2"))));
        // Two vector parameters:
        ld->declarations.push_back(factory.subprogram(
            factory.array(
                new Range(
                    new IntValue(0),
                    factory.expression(new Identifier("left1"), hif::op_minus, new Identifier("right1")),
                    hif::dir_upto),
                factory.typeRef("T")),
            _makeHifName("vams_branch", hifFormat).c_str(),
            (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(hif::copy(integerBV), "left1"),
             factory.templateValueParameter(hif::copy(integerBV), "right1"),
             factory.templateValueParameter(hif::copy(integerBV), "left2"),
             factory.templateValueParameter(hif::copy(integerBV), "right2")),
            (factory.parameter(_makeTemplateArray("1", factory.typeRef("T")), "param1"),
             factory.parameter(_makeTemplateArray("2", factory.typeRef("T")), "param2"))));
        // Vector and scalar parameters:
        ld->declarations.push_back(factory.subprogram(
            factory.array(
                new Range(
                    new IntValue(0),
                    factory.expression(new Identifier("left1"), hif::op_minus, new Identifier("right1")),
                    hif::dir_upto),
                factory.typeRef("T")),
            _makeHifName("vams_branch", hifFormat).c_str(),
            (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(hif::copy(integerBV), "left1"),
             factory.templateValueParameter(hif::copy(integerBV), "right1")),
            (factory.parameter(_makeTemplateArray("1", factory.typeRef("T")), "param1"),
             factory.parameter(factory.typeRef("T"), "param2"))));
        // Scalar and vector parameters:
        ld->declarations.push_back(factory.subprogram(
            factory.array(
                new Range(
                    new IntValue(0),
                    factory.expression(new Identifier("left2"), hif::op_minus, new Identifier("right2")),
                    hif::dir_upto),
                factory.typeRef("T")),
            _makeHifName("vams_branch", hifFormat).c_str(),
            (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(hif::copy(integerBV), "left2"),
             factory.templateValueParameter(hif::copy(integerBV), "right2")),
            (factory.parameter(factory.typeRef("T"), "param1"),
             factory.parameter(_makeTemplateArray("2", factory.typeRef("T")), "param2"))));
    }

    // initial_step() and final_step()
    _addMultiparamFunction(ld, "initial_step", factory, hifFormat, factory.bit(true, true));
    _addMultiparamFunction(ld, "final_step", factory, hifFormat, factory.bit(true, true));

    // flow through port (e.g. I(<in_port>)):
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef("T"), _makeHifName("vams_flow_of_port", hifFormat).c_str(),
        factory.templateTypeParameter(nullptr, "T"), factory.parameter(factory.typeRef("T"), "param1")));
    // AMS 4.3 Built-in mathematical functions

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("ln", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(1))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("log", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(1))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("exp", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(1))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("sqrt", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("min", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("max", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("abs", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    //    ld->declarations.push_back(factory.subprogram(
    //        factory.real(),
    //        _makeHifName("pow", hifFormat).c_str(),
    //        factory.noTemplates(),
    //        (
    //            factory.parameter(factory.real(), "param1", factory.realval(1)),
    //            factory.parameter(factory.real(), "param2", factory.realval(0))
    //        )
    //    ));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("floor", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("ceil", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("sin", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("cos", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("tan", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("asin", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("acos", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("atan", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("atan2", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("hypot", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("sinh", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("cosh", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("tanh", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("asinh", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("acosh", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("atanh", hifFormat).c_str(), factory.noTemplates(),
        factory.parameter(factory.real(), "param1", factory.realval(0))));

    // AMS 4.4 Signal access functions are defined inside disciplines library

    // AMS 4.5 analog operators

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("ddt", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("ddx", hifFormat).c_str(),
        factory.templateTypeParameter(nullptr, "T"), // branch_probe_function_call may return different disciplines
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.typeRef("T"), "param2") // branch_probe_function_call
         )));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("idt", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)),
         factory.parameter(factory.real(), "param3", factory.realval(0)),
         factory.parameter(factory.real(), "param4", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("idtmod", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)),
         factory.parameter(factory.real(), "param3", factory.realval(0)),
         factory.parameter(factory.real(), "param4", factory.realval(0)),
         factory.parameter(factory.real(), "param5", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("absdelay", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)),
         factory.parameter(factory.real(), "param3", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("transition", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)),
         factory.parameter(factory.real(), "param3", factory.realval(0)),
         factory.parameter(factory.real(), "param4", factory.realval(0)),
         factory.parameter(factory.real(), "param5", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("slew", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)),
         factory.parameter(factory.real(), "param3", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("last_crossing", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)),
         factory.parameter(factory.real(), "param2", factory.realval(0)))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("limexp", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(0)))));

    //laplace_zp
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("laplace_zp", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //laplace_zd
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("laplace_zd", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //laplace_np
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("laplace_np", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //laplace_nd
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("laplace_nd", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //zi_zp
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("zi_zp", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //zi_zd
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("zi_zd", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //zi_np
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("zi_np", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));
    //zi_nd
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("zi_nd", hifFormat).c_str(),
        (factory.templateValueParameter(hif::copy(integerBV), "left1"),
         factory.templateValueParameter(hif::copy(integerBV), "right1"),
         factory.templateValueParameter(hif::copy(integerBV), "left2"),
         factory.templateValueParameter(hif::copy(integerBV), "right2")),
        (factory.parameter(factory.real(), "param1", factory.realval(0.0)),
         factory.parameter(
             factory.array(new Range(new Identifier("left1"), new Identifier("right1"), dir_downto), factory.real()),
             "param2", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(
             factory.array(new Range(new Identifier("left2"), new Identifier("right2"), dir_downto), factory.real()),
             "param3", factory.aggregate(nullptr, factory.aggregatealt(factory.intval(0), factory.realval(0.0)))),
         factory.parameter(factory.real(), "param4", factory.realval(0.0)))));

    // AMS 4.6 Analysis-dependent functions

    // analysis(string)
    // where string is one of {nodeset, static, ic, dc, tran, ac, noise}
    // and return value is a bitvector of span 9 (see Table 4-22 of standard)
    // TODO: analysis_list could be a list of strings
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(new Range(9, 0), true, true, false, true), _makeHifName("analysis", hifFormat).c_str(),
        factory.noTemplates(), factory.parameter(factory.string(), "param1", factory.stringval("static"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("ac_stim", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.string(), "param1", factory.stringval("ac")), // analysis_name
         factory.parameter(factory.real(), "param2", factory.realval(1)),        // magnitude (default = 1)
         factory.parameter(factory.real(), "param3", factory.realval(0))         // phase (default = 0)
         )));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("white_noise", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(1)),     // pwr
         factory.parameter(factory.string(), "param2", factory.stringval("")) // name
         )));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("flicker_noise", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", factory.realval(1)),     // pwr
         factory.parameter(factory.real(), "param2", factory.realval(1)),     // exp
         factory.parameter(factory.string(), "param3", factory.stringval("")) // name
         )));

    // TODO vector input is currently unsupported
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("noise_table", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.string(), "param1", factory.stringval("")) // file name input
         )));
    // AMS 5.10.3 Monitored events
    ld->declarations.push_back(factory.subprogram(
        factory.event(), _makeHifName("cross", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", nullptr),                 // expr
         factory.parameter(hif::copy(integerBV), "param2", factory.intval(0)), // dir
         factory.parameter(factory.real(), "param3", factory.realval(0)),      // time_tol
         factory.parameter(factory.real(), "param4", factory.realval(0)),      // expr_tol
         factory.parameter(factory.real(), "param5", factory.realval(0))       // enable
         )));

    ld->declarations.push_back(factory.subprogram(
        factory.event(), _makeHifName("above", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", nullptr),            // expr
         factory.parameter(factory.real(), "param2", factory.realval(0)), // time_tol
         factory.parameter(factory.real(), "param3", factory.realval(0)), // expr_tol
         factory.parameter(factory.real(), "param4", factory.realval(0))  // enable
         )));

    ld->declarations.push_back(factory.subprogram(
        factory.event(), _makeHifName("timer", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", nullptr),            // start_time
         factory.parameter(factory.real(), "param2", factory.realval(0)), // period
         factory.parameter(factory.real(), "param3", factory.realval(0)), // time_tol
         factory.parameter(factory.real(), "param4", factory.realval(0))  // enable
         )));

    ld->declarations.push_back(factory.subprogram(
        factory.event(), _makeHifName("absdelta", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1", nullptr),            // expr
         factory.parameter(factory.real(), "param2", nullptr),            // delta
         factory.parameter(factory.real(), "param3", factory.realval(0)), // expr_tol
         factory.parameter(factory.real(), "param4", factory.realval(0)), // time_tol
         factory.parameter(factory.real(), "param5", factory.realval(0))  // enable
         )));

    // AMS 9: System Tasks
    // Not reported system tasks are already defined in Verilog standard

    // $debug
    _addMultiparamFunction(ld, "_system_debug", factory, hifFormat, nullptr);
    // $fdebug
    _addMultiparamFunction(ld, "_system_fdebug", factory, hifFormat, nullptr);
    // $fatal
    _addMultiparamFunction(ld, "_system_fatal", factory, hifFormat, nullptr);
    // $warning
    _addMultiparamFunction(ld, "_system_warning", factory, hifFormat, nullptr);
    // $error
    _addMultiparamFunction(ld, "_system_error", factory, hifFormat, nullptr);
    // $info
    _addMultiparamFunction(ld, "_system_info", factory, hifFormat, nullptr);
    // $abstime
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_abstime", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $arandom
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_arandom", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"),
             factory.parameter(factory.string(), "param2", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_chi_square
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_chi_square", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.string(), "param3", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_exponential
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_exponential", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.string(), "param3", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_poisson
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_poisson", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.string(), "param3", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_uniform
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_poisson", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.real(), "param3"),
             factory.parameter(factory.string(), "param4", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_erlang
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_erlang", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.real(), "param3"),
             factory.parameter(factory.string(), "param4", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_normal
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_normal", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.real(), "param3"),
             factory.parameter(factory.string(), "param4", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $rdist_t
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_rdist_t", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.string(), "param3", factory.stringval("global"))));

        ld->declarations.push_back(p);
    }
    // $temperature
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_temperature", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $vt
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_vt", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(factory.real(), "param1", factory.realval(0.0)));

        ld->declarations.push_back(p);
    }
    // $simparam
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_simparam", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.string(), "param1"),
             factory.parameter(factory.real(), "param2", factory.realval(0.0))));

        ld->declarations.push_back(p);
    }
    // $simparam$str
    {
        SubProgram *p = factory.subprogram(
            factory.string(), _makeHifName("_system_simparam_str", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(factory.string(), "param1"));

        ld->declarations.push_back(p);
    }
    // $simprobe
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_simprobe", hifFormat).c_str(), factory.noTemplates(),
            (factory.parameter(factory.string(), "param1"), factory.parameter(factory.string(), "param2"),
             factory.parameter(factory.real(), "param3", factory.realval(0.0))));

        ld->declarations.push_back(p);
    }
    // $discontinuity
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_discontinuity", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(factory.real(), "param1", factory.realval(0.0)));

        ld->declarations.push_back(p);
    }
    // $limit
    _addMultiparamFunction(ld, "_system_limit", factory, hifFormat, factory.real());
    // $bound_step
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_bound_step", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(factory.real(), "param1"));

        ld->declarations.push_back(p);
    }
    // $mfactor
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_mfactor", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $xposition
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_xposition", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $yposition
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_yposition", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $angle
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_angle", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $hflip
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_hflip", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $vflip
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_vflip", hifFormat).c_str(), factory.noTemplates(),
            factory.noParameters());

        ld->declarations.push_back(p);
    }
    // $param_given
    {
        SubProgram *p = factory.subprogram(
            factory.bit(true, true), _makeHifName("_system_param_given", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(factory.string(), "param1"));

        ld->declarations.push_back(p);
    }
    // $port_connected
    {
        SubProgram *p = factory.subprogram(
            factory.bit(true, true), _makeHifName("_system_port_connected", hifFormat).c_str(), factory.noTemplates(),
            factory.parameter(factory.string(), "param1"));

        ld->declarations.push_back(p);
    }
    // $analog_node_alias
    {
        SubProgram *p = factory.subprogram(
            factory.bit(true, true), _makeHifName("_system_analog_node_alias", hifFormat).c_str(),
            factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.string(), "param2")));

        ld->declarations.push_back(p);
    }
    // $analog_port_alias
    {
        SubProgram *p = factory.subprogram(
            factory.bit(true, true), _makeHifName("_system_analog_port_alias", hifFormat).c_str(),
            factory.noTemplates(),
            (factory.parameter(factory.real(), "param1"), factory.parameter(factory.string(), "param2")));

        ld->declarations.push_back(p);
    }
    // $table_model
    _addMultiparamFunction(ld, "_system_table_model", factory, hifFormat, factory.real());
    // $driver_count
    {
        SubProgram *p = factory.subprogram(
            hif::copy(integerBV), _makeHifName("_system_driver_count", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"), factory.parameter(factory.typeRef("T"), "param1"));

        ld->declarations.push_back(p);
    }
    // $driver_state
    {
        SubProgram *p = factory.subprogram(
            factory.bit(true, true), _makeHifName("_system_driver_state", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(hif::copy(integerBV), "param2")));

        ld->declarations.push_back(p);
    }
    // $driver_strength
    {
        SubProgram *p = factory.subprogram(
            factory.bitvector(factory.range(new IntValue(5), hif::dir_downto, new IntValue(0)), true, true),
            _makeHifName("_system_driver_strength", hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(hif::copy(integerBV), "param2")));

        ld->declarations.push_back(p);
    }
    // @(driver_update)
    {
        SubProgram *p = factory.subprogram(
            factory.event(), _makeHifName("driver_update", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"), (factory.parameter(factory.typeRef("T"), "param1")));

        ld->declarations.push_back(p);
    }
    // $driver_delay
    {
        SubProgram *p = factory.subprogram(
            factory.real(), _makeHifName("_system_driver_delay", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(factory.real(), "param2")));

        ld->declarations.push_back(p);
    }
    // $driver_next_state
    {
        SubProgram *p = factory.subprogram(
            factory.bit(true, true), _makeHifName("_system_driver_next_state", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(hif::copy(integerBV), "param2")));

        ld->declarations.push_back(p);
    }
    // $driver_next_strength
    {
        SubProgram *p = factory.subprogram(
            hif::copy(integerBV), _makeHifName("_system_driver_next_strength", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(hif::copy(integerBV), "param2")));

        ld->declarations.push_back(p);
    }
    // $driver_type
    {
        SubProgram *p = factory.subprogram(
            hif::copy(integerBV), _makeHifName("_system_driver_type", hifFormat).c_str(),
            factory.templateTypeParameter(nullptr, "T"),
            (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(hif::copy(integerBV), "param2")));

        ld->declarations.push_back(p);
    }
    delete integerBV;
    return ld;
}

LibraryDef *VerilogSemantics::getVAMSConstantsPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("vams_constants", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::ams);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_E", hifFormat).c_str(), factory.realval(2.7182818284590452354)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_LOG2E", hifFormat).c_str(), factory.realval(1.4426950408889634074)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_LOG10E", hifFormat).c_str(), factory.realval(0.43429448190325182765)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_LN2", hifFormat).c_str(), factory.realval(0.69314718055994530942)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_LN10", hifFormat).c_str(), factory.realval(2.30258509299404568402)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_PI", hifFormat).c_str(), factory.realval(3.14159265358979323846)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_TWO_PI", hifFormat).c_str(), factory.realval(6.28318530717958647693)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_PI_2", hifFormat).c_str(), factory.realval(1.57079632679489661923)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_PI_4", hifFormat).c_str(), factory.realval(0.78539816339744830962)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_1_PI", hifFormat).c_str(), factory.realval(0.31830988618379067154)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_2_PI", hifFormat).c_str(), factory.realval(0.63661977236758134308)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_2_SQRTPI", hifFormat).c_str(), factory.realval(1.12837916709551257390)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_SQRT2", hifFormat).c_str(), factory.realval(1.41421356237309504880)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("M_SQRT1_2", hifFormat).c_str(), factory.realval(0.70710678118654752440)));

    ld->declarations.push_back(
        factory.constant(factory.real(), _makeHifName("P_Q", hifFormat).c_str(), factory.realval(1.602176462e-19)));
    ld->declarations.push_back(
        factory.constant(factory.real(), _makeHifName("P_C", hifFormat).c_str(), factory.realval(2.99792458e8)));
    ld->declarations.push_back(
        factory.constant(factory.real(), _makeHifName("P_K", hifFormat).c_str(), factory.realval(1.3806503e-23)));
    ld->declarations.push_back(
        factory.constant(factory.real(), _makeHifName("P_H", hifFormat).c_str(), factory.realval(6.62606876e-34)));
    ld->declarations.push_back(
        factory.constant(factory.real(), _makeHifName("P_EPS0", hifFormat).c_str(), factory.realval(8.854187817e-12)));
    ld->declarations.push_back(factory.constant(
        factory.real(), _makeHifName("P_U0", hifFormat).c_str(), factory.realval(4.0e-7 * 3.14159265358979323846)));
    ld->declarations.push_back(
        factory.constant(factory.real(), _makeHifName("P_CELSIUS0", hifFormat).c_str(), factory.realval(273.15)));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    return ld;
}

LibraryDef *VerilogSemantics::getVAMSDisciplinesPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("vams_disciplines", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::ams);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // natures

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Current", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Charge", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Charge", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Voltage", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(factory.viewRef(_makeHifName("Flux", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Flux", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));
    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Magneto_Motive_Force", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));
    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Temperature", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Power", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Position", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Velocity", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Acceleration", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Position", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Acceleration", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Impulse", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));
    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Impulse", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Acceleration", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));
    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Force", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Angle", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));
    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Angular_Velocity", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Angle", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Angular_Acceleration", hifFormat).c_str(), "ams_nature"), "ddt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Angular_Acceleration", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"), "idt_nature"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("Angular_Force", hifFormat).c_str(),
        factory.view(
            "ams_nature", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.string(), "units"), factory.variableDecl(factory.string(), "access"),
             factory.variableDecl(factory.real(), "abstol")),
            factory.noLibraries(), factory.noTemplates(), true)));
    // disciplines

    // logic discipline has discrete domain => translated as bit (logic, resolved) type.
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("ams_logic", hifFormat).c_str(), factory.bit(true, true, false), false));
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("ams_ddiscrete", hifFormat).c_str(), factory.bit(true, true, false), false));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("electrical", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                 factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature")))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("voltage", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"), "potential")),
            factory.noLibraries(), factory.noTemplates(), true,
            factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("current", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("magnetic", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                 factory.viewRef(_makeHifName("Magneto_Motive_Force", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(factory.viewRef(_makeHifName("Flux", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Magneto_Motive_Force", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Flux", hifFormat).c_str(), "ams_nature")))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("thermal", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                 factory.viewRef(_makeHifName("Temperature", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(factory.viewRef(_makeHifName("Power", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Temperature", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Power", hifFormat).c_str(), "ams_nature")))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("kinematic", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                 factory.viewRef(_makeHifName("Position", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Position", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature")))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("kinematic_v", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                 factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature")))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("rotational", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(factory.viewRef(_makeHifName("Angle", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Angle", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature")))));

    ld->declarations.push_back(factory.designUnit(
        _makeHifName("rotational_omega", hifFormat).c_str(),
        factory.view(
            "ams_discipline", nullptr, nullptr, hif::rtl,
            (factory.variableDecl(
                 factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"), "potential"),
             factory.variableDecl(
                 factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature"), "flow")),
            factory.noLibraries(), factory.noTemplates(), true,
            (factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"),
             factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature")))));

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // AMS 4.5 signal access functions
    // Access method are implemented for each nature, and overloaded for each
    // discipline containing that nature --> actually, exploiting template

    // I access. Defined for Current.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("I", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("I", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Current", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Q access. Defined for Charge.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Q", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Charge", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Q", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Charge", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Charge", hifFormat).c_str(), "ams_nature"), "param2"))));

    // V access. Defined for Voltage.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("V", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("V", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Voltage", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Phi access. Defined for Flux.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Phi", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Flux", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Phi", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Flux", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Flux", hifFormat).c_str(), "ams_nature"), "param2"))));

    // MMF access. Defined for Magneto_Motive_Force.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("MMF", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
            factory.viewRef(_makeHifName("Magneto_Motive_Force", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("MMF", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
             factory.viewRef(_makeHifName("Magneto_Motive_Force", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(
             factory.viewRef(_makeHifName("Magneto_Motive_Force", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Temp access. Defined for Temperature.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Temp", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Temperature", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Temp", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Temperature", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Temperature", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Pwr access. Defined for Power.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Pwr", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Power", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Pwr", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Power", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Power", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Pos access. Defined for Position.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Pos", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Position", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Pos", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Position", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Position", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Vel access. Defined for Velocity.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Vel", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Vel", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Velocity", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Acc access. Defined for Acceleration.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Acc", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Acceleration", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Acc", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Acceleration", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Acceleration", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Imp access. Defined for Impulse.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Imp", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Impulse", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Imp", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Impulse", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Impulse", hifFormat).c_str(), "ams_nature"), "param2"))));

    // F access. Defined for Force.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("F", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("F", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Force", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Theta access. Defined for Angle.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Theta", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Angle", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Theta", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Angle", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(factory.viewRef(_makeHifName("Angle", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Omega access. Defined for Angular_Velocity.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Omega", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
            factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Omega", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
             factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(
             factory.viewRef(_makeHifName("Angular_Velocity", hifFormat).c_str(), "ams_nature"), "param2"))));
    // Alpha defined for Angular_Acceleration
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Alpha", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
            factory.viewRef(_makeHifName("Angular_Acceleration", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Alpha", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
             factory.viewRef(_makeHifName("Angular_Acceleration", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(
             factory.viewRef(_makeHifName("Angular_Acceleration", hifFormat).c_str(), "ams_nature"), "param2"))));

    // Tau access. Defined for Angular_Force.
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Tau", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(
            factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature"), "param1"))));

    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("Tau", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature"), "param1"),
         factory.parameter(
             factory.viewRef(_makeHifName("Angular_Force", hifFormat).c_str(), "ams_nature"), "param2"))));

    return ld;
}

LibraryDef *VerilogSemantics::getVAMSDriverAccessPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("vams_driver_access", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::ams);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Type *integerBV = nullptr;
    if (hifFormat)
        integerBV = factory.integer();
    else
        integerBV = factory.bitvector(factory.range(31, 0), true, true, false, true);

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    ld->declarations.push_back(factory.constant(hif::copy(integerBV), "DRIVER_ACCESS_VAMS", factory.intval(1)));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_UNKNOWN",
        factory.bitvectorval("00000000000000000000000000000000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_DELAYED",
        factory.bitvectorval("00000000000000000000000000000001")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_GATE",
        factory.bitvectorval("00000000000000000000000000000010")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_UDP",
        factory.bitvectorval("00000000000000000000000000000100")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_ASSIGN",
        factory.bitvectorval("00000000000000000000000000001000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_BEHAVIORAL",
        factory.bitvectorval("00000000000000000000000000010000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_SDF",
        factory.bitvectorval("00000000000000000000000000100000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_NODELETE",
        factory.bitvectorval("00000000000000000000000001000000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_NOPREEMPT",
        factory.bitvectorval("00000000000000000000000010000000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_KERNEL",
        factory.bitvectorval("00000000000000000000000100000000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_WOR",
        factory.bitvectorval("00000000000000000000001000000000")));
    ld->declarations.push_back(factory.constant(
        factory.bitvector(new Range(31, 0), false, false, true), "DRIVER_WAND",
        factory.bitvectorval("00000000000000000000010000000000")));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////
    delete integerBV;
    return ld;
}

LibraryDef *VerilogSemantics::getStandardLibrary(const std::string &n)
{
    if (n == "standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getStandardPackage(false);
        return lStandard;
    } else if (n == "vams_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getVAMSStandardPackage(false);
        return lStandard;
    } else if (n == "vams_constants") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getVAMSConstantsPackage(false);
        return lStandard;
    } else if (n == "vams_disciplines") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getVAMSDisciplinesPackage(false);
        return lStandard;
    } else if (n == "vams_driver_access") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getVAMSDriverAccessPackage(false);
        return lStandard;
    }

    return nullptr;
}

bool VerilogSemantics::isNativeLibrary(const std::string &n, bool hifFormat)
{
    if (n == _makeHifName("standard", hifFormat) || n == _makeHifName("vams_standard", hifFormat) ||
        n == _makeHifName("vams_constants", hifFormat) || n == _makeHifName("vams_discipline", hifFormat) ||
        n == _makeHifName("vams_driver_access", hifFormat))
        return true;

    return false;
}

void VerilogSemantics::addStandardPackages(System *s)
{
    ILanguageSemantics::addStandardPackages(s);

    // Add verilog standard library
    LibraryDef *ld = getStandardLibrary("standard");
    s->libraryDefs.push_front(ld);

    Library *lib = new Library();
    lib->setName("standard");
    lib->setSystem(true);
    s->libraries.push_front(lib);
}

VerilogSemantics::MapCases VerilogSemantics::mapStandardSymbol(
    Declaration *decl,
    KeySymbol &key,
    ValueSymbol &value,
    ILanguageSemantics * /*srcSem*/)
{
    std::string libName;
    const bool isMine = _isHifPrefixed(key.first, libName);
    // messageAssert(isMine, "Asked name not prefixed with 'hif_': " + unprefixed, nullptr, nullptr);
    if (isMine) {
        std::string symName;
        const bool ok = _isHifPrefixed(key.second, symName);
        messageAssert(ok, "Found not prefixed symbol in prefixed library", decl, this);

        value.libraries.clear();
        value.libraries.push_back(libName);
        value.mappedSymbol   = symName;
        // Libraries are always replaced, therefore internal symbols must be kept
        const bool isLibrary = (libName == symName);
        value.mapAction      = isLibrary ? MAP_DELETE : MAP_KEEP;
        return value.mapAction;
    }

    StandardSymbols::iterator it(_standardSymbols.find(key));
    if (it == _standardSymbols.end())
        return ILanguageSemantics::UNKNOWN;

    value = it->second;
    return value.mapAction;
}

Object *VerilogSemantics::getSimplifiedSymbol(KeySymbol & /*key*/, Object *s)
{
    messageError("Not implemented yet", s, this);
}

bool VerilogSemantics::isStandardInclusion(const std::string &n, const bool /*isLibInclusion*/)
{
    if (n == "standard")
        return true;

    return false;
}

std::string VerilogSemantics::getEventMethodName(const bool /*hifFormat*/)
{
    // has no event method
    return NameTable::getInstance()->none();
}

bool VerilogSemantics::isEventCall(FunctionCall * /*call*/)
{
    // has no event method
    return false;
}

} // namespace semantics
} // namespace hif
