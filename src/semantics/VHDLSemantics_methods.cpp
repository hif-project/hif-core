/// @file VHDLSemantics_methods.cpp
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
#include "hif/semantics/VHDLSemantics.hpp"

namespace hif
{
namespace semantics
{

LibraryDef *VHDLSemantics::getStandardPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("standard", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // BASE
    SubProgram *base = _makeAttribute("base", nullptr, nullptr, nullptr, false, hifFormat);
    base->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(base);

    // LEFT + LEFT(X) int + real + T
    ld->declarations.push_back(
        _makeAttribute("left", factory.integer(), factory.integer(), factory.intval(1), false, hifFormat));
    SubProgram *left = _makeAttribute("left", nullptr, nullptr, nullptr, false, hifFormat);
    left->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(left);

    // RIGHT + RIGHT(X) int + real + T
    ld->declarations.push_back(
        _makeAttribute("right", factory.integer(), factory.integer(), factory.intval(1), false, hifFormat));
    SubProgram *right = _makeAttribute("right", nullptr, nullptr, nullptr, false, hifFormat);
    right->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(right);

    // HIGH + HIGH(X) int + real + T
    ld->declarations.push_back(
        _makeAttribute("high", factory.integer(), factory.integer(), factory.intval(1), false, hifFormat));
    SubProgram *high = _makeAttribute("high", nullptr, nullptr, nullptr, false, hifFormat);
    high->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(high);

    // LOW + LOW(X) int + real + T
    ld->declarations.push_back(
        _makeAttribute("low", factory.integer(), factory.integer(), factory.intval(1), false, hifFormat));
    SubProgram *low = _makeAttribute("low", nullptr, nullptr, nullptr, false, hifFormat);
    low->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(low);

    // ASCENDING + ASCENDING(X)
    ld->declarations.push_back(
        _makeAttribute("ascending", factory.boolean(), factory.integer(), factory.intval(1), false, hifFormat));

    // IMAGE(X)
    ld->declarations.push_back(
        _makeAttribute("image", factory.string(), factory.typeRef("T"), nullptr, true, hifFormat));

    // VALUE(X)
    ld->declarations.push_back(_makeAttribute("value", nullptr, factory.string(), nullptr, true, hifFormat));

    // POS(X)
    ld->declarations.push_back(
        _makeAttribute("pos", factory.integer(), factory.typeRef("T"), nullptr, true, hifFormat));

    // VAL(X)
    ld->declarations.push_back(_makeAttribute("val", nullptr, factory.integer(), nullptr, true, hifFormat));

    // SUCC(X)
    ld->declarations.push_back(_makeAttribute("succ", nullptr, factory.typeRef("T"), nullptr, true, hifFormat));

    // PRED(X)
    ld->declarations.push_back(_makeAttribute("pred", nullptr, factory.typeRef("T"), nullptr, true, hifFormat));

    // LEFTOF(X)
    ld->declarations.push_back(_makeAttribute("leftof", nullptr, factory.typeRef("T"), nullptr, true, hifFormat));

    // RIGHTOF(X)
    ld->declarations.push_back(_makeAttribute("rightof", nullptr, factory.typeRef("T"), nullptr, true, hifFormat));

    // RANGE(X)
    ld->declarations.push_back(_makeAttribute("range", nullptr, factory.integer(), factory.intval(1), true, hifFormat));

    // REVERSE_RANGE(X)
    ld->declarations.push_back(
        _makeAttribute("reverse_range", nullptr, factory.integer(), factory.intval(1), true, hifFormat));

    // LENGTH(X)
    ld->declarations.push_back(
        _makeAttribute("length", factory.integer(), factory.integer(), factory.intval(1), false, hifFormat));

    // DELAYED(X)
    ld->declarations.push_back(
        _makeAttribute("delayed", nullptr, factory.time(), factory.timeval(0.0), true, hifFormat));

    // STABLE
    ld->declarations.push_back(
        _makeAttribute("stable", factory.boolean(), factory.time(), factory.timeval(0.0), false, hifFormat));

    // QUIET
    ld->declarations.push_back(
        _makeAttribute("quiet", factory.boolean(), factory.time(), factory.timeval(0.0), true, hifFormat));

    // TRANSACTION
    ld->declarations.push_back(_makeAttribute("transaction", factory.bit(), nullptr, nullptr, true, hifFormat));

    // EVENT
    ld->declarations.push_back(_makeAttribute("event", factory.boolean(), nullptr, nullptr, false, hifFormat));

    // ACTIVE
    ld->declarations.push_back(_makeAttribute("active", factory.boolean(), nullptr, nullptr, true, hifFormat));

    // LAST_EVENT
    ld->declarations.push_back(_makeAttribute("last_event", factory.time(), nullptr, nullptr, true, hifFormat));

    // LAST_ACTIVE
    ld->declarations.push_back(_makeAttribute("last_active", factory.time(), nullptr, nullptr, true, hifFormat));

    // LAST_VALUE
    SubProgram *lastValue = _makeAttribute("last_value", nullptr, nullptr, nullptr, false, hifFormat);
    lastValue->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(lastValue);

    // DRIVING
    ld->declarations.push_back(_makeAttribute("driving", factory.boolean(), nullptr, nullptr, true, hifFormat));

    // DRIVING_VALUE
    SubProgram *drivingValue = _makeAttribute("driving_value", nullptr, nullptr, nullptr, true, hifFormat);
    drivingValue->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(drivingValue);

    // SIMPLE_NAME
    ld->declarations.push_back(_makeAttribute("simple_name", factory.string(), nullptr, nullptr, true, hifFormat));

    // INSTANCE_NAME
    ld->declarations.push_back(_makeAttribute("instance_name", factory.string(), nullptr, nullptr, true, hifFormat));

    // PATH_NAME
    ld->declarations.push_back(_makeAttribute("path_name", factory.string(), nullptr, nullptr, true, hifFormat));

    // FOREIGN
    ld->declarations.push_back(_makeAttribute("foreign", factory.string(), nullptr, nullptr, true, hifFormat));
    // ///////////////////////////////////////////////////////////////////
    // Types
    // ///////////////////////////////////////////////////////////////////

    // SEVERITY_LEVEL
    {
        const char *values[] = {"note", "warning", "error", "failure"};
        ld->declarations.push_back(_makeEnum("severity_level", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // time_units
    {
        const char *values[] = {"fs", "ps", "ns", "us", "ms", "sec", "min", "hr"};
        ld->declarations.push_back(_makeEnum("time_units", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // DELAY_LENGTH
    {
        ld->declarations.push_back(factory.typeDef(
            _makeHifName("delay_length", hifFormat).c_str(), factory.time(), false, factory.range(0LL, 2147483647LL)));
    }

    // FILE_OPEN_KIND
    {
        const char *values[] = {"read_mode", "write_mode", "append_mode"};
        ld->declarations.push_back(_makeEnum("file_open_kind", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // FILE_OPEN_STATUS
    {
        const char *values[] = {"open_ok", "status_error", "name_error", "mode_error"};
        ld->declarations.push_back(_makeEnum("file_open_status", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // ///////////////////////////////////////////////////////////////////
    // Methods
    // ///////////////////////////////////////////////////////////////////

    // void ASSERT(bool CONDITION, string REPORT = "", severity_level LEVEL = NOTE)
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("assert", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.boolean(), "condition", nullptr),
         factory.parameter(factory.string(), "report", factory.stringval("")),
         factory.parameter(
             factory.typeRef(_makeHifName("severity_level", hifFormat).c_str()), "severity",
             factory.identifier(_makeHifName("error", hifFormat).c_str())))));

    // delay_length NOW()
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("delay_length", hifFormat).c_str()), _makeHifName("now", hifFormat).c_str(),
        factory.noTemplates(), factory.noParameters()));
    // function hif_castRealToInt(X: real, size: int, sign: bool)
    {
        SubProgram *f = _makeTernaryAttribute(
            "castRealToInt", factory.integer(factory.range(63, 0)), factory.real(), factory.noValue(),
            factory.integer(), factory.noValue(), factory.boolean(), factory.noValue(), false, hifFormat);

        //        // build body
        //        StateTable * st = new StateTable();
        //        st->setName(f->getName());
        //        st->declarations.push_back(factory.variable(factory.real(), "d", factory.realval(0.0)));
        //        st->declarations.push_back(factory.variable(factory.real(), "min", factory.realval(0.0)));
        //        st->declarations.push_back(factory.variable(factory.real(), "max", factory.realval(0.0)));
        //        f->setStateTable(st);
        //
        //        State * state = new State();
        //        state->setName(f->getName());
        //        st->states.push_back(state);
        //
        //        // TODO: to implement

        ld->declarations.push_back(f);
    }

    // procedure DEALLOCATE (P: inout AT) ;
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("deallocate", hifFormat).c_str(), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.pointer(factory.typeRef("T")), "param1"))));

    // T * new(T)
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.typeRef("T")), _makeHifName("new", hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T")), (factory.parameter(factory.typeRef("T"), "param1"))));

    // T * new()
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.typeRef("T")), _makeHifName("new", hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T")), factory.noParameters()));

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeStdLogic1164Package(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_1164", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Bit *bit                     = factory.bit();
    Bit *std_ulogic              = factory.bit(true, false);
    Bit *std_logic               = factory.bit(true, true);
    Bitvector *bitvector         = factory.bitvector();
    Bitvector *std_ulogic_vector = factory.bitvector(nullptr, true, false);
    Bitvector *std_logic_vector  = factory.bitvector(nullptr, true, true);

    Range *retSpan                   = factory.range(new Identifier("left1"), dir_downto, new Identifier("right1"));
    Bitvector *ret_bitvector         = factory.bitvector(retSpan);
    Bitvector *ret_std_ulogic_vector = factory.bitvector(hif::copy(retSpan), true, false);
    Bitvector *ret_std_logic_vector  = factory.bitvector(hif::copy(retSpan), true, true);

    TypeReference *x01   = factory.typeRef(_makeHifName("x01", hifFormat).c_str());
    TypeReference *x01z  = factory.typeRef(_makeHifName("x01z", hifFormat).c_str());
    TypeReference *ux01  = factory.typeRef(_makeHifName("ux01", hifFormat).c_str());
    TypeReference *ux01z = factory.typeRef(_makeHifName("ux01z", hifFormat).c_str());

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // SUBTYPE X01
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("x01", hifFormat).c_str(), factory.bit(true, true, false), false, nullptr));

    // SUBTYPE X01Z
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("x01z", hifFormat).c_str(), factory.bit(true, true, false), false, nullptr));

    // SUBTYPE UX01
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("ux01", hifFormat).c_str(), factory.bit(true, true, false), false, nullptr));

    // SUBTYPE UX01Z
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("ux01z", hifFormat).c_str(), factory.bit(true, true, false), false, nullptr));

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // FUNCTION resolved ( s : std_ulogic_vector ) RETURN std_ulogic;
    ld->declarations.push_back(_makeAttribute(
        "resolved", hif::copy(std_ulogic), hif::copy(std_ulogic_vector), factory.noValue(), false, hifFormat));

    // FUNCTION To_bit ( s : std_ulogic ; xmap : BIT := '0' ) RETURN BIT;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_bit", hif::copy(bit), hif::copy(std_ulogic), factory.noValue(), hif::copy(bit),
        factory.bitval(bit_zero, hif::copy(bit)), false, hifFormat));

    // FUNCTION To_bitvector ( s : std_logic_vector ; xmap : BIT := '0' ) RETURN BIT_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_bitvector", hif::copy(ret_bitvector), hif::copy(std_logic_vector), factory.noValue(), hif::copy(bit),
        factory.bitval(bit_zero, hif::copy(bit)), false, hifFormat));
    // FUNCTION To_bitvector ( s : std_ulogic_vector ; xmap : BIT := '0'  ) RETURN BIT_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_bitvector", hif::copy(ret_bitvector), hif::copy(std_ulogic_vector), factory.noValue(), hif::copy(bit),
        factory.bitval(bit_zero, hif::copy(bit)), false, hifFormat));

    // FUNCTION To_StdULogic       ( b : BIT               ) RETURN std_ulogic;
    ld->declarations.push_back(
        _makeAttribute("to_stdulogic", hif::copy(std_ulogic), hif::copy(bit), factory.noValue(), false, hifFormat));

    // FUNCTION To_StdLogicVector  ( b : BIT_VECTOR        ) RETURN std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_stdlogicvector", hif::copy(ret_std_logic_vector), hif::copy(bitvector), factory.noValue(), false,
        hifFormat));
    // FUNCTION To_StdLogicVector  ( s : std_ulogic_vector ) RETURN std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_stdlogicvector", hif::copy(ret_std_logic_vector), hif::copy(std_ulogic_vector), factory.noValue(), false,
        hifFormat));

    // FUNCTION To_StdULogicVector ( b : BIT_VECTOR        ) RETURN std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_stdulogicvector", hif::copy(ret_std_ulogic_vector), hif::copy(bitvector), factory.noValue(), false,
        hifFormat));
    // FUNCTION To_StdULogicVector ( s : std_logic_vector  ) RETURN std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_stdulogicvector", hif::copy(ret_std_ulogic_vector), hif::copy(std_logic_vector), factory.noValue(), false,
        hifFormat));

    // FUNCTION To_X01  ( s : std_logic_vector  ) RETURN  std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01  ( s : std_ulogic_vector ) RETURN  std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01", hif::copy(ret_std_ulogic_vector), hif::copy(std_ulogic_vector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01  ( s : std_ulogic        ) RETURN  X01;
    ld->declarations.push_back(
        _makeAttribute("to_x01", hif::copy(x01), hif::copy(std_ulogic), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01  ( b : BIT_VECTOR        ) RETURN  std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01", hif::copy(ret_std_logic_vector), hif::copy(bitvector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01  ( b : BIT_VECTOR        ) RETURN  std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01", hif::copy(ret_std_ulogic_vector), hif::copy(bitvector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01  ( b : BIT               ) RETURN  X01;
    ld->declarations.push_back(
        _makeAttribute("to_x01", hif::copy(x01), hif::copy(bit), factory.noValue(), false, hifFormat));

    // FUNCTION To_X01Z ( s : std_logic_vector  ) RETURN  std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01z", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01Z ( s : std_ulogic_vector ) RETURN  std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01z", hif::copy(ret_std_ulogic_vector), hif::copy(std_ulogic_vector), factory.noValue(), false,
        hifFormat));
    // FUNCTION To_X01Z ( s : std_ulogic        ) RETURN  X01Z;
    ld->declarations.push_back(
        _makeAttribute("to_x01z", hif::copy(x01z), hif::copy(std_ulogic), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01Z ( b : BIT_VECTOR        ) RETURN  std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01z", hif::copy(ret_std_logic_vector), hif::copy(bitvector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01Z ( b : BIT_VECTOR        ) RETURN  std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_x01z", hif::copy(ret_std_ulogic_vector), hif::copy(bitvector), factory.noValue(), false, hifFormat));
    // FUNCTION To_X01Z ( b : BIT               ) RETURN  X01Z;
    ld->declarations.push_back(
        _makeAttribute("to_x01z", hif::copy(x01z), hif::copy(bit), factory.noValue(), false, hifFormat));

    // FUNCTION To_UX01  ( s : std_logic_vector  ) RETURN  std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_ux01", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    // FUNCTION To_UX01  ( s : std_ulogic_vector ) RETURN  std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_ux01", hif::copy(ret_std_ulogic_vector), hif::copy(std_ulogic_vector), factory.noValue(), false,
        hifFormat));
    // FUNCTION To_UX01  ( s : std_ulogic        ) RETURN  UX01;
    ld->declarations.push_back(
        _makeAttribute("to_ux01", hif::copy(ux01), hif::copy(std_ulogic), factory.noValue(), false, hifFormat));
    // FUNCTION To_UX01  ( b : BIT_VECTOR        ) RETURN  std_logic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_ux01", hif::copy(ret_std_logic_vector), hif::copy(bitvector), factory.noValue(), false, hifFormat));
    // FUNCTION To_UX01  ( b : BIT_VECTOR        ) RETURN  std_ulogic_vector;
    ld->declarations.push_back(_makeAttribute(
        "to_ux01", hif::copy(ret_std_ulogic_vector), hif::copy(bitvector), factory.noValue(), false, hifFormat));
    // FUNCTION To_UX01  ( b : BIT               ) RETURN  UX01;
    ld->declarations.push_back(
        _makeAttribute("to_ux01", hif::copy(ux01), hif::copy(bit), factory.noValue(), false, hifFormat));

    // FUNCTION rising_edge  (SIGNAL s : std_ulogic) RETURN BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("rising_edge", factory.boolean(), hif::copy(std_ulogic), factory.noValue(), false, hifFormat));
    // FUNCTION falling_edge (SIGNAL s : std_ulogic) RETURN BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("falling_edge", factory.boolean(), hif::copy(std_ulogic), factory.noValue(), false, hifFormat));

    // FUNCTION Is_X ( s : std_ulogic_vector ) RETURN  BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("is_x", factory.boolean(), hif::copy(std_ulogic_vector), factory.noValue(), false, hifFormat));
    // FUNCTION Is_X ( s : std_logic_vector  ) RETURN  BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("is_x", factory.boolean(), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    // FUNCTION Is_X ( s : std_ulogic        ) RETURN  BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("is_x", factory.boolean(), hif::copy(std_ulogic), factory.noValue(), false, hifFormat));

    delete bit;
    delete std_ulogic;
    delete std_logic;
    delete bitvector;
    delete std_ulogic_vector;
    delete std_logic_vector;

    delete ret_bitvector;
    delete ret_std_ulogic_vector;
    delete ret_std_logic_vector;

    delete x01;
    delete x01z;
    delete ux01;
    delete ux01z;

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeStdLogicArithPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_arith", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Range *retTypeSpan = factory.range(
        factory.expression(new Identifier("param2"), op_minus, factory.intval(1)), dir_downto, new IntValue(0));
    Range *retTPSpan            = factory.range(new Identifier("left1"), dir_downto, new Identifier("right1"));
    Bit *std_ulogic             = factory.bit(true, false);
    Bitvector *std_logic_vector = factory.bitvector(hif::copy(retTypeSpan), true, true);
    Range *retSpanInc           = factory.range(
        factory.expression(
            factory.expression(
                factory.expression(
                    factory.expression(new Identifier("left1"), op_plus, new Identifier("left2")), op_minus,
                    new Identifier("right1")),
                op_minus, new Identifier("right2")),
            op_plus, factory.intval(1)),
        dir_downto, new IntValue(0));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////
    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // subtype SMALL_INT is INTEGER range 0 to 1;
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("small_int", hifFormat).c_str(), factory.integer(), false, nullptr));

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function SHL(ARG: UNSIGNED; COUNT: UNSIGNED) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shl", factory.unsignedType(hif::copy(retTPSpan)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    // function SHL(ARG: SIGNED; COUNT: UNSIGNED) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shl", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    // function SHR(ARG: UNSIGNED; COUNT: UNSIGNED) return UNSIGNED;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "shr", factory.unsignedType(hif::copy(retTPSpan)), factory.unsignedType(), factory.noValue(),
            factory.unsignedType(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);
        ld->declarations.push_back(getSuffixedCopy(sp, "_unsigned"));
    }
    // function SHR(ARG: SIGNED; COUNT: UNSIGNED) return SIGNED;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "shr", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(),
            factory.unsignedType(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);
        ld->declarations.push_back(getSuffixedCopy(sp, "_signed"));
    }

    // function CONV_INTEGER(ARG: INTEGER) return INTEGER;
    ld->declarations.push_back(
        _makeAttribute("conv_integer", factory.integer(), factory.integer(), factory.noValue(), false, hifFormat));

    // function CONV_INTEGER(ARG: UNSIGNED) return INTEGER;
    {
        SubProgram *sp = _makeAttribute(
            "conv_integer", factory.integer(), factory.unsignedType(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_unsigned = getSuffixedCopy(sp, "_unsigned");
        ld->declarations.push_back(sp_unsigned);
    }

    // function CONV_INTEGER(ARG: SIGNED) return INTEGER;
    {
        SubProgram *sp = _makeAttribute(
            "conv_integer", factory.integer(), factory.signedType(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_signed = getSuffixedCopy(sp, "_signed");
        ld->declarations.push_back(sp_signed);
    }

    // function CONV_INTEGER(ARG: STD_ULOGIC) return SMALL_INT;
    ld->declarations.push_back(_makeAttribute(
        "conv_integer", factory.typeRef(_makeHifName("small_int", hifFormat).c_str()), hif::copy(std_ulogic),
        factory.noValue(), false, hifFormat));

    // function CONV_UNSIGNED(ARG: INTEGER; SIZE: INTEGER) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "conv_unsigned", factory.unsignedType(hif::copy(retTypeSpan)), factory.integer(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function CONV_UNSIGNED(ARG: UNSIGNED; SIZE: INTEGER) return UNSIGNED;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "conv_unsigned", factory.unsignedType(hif::copy(retTypeSpan)), factory.unsignedType(), factory.noValue(),
            factory.integer(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_unsigned = getSuffixedCopy(sp, "_unsigned");
        ld->declarations.push_back(sp_unsigned);
    }

    // function CONV_UNSIGNED(ARG: SIGNED; SIZE: INTEGER) return UNSIGNED;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "conv_unsigned", factory.unsignedType(hif::copy(retTypeSpan)), factory.signedType(), factory.noValue(),
            factory.integer(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_signed = getSuffixedCopy(sp, "_signed");
        ld->declarations.push_back(sp_signed);
    }

    // function CONV_UNSIGNED(ARG: STD_ULOGIC; SIZE: INTEGER) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "conv_unsigned", factory.unsignedType(hif::copy(retTypeSpan)), hif::copy(std_ulogic), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function CONV_SIGNED(ARG: INTEGER; SIZE: INTEGER) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "conv_signed", factory.signedType(hif::copy(retTypeSpan)), factory.integer(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function CONV_SIGNED(ARG: UNSIGNED; SIZE: INTEGER) return SIGNED;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "conv_signed", factory.signedType(hif::copy(retTypeSpan)), factory.unsignedType(), factory.noValue(),
            factory.integer(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_unsigned = getSuffixedCopy(sp, "_unsigned");
        ld->declarations.push_back(sp_unsigned);
    }

    // function CONV_SIGNED(ARG: SIGNED; SIZE: INTEGER) return SIGNED;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "conv_signed", factory.signedType(hif::copy(retTypeSpan)), factory.signedType(), factory.noValue(),
            factory.integer(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_signed = getSuffixedCopy(sp, "_signed");
        ld->declarations.push_back(sp_signed);
    }

    // function CONV_SIGNED(ARG: STD_ULOGIC; SIZE: INTEGER) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "conv_signed", factory.signedType(hif::copy(retTypeSpan)), hif::copy(std_ulogic), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function CONV_STD_LOGIC_VECTOR(ARG: INTEGER; SIZE: INTEGER) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "conv_std_logic_vector", hif::copy(std_logic_vector), factory.integer(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));

    // function CONV_STD_LOGIC_VECTOR(ARG: UNSIGNED; SIZE: INTEGER) return STD_LOGIC_VECTOR;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "conv_std_logic_vector", hif::copy(std_logic_vector), factory.unsignedType(), factory.noValue(),
            factory.integer(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_unsigned = getSuffixedCopy(sp, "_unsigned");
        ld->declarations.push_back(sp_unsigned);
    }

    // function CONV_STD_LOGIC_VECTOR(ARG: SIGNED; SIZE: INTEGER) return STD_LOGIC_VECTOR;
    {
        SubProgram *sp = _makeBinaryAttribute(
            "conv_std_logic_vector", hif::copy(std_logic_vector), factory.signedType(), factory.noValue(),
            factory.integer(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_signed = getSuffixedCopy(sp, "_signed");
        ld->declarations.push_back(sp_signed);
    }

    // function CONV_STD_LOGIC_VECTOR(ARG: STD_ULOGIC; SIZE: INTEGER) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "conv_std_logic_vector", hif::copy(std_logic_vector), hif::copy(std_ulogic), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function EXT(ARG: STD_LOGIC_VECTOR; SIZE: INTEGER) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "ext", hif::copy(std_logic_vector), factory.bitvector(nullptr, true, true), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));
    // function SXT(ARG: STD_LOGIC_VECTOR; SIZE: INTEGER) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "sxt", hif::copy(std_logic_vector), factory.bitvector(nullptr, true, true), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // Overloaded operators
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_eq_signed", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_neq_signed", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_eq_unsigned", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_neq_unsigned", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));

    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_plus_signed", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(),
        factory.signedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_minus_signed", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(),
        factory.signedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_mult_signed", factory.signedType(hif::copy(retSpanInc)), factory.signedType(), factory.noValue(),
        factory.signedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_plus_unsigned", factory.unsignedType(hif::copy(retTPSpan)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_minus_unsigned", factory.unsignedType(hif::copy(retTPSpan)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_mult_unsigned", factory.unsignedType(hif::copy(retSpanInc)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));

    ld->declarations.push_back(_makeAttribute(
        "_op_abs", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(), false,
        hifFormat));

    delete std_ulogic;
    delete std_logic_vector;
    delete retTypeSpan;
    delete retTPSpan;
    delete retSpanInc;

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeStdLogicArithExPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_arith_ex", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Range *tpRange                   = new Range(new Identifier("left"), new Identifier("right"), dir_downto);
    Bitvector *ret_std_logic_vector  = factory.bitvector(hif::copy(tpRange), true, true);
    Bitvector *ret_std_ulogic_vector = factory.bitvector(hif::copy(tpRange), true, false);

    Range *sizeRange =
        new Range(factory.expression(new Identifier("size"), op_minus, factory.intval(1)), new IntValue(0), dir_downto);
    Bitvector *size_std_logic_vector  = factory.bitvector(hif::copy(sizeRange), true, true);
    Bitvector *size_std_ulogic_vector = factory.bitvector(hif::copy(sizeRange), true, false);

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // type tbl_type is array (STD_ULOGIC) of STD_ULOGIC;
    ld->declarations.push_back(factory.typeDef(
        _makeHifName("tbl_type", hifFormat).c_str(),
        factory.bitvector(new Range(8, 0), true, false) // trick to simplify tbl_binary constant initial value
        ));

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // constant tbl_BINARY : tbl_type := ('X', 'X', '0', '1', 'X', 'X', '0', '1', 'X');
    ld->declarations.push_back(factory.constant(
        factory.typeRef(_makeHifName("tbl_type", hifFormat).c_str()), _makeHifName("tbl_binary", hifFormat).c_str(),
        factory.bitvectorval("XX01XX01X")));

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function cond_op (cond : boolean; left_val, right_val: STD_LOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_logic_vector), _makeHifName("cond_op", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.boolean(), "param1"), factory.parameter(hif::copy(ret_std_logic_vector), "param2"),
         factory.parameter(hif::copy(ret_std_logic_vector), "param3"))));
    // function cond_op (cond : boolean; left_val, right_val: STD_ULOGIC) return STD_ULOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, false), _makeHifName("cond_op", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.boolean(), "param1"), factory.parameter(factory.bit(true, false), "param2"),
         factory.parameter(factory.bit(true, false), "param3"))));
    // function cond_op (cond : boolean; left_val, right_val: STD_ULOGIC_VECTOR) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_ulogic_vector), _makeHifName("cond_op", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.boolean(), "param1"), factory.parameter(hif::copy(ret_std_ulogic_vector), "param2"),
         factory.parameter(hif::copy(ret_std_ulogic_vector), "param3"))));

    // function sh_left(op1: STD_LOGIC_VECTOR; op2: NATURAL) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_logic_vector), _makeHifName("sh_left", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_logic_vector), "param1"),
         factory.parameter(factory.integer(), "param2"))));
    // function sh_right(op1: STD_LOGIC_VECTOR; op2: NATURAL) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_logic_vector), _makeHifName("sh_right", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_logic_vector), "param1"),
         factory.parameter(factory.integer(), "param2"))));
    // function sh_left(op1: STD_ULOGIC_VECTOR; op2: NATURAL) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_ulogic_vector), _makeHifName("sh_left", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_ulogic_vector), "param1"),
         factory.parameter(factory.integer(), "param2"))));
    // function sh_right(op1: STD_ULOGIC_VECTOR; op2: NATURAL) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_ulogic_vector), _makeHifName("sh_right", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_ulogic_vector), "param1"),
         factory.parameter(factory.integer(), "param2"))));

    // function align_size (oper : STD_LOGIC_VECTOR; size : NATURAL) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(size_std_logic_vector), _makeHifName("align_size", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_logic_vector), "param1"), factory.parameter(factory.integer(), "size"))));
    // function align_size (oper : STD_ULOGIC; size : NATURAL) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(size_std_logic_vector), _makeHifName("align_size", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bit(true, false), "param1"), factory.parameter(factory.integer(), "size"))));

    // function align_size (oper : STD_ULOGIC_VECTOR; size : NATURAL) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(size_std_ulogic_vector), _makeHifName("align_size", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_ulogic_vector), "param1"), factory.parameter(factory.integer(), "size"))));
    // function align_size (oper : STD_ULOGIC; size : NATURAL) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(size_std_ulogic_vector), _makeHifName("align_size", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bit(true, false), "param1"), factory.parameter(factory.integer(), "size"))));
    // function to_integer (oper : STD_LOGIC_VECTOR) return INTEGER;
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("to_integer", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_logic_vector), "param1"))));
    // function to_integer (oper: STD_ULOGIC)         return INTEGER;
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("to_integer", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"))));
    // function to_integer (oper : STD_ULOGIC_VECTOR) return INTEGER;
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("to_integer", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_ulogic_vector), "param1"))));

    // function To_StdLogicVector (oper: INTEGER; length: NATURAL) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(size_std_logic_vector), _makeHifName("to_stdlogicvector", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.integer(), "param1"), factory.parameter(factory.integer(), "size"))));
    // function To_StdUlogicVector (oper: INTEGER; length: NATURAL) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(size_std_ulogic_vector), _makeHifName("to_stdulogicvector", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.integer(), "param1"), factory.parameter(factory.integer(), "size"))));

    // function drive (V: STD_LOGIC_VECTOR)  return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_ulogic_vector), _makeHifName("drive", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_logic_vector), "param1"))));
    // function drive (V: STD_ULOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_logic_vector), _makeHifName("drive", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_ulogic_vector), "param1"))));

    // function Sense (V: STD_ULOGIC; vZ, vU, vDC: STD_ULOGIC) return STD_ULOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, false), _makeHifName("sense", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"), factory.parameter(factory.bit(true, false), "param2"),
         factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function Sense (V: STD_ULOGIC_VECTOR; vZ, vU, vDC: STD_ULOGIC) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_ulogic_vector), _makeHifName("sense", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_ulogic_vector), "param1"),
         factory.parameter(factory.bit(true, false), "param2"), factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function Sense (V: STD_LOGIC_VECTOR; vZ, vU, vDC: STD_ULOGIC) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret_std_ulogic_vector), _makeHifName("sense", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(hif::copy(ret_std_logic_vector), "param1"),
         factory.parameter(factory.bit(true, false), "param2"), factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    delete tpRange;
    delete ret_std_logic_vector;
    delete ret_std_ulogic_vector;

    delete sizeRange;
    delete size_std_logic_vector;
    delete size_std_ulogic_vector;

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeNumericStdPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_numeric_std", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // span build watching param 2 value
    Range *retTypeSpan = factory.range(
        factory.expression(new Identifier("param2"), op_minus, factory.intval(1)), dir_downto, new IntValue(0));

    // types
    Bit *std_ulogic             = factory.bit(true, false);
    Bitvector *std_logic_vector = factory.bitvector(nullptr, true, true);

    // return types
    Range *retSpan                  = factory.range(new Identifier("left1"), dir_downto, new Identifier("right1"));
    Bitvector *ret_std_logic_vector = factory.bitvector(hif::copy(retSpan), true, true);
    Signed *ret_signed              = factory.signedType(hif::copy(retSpan));
    Unsigned *ret_unsigned          = factory.unsignedType(hif::copy(retSpan));

    Range *retTPSpan  = factory.range(new Identifier("left1"), dir_downto, new Identifier("right1"));
    Range *retSpanInc = factory.range(
        factory.expression(
            factory.expression(
                factory.expression(
                    factory.expression(new Identifier("left1"), op_plus, new Identifier("left2")), op_minus,
                    new Identifier("right1")),
                op_minus, new Identifier("right2")),
            op_plus, factory.intval(1)),
        dir_downto, new IntValue(0));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function "abs" ( X : SIGNED) return SIGNED;
    ld->declarations.push_back(
        _makeAttribute("_op_abs", hif::copy(ret_signed), factory.signedType(), factory.noValue(), false, hifFormat));

    //function shift_left  (  ARG: UNSIGNED; COUNT: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shift_left", hif::copy(ret_unsigned), factory.unsignedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));
    //function shift_right (  ARG: UNSIGNED; COUNT: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shift_right", hif::copy(ret_unsigned), factory.unsignedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));
    //function shift_left  (  ARG: SIGNED; COUNT: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shift_left", hif::copy(ret_signed), factory.signedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));
    //function shift_right (  ARG: SIGNED; COUNT: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shift_right", hif::copy(ret_signed), factory.signedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));

    //function rotate_left  (  ARG: UNSIGNED; COUNT: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "rotate_left", hif::copy(ret_unsigned), factory.unsignedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));
    //function rotate_right (  ARG: UNSIGNED; COUNT: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "rotate_right", hif::copy(ret_unsigned), factory.unsignedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));
    //function rotate_left  (  ARG: SIGNED; COUNT: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "rotate_left", hif::copy(ret_signed), factory.signedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));
    //function rotate_right (  ARG: SIGNED; COUNT: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "rotate_right", hif::copy(ret_signed), factory.signedType(), factory.noValue(), factory.integer(),
        factory.noValue(), false, hifFormat));

    // function RESIZE ( ARG: SIGNED; NEW_SIZE: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "resize", factory.signedType(hif::copy(retTypeSpan)), factory.signedType(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));
    // function RESIZE ( ARG: UNSIGNED; NEW_SIZE: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "resize", factory.unsignedType(hif::copy(retTypeSpan)), factory.unsignedType(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function TO_INTEGER ( ARG: UNSIGNED) return NATURAL;
    {
        SubProgram *sp = _makeAttribute(
            "to_integer", factory.integer(), factory.unsignedType(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_unsigned = hif::copy(sp);
        sp_unsigned->setName(_makeHifName("to_integer_unsigned", hifFormat));
        ld->declarations.push_back(sp_unsigned);
    }
    // function TO_INTEGER ( ARG: SIGNED) return INTEGER;
    {
        SubProgram *sp =
            _makeAttribute("to_integer", factory.integer(), factory.signedType(), factory.noValue(), false, hifFormat);
        ld->declarations.push_back(sp);

        SubProgram *sp_signed = hif::copy(sp);
        sp_signed->setName(_makeHifName("to_integer_signed", hifFormat));
        ld->declarations.push_back(sp_signed);
    }

    // function TO_UNSIGNED ( ARG INTEGER; SIZE: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_unsigned", factory.unsignedType(hif::copy(retTypeSpan)), factory.integer(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));
    // function TO_SIGNED ( ARG: INTEGER; SIZE: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_signed", factory.signedType(hif::copy(retTypeSpan)), factory.integer(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function TO_UNSIGNED ( ARG: STD_LOGIC_VECTOR) return UNSIGNED;
    ld->declarations.push_back(_makeAttribute(
        "to_unsigned", hif::copy(ret_unsigned), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    // function TO_SIGNED ( ARG: STD_LOGIC_VECTOR) return SIGNED;
    ld->declarations.push_back(_makeAttribute(
        "to_signed", hif::copy(ret_signed), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // function TO_STDLOGICVECTOR ( ARG: UNSIGNED) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeAttribute(
        "to_signed", hif::copy(ret_std_logic_vector), factory.unsignedType(), factory.noValue(), false, hifFormat));
    // function TO_STDLOGICVECTOR ( ARG: SIGNED) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeAttribute(
        "to_signed", hif::copy(ret_std_logic_vector), factory.signedType(), factory.noValue(), false, hifFormat));

    // function STD_MATCH (L, R: STD_ULOGIC) return BOOLEAN;
    ld->declarations.push_back(_makeBinaryAttribute(
        "std_match", factory.boolean(), hif::copy(std_ulogic), factory.noValue(), hif::copy(std_ulogic),
        factory.noValue(), false, hifFormat));
    // function STD_MATCH (L, R: STD_LOGIC_VECTOR) return BOOLEAN;
    ld->declarations.push_back(_makeBinaryAttribute(
        "std_match", factory.boolean(), hif::copy(std_logic_vector), factory.noValue(), hif::copy(std_logic_vector),
        factory.noValue(), false, hifFormat));
    // Overloaded operators
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_eq_signed", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_neq_signed", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_eq_unsigned", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_neq_unsigned", factory.boolean(), factory.unsignedType(), factory.noValue(), factory.unsignedType(),
        factory.noValue(), false, hifFormat));

    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_plus_signed", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(),
        factory.signedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_minus_signed", factory.signedType(hif::copy(retTPSpan)), factory.signedType(), factory.noValue(),
        factory.signedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_mult_signed", factory.signedType(hif::copy(retSpanInc)), factory.signedType(), factory.noValue(),
        factory.signedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_plus_unsigned", factory.unsignedType(hif::copy(retTPSpan)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_minus_unsigned", factory.unsignedType(hif::copy(retTPSpan)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_mult_unsigned", factory.unsignedType(hif::copy(retSpanInc)), factory.unsignedType(), factory.noValue(),
        factory.unsignedType(), factory.noValue(), false, hifFormat));
    delete retTypeSpan;
    delete retSpan;
    delete std_ulogic;
    delete std_logic_vector;
    delete retTPSpan;
    delete retSpanInc;

    delete ret_std_logic_vector;
    delete ret_signed;
    delete ret_unsigned;

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeNumericBitPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_numeric_bit", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // span build watching param 2 value
    Range *retTypeSpan = factory.range(
        factory.expression(new Identifier("param2"), op_minus, factory.intval(1)), dir_downto, new IntValue(0));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////
    // function RESIZE ( ARG: SIGNED; NEW_SIZE: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "resize", factory.signedType(hif::copy(retTypeSpan)), factory.signedType(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));
    // function RESIZE ( ARG: UNSIGNED; NEW_SIZE: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "resize", factory.unsignedType(hif::copy(retTypeSpan)), factory.signedType(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function TO_INTEGER ( ARG: UNSIGNED) return NATURAL;
    ld->declarations.push_back(
        _makeAttribute("to_integer", factory.integer(), factory.unsignedType(), factory.noValue(), false, hifFormat));
    // function TO_INTEGER ( ARG: SIGNED) return INTEGER;
    ld->declarations.push_back(
        _makeAttribute("to_integer", factory.integer(), factory.signedType(), factory.noValue(), false, hifFormat));

    // function TO_UNSIGNED ( ARG INTEGER; SIZE: NATURAL) return UNSIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_unsigned", factory.unsignedType(hif::copy(retTypeSpan)), factory.integer(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));
    // function TO_SIGNED ( ARG: INTEGER; SIZE: NATURAL) return SIGNED;
    ld->declarations.push_back(_makeBinaryAttribute(
        "to_signed", factory.signedType(hif::copy(retTypeSpan)), factory.integer(), factory.noValue(),
        factory.integer(), factory.noValue(), false, hifFormat));

    // function RISING_EDGE (signal S: BIT) return BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("rising_edge", factory.boolean(), new Bit(), factory.noValue(), true, hifFormat));
    // function FALLING_EDGE (signal S: BIT) return BOOLEAN;
    ld->declarations.push_back(
        _makeAttribute("falling_edge", factory.boolean(), new Bit(), factory.noValue(), true, hifFormat));

    delete retTypeSpan;

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeStdLogicMiscPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_misc", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Range *tpRange      = new Range(new Identifier("left"), new Identifier("right"), dir_downto);
    Range *retRange     = new Range(new Identifier("left1"), new Identifier("right1"), dir_downto);
    TypeReference *ux01 = factory.typeRef(
        _makeHifName("ux01", hifFormat).c_str(),
        factory.library(_makeHifName("ieee_std_logic_1164", hifFormat).c_str(), nullptr, nullptr, false, true));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // type STRENGTH is (strn_X01, strn_X0H, strn_XL1, strn_X0Z, strn_XZ1,
    // strn_WLH, strn_WLZ, strn_WZH, strn_W0H, strn_WL1);
    {
        const char *values[] = {"strn_X01", "strn_X0H", "strn_XL1", "strn_X0Z", "strn_XZ1",
                                "strn_WLH", "strn_WLZ", "strn_WZH", "strn_W0H", "strn_WL1"};
        ld->declarations.push_back(_makeEnum("strength", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // type MINOMAX is array (1 to 3) of TIME;
    ld->declarations.push_back(factory.typeDef("minomax", factory.array(new Range(1, 3), factory.time())));
    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function strength_map(input: STD_ULOGIC; strn: STRENGTH) return STD_LOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, true), _makeHifName("strength_map", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"),
         factory.parameter(factory.typeRef(_makeHifName("strength", hifFormat).c_str()), "param2"))));
    //function strength_map_z(input:STD_ULOGIC; strn:STRENGTH) return STD_LOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, true), _makeHifName("strength_map_z", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"),
         factory.parameter(factory.typeRef(_makeHifName("strength", hifFormat).c_str()), "param2"))));

    // function Drive (V: STD_ULOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeAttribute(
        "drive", factory.bitvector(hif::copy(retRange), true, true), factory.bitvector(nullptr, true, false),
        factory.noValue(), true, hifFormat));
    // function Drive (V: STD_LOGIC_VECTOR) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(_makeAttribute(
        "drive", factory.bitvector(hif::copy(retRange), true, false), factory.bitvector(nullptr, true, true),
        factory.noValue(), true, hifFormat));

    // function Sense (V: STD_ULOGIC; vZ, vU, vDC: STD_ULOGIC) return STD_LOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, true), _makeHifName("sense", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"), factory.parameter(factory.bit(true, false), "param2"),
         factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function Sense (V: STD_ULOGIC_VECTOR; vZ, vU, vDC: STD_ULOGIC) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(hif::copy(tpRange), true, true), _makeHifName("sense", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bitvector(hif::copy(tpRange), true, false), "param1"),
         factory.parameter(factory.bit(true, false), "param2"), factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function Sense (V: STD_ULOGIC_VECTOR; vZ, vU, vDC: STD_ULOGIC) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(hif::copy(tpRange), true, false), _makeHifName("sense", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bitvector(hif::copy(tpRange), true, false), "param1"),
         factory.parameter(factory.bit(true, false), "param2"), factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function Sense (V: STD_LOGIC_VECTOR; vZ, vU, vDC: STD_ULOGIC) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(hif::copy(tpRange), true, true), _makeHifName("sense", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bitvector(hif::copy(tpRange), true, true), "param1"),
         factory.parameter(factory.bit(true, false), "param2"), factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function Sense (V: STD_LOGIC_VECTOR; vZ, vU, vDC: STD_ULOGIC) return STD_ULOGIC_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(hif::copy(tpRange), true, false), _makeHifName("sense", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bitvector(hif::copy(tpRange), true, true), "param1"),
         factory.parameter(factory.bit(true, false), "param2"), factory.parameter(factory.bit(true, false), "param3"),
         factory.parameter(factory.bit(true, false), "param4"))));
    // function STD_LOGIC_VECTORtoBIT_VECTOR (V: STD_LOGIC_VECTOR; vX, vZ, vU, vDC: BIT := '0';
    //    Xflag, Zflag, Uflag, DCflag: BOOLEAN := FALSE) return BIT_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(hif::copy(tpRange), false), _makeHifName("std_logic_vectortobit_vector", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bitvector(hif::copy(tpRange), true, true), "param1"),
         factory.parameter(factory.bit(false), "param2", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param3", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param4", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param5", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.boolean(), "param6", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param7", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param8", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param9", factory.boolval(false)))));
    // function STD_ULOGIC_VECTORtoBIT_VECTOR (V: STD_ULOGIC_VECTOR; vX, vZ, vU, vDC: BIT := '0';
    //     Xflag, Zflag, Uflag, DCflag: BOOLEAN := FALSE) return BIT_VECTOR;
    ld->declarations.push_back(factory.subprogram(
        factory.bitvector(hif::copy(tpRange), false), _makeHifName("std_ulogic_vectortobit_vector", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.bitvector(hif::copy(tpRange), true, false), "param1"),
         factory.parameter(factory.bit(false), "param2", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param3", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param4", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param5", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.boolean(), "param6", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param7", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param8", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param9", factory.boolval(false)))));
    // function STD_ULOGICtoBIT (V: STD_ULOGIC; vX, vZ, vU, vDC: BIT := '0';
    //     Xflag, Zflag, Uflag, DCflag: BOOLEAN := FALSE) return BIT;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(false), _makeHifName("std_ulogictobit", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"),
         factory.parameter(factory.bit(false), "param2", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param3", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param4", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.bit(false), "param5", factory.bitval(bit_zero, factory.bit(false))),
         factory.parameter(factory.boolean(), "param6", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param7", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param8", factory.boolval(false)),
         factory.parameter(factory.boolean(), "param9", factory.boolval(false)))));

    // function AND_REDUCE(ARG: STD_LOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "and_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, true), factory.noValue(), true, hifFormat));
    // function NAND_REDUCE(ARG: STD_LOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "nand_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, true), factory.noValue(), true, hifFormat));
    // function OR_REDUCE(ARG: STD_LOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "or_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, true), factory.noValue(), true, hifFormat));
    // function NOR_REDUCE(ARG: STD_LOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "nor_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, true), factory.noValue(), true, hifFormat));
    // function XOR_REDUCE(ARG: STD_LOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "xor_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, true), factory.noValue(), true, hifFormat));
    // function XNOR_REDUCE(ARG: STD_LOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "xnor_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, true), factory.noValue(), true, hifFormat));

    // function AND_REDUCE(ARG: STD_ULOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "and_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, false), factory.noValue(), true, hifFormat));
    // function NAND_REDUCE(ARG: STD_ULOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "nand_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, false), factory.noValue(), true, hifFormat));
    // function OR_REDUCE(ARG: STD_ULOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "or_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, false), factory.noValue(), true, hifFormat));
    // function NOR_REDUCE(ARG: STD_ULOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "nor_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, false), factory.noValue(), true, hifFormat));
    // function XOR_REDUCE(ARG: STD_ULOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "xor_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, false), factory.noValue(), true, hifFormat));
    // function XNOR_REDUCE(ARG: STD_ULOGIC_VECTOR) return UX01;
    ld->declarations.push_back(_makeAttribute(
        "xnor_reduce", hif::copy(ux01), factory.bitvector(nullptr, true, false), factory.noValue(), true, hifFormat));

    // function fun_BUF3S(Input, Enable: UX01; Strn: STRENGTH) return STD_LOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, true), _makeHifName("fun_buf3s", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(hif::copy(ux01), "param1"), factory.parameter(hif::copy(ux01), "param2"),
         factory.parameter(factory.typeRef(_makeHifName("strength", hifFormat).c_str()), "param3"))));
    // function fun_BUF3SL(Input, Enable: UX01; Strn: STRENGTH) return STD_LOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, true), _makeHifName("fun_buf3sl", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(hif::copy(ux01), "param1"), factory.parameter(hif::copy(ux01), "param2"),
         factory.parameter(factory.typeRef(_makeHifName("strength", hifFormat).c_str()), "param3"))));
    // function fun_MUX2x1(Input0, Input1, Sel: UX01) return UX01;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ux01), _makeHifName("fun_mux2x1", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(hif::copy(ux01), "param1"), factory.parameter(hif::copy(ux01), "param2"),
         factory.parameter(hif::copy(ux01), "param3"))));
    // function fun_MAJ23(Input0, Input1, Input2: UX01) return UX01;
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ux01), _makeHifName("fun_maj23", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(hif::copy(ux01), "param1"), factory.parameter(hif::copy(ux01), "param2"),
         factory.parameter(hif::copy(ux01), "param3"))));
    // function fun_WiredX(Input0, Input1: std_ulogic) return STD_LOGIC;
    ld->declarations.push_back(factory.subprogram(
        factory.bit(true, true), _makeHifName("fun_wiredx", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.bit(true, false), "param1"),
         factory.parameter(factory.bit(true, false), "param2"))));

    delete tpRange;
    delete retRange;
    delete ux01;
    return ld;
}
LibraryDef *VHDLSemantics::getIeeeStdLogicSignedPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_signed", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // types
    Bitvector *std_logic_vector = factory.bitvector(nullptr, true, true);

    // return types
    Range *retSpan                  = factory.range(new Identifier("left1"), dir_downto, new Identifier("right1"));
    Bitvector *ret_std_logic_vector = factory.bitvector(hif::copy(retSpan), true, true);
    Range *retSpanInc               = factory.range(
        factory.expression(
            factory.expression(
                factory.expression(
                    factory.expression(new Identifier("left1"), op_plus, new Identifier("left2")), op_minus,
                    new Identifier("right1")),
                op_minus, new Identifier("right2")),
            op_plus, factory.intval(1)),
        dir_downto, new IntValue(0));
    Bitvector *ret_std_logic_vector_inc = factory.bitvector(hif::copy(retSpanInc), true, true);

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////
    // function SHL(ARG:STD_LOGIC_VECTOR;COUNT: STD_LOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shl", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // function SHR(ARG:STD_LOGIC_VECTOR;COUNT: STD_LOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shr", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // function CONV_INTEGER(ARG: STD_LOGIC_VECTOR) return INTEGER
    ld->declarations.push_back(_makeAttribute(
        "conv_integer", factory.integer(), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // Overloaded operators
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_plus", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_minus", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_mult", hif::copy(ret_std_logic_vector_inc), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeAttribute(
        "_op_abs", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    delete std_logic_vector;
    delete ret_std_logic_vector;
    delete retSpan;
    delete ret_std_logic_vector_inc;
    delete retSpanInc;

    return ld;
}

LibraryDef *VHDLSemantics::getIeeeStdLogicTextIOPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_textio", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    ld->libraries.push_back(
        factory.library(_makeHifName("std_textio", hifFormat).c_str(), nullptr, nullptr, false, true));
    Range *tpRange          = new Range(new Identifier("left"), new Identifier("right"), dir_downto);
    Bitvector *logicVector  = factory.bitvector(hif::copy(tpRange), true, true);
    Bitvector *uLogicVector = factory.bitvector(hif::copy(tpRange), true, false);
    Int *intType            = factory.integer(hif::copy(tpRange), false, false);

    Parameter *line      = factory.parameter( // check inout
        factory.typeRef(
            _makeHifName("line", hifFormat).c_str(),
            factory.library(_makeHifName("std_textio", hifFormat).c_str(), nullptr, nullptr, false, true)),
        "param1");
    Parameter *justified = factory.parameter(
        factory.typeRef(
            _makeHifName("side", hifFormat).c_str(),
            factory.library(_makeHifName("std_textio", hifFormat).c_str(), nullptr, nullptr, false, true)),
        "param3",
        factory.fieldRef(
            factory.libraryInstance(_makeHifName("std_textio", hifFormat).c_str(), false, true),
            _makeHifName("right", hifFormat).c_str()));
    Parameter *field = factory.parameter(
        factory.typeRef(
            _makeHifName("width", hifFormat).c_str(),
            factory.library(_makeHifName("std_textio", hifFormat).c_str(), nullptr, nullptr, false, true)),
        "param4", factory.intval(0));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // procedure READ(L:inout LINE; VALUE:out STD_ULOGIC);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(factory.bit(true, false), "param2", nullptr, nullptr, dir_out) // check out
         )));
    // procedure READ(L:inout LINE; VALUE:out STD_ULOGIC; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(factory.bit(true, false), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)         // check out
         )));
    // procedure READ(L:inout LINE; VALUE:out STD_ULOGIC_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2", nullptr, nullptr, dir_out) // check out
         )));
    // procedure READ(L:inout LINE; VALUE:out STD_ULOGIC_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)        // check out
         )));
    // procedure WRITE(L:inout LINE; VALUE:in STD_ULOGIC; JUSTIFIED:in SIDE := RIGHT;
    // FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (hif::HifFactory::parameter_t(hif::copy(line)), factory.parameter(factory.bit(true, false), "param2"),
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));
    // procedure WRITE(L:inout LINE; VALUE:in STD_ULOGIC_VECTOR; JUSTIFIED:in SIDE := RIGHT; FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)), factory.parameter(hif::copy(uLogicVector), "param2"),
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));

    // procedure READ(L:inout LINE; VALUE:out STD_LOGIC_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(logicVector), "param2", nullptr, nullptr, dir_out) // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(intType), "param2", nullptr, nullptr, dir_out) // check out
         )));
    // procedure READ(L:inout LINE; VALUE:out STD_LOGIC_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(logicVector), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)       // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(intType), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)   // check out
         )));
    // procedure WRITE(L:inout LINE; VALUE:in STD_LOGIC_VECTOR; JUSTIFIED:in SIDE := RIGHT; FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)), factory.parameter(hif::copy(logicVector), "param2"),
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));

    // procedure HREAD(L:inout LINE; VALUE:out STD_ULOGIC_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2", nullptr, nullptr, dir_out))));
    // procedure HREAD(L:inout LINE; VALUE:out STD_ULOGIC_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)        // check out
         )));
    // procedure HWRITE(L:inout LINE; VALUE:in STD_ULOGIC_VECTOR; JUSTIFIED:in SIDE := RIGHT; FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hwrite", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2"), // check out
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));
    // procedure HREAD(L:inout LINE; VALUE:out STD_LOGIC_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(logicVector), "param2", nullptr, nullptr, dir_out) // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(
             _factory.integer(
                 _factory.range(_factory.identifier("left"), hif::dir_downto, _factory.identifier("right"))),
             "param2", nullptr, nullptr, dir_out) // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(
             _factory.integer(
                 _factory.range(_factory.identifier("left"), hif::dir_downto, _factory.identifier("right")), false),
             "param2", nullptr, nullptr, dir_out) // check out
         )));
    // procedure HREAD(L:inout LINE; VALUE:out STD_LOGIC_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(logicVector), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)       // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(
             _factory.integer(
                 _factory.range(_factory.identifier("left"), hif::dir_downto, _factory.identifier("right"))),
             "param2", nullptr, nullptr, dir_out),                                 // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out) // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(
             _factory.integer(
                 _factory.range(_factory.identifier("left"), hif::dir_downto, _factory.identifier("right")), false),
             "param2", nullptr, nullptr, dir_out),                                 // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out) // check out
         )));
    // procedure HWRITE(L:inout LINE; VALUE:in STD_LOGIC_VECTOR; JUSTIFIED:in SIDE := RIGHT; FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hwrite", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)), factory.parameter(hif::copy(logicVector), "param2"),
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));

    // procedure OREAD(L:inout LINE; VALUE:out STD_ULOGIC_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("oread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2", nullptr, nullptr, dir_out) // check out
         )));
    // procedure OREAD(L:inout LINE; VALUE:out STD_ULOGIC_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("oread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(uLogicVector), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)        // check out
         )));
    // procedure OWRITE(L:inout LINE; VALUE:in STD_ULOGIC_VECTOR; JUSTIFIED:in SIDE := RIGHT; FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("owrite", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)), factory.parameter(hif::copy(uLogicVector), "param2"),
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));
    // procedure OREAD(L:inout LINE; VALUE:out STD_LOGIC_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("oread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(logicVector), "param2", nullptr, nullptr, dir_out) // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("oread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(intType), "param2", nullptr, nullptr, dir_out) // check out
         )));
    // procedure OREAD(L:inout LINE; VALUE:out STD_LOGIC_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("oread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(logicVector), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)       // check out
         )));
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("oread", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)),
         factory.parameter(hif::copy(intType), "param2", nullptr, nullptr, dir_out), // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)   // check out
         )));
    // procedure OWRITE(L:inout LINE; VALUE:in STD_LOGIC_VECTOR; JUSTIFIED:in SIDE := RIGHT; FIELD:in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("owrite", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (hif::HifFactory::parameter_t(hif::copy(line)), factory.parameter(hif::copy(logicVector), "param2"),
         hif::HifFactory::parameter_t(hif::copy(justified)), hif::HifFactory::parameter_t(hif::copy(field)))));

    delete tpRange;
    delete logicVector;
    delete uLogicVector;
    delete intType;
    delete justified;
    delete line;
    delete field;

    return ld;
}

LibraryDef *VHDLSemantics::getIeeeStdLogicUnsignedPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_std_logic_unsigned", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // types
    Bitvector *std_logic_vector = factory.bitvector(nullptr, true, true);

    // return types
    Range *retSpan                  = factory.range(new Identifier("left1"), dir_downto, new Identifier("right1"));
    Bitvector *ret_std_logic_vector = factory.bitvector(hif::copy(retSpan), true, true);
    Range *retSpanInc               = factory.range(
        factory.expression(
            factory.expression(
                factory.expression(
                    factory.expression(new Identifier("left1"), op_plus, new Identifier("left2")), op_minus,
                    new Identifier("right1")),
                op_minus, new Identifier("right2")),
            op_plus, factory.intval(1)),
        dir_downto, new IntValue(0));
    Bitvector *ret_std_logic_vector_inc = factory.bitvector(hif::copy(retSpanInc), true, true);

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function SHL(ARG:STD_LOGIC_VECTOR;COUNT: STD_LOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shl", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // function SHR(ARG:STD_LOGIC_VECTOR;COUNT: STD_LOGIC_VECTOR) return STD_LOGIC_VECTOR;
    ld->declarations.push_back(_makeBinaryAttribute(
        "shr", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // function CONV_INTEGER(ARG: STD_LOGIC_VECTOR) return INTEGER
    ld->declarations.push_back(_makeAttribute(
        "conv_integer", factory.integer(), hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    // Overloaded operators
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_plus", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_minus", hif::copy(ret_std_logic_vector), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));
    ld->declarations.push_back(_makeBinaryAttribute(
        "_op_mult", hif::copy(ret_std_logic_vector_inc), hif::copy(std_logic_vector), factory.noValue(),
        hif::copy(std_logic_vector), factory.noValue(), false, hifFormat));

    delete std_logic_vector;
    delete ret_std_logic_vector;
    delete retSpan;
    delete ret_std_logic_vector_inc;
    delete retSpanInc;

    return ld;
}
LibraryDef *VHDLSemantics::getIeeeMathComplexPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_math_complex", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // type COMPLEX        is record RE, IM: real; end record;
    ld->declarations.push_back(factory.recordTypeDef(
        _makeHifName("complex", hifFormat).c_str(),
        (factory.field(factory.real(), "re"), factory.field(factory.real(), "im"))));
    // type COMPLEX_VECTOR is array (integer range <>) of COMPLEX;
    ld->declarations.push_back(factory.typeDef(
        _makeHifName("complex_vector", hifFormat).c_str(),
        factory.array(
            new Range(new Identifier("left"), new Identifier("right"), dir_downto),
            factory.typeRef(_makeHifName("complex", hifFormat).c_str())),
        true,
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right"))));
    // type COMPLEX_POLAR  is record MAG: real; ARG: real; end record;
    ld->declarations.push_back(factory.recordTypeDef(
        _makeHifName("complex_polar", hifFormat).c_str(),
        (factory.field(factory.real(), "mag"), factory.field(factory.real(), "arg"))));

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // constant  CBASE_1: complex := COMPLEX'(1.0, 0.0);
    ld->declarations.push_back(factory.constant(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()), _makeHifName("cbase_1", hifFormat).c_str(),
        factory.recordval(
            (factory.recordvaluealt("re", factory.realval(1.0)), factory.recordvaluealt("im", factory.realval(0.0))))));
    // constant  CBASE_j: complex := COMPLEX'(0.0, 1.0);
    ld->declarations.push_back(factory.constant(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()), _makeHifName("cbase_j", hifFormat).c_str(),
        factory.recordval(
            (factory.recordvaluealt("re", factory.realval(0.0)), factory.recordvaluealt("im", factory.realval(1.0))))));
    // constant  CZERO: complex := COMPLEX'(0.0, 0.0);
    ld->declarations.push_back(factory.constant(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()), _makeHifName("czero", hifFormat).c_str(),
        factory.recordval(
            (factory.recordvaluealt("re", factory.realval(0.0)), factory.recordvaluealt("im", factory.realval(0.0))))));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function CABS(Z: in complex ) return real;
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("cabs", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex", hifFormat).c_str()), "param1"))));
    // function CARG(Z: in complex ) return real;
    ld->declarations.push_back(factory.subprogram(
        factory.real(), _makeHifName("carg", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex", hifFormat).c_str()), "param1"))));
    // function CMPLX(X: in real;  Y: in real:= 0.0 ) return complex;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()), _makeHifName("cmplx", hifFormat).c_str(),
        factory.noTemplates(),
        (factory.parameter(factory.real(), "param1"),
         factory.parameter(factory.real(), "param2", factory.realval(0.0)))));
    // function CONJ (Z: in complex) return complex;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()), _makeHifName("conj", hifFormat).c_str(),
        factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex", hifFormat).c_str()), "param1"))));
    // function CONJ (Z: in complex_polar) return complex_polar;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("complex_polar", hifFormat).c_str()), _makeHifName("conj", hifFormat).c_str(),
        factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex_polar", hifFormat).c_str()), "param1"))));
    // function CSQRT(Z: in complex ) return complex_vector;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(
            _makeHifName("complex_vector", hifFormat).c_str(),
            (factory.templateValueArgument("left", new Identifier("left")),
             factory.templateValueArgument("right", new Identifier("right")))),
        _makeHifName("csqrt", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.typeRef(_makeHifName("complex", hifFormat).c_str()), "param1"))));
    // function CEXP(Z: in complex ) return complex;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()), _makeHifName("cexp", hifFormat).c_str(),
        factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex", hifFormat).c_str()), "param1"))));
    // function COMPLEX_TO_POLAR(Z: in complex ) return complex_polar;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("complex_polar", hifFormat).c_str()),
        _makeHifName("complex_to_polar", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex", hifFormat).c_str()), "param1"))));
    // function POLAR_TO_COMPLEX(Z: in complex_polar ) return complex;
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("complex", hifFormat).c_str()),
        _makeHifName("polar_to_complex", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("complex_polar", hifFormat).c_str()), "param1"))));

    return ld;
}

LibraryDef *VHDLSemantics::getTextIOPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("std_textio", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Range *tpRange         = new Range(new Identifier("left"), new Identifier("right"), dir_downto);
    Bitvector *bitVector   = factory.bitvector(hif::copy(tpRange), false);
    Parameter *justifParam = factory.parameter(
        factory.typeRef(_makeHifName("side", hifFormat).c_str()), "param3",
        new Identifier(_makeHifName("right", hifFormat)));

    TypeReference *file_open_kind = factory.typeRef(
        _makeHifName("file_open_kind", hifFormat).c_str(),
        _factory.library(_makeHifName("standard", hifFormat).c_str(), nullptr, nullptr, false, true));

    TypeReference *file_open_status = factory.typeRef(
        _makeHifName("file_open_status", hifFormat).c_str(),
        _factory.library(_makeHifName("standard", hifFormat).c_str(), nullptr, nullptr, false, true));

    FieldReference *read_mode = _factory.fieldRef(
        _factory.libraryInstance(_makeHifName("standard", hifFormat).c_str(), false, true),
        _makeHifName("read_mode", hifFormat).c_str());

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // type LINE is access STRING;
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("line", hifFormat).c_str(), factory.pointer(factory.string()), true));

    // type TEXT is file of STRING;
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("text", hifFormat).c_str(), factory.file(factory.string()), true));

    // type SIDE is (RIGHT, LEFT);
    {
        const char *values[] = {"right", "left"};
        ld->declarations.push_back(_makeEnum("side", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // subtype WIDTH is NATURAL;
    ld->declarations.push_back(factory.typeDef(_makeHifName("width", hifFormat).c_str(), factory.integer(), false));

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // variables
    // ///////////////////////////////////////////////////////////////////

    // file INPUT: TEXT open READ_MODE is "STD_INPUT";
    ld->declarations.push_back(factory.variable(
        factory.typeRef(_makeHifName("text", hifFormat).c_str()), _makeHifName("input", hifFormat).c_str(),
        factory.cast(factory.typeRef(_makeHifName("text", hifFormat).c_str()), factory.stringval("std_input"))));

    // file OUTPUT: TEXT open WRITE_MODE is "STD_OUTPUT";
    ld->declarations.push_back(factory.variable(
        factory.typeRef(_makeHifName("text", hifFormat).c_str()), _makeHifName("output", hifFormat).c_str(),
        factory.cast(factory.typeRef(_makeHifName("text", hifFormat).c_str()), factory.stringval("std_output"))));

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("text", hifFormat).c_str()), _makeHifName("file_open", hifFormat).c_str(),
        factory.noTemplates(),
        (factory.parameter(factory.string(), "param1"),
         factory.parameter(hif::copy(file_open_kind), "param2", hif::copy(read_mode)))));
    // procedure FILE_OPEN (file F: TEXT; External_Name: in STRING;
    // Open_Kind: in FILE_OPEN_KIND := READ_MODE);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("file_open", hifFormat).c_str(), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1"), factory.parameter(factory.string(), "param2"),
         factory.parameter(hif::copy(file_open_kind), "param3", hif::copy(read_mode)))));

    // procedure FILE_OPEN (Status: out FILE_OPEN_STATUS; file F: TEXT;
    // External_Name: in STRING; Open_Kind: in FILE_OPEN_KIND := READ_MODE);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("file_open", hifFormat).c_str(), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(hif::copy(file_open_status), "param1"),
         factory.parameter(factory.file(factory.typeRef("T")), "param2"), factory.parameter(factory.string(), "param3"),
         factory.parameter(hif::copy(file_open_kind), "param4", hif::copy(read_mode)))));

    // procedure FILE_CLOSE (file F: TEXT);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("file_close", hifFormat).c_str(), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1"))));

    // procedure READ (file F: TEXT; VALUE: out STRING);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T"),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1"),
         factory.parameter(factory.typeRef("T"), "param2", nullptr, nullptr, dir_out))));

    // procedure WRITE (file F: TEXT; VALUE: in STRING);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T"),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1"),
         factory.parameter(factory.typeRef("T"), "param2"))));

    // procedure READLINE (file F: TEXT; L: inout LINE);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("readline", hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T"),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1"),
         factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param2") // check inout
         )));

    // procedure READ (L: inout LINE; VALUE: out BIT; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.bit(false), "param2", nullptr, nullptr, dir_out),            // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out BIT);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.bit(false), "param2", nullptr, nullptr, dir_out)             // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out BIT_VECTOR; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(hif::copy(bitVector), "param2", nullptr, nullptr, dir_out),          // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out BIT_VECTOR);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(hif::copy(bitVector), "param2", nullptr, nullptr, dir_out)           // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out BOOLEAN; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.boolean(), "param2", nullptr, nullptr, dir_out),             // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.boolean(), "param2", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out CHARACTER; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.character(), "param2", nullptr, nullptr, dir_out),           // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out CHARACTER);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.character(), "param2", nullptr, nullptr, dir_out)            // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out INTEGER; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.integer(), "param2", nullptr, nullptr, dir_out),             // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out INTEGER);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.integer(), "param2", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out REAL; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.real(), "param2", nullptr, nullptr, dir_out),                // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out REAL);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.real(), "param2", nullptr, nullptr, dir_out)                 // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out STRING; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.string(), "param2", nullptr, nullptr, dir_out),              // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out STRING);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.string(), "param2", nullptr, nullptr, dir_out)               // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out TIME; GOOD: out BOOLEAN);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.time(), "param2", nullptr, nullptr, dir_out),                // check out
         factory.parameter(factory.boolean(), "param3", nullptr, nullptr, dir_out)              // check out
         )));

    // procedure READ (L: inout LINE; VALUE: out TIME);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("read", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.time(), "param2", nullptr, nullptr, dir_out)                 // check out
         )));

    // procedure WRITELINE (file F: TEXT; L: inout LINE);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("writeline", hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T"),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1"),
         factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param2") // check inout
         )));

    // procedure WRITE (L: inout LINE; VALUE: in BIT; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.bit(false), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in BIT_VECTOR; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(),
        (factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(hif::copy(bitVector), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in BOOLEAN; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.boolean(), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in CHARACTER; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.character(), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in INTEGER; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.integer(), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in REAL; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0; DIGITS: in NATURAL := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.real(), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)),
         factory.parameter(factory.integer(), "param5", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in STRING; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.string(), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)))));
    // procedure WRITE (L: inout LINE; VALUE: in TIME; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0; UNIT: in TIME := ns);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("write", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("line", hifFormat).c_str()), "param1"), // check inout
         factory.parameter(factory.time(), "param2"), hif::copy(justifParam),
         factory.parameter(factory.typeRef(_makeHifName("width", hifFormat).c_str()), "param4", factory.intval(0)),
         factory.parameter(
             factory.typeRef(
                 _makeHifName("time_units", hifFormat).c_str(),
                 factory.library(_makeHifName("standard", hifFormat).c_str(), nullptr, nullptr, false, true)),
             "param5",
             factory.fieldRef(
                 factory.libraryInstance(_makeHifName("standard", hifFormat).c_str(), false, true),
                 _makeHifName("ns", hifFormat).c_str())))));

    // function ENDFILE (L: in LINE) return BOOLEAN;
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("endfile", hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T"),
        (factory.parameter(factory.file(factory.typeRef("T")), "param1") // check input
         )));

    delete tpRange;
    delete bitVector;
    delete justifParam;

    delete file_open_kind;
    delete read_mode;
    delete file_open_status;

    return ld;
}

LibraryDef *VHDLSemantics::getIeeeMathRealPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ieee_math_real", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    //constant  MATH_E :   real := 2.71828_18284_59045_23536;
    ld->declarations.push_back(factory.constant(factory.real(), "math_e", factory.realval(2.71828182845904523536)));
    //constant  MATH_1_E:  real := 0.36787_94411_71442_32160;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_1_over_e", factory.realval(0.36787944117144232160)));
    //constant  MATH_PI :  real := 3.14159_26535_89793_23846;
    ld->declarations.push_back(factory.constant(factory.real(), "math_pi", factory.realval(3.14159265358979323846)));
    //constant  MATH_2_PI :  real := 6.28318_53071_79586_47693;
    ld->declarations.push_back(factory.constant(factory.real(), "math_2_pi", factory.realval(6.28318530717958647693)));
    //constant  MATH_1_OVER_PI :  real := 0.31830_98861_83790_67154;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_1_over_pi", factory.realval(0.31830988618379067154)));
    //constant  MATH_PI_OVER 2 :  real := 1.57079_63267_94896_61923;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_pi_over_2", factory.realval(1.57079632679489661923)));
    //constant  MATH_PI_OVER_3 :  real := 1.04719_75511_96597_74615;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_pi_over_3", factory.realval(1.04719755119659774615)));
    //constant  MATH_PI_OVER_4 :  real := 0.78539_81633_97448_30962;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_pi_over_4", factory.realval(0.78539816339744830962)));
    //constant  MATH_3_PI_OVER_2 :  real := 4.71238_89803_84689_85769;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_3_pi_over_2", factory.realval(4.71238898038468985769)));
    //constant  MATH_LOG_OF_2:  real := 0.69314_71805_59945_30942;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_log_of_2", factory.realval(0.69314718055994530942)));
    //constant  MATH_LOG_OF_10: real := 2.30258_50929_94045_68402;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_log_of_10", factory.realval(2.30258509299404568402)));
    //constant  MATH_LOG2_OF_E:  real := 1.44269_50408_88963_4074;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_log2_of_e", factory.realval(1.4426950408889634074)));
    //constant  MATH_LOG10_OF_E: real := 0.43429_44819_03251_82765;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_log10_of_e", factory.realval(0.43429448190325182765)));
    //constant  MATH_SQRT_2: real := 1.41421_35623_73095_04880;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_sqrt_2", factory.realval(1.41421356237309504880)));
    //constant  MATH_1_OVER_SQRT_2: real := 0.70710_67811_86547_52440;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_1_over_sqrt_2", factory.realval(0.70710678118654752440)));
    //constant  MATH_SQRT_PI: real := 1.77245_38509_05516_02730;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_sqrt_pi", factory.realval(1.77245385090551602730)));
    //constant  MATH_DEG_TO_RAD: real := 0.01745_32925_19943_29577;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_deg_to_rad", factory.realval(0.01745329251994329577)));
    //constant  MATH_RAD_TO_DEG: real := 57.29577_95130_82320_87685;
    ld->declarations.push_back(
        factory.constant(factory.real(), "math_rad_to_deg", factory.realval(57.29577951308232087685)));

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // function SIGN (X: real ) return real;
    ld->declarations.push_back(
        _makeAttribute("sign", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function CEIL (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("ceil", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function FLOOR (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("floor", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function ROUND (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("round", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function TRUNC (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("trunc", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function REALMAX (X, Y : real ) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "realmax", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false,
        hifFormat));
    //function REALMIN (X, Y : real ) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "realmin", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false,
        hifFormat));
    //procedure UNIFORM (variable Seed1,Seed2:inout integer; variable X:out real);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("uniform", hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1", nullptr),
         factory.parameter(factory.typeRef("T2"), "param2", nullptr),
         factory.parameter(factory.real(), "param3", nullptr))));
    //function SRAND (seed: in integer ) return integer;
    ld->declarations.push_back(
        _makeAttribute("srand", factory.integer(), factory.integer(), factory.noValue(), false, hifFormat));
    //function RAND return integer;
    ld->declarations.push_back(_makeAttribute("rand", factory.integer(), nullptr, factory.noValue(), false, hifFormat));
    //function GET_RAND_MAX  return integer;
    ld->declarations.push_back(
        _makeAttribute("get_rand_max", factory.integer(), nullptr, factory.noValue(), false, hifFormat));

    //function SQRT (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("sqrt", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function CBRT (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("cbrt", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function EXP  (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("exp", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function LOG (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("log", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function LOG2 (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("log2", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function LOG10 (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("log10", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function LOG (BASE: real; X : real) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "log", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false, hifFormat));
    //function  SIN (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("sin", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  COS ( X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("cos", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  TAN (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("tan", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  ARCSIN (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("arcsin", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  ARCCOS (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("arccos", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  ARCTAN (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("arctan", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  ARCTAN (X : real; Y : real) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "arctan", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false,
        hifFormat));
    //function SINH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("sinh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  COSH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("cosh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function  TANH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("tanh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function ARCSINH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("arcsinh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function ARCCOSH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("arccosh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    //function AECTANH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("arctanh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    return ld;
}

LibraryDef *VHDLSemantics::getPSLStandardPackage(const bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("psl_standard", hifFormat));
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(VHDLSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // fl_operators
    {
        const char *values[] = {// fl_invariance_operators
                                "psl_always", "psl_never", "psl_G",
                                // fl_bounding_operators
                                "psl_U", "psl_W", "psl_until", "psl_until!", "psl_until!_", "psl_until_", "psl_before",
                                "psl_before!", "psl_before!_", "psl_before_",
                                // fl_occurence_operators
                                "psl_X!", "psl_X", "psl_F", "psl_next", "psl_next!", "psl_next_a", "psl_next_a!",
                                "psl_next_e", "psl_next_e!", "psl_next_event", "psl_next_event!", "psl_next_event_a",
                                "psl_next_event_a!", "psl_next_event_e", "psl_next_event_e!", "psl_eventually!",
                                // fl_termination_operators
                                "psl_async_abort", "psl_sync_abort", "psl_abort",
                                // boolean_implication_operators
                                "psl_imply", "psl_double_imply",
                                // sequence_implication_operators
                                "psl_sequence_imply", "psl_sequence_double_imply",
                                // clocked:
                                "psl_at_clause"};

        ld->declarations.push_back(_makeEnum("psl_operators", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // void PSL_ASSERT(bool property, string REPORT = "")
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_assert", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.boolean(), "property"),
         factory.parameter(factory.string(), "report", factory.stringval("")))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property"))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property1, bool fl_property2)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property1"), factory.parameter(factory.boolean(), "fl_property2"))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property, integer cycles)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property"), factory.parameter(factory.integer(), "cycles"))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property,
    //                      integer range_lbound, integer range_rbound)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property"), factory.parameter(factory.integer(), "range_lbound"),
         factory.parameter(factory.integer(), "range_rbound"))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property, integer range_lbound,
    //                      boolean occurrence_expression)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property"),
         factory.parameter(factory.boolean(), "occurrence_expression"))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property, integer range_lbound,
    //                      boolean occurrence_expression, integer cycles)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property"),
         factory.parameter(factory.boolean(), "occurrence_expression"),
         factory.parameter(factory.integer(), "cycles"))));

    // void PSL_FL_PROPERTY(psl_operators op, bool fl_property, integer range_lbound,
    //                      boolean occurrence_expression,
    //                      integer range_lbound, integer range_rbound)
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("psl_fl_property", hifFormat).c_str(), factory.noTemplates(),
        (factory.parameter(factory.typeRef(_makeHifName("psl_operators", hifFormat).c_str()), "op"),
         factory.parameter(factory.boolean(), "fl_property"),
         factory.parameter(factory.boolean(), "occurrence_expression"),
         factory.parameter(factory.integer(), "range_lbound"), factory.parameter(factory.integer(), "range_rbound"))));

    return ld;
}
LibraryDef *VHDLSemantics::getStandardLibrary(const std::string &n)
{
    if (n == "ieee_math_complex") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeMathComplexPackage(false);
        return lStandard;
    } else if (n == "ieee_math_real") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeMathRealPackage(false);
        return lStandard;
    } else if (n == "ieee_numeric_bit") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeNumericBitPackage(false);
        return lStandard;
    } else if (n == "ieee_numeric_std") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeNumericStdPackage(false);
        return lStandard;
    } else if (n == "ieee_std_logic_1164") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogic1164Package(false);
        return lStandard;
    } else if (n == "ieee_std_logic_arith") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogicArithPackage(false);
        return lStandard;
    } else if (n == "ieee_std_logic_arith_ex") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogicArithExPackage(false);
        return lStandard;
    } else if (n == "ieee_std_logic_misc") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogicMiscPackage(false);
        return lStandard;
    } else if (n == "ieee_std_logic_signed") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogicSignedPackage(false);
        return lStandard;
    } else if (n == "ieee_std_logic_textio") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogicTextIOPackage(false);
        return lStandard;
    } else if (n == "ieee_std_logic_unsigned") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIeeeStdLogicUnsignedPackage(false);
        return lStandard;
    } else if (n == "standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getStandardPackage(false);
        return lStandard;
    } else if (n == "std_textio") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getTextIOPackage(false);
        return lStandard;
    } else if (n == "psl_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getPSLStandardPackage(false);
        return lStandard;
    }

    return nullptr;
}

bool VHDLSemantics::isNativeLibrary(const std::string &n, const bool hifFormat)
{
    if (n == _makeHifName("ieee_math_complex", hifFormat) || n == _makeHifName("ieee_math_real", hifFormat) ||
        n == _makeHifName("ieee_numeric_std", hifFormat) || n == _makeHifName("ieee_numeric_bit", hifFormat) ||
        n == _makeHifName("ieee_std_logic_1164", hifFormat) || n == _makeHifName("ieee_std_logic_arith", hifFormat) ||
        n == _makeHifName("ieee_std_logic_arith_ex", hifFormat) ||
        n == _makeHifName("ieee_std_logic_misc", hifFormat) || n == _makeHifName("ieee_std_logic_signed", hifFormat) ||
        n == _makeHifName("ieee_std_logic_textio", hifFormat) ||
        n == _makeHifName("ieee_std_logic_unsigned", hifFormat) || n == _makeHifName("standard", hifFormat) ||
        n == _makeHifName("std_textio", hifFormat))
        return true;

    return false;
}

void VHDLSemantics::addStandardPackages(System *s)
{
    ILanguageSemantics::addStandardPackages(s);

    // Add vhdl standard library
    LibraryDef *ld = getStandardLibrary("standard");
    s->libraryDefs.push_front(ld);

    Library *lib = new Library();
    lib->setName("standard");
    lib->setSystem(true);
    s->libraries.push_front(lib);
}

VHDLSemantics::MapCases
VHDLSemantics::mapStandardSymbol(Declaration *decl, KeySymbol &key, ValueSymbol &value, ILanguageSemantics * /*srcSem*/)
{
    std::string libName;
    const bool isMine = _isHifPrefixed(key.first, libName);
    // messageAssert(isMine, "Asked name not prefixed with 'hif_': " + unprefixed, nullptr, nullptr);
    if (isMine) {
        std::string symName;
        messageAssert(_isHifPrefixed(key.second, symName), "Found not prefixed symbol in prefixed library", decl, this);

        value.libraries.clear();
        value.libraries.push_back(libName);
        value.mappedSymbol   = symName;
        // Libraries are always replaced, therefore internal symbols must be kept
        const bool isLibrary = (libName == symName);
        value.mapAction      = isLibrary ? MAP_DELETE : MAP_KEEP;
        return value.mapAction;
    }
    // custom cases
    // IDEA: methods that are mapped into methods which differs for parameter numbers,
    // or whose mapping cannot be decided only by the name (i.e. decl must be inspected).
    else if (key.first != key.second) {
        // nothing to do at the moment
    }

    StandardSymbols::iterator it(_standardSymbols.find(key));
    if (it == _standardSymbols.end())
        return ILanguageSemantics::UNKNOWN;

    value = it->second;
    return value.mapAction;
}

Object *VHDLSemantics::getSimplifiedSymbol(KeySymbol & /*key*/, Object *s) { messageError("TODO", s, this); }

bool VHDLSemantics::isStandardInclusion(const std::string &n, const bool /*isLibInclusion*/)
{
    if (n == "standard")
        return true;
    else if (n == "std_textio")
        return true;

    return false;
}

std::string VHDLSemantics::getEventMethodName(const bool hifFormat) { return _makeHifName("event", hifFormat); }

bool VHDLSemantics::isEventCall(FunctionCall *call)
{
    return call->getInstance() != nullptr && call->getName() == getEventMethodName(false);
}

} // namespace semantics
} // namespace hif
