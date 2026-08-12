/// @file SystemCSemantics_methods.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/HifFactory.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{
namespace
{ // anon
} // namespace

DesignUnit *SystemCSemantics::_makeHifAggregateDU(
    HifFactory &factory,
    const char *duName,
    Type *aggregateType,
    Type *parameterType,
    bool hifFormat)
{
    View *v             = nullptr;
    ViewReference *vref = nullptr;
    Array *arr          = dynamic_cast<Array *>(aggregateType);
    if (arr != nullptr) {
        v = factory.view(
            "cpp",
            factory.contents(
                nullptr, factory.noDeclarations(), factory.noGenerates(), factory.noInstances(),
                factory.noStateTables(), factory.noLibraries()),
            nullptr, hif::cpp, factory.noDeclarations(), factory.noLibraries(),
            (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(factory.integer(), "size")),
            false);

        vref = factory.viewRef(
            _makeHifName(duName, hifFormat), "cpp", nullptr,
            (factory.templateTypeArgument("T", factory.typeRef("T", nullptr)),
             factory.templateValueArgument("size", new Identifier("size"))));
    } else // if vector
    {
        v = factory.view(
            "cpp",
            factory.contents(
                nullptr, factory.noDeclarations(), factory.noGenerates(), factory.noInstances(),
                factory.noStateTables(), factory.noLibraries()),
            nullptr, hif::cpp, factory.noDeclarations(), factory.noLibraries(),
            (factory.templateValueParameter(factory.integer(), "size")), false);

        vref = factory.viewRef(
            _makeHifName(duName, hifFormat), "cpp", nullptr,
            (factory.templateValueArgument("size", new Identifier("size"))));
    }

    DesignUnit *du = factory.designUnit(_makeHifName(duName, hifFormat), v);

    du->views.front()->getContents()->declarations.push_back(
        factory.subprogram(aggregateType, "getResult", factory.noTemplates(), factory.noParameters()));
    du->views.front()->getContents()->declarations.push_back(factory.subprogram(
        vref, "addPair", factory.noTemplates(),
        (factory.parameter(factory.integer(), "index"), factory.parameter(parameterType, "value"))));
    du->views.front()->getContents()->declarations.push_back(factory.subprogram(
        hif::copy(vref), "addPairSet", factory.noTemplates(),
        (factory.parameter(factory.integer(), "lbound"), factory.parameter(factory.integer(), "rbound"),
         factory.parameter(hif::copy(parameterType), "value"))));
    du->views.front()->getContents()->declarations.push_back(factory.subprogram(
        hif::copy(vref), "setOthers", factory.noTemplates(), factory.parameter(hif::copy(parameterType), "others")));

    Function *ctor = factory.classConstructor(du, factory.noParameters(), factory.noTemplates());
    du->views.front()->getContents()->declarations.push_front(ctor);

    return du;
}

LibraryDef *SystemCSemantics::getScAmsELNPackage(bool hifFormat)
{
    LibraryDef *lib = new LibraryDef();
    lib->setName(_makeHifName("sca_eln", hifFormat));
    lib->setLanguageID(hif::ams);
    lib->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // This trick avoids to re-define SystemC-like standard methods,
    // and therefore avoids ambiguity.
    Library *sc_core = factory.library(_makeHifName("sc_core", hifFormat), nullptr, "systemc", false, true);
    lib->libraries.push_back(sc_core);

    // Library entries for the interconnecting constructs of the ELN namespace
    // of the AMS library.
    // Terminal: used to define external interfaces of eln modules.
    // Node: used to define interconnections/bindings among basic eln blocks.
    //
    //
    // electrical type of SystemC AMS
    {
        DesignUnit *terminal = factory.designUnit(_makeHifName("scams_electrical", hifFormat), "class", hif::cpp);

        Function *constructor = factory.classConstructor(terminal, factory.noParameters(), factory.noTemplates());
        terminal->views.front()->getContents()->declarations.push_front(constructor);

        lib->declarations.push_back(terminal);
    }

    // sca_node
    {
        TypeDef *node = factory.typeDef(
            _makeHifName("sca_node", hifFormat), factory.viewRef(_makeHifName("scams_electrical", hifFormat), "class"),
            false);
        lib->declarations.push_back(node);
    }

    // sca_node_ref
    {
        TypeDef *nodeRef = factory.typeDef(
            _makeHifName("sca_node_ref", hifFormat),
            factory.viewRef(_makeHifName("scams_electrical", hifFormat), "class"), false);
        lib->declarations.push_back(nodeRef);
    }

    // sca_terminal
    {
        TypeDef *terminal = factory.typeDef(
            _makeHifName("sca_terminal", hifFormat),
            factory.viewRef(_makeHifName("scams_electrical", hifFormat), "class"), false);
        lib->declarations.push_back(terminal);
    }

    // Library entries for the basic components of the ELN namespace
    // of the AMS library.

    // Resistor
    {
        DesignUnit *resistor = factory.designUnit(_makeHifName("sca_r", hifFormat), "class", hif::cpp);
        View *view           = resistor->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        entity->ports.push_back(n);
        entity->ports.push_back(p);

        // Attributes of the component.
        hif::Contents *content = view->getContents();
        /*        Variable * variable = factory.variable(
                factory.real(),
                "value",
                factory.realval(1.0));*/

        content->declarations.push_back(factory.classConstructor(
            resistor,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0))),
            nullptr));

        lib->declarations.push_back(resistor);
    }

    {
    }

    // Capacitor
    {
        DesignUnit *capacitor = factory.designUnit(_makeHifName("sca_c", hifFormat), "class", hif::cpp);
        View *view            = capacitor->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        entity->ports.push_back(n);
        entity->ports.push_back(p);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            capacitor,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0)),
             factory.parameter(factory.real(), "q0", factory.realval(0.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(capacitor);
    }

    // Inductor
    {
        DesignUnit *inductor = factory.designUnit(_makeHifName("sca_l", hifFormat), "class", hif::cpp);
        View *view           = inductor->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        entity->ports.push_back(n);
        entity->ports.push_back(p);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            inductor,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0))),
            (factory.noTemplates())));

        content->declarations.push_back(factory.classConstructor(
            inductor,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0)),
             factory.parameter(factory.real(), "phi0", factory.realval(0.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(inductor);
    }

    // Voltage Controlled Voltage Source (VCVS)
    {
        DesignUnit *vcvs = factory.designUnit(_makeHifName("sca_vcvs", hifFormat), "class", hif::cpp);
        View *view       = vcvs->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *ncp =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncp", hif::dir_inout);
        Port *ncn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncn", hif::dir_inout);
        Port *np =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "np", hif::dir_inout);
        Port *nn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "nn", hif::dir_inout);

        entity->ports.push_back(ncp);
        entity->ports.push_back(ncn);
        entity->ports.push_back(np);
        entity->ports.push_back(nn);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            vcvs,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(vcvs);
    }

    // Voltage Controlled Current Source (VCCS)
    {
        DesignUnit *vccs = factory.designUnit(_makeHifName("sca_vccs", hifFormat), "class", hif::cpp);
        View *view       = vccs->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *ncp =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncp", hif::dir_inout);
        Port *ncn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncn", hif::dir_inout);
        Port *np =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "np", hif::dir_inout);
        Port *nn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "nn", hif::dir_inout);

        entity->ports.push_back(ncp);
        entity->ports.push_back(ncn);
        entity->ports.push_back(np);
        entity->ports.push_back(nn);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            vccs,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(vccs);
    }

    // Current Controlled Voltage Source (CCVS)
    {
        DesignUnit *ccvs = factory.designUnit(_makeHifName("sca_ccvs", hifFormat), "class", hif::cpp);
        View *view       = ccvs->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *ncp =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncp", hif::dir_inout);
        Port *ncn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncn", hif::dir_inout);
        Port *np =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "np", hif::dir_inout);
        Port *nn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "nn", hif::dir_inout);

        entity->ports.push_back(ncp);
        entity->ports.push_back(ncn);
        entity->ports.push_back(np);
        entity->ports.push_back(nn);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            ccvs,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(ccvs);
    }

    // Current Controlled Current Source (CCCS)
    {
        DesignUnit *cccs = factory.designUnit(_makeHifName("sca_cccs", hifFormat), "class", hif::cpp);
        View *view       = cccs->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *ncp =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncp", hif::dir_inout);
        Port *ncn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "ncn", hif::dir_inout);
        Port *np =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "np", hif::dir_inout);
        Port *nn =
            factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "nn", hif::dir_inout);

        entity->ports.push_back(ncp);
        entity->ports.push_back(ncn);
        entity->ports.push_back(np);
        entity->ports.push_back(nn);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            cccs,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "value", factory.realval(1.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(cccs);
    }

    // Voltage Source
    {
        DesignUnit *vsource = factory.designUnit(_makeHifName("sca_vsource", hifFormat), "class", hif::cpp);
        View *view          = vsource->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        entity->ports.push_back(n);
        entity->ports.push_back(p);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            vsource,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "init_value", factory.realval(0.0)),
             factory.parameter(factory.real(), "offset", factory.realval(0.0))),
            (factory.noTemplates())));

        content->declarations.push_back(factory.classConstructor(
            vsource,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "init_value", factory.realval(0.0)),
             factory.parameter(factory.real(), "offset", factory.realval(0.0)),
             factory.parameter(factory.real(), "amplitude", factory.realval(0.0)),
             factory.parameter(factory.real(), "frequency", factory.realval(0.0)),
             factory.parameter(factory.real(), "phase", factory.realval(0.0)),
             /// @todo Check corretness for time attribute.
             factory.parameter(new Time(), "delay", factory.noValue()),
             factory.parameter(factory.real(), "ac_amplitude", factory.realval(0.0)),
             factory.parameter(factory.real(), "ac_phase", factory.realval(0.0)),
             factory.parameter(factory.real(), "ac_noise_amplitude", factory.realval(0.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(vsource);
    }
    // Current Source
    {
        DesignUnit *isource = factory.designUnit(_makeHifName("sca_isource", hifFormat), "class", hif::cpp);
        View *view          = isource->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        entity->ports.push_back(n);
        entity->ports.push_back(p);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            isource,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "init_value", factory.realval(0.0)),
             factory.parameter(factory.real(), "offset", factory.realval(0.0))),
            (factory.noTemplates())));

        content->declarations.push_back(factory.classConstructor(
            isource,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "init_value", factory.realval(0.0)),
             factory.parameter(factory.real(), "offset", factory.realval(0.0)),
             factory.parameter(factory.real(), "amplitude", factory.realval(0.0)),
             factory.parameter(factory.real(), "frequency", factory.realval(0.0)),
             factory.parameter(factory.real(), "phase", factory.realval(0.0)),
             /// @todo Check correctness
             factory.parameter(new Time(), "delay", factory.noValue()),
             factory.parameter(factory.real(), "ac_amplitude", factory.realval(0.0)),
             factory.parameter(factory.real(), "ac_phase", factory.realval(0.0)),
             factory.parameter(factory.real(), "ac_noise_amplitude", factory.realval(0.0))),
            (factory.noTemplates())));

        lib->declarations.push_back(isource);
    }

    // Discrete Event Controlled Current Source
    {
        DesignUnit *isource = factory.designUnit(_makeHifName("sca_de_isource", hifFormat), "class", hif::cpp);
        View *view          = isource->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        Port *inp = factory.port(factory.real(), "inp", hif::dir_in);

        entity->ports.push_back(n);
        entity->ports.push_back(p);
        entity->ports.push_back(inp);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            isource,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "scale_", factory.realval(1.0))),
            (factory.noTemplates())));

        content->declarations.push_back(
            factory.classConstructor(isource, (factory.parameter(factory.string(), "nm")), (factory.noTemplates())));

        lib->declarations.push_back(isource);
    }

    // Discrete Event Controlled Current Source
    {
        DesignUnit *vsource = factory.designUnit(_makeHifName("sca_de_vsource", hifFormat), "class", hif::cpp);
        View *view          = vsource->views.front();

        // Interface of the component.
        Entity *entity = view->getEntity();
        Port *n = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "n", hif::dir_inout);
        Port *p = factory.port(factory.typeRef(_makeHifName("sca_terminal", hifFormat), nullptr), "p", hif::dir_inout);

        Port *inp = factory.port(factory.real(), "inp", hif::dir_in);

        entity->ports.push_back(n);
        entity->ports.push_back(p);
        entity->ports.push_back(inp);

        // Attributes of the component.
        hif::Contents *content = view->getContents();

        content->declarations.push_back(factory.classConstructor(
            vsource,
            (factory.parameter(factory.string(), "nm"),
             factory.parameter(factory.real(), "scale_", factory.realval(1.0))),
            (factory.noTemplates())));

        content->declarations.push_back(
            factory.classConstructor(vsource, (factory.parameter(factory.string(), "nm")), (factory.noTemplates())));

        lib->declarations.push_back(vsource);
    }
    return lib;
}
LibraryDef *SystemCSemantics::getTlmUtils(bool hifFormat)
{
    LibraryDef *lib = new LibraryDef();
    lib->setName(_makeHifName("tlm_utils", hifFormat));
    lib->setLanguageID(hif::tlm);
    lib->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // tlm_quantumkeeper
    {
        DesignUnit *du = factory.designUnit(
            _makeHifName("tlm_quantumkeeper", hifFormat),
            factory.view(
                "class", nullptr, nullptr, hif::tlm, factory.noDeclarations(), factory.noLibraries(),
                factory.noTemplates(), true));
        lib->declarations.push_back(du);

        // void	inc (const sc_core::sc_time &t)
        SubProgram *inc = factory.subprogram(
            factory.noType(), "inc", factory.noTemplates(), factory.parameter(factory.time(), "param1"));
        du->views.front()->declarations.push_back(inc);

        // void	set (const sc_core::sc_time &t)
        SubProgram *set = factory.subprogram(
            factory.noType(), "set", factory.noTemplates(), factory.parameter(factory.time(), "param1"));
        du->views.front()->declarations.push_back(set);

        // bool	need_sync()
        SubProgram *need_sync =
            factory.subprogram(factory.boolean(), "need_sync", factory.noTemplates(), factory.noParameters());
        du->views.front()->declarations.push_back(need_sync);

        // void	sync ()
        SubProgram *sync = factory.subprogram(factory.noType(), "sync", factory.noTemplates(), factory.noParameters());
        du->views.front()->declarations.push_back(sync);

        // void	set_and_sync (const sc_core::sc_time &t)
        SubProgram *setAndSync = factory.subprogram(
            factory.noType(), "set_and_sync", factory.noTemplates(), factory.parameter(factory.time(), "param1"));
        du->views.front()->declarations.push_back(setAndSync);

        // void	reset ()
        SubProgram *reset =
            factory.subprogram(factory.noType(), "reset", factory.noTemplates(), factory.noParameters());
        du->views.front()->declarations.push_back(reset);

        // sc_core::sc_time	get_current_time () const
        SubProgram *getCurrentTime =
            factory.subprogram(factory.time(), "get_current_time", factory.noTemplates(), factory.noParameters());
        du->views.front()->declarations.push_back(getCurrentTime);

        // sc_core::sc_time	get_local_time () const
        SubProgram *getLocalTime =
            factory.subprogram(factory.time(), "get_local_time", factory.noTemplates(), factory.noParameters());
        du->views.front()->declarations.push_back(getLocalTime);

        // static void set_global_quantum (const sc_core::sc_time &t)
        SubProgram *setGlobalQuantum = factory.subprogram(
            factory.noType(), "set_global_quantum", factory.noTemplates(), factory.parameter(factory.time(), "param1"));
        setGlobalQuantum->setKind(SubProgram::STATIC);
        du->views.front()->declarations.push_back(setGlobalQuantum);

        //static const sc_core::sc_time & get_global_quantum ()
        SubProgram *getGlobalQuantum =
            factory.subprogram(factory.time(), "get_local_time", factory.noTemplates(), factory.noParameters());
        getGlobalQuantum->setKind(SubProgram::STATIC);
        du->views.front()->declarations.push_back(getGlobalQuantum);
    }

    return lib;
}

LibraryDef *SystemCSemantics::getScDtPackage(bool hifFormat)
{
    LibraryDef *lib = new LibraryDef();
    lib->setName(_makeHifName("sc_dt", hifFormat));
    lib->setLanguageID(hif::rtl);
    lib->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    lib->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("is_01", hifFormat), factory.noTemplates(), factory.noParameters()));

    return lib;
}

LibraryDef *SystemCSemantics::getScCorePackage(bool hifFormat)
{
    LibraryDef *lib = new LibraryDef();
    lib->setName(_makeHifName("sc_core", hifFormat));
    lib->setLanguageID(hif::rtl);
    lib->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // sc_time_unit
    {
        const char *values[] = {"SC_FS", "SC_PS", "SC_NS", "SC_US", "SC_MS", "SC_SEC"};
        lib->declarations.push_back(_makeEnum("sc_time_unit", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // enum  sc_logic_value_t { Log_0 = 0, Log_1, Log_Z, Log_X }
    {
        const char *values[] = {"Log_0", "Log_1", "Log_Z", "Log_X"};
        lib->declarations.push_back(_makeEnum("sc_logic_value_t", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // sc_severity
    {
        const char *values[] = {"SC_INFO", "SC_WARNING", "SC_ERROR", "SC_FATAL", "SC_MAX_SEVERITY"};
        lib->declarations.push_back(_makeEnum("sc_severity", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // sc_actions
    {
        const char *values[] = {"SC_UNSPECIFIED  ", "SC_DO_NOTHING", "SC_THROW", "SC_LOG",  "SC_DISPLAY",
                                "SC_CACHE_REPORT",  "SC_INTERRUPT",  "SC_STOP",  "SC_ABORT"};
        lib->declarations.push_back(_makeEnum("sc_actions", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // SC_ZERO_TIME
    lib->declarations.push_back(
        factory.constant(factory.time(), _makeHifName("SC_ZERO_TIME", hifFormat), factory.timeval(0.0)));

    // sc_time_stamp
    lib->declarations.push_back(_makeAttribute("sc_time_stamp", factory.time(), nullptr, nullptr, false, hifFormat));

    // sc_time sc_get_time_resolution()
    lib->declarations.push_back(factory.subprogram(
        factory.time(), _makeHifName("sc_get_time_resolution", hifFormat), factory.noTemplates(),
        factory.noParameters()));

    // void sc_set_time_resolution(double, sc_time_unit)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_set_time_resolution", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1"),
         factory.parameter(factory.typeRef(_makeHifName("sc_time_unit", hifFormat)), "param2"))));

    // to_default_time_units
    lib->declarations.push_back(
        _makeAttribute("to_default_time_units", factory.real(), nullptr, nullptr, false, hifFormat));

    // to_double
    lib->declarations.push_back(_makeAttribute("to_double", factory.real(), nullptr, nullptr, false, hifFormat));

    // to_seconds
    lib->declarations.push_back(_makeAttribute("to_seconds", factory.real(), nullptr, nullptr, false, hifFormat));

    // sc_event
    {
        SubProgram *notify = factory.subprogram(
            factory.noType(), _makeHifName("notify", hifFormat), factory.noTemplates(),
            factory.parameter(factory.time(), "t", factory.timeval(0.0)));
        lib->declarations.push_back(notify);
    }

    // read
    SubProgram *sub = _makeAttribute("read", nullptr, nullptr, nullptr, false, hifFormat);
    sub->setKind(SubProgram::IMPLICIT_INSTANCE);
    lib->declarations.push_back(sub);

    // template< int W >
    // function range(int param1, int param2) return logic_vector<W>;
    // and
    // template< int W >
    // function range(int param1, int param2) return bit_vector<W>;
    {
        // sc_proxy_lv
        Bitvector *scProxyLv = factory.bitvector(
            factory.range(
                factory.expression(new Identifier("param1"), op_minus, new Identifier("param2")), dir_downto,
                factory.intval(0)),
            true, false, false, false, Type::SYSTEMC_BITVECTOR_PROXY);

        // sc_proxy_bv
        Bitvector *scProxyBv = factory.bitvector(
            factory.range(
                factory.expression(new Identifier("param1"), op_minus, new Identifier("param2")), dir_downto,
                factory.intval(0)),
            false, false, false, false, Type::SYSTEMC_BITVECTOR_PROXY);

        lib->declarations.push_back(factory.subprogram(
            scProxyLv, _makeHifName("range", hifFormat),
            //factory.templateValueParameter(factory.integer(), "left"),
            factory.noTemplates(),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.integer(), "param2", nullptr))));

        lib->declarations.push_back(factory.subprogram(
            scProxyBv, _makeHifName("range", hifFormat),
            //factory.templateValueParameter(factory.integer(), "left"),
            factory.noTemplates(),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.integer(), "param2", nullptr))));
    }

    // sc_clock
    {
        DesignUnit *du = factory.designUnit(
            _makeHifName("sc_clock", hifFormat), factory.view(
                                                     "class", nullptr, nullptr, hif::cpp, factory.noDeclarations(),
                                                     factory.noLibraries(), factory.noTemplates(), true));
        lib->declarations.push_back(du);

        du->views.front()->getContents()->declarations.push_back(
            factory.classConstructor(du, factory.noParameters(), factory.noTemplates()));
        du->views.front()->getContents()->declarations.push_back(
            factory.classConstructor(du, (factory.parameter(factory.string(), "param1")), factory.noTemplates()));
        du->views.front()->getContents()->declarations.push_back(factory.classConstructor(
            du,
            (factory.parameter(factory.string(), "param1"), factory.parameter(factory.time(), "param2"),
             factory.parameter(factory.real(), "param3", factory.realval(0.5)),
             factory.parameter(factory.time(), "param4", factory.identifier(_makeHifName("SC_ZERO_TIME", hifFormat))),
             factory.parameter(factory.boolean(), "param5", factory.boolval(true))),
            factory.noTemplates()));
        du->views.front()->getContents()->declarations.push_back(factory.classConstructor(
            du,
            (factory.parameter(factory.string(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(
                 factory.typeRef(_makeHifName("sc_time_unit", hifFormat), factory.noTemplateArguments()), "param3",
                 factory.boolval(true)),
             factory.parameter(factory.real(), "param4", factory.realval(0.5))),
            factory.noTemplates()));
        du->views.front()->getContents()->declarations.push_back(factory.classConstructor(
            du,
            (factory.parameter(factory.string(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(
                 factory.typeRef(_makeHifName("sc_time_unit", hifFormat), factory.noTemplateArguments()), "param3",
                 factory.boolval(true)),
             factory.parameter(factory.real(), "param4"), factory.parameter(factory.real(), "param5"),
             factory.parameter(
                 factory.typeRef(_makeHifName("sc_time_unit", hifFormat), factory.noTemplateArguments()), "param6",
                 factory.boolval(true)),
             factory.parameter(factory.boolean(), "param7", factory.boolval(true))),
            factory.noTemplates()));
        du->views.front()->getContents()->declarations.push_back(factory.classConstructor(
            du,
            (factory.parameter(factory.string(), "param1"), factory.parameter(factory.real(), "param2"),
             factory.parameter(factory.real(), "param3", factory.realval(0.5)),
             factory.parameter(factory.real(), "param4", factory.realval(0.0)),
             factory.parameter(factory.boolean(), "param5", factory.boolval(true))),
            factory.noTemplates()));
    }

    // event
    lib->declarations.push_back(_makeAttribute("event", factory.boolean(), nullptr, nullptr, false, hifFormat));

    // value_changed_event
    lib->declarations.push_back(
        _makeAttribute("value_changed_event", factory.event(), nullptr, nullptr, false, hifFormat));

    // posedge_event
    lib->declarations.push_back(_makeAttribute("posedge_event", factory.event(), nullptr, nullptr, false, hifFormat));
    // negedge_event
    lib->declarations.push_back(_makeAttribute("negedge_event", factory.event(), nullptr, nullptr, false, hifFormat));

    // to_string
    lib->declarations.push_back(_makeAttribute("to_string", factory.string(), nullptr, nullptr, false, hifFormat));

    // to_int
    lib->declarations.push_back(_makeAttribute("to_int", factory.integer(), nullptr, nullptr, false, hifFormat));

    // to_uint
    lib->declarations.push_back(
        _makeAttribute("to_uint", factory.integer(nullptr, false), nullptr, nullptr, false, hifFormat));

    // to_int64
    lib->declarations.push_back(
        _makeAttribute("to_int64", factory.integer(factory.range(63, 0)), nullptr, nullptr, false, hifFormat));

    // to_uint64
    lib->declarations.push_back(
        _makeAttribute("to_uint64", factory.integer(factory.range(63, 0), false), nullptr, nullptr, false, hifFormat));

    // to_bool
    lib->declarations.push_back(_makeAttribute("to_bool", factory.boolean(), nullptr, nullptr, false, hifFormat));

    // to_char
    lib->declarations.push_back(_makeAttribute("to_char", factory.character(), nullptr, nullptr, false, hifFormat));

    // or_reduce
    lib->declarations.push_back(_makeAttribute(
        "or_reduce", factory.typeRef(_makeHifName("sc_logic_value_t", hifFormat)), nullptr, nullptr, false, hifFormat));

    // and_reduce
    lib->declarations.push_back(_makeAttribute(
        "and_reduce", factory.typeRef(_makeHifName("sc_logic_value_t", hifFormat)), nullptr, nullptr, false,
        hifFormat));

    // xor_reduce
    lib->declarations.push_back(_makeAttribute(
        "xor_reduce", factory.typeRef(_makeHifName("sc_logic_value_t", hifFormat)), nullptr, nullptr, false,
        hifFormat));

    // nor_reduce
    lib->declarations.push_back(_makeAttribute(
        "nor_reduce", factory.typeRef(_makeHifName("sc_logic_value_t", hifFormat)), nullptr, nullptr, false,
        hifFormat));

    // nand_reduce
    lib->declarations.push_back(_makeAttribute(
        "nand_reduce", factory.typeRef(_makeHifName("sc_logic_value_t", hifFormat)), nullptr, nullptr, false,
        hifFormat));

    // xnor_reduce
    lib->declarations.push_back(_makeAttribute(
        "xnor_reduce", factory.typeRef(_makeHifName("sc_logic_value_t", hifFormat)), nullptr, nullptr, false,
        hifFormat));
    // void sc_start(sc_clock &clock, const sc_time &duration)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("sc_clock", hifFormat), "class"), "param1"),
         factory.parameter(factory.time(), "param2"))));
    // void sc_start(sc_clock &clock, double v, sc_time_unit tu)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("sc_clock", hifFormat), "class"), "param1"),
         factory.parameter(factory.real(), "param2"),
         factory.parameter(factory.typeRef(_makeHifName("sc_time_unit", hifFormat)), "param3"))));
    // void sc_start(sc_clock &clock, double duration=-1)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.viewRef(_makeHifName("sc_clock", hifFormat), "class"), "param1"),
         factory.parameter(factory.real(), "param2", factory.realval(-1)))));

    // void sc_start(const sc_time &duration)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.time(), "param1"))));
    // void sc_start()
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(), factory.noParameters()));
    // void sc_start(double duration)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1"))));
    // void sc_start(double duration, sc_time_unit tu)
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("sc_start", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.real(), "param1"),
         factory.parameter(factory.typeRef(_makeHifName("sc_time_unit", hifFormat)), "param2"))));
    // void sc_stop()
    lib->declarations.push_back(
        factory.subprogram(nullptr, _makeHifName("sc_stop", hifFormat), factory.noTemplates(), factory.noParameters()));

    // sc_report_handler
    {
        DesignUnit *du = factory.designUnit(
            _makeHifName("sc_report_handler", hifFormat),
            factory.view(
                "class", nullptr, nullptr, hif::cpp, factory.noDeclarations(), factory.noLibraries(),
                factory.noTemplates(), true));
        lib->declarations.push_back(du);

        // set_actions
        SubProgram *setAction1 = factory.subprogram(
            factory.typeRef(_makeHifName("sc_actions", hifFormat)), "set_actions", factory.noTemplates(),
            (factory.parameter(factory.string(), "param1"),
             factory.parameter(factory.typeRef(_makeHifName("sc_severity", hifFormat)), "param2"),
             factory.parameter(
                 factory.typeRef(_makeHifName("sc_actions", hifFormat)), "param3",
                 factory.identifier(_makeHifName("SC_DO_NOTHING", hifFormat)))));
        setAction1->setKind(SubProgram::STATIC);
        du->views.front()->declarations.push_back(setAction1);

        SubProgram *setAction2 = factory.subprogram(
            factory.typeRef(_makeHifName("sc_actions", hifFormat)), "set_actions", factory.noTemplates(),
            (factory.parameter(factory.string(), "param1"),
             factory.parameter(
                 factory.typeRef(_makeHifName("sc_actions", hifFormat)), "param2",
                 factory.identifier(_makeHifName("SC_DO_NOTHING", hifFormat)))));
        setAction2->setKind(SubProgram::STATIC);
        du->views.front()->declarations.push_back(setAction2);

        SubProgram *setAction3 = factory.subprogram(
            factory.typeRef(_makeHifName("sc_actions", hifFormat)), "set_actions", factory.noTemplates(),
            (factory.parameter(factory.typeRef(_makeHifName("sc_severity", hifFormat)), "param1"),
             factory.parameter(
                 factory.typeRef(_makeHifName("sc_actions", hifFormat)), "param2",
                 factory.identifier(_makeHifName("SC_DO_NOTHING", hifFormat)))));
        setAction3->setKind(SubProgram::STATIC);
        du->views.front()->declarations.push_back(setAction3);
    }

    // sc_trace_file
    {
        DesignUnit *du = factory.designUnit(
            _makeHifName("sc_trace_file", hifFormat), factory.view(
                                                          "class", nullptr, nullptr, hif::cpp, factory.noDeclarations(),
                                                          factory.noLibraries(), factory.noTemplates(), true));
        lib->declarations.push_back(du);
    }

    // sc_create_vcd_trace_file
    {
        SubProgram *createVcdTrace = factory.subprogram(
            factory.viewRef(_makeHifName("sc_trace_file", hifFormat), "class"),
            _makeHifName("sc_create_vcd_trace_file", hifFormat), factory.noTemplates(),
            factory.parameter(factory.string(), "param1"));
        createVcdTrace->setKind(SubProgram::STATIC);
        lib->declarations.push_back(createVcdTrace);
    }

    // hif_systemc_sc_close_vcd_trace_file
    lib->declarations.push_back(factory.subprogram(
        factory.noType(), _makeHifName("sc_close_vcd_trace_file", hifFormat), factory.noTemplates(),
        factory.parameter(factory.viewRef(_makeHifName("sc_trace_file", hifFormat), "class"), "param1")));

    // sc_trace
    lib->declarations.push_back(factory.subprogram(
        factory.noType(), _makeHifName("sc_trace", hifFormat), factory.templateTypeParameter(factory.noType(), "T"),
        (factory.parameter(factory.viewRef(_makeHifName("sc_trace_file", hifFormat), "class"), "param1"),
         factory.parameter(factory.typeRef("T"), "param2"), factory.parameter(factory.string(), "param3"))));

    return lib;
}
LibraryDef *SystemCSemantics::getTlmPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("tlm", hifFormat));
    ld->setLanguageID(hif::tlm);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // This trick avoids to re-define SystemC-like standard methods,
    // and therefore avoids ambiguity.
    Library *lib = factory.library(_makeHifName("sc_core", hifFormat), nullptr, "systemc", false, true);
    ld->libraries.push_back(lib);

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // enum tlm_command
    {
        const char *values[] = {"TLM_READ_COMMAND", "TLM_WRITE_COMMAND", "TLM_IGNORE_COMMAND"};
        ld->declarations.push_back(_makeEnum("tlm_command", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // enum tlm_response_status
    {
        const char *values[] = {
            "TLM_OK_RESPONSE",
            "TLM_INCOMPLETE_RESPONSE",
            "TLM_GENERIC_ERROR_RESPONSE",
            "TLM_ADDRESS_ERROR_RESPONSE",
            "TLM_COMMAND_ERROR_RESPONSE",
            "TLM_BURST_ERROR_RESPONSE",
            "TLM_BYTE_ENABLE_ERROR_RESPONSE"};
        ld->declarations.push_back(
            _makeEnum("tlm_response_status", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // enum tlm_sync_enum
    {
        const char *values[] = {"TLM_ACCEPTED", "TLM_UPDATED", "TLM_COMPLETED"};
        ld->declarations.push_back(_makeEnum("tlm_sync_enum", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // enum tlm_phase
    {
        const char *values[] = {"UNINITIALIZED_PHASE", "BEGIN_REQ", "END_REQ", "BEGIN_RESP", "END_RESP"};
        ld->declarations.push_back(_makeEnum("tlm_phase", values, sizeof(values) / sizeof(char *), hifFormat));
    }

    // tlm_dmi workaround (use a typedef of integer)
    {
        TypeDef *tlmDmi = new TypeDef();
        tlmDmi->setType(factory.integer());
        tlmDmi->setName(_makeHifName("tlm_dmi", hifFormat));
        tlmDmi->setOpaque(true);
        ld->declarations.push_back(tlmDmi);
    }

    // ///////////////////////////////////////////////////////////////////
    // classes
    // ///////////////////////////////////////////////////////////////////

    // class tlm_bw_transport_if
    {
        DesignUnit *tlmBwIf = factory.designUnit(_makeHifName("tlm_bw_transport_if", hifFormat), "class", hif::cpp);

        // add fake template parameter
        View *view = tlmBwIf->views.front();
        view->templateParameters.push_back(factory.templateValueParameter(
            factory.integer(factory.range(31, 0)), "_fake", factory.intval(0, factory.integer())));
        ld->declarations.push_back(tlmBwIf);
    }

    // class tlm_fw_transport_if
    {
        DesignUnit *tlmFwIf = factory.designUnit(_makeHifName("tlm_fw_transport_if", hifFormat), "class", hif::cpp);

        // add fake template parameter
        View *view = tlmFwIf->views.front();
        view->templateParameters.push_back(factory.templateValueParameter(
            factory.integer(factory.range(31, 0)), "_fake", factory.intval(0, factory.integer())));
        ld->declarations.push_back(tlmFwIf);
    }

    // class tlm_generic_payload
    {
        DesignUnit *tlmGenericPayload =
            factory.designUnit(_makeHifName("tlm_generic_payload", hifFormat), "class", hif::cpp);
        ld->declarations.push_back(tlmGenericPayload);
        Contents *payloadContents = tlmGenericPayload->views.front()->getContents();

        // bool is_write()
        SubProgram *isWrite =
            factory.subprogram(factory.boolean(), "is_write", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(isWrite);

        // bool is_read()
        SubProgram *isRead =
            factory.subprogram(factory.boolean(), "is_read", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(isRead);

        // void set_write()
        SubProgram *setWrite = factory.subprogram(nullptr, "set_write", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(setWrite);

        // void set_read()
        SubProgram *setRead = factory.subprogram(nullptr, "set_read", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(setRead);

        // unsigned char* get_data_ptr()
        SubProgram *getDataPtr = factory.subprogram(
            factory.pointer(factory.unsignedChar()), "get_data_ptr", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(getDataPtr);

        // void set_data_ptr(unsigned char* data)
        SubProgram *setDataPtr = factory.subprogram(
            factory.noType(), "set_data_ptr", factory.noTemplates(),
            factory.parameter(factory.pointer(factory.unsignedChar()), "data", factory.noValue()));
        payloadContents->declarations.push_back(setDataPtr);

        // void set_response_status(tlm_response_status response_status)
        SubProgram *setResponseStatus = factory.subprogram(
            factory.noType(), "set_response_status", factory.noTemplates(),
            factory.parameter(
                factory.typeRef(_makeHifName("tlm_response_status", hifFormat)), "response_status", factory.noValue()));
        payloadContents->declarations.push_back(setResponseStatus);

        //void set_dmi_allowed( bool );
        SubProgram *setDmiAllowed = factory.subprogram(
            factory.noType(), "set_dmi_allowed", factory.noTemplates(), factory.parameter(factory.boolean(), "param1"));
        payloadContents->declarations.push_back(setDmiAllowed);

        //bool is_dmi_allowed() const;
        SubProgram *isDmiAllowed =
            factory.subprogram(factory.boolean(), "is_dmi_allowed", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(isDmiAllowed);

        // tlm_command get_command() const;
        SubProgram *getCommand = factory.subprogram(
            factory.typeRef(_makeHifName("tlm_command", hifFormat)), "get_command", factory.noTemplates(),
            factory.noParameters());
        payloadContents->declarations.push_back(getCommand);

        // void set_command( const tlm_command );
        SubProgram *setCommand = factory.subprogram(
            factory.noType(), "set_command", factory.noTemplates(),
            factory.parameter(factory.typeRef(_makeHifName("tlm_command", hifFormat)), "param1"));
        payloadContents->declarations.push_back(setCommand);

        // sc_dt::uint64 get_address() const;
        SubProgram *getAddress = factory.subprogram(
            factory.integer(factory.range(63, 0), false), "get_address", factory.noTemplates(), factory.noParameters());
        payloadContents->declarations.push_back(getAddress);

        // void set_address( const sc_dt::uint64 );
        SubProgram *setAddress = factory.subprogram(
            factory.noType(), "set_address", factory.noTemplates(),
            factory.parameter(factory.integer(factory.range(63, 0), false), "param1"));
        payloadContents->declarations.push_back(setAddress);
    }

    // class tlm_target_socket
    {
        DesignUnit *tlmTargetSocket =
            factory.designUnit(_makeHifName("tlm_target_socket", hifFormat), "class", hif::tlm);

        // add fake template parameter
        View *view = tlmTargetSocket->views.front();
        view->templateParameters.push_back(factory.templateValueParameter(
            factory.integer(factory.range(31, 0)), "_fake", factory.intval(0, factory.integer())));

        /*// tlm_target socket template parameters (BUSWIDTH and TYPES)
        ValueTP* buswidthTP = new ValueTP();
        buswidthTP->setName("BUSWIDTH");
        buswidthTP->SetType(factory.integer(nullptr, false));
        buswidthTP->setValue(factory.intval(32));
        tlmTargetSocket->views.front()->templateParameters.push_back(buswidthTP);
        TypeTP* typesTP = new TypeTP();
        typesTP->setName("TYPES");
        typesTP->SetDefault(factory.typeRef("tlm_base_protocol_types"));
        tlmTargetSocket->views.front()->templateParameters.push_back(typesTP);*/
        ld->declarations.push_back(tlmTargetSocket);

        // nb_transport_bw(trans, phase, t) function
        SubProgram *nbTransBw = factory.subprogram(
            factory.typeRef(_makeHifName("tlm_sync_enum", hifFormat)), "nb_transport_bw", factory.noTemplates(),
            (factory.parameter(
                 factory.viewRef(_makeHifName("tlm_generic_payload", hifFormat), "class"), "trans", factory.noValue()),
             factory.parameter(factory.typeRef(_makeHifName("tlm_phase", hifFormat)), "phase", factory.noValue()),
             factory.parameter(new Time(), "t", factory.noValue())));
        tlmTargetSocket->views.front()->getContents()->declarations.push_back(nbTransBw);
    }

    // class tlm_initiator_socket
    {
        DesignUnit *tlmInitiatorSocket =
            factory.designUnit(_makeHifName("tlm_initiator_socket", hifFormat), "class", hif::tlm);

        // add fake template parameter
        View *view = tlmInitiatorSocket->views.front();
        view->templateParameters.push_back(factory.templateValueParameter(
            factory.integer(factory.range(31, 0)), "_fake", factory.intval(0, factory.integer())));

        ld->declarations.push_back(tlmInitiatorSocket);

        // b_transport(trans, phase, t) function
        SubProgram *bTrans = factory.subprogram(
            factory.typeRef(_makeHifName("tlm_sync_enum", hifFormat)), "b_transport", factory.noTemplates(),
            (factory.parameter(
                 factory.viewRef(_makeHifName("tlm_generic_payload", hifFormat), "class"), "param1", factory.noValue()),
             factory.parameter(new Time(), "param2", factory.noValue())));
        tlmInitiatorSocket->views.front()->getContents()->declarations.push_back(bTrans);

        // nb_transport_fw(trans, phase, t) function
        SubProgram *nbTrans_fw = factory.subprogram(
            factory.typeRef(_makeHifName("tlm_sync_enum", hifFormat)), "nb_transport_fw", factory.noTemplates(),
            (factory.parameter(
                 factory.viewRef(_makeHifName("tlm_generic_payload", hifFormat), "class"), "param1", factory.noValue()),
             factory.parameter(factory.typeRef(_makeHifName("tlm_phase", hifFormat)), "param2"),
             factory.parameter(new Time(), "param3", factory.noValue())));
        tlmInitiatorSocket->views.front()->getContents()->declarations.push_back(nbTrans_fw);
    }

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    return ld;
}
LibraryDef *SystemCSemantics::getCMathPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("cmath", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);

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

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    //double abs (double x);
    ld->declarations.push_back(
        _makeAttribute("abs", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  ACOS (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("acos", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function ACOSH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("acosh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  ASIN (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("asin", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function ASINH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("asinh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  ATAN (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("atan", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function ATANH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("atanh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  ATAN2 (X : real; Y : real) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "atan2", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false,
        hifFormat));

    //function CBRT (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("cbrt", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function CEIL (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("ceil", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function COS ( X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("cos", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  COSH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("cosh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function EXP  (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("exp", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function FLOOR (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("floor", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function FMAX (X, Y : real ) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "fmax", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false,
        hifFormat));

    //function FMIN (X, Y : real ) return real;
    ld->declarations.push_back(_makeBinaryAttribute(
        "fmin", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false,
        hifFormat));

    //function LOG (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("log", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function LOG2 (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("log2", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function LOG10 (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("log10", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //double pow (double x);
    ld->declarations.push_back(_makeBinaryAttribute(
        "pow", factory.real(), factory.real(), factory.noValue(), factory.real(), factory.noValue(), false, hifFormat));

    //function ROUND (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("round", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function SINH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("sinh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  SIN (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("sin", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function SQRT (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("sqrt", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  TAN (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("tan", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function  TANH (X : real) return real;
    ld->declarations.push_back(
        _makeAttribute("tanh", factory.real(), factory.real(), factory.noValue(), false, hifFormat));

    //function TRUNC (X : real ) return real;
    ld->declarations.push_back(
        _makeAttribute("trunc", factory.real(), factory.real(), factory.noValue(), false, hifFormat));
    return ld;
}
LibraryDef *SystemCSemantics::getCTimePackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ctime", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Record *emptyStruct = new Record();

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // clock_t
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("clock_t", hifFormat), hif::copy(emptyStruct), true, nullptr));

    // size_t
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("size_t", hifFormat), factory.integer(nullptr, false, true), false, nullptr));

    // time_t
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("time_t", hifFormat), hif::copy(emptyStruct), true, nullptr));

    // struct tm
    {
        // tm_sec  int seconds after the minute    0-60*
        // tm_min  int minutes after the hour  0-59
        // tm_hour int hours since midnight    0-23
        // tm_mday int day of the month    1-31
        // tm_mon  int months since January    0-11
        // tm_year int years since 1900
        // tm_wday int days since Sunday   0-6
        // tm_yday int days since January 1    0-365
        // tm_isdst    int Daylight Saving Time flag
        // * tm_sec is generally 0-59. The extra range is to accommodate for leap seconds in certain systems.
        // The Daylight Saving Time flag (tm_isdst) is greater than zero if Daylight Saving Time is in effect,
        // zero if Daylight Saving Time is not in effect, and less than zero if the information is not available.

        Record *tm = new Record();
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_sec", factory.intval(0),
            new Range(new IntValue(0), new IntValue(59), dir_upto)));
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_min", factory.intval(0),
            new Range(new IntValue(0), new IntValue(59), dir_upto)));
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_hour", factory.intval(0),
            new Range(new IntValue(0), new IntValue(23), dir_upto)));
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_mday", factory.intval(1),
            new Range(new IntValue(1), new IntValue(31), dir_upto)));
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_mon", factory.intval(0),
            new Range(new IntValue(0), new IntValue(11), dir_upto)));
        tm->fields.push_back(
            factory.field(factory.integer(nullptr, true, true), "tm_year", factory.intval(1900), nullptr));
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_wday", factory.intval(0),
            new Range(new IntValue(0), new IntValue(6), dir_upto)));
        tm->fields.push_back(factory.field(
            factory.integer(nullptr, true, true), "tm_yday", factory.intval(1),
            new Range(new IntValue(0), new IntValue(365), dir_upto)));
        tm->fields.push_back(
            factory.field(factory.integer(nullptr, true, true), "tm_isdst", factory.intval(1), nullptr));

        ld->declarations.push_back(factory.typeDef(_makeHifName("tm", hifFormat), tm, true, nullptr));
    }

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // CLOCKS_PER_SEC
    {
        Const *cps = factory.constant(
            factory.typeRef(_makeHifName("clock_t", hifFormat)), _makeHifName("CLOCKS_PER_SEC", hifFormat),
            factory.recordval(factory.noRecordValueAlts()), nullptr);
        cps->setDefine(true);
        ld->declarations.push_back(cps);
    }

    // nullptr is not re-defined

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // clock_t clock (void);
    ld->declarations.push_back(_makeAttribute(
        "clock", factory.typeRef(_makeHifName("clock_t", hifFormat)), nullptr, nullptr, false, hifFormat));

    // double difftime (time_t end, time_t beginning);
    ld->declarations.push_back(_makeBinaryAttribute(
        "difftime", factory.real(nullptr, true), factory.typeRef(_makeHifName("time_t", hifFormat)), factory.noValue(),
        factory.typeRef(_makeHifName("time_t", hifFormat)), factory.noValue(), false, hifFormat));

    // time_t mktime (struct tm * timeptr);
    ld->declarations.push_back(_makeAttribute(
        "mktime", factory.typeRef(_makeHifName("time_t", hifFormat)),
        factory.pointer(factory.typeRef(_makeHifName("tm", hifFormat))), factory.noValue(), false, hifFormat));

    // time_t time (time_t* timer);
    ld->declarations.push_back(_makeAttribute(
        "time", factory.typeRef(_makeHifName("time_t", hifFormat)),
        factory.pointer(factory.typeRef(_makeHifName("time_t", hifFormat))), factory.noValue(), false, hifFormat));

    // char* asctime (const struct tm * timeptr);
    ld->declarations.push_back(_makeAttribute(
        "asctime", factory.pointer(factory.character(true)),
        factory.pointer(factory.typeRef(_makeHifName("tm", hifFormat))), factory.noValue(), false, hifFormat));

    // char* ctime (const time_t * timer);
    ld->declarations.push_back(_makeAttribute(
        "ctime", factory.pointer(factory.character(true)),
        factory.pointer(factory.typeRef(_makeHifName("tm", hifFormat))), factory.noValue(), false, hifFormat));

    // struct tm * gmtime (const time_t * timer);
    ld->declarations.push_back(_makeAttribute(
        "gmtime", factory.pointer(factory.character(true)),
        factory.pointer(factory.typeRef(_makeHifName("tm", hifFormat))), factory.noValue(), false, hifFormat));

    // struct tm * localtime (const time_t * timer);
    ld->declarations.push_back(_makeAttribute(
        "localtime", factory.pointer(factory.character(true)),
        factory.pointer(factory.typeRef(_makeHifName("tm", hifFormat))), factory.noValue(), false, hifFormat));

    // size_t strftime (char* ptr, size_t maxsize, const char* format, const struct tm* timeptr );
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("size_t", hifFormat)), _makeHifName("strftime", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.pointer(factory.character(true)), "param1", nullptr, nullptr),
         factory.parameter(factory.typeRef(_makeHifName("size_t", hifFormat)), "param2", nullptr, nullptr),
         factory.parameter(factory.pointer(factory.character(true)), "param3", nullptr, nullptr),
         factory.parameter(
             factory.pointer(factory.typeRef(_makeHifName("tm", hifFormat))), "param4", nullptr, nullptr))));

    delete emptyStruct;
    return ld;
}
LibraryDef *SystemCSemantics::getCStdLibPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("cstdlib", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);
    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // EXIT_FAILURE
    Const *exitFailure = factory.constant(
        factory.integer(nullptr, true, false), _makeHifName("EXIT_FAILURE", hifFormat),
        factory.intval(1, factory.integer(nullptr, true, true)), nullptr);
    ld->declarations.push_back(exitFailure);

    // EXIT_SUCCESS
    Const *exitSuccess = factory.constant(
        factory.integer(nullptr, true, false), _makeHifName("EXIT_SUCCESS", hifFormat),
        factory.intval(1, factory.integer(nullptr, true, true)), nullptr);
    ld->declarations.push_back(exitSuccess);

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    //int abs (int x);
    ld->declarations.push_back(
        _makeAttribute("abs", factory.integer(), factory.integer(), factory.noValue(), false, hifFormat));

    //function SRAND (seed: in integer ) return integer;
    ld->declarations.push_back(
        _makeAttribute("srand", factory.integer(), factory.integer(), factory.noValue(), false, hifFormat));

    //function RAND return integer;
    ld->declarations.push_back(_makeAttribute("rand", factory.integer(), nullptr, factory.noValue(), false, hifFormat));

    // void exit(int)
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("exit", hifFormat), factory.noTemplates(),
        factory.parameter(
            factory.integer(nullptr, true, true), "param1", factory.intval(0, factory.integer(nullptr, true, true)))));

    // void abort()
    ld->declarations.push_back(
        factory.subprogram(nullptr, _makeHifName("abort", hifFormat), factory.noTemplates(), factory.noParameters()));

    // void * malloc(size_t size);
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.viewRef(
            _makeHifName("void", hifFormat), "class",
            factory.library(_makeHifName("standard", hifFormat), nullptr, nullptr, true, true))),
        _makeHifName("malloc", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.integer(nullptr, true, true), "param1"))));

    // void free(void * ptr);
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("free", hifFormat), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("T"), "param1"))));

    return ld;
}
LibraryDef *SystemCSemantics::getVectorPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("vector", hifFormat));
    ld->setLanguageID(hif::cpp);
    ld->setStandard(true);

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

    // std::vector<T>
    {
        SubProgram *at = factory.subprogram(
            factory.typeRef("T"), "at", factory.noTemplates(),
            factory.parameter(
                factory.integer(nullptr, false, true), // originally, size_type
                "param1",                              // originally, n
                factory.intval(0, factory.integer(nullptr, false, true))));
        SubProgram *size = factory.subprogram(
            factory.integer(nullptr, false, true), "size", factory.noTemplates(), factory.noParameters());

        DesignUnit *vector = factory.designUnit(
            _makeHifName("vector", hifFormat),
            factory.view(
                "cpp",
                factory.contents(
                    new GlobalAction(), factory.noDeclarations(), factory.noGenerates(), factory.noInstances(),
                    factory.noStateTables(), factory.noLibraries()),
                new Entity(), hif::cpp, factory.noDeclarations(), factory.noLibraries(),
                factory.templateTypeParameter(nullptr, "T"), true));

        vector->views.front()->getContents()->declarations.push_back(at);
        vector->views.front()->getContents()->declarations.push_back(size);
        ld->declarations.push_back(vector);
    }

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////
    return ld;
}
LibraryDef *SystemCSemantics::getStringPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("string", hifFormat));
    ld->setLanguageID(hif::cpp);
    ld->setStandard(true);

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

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // string c_str()
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.character()), _makeHifName("c_str", hifFormat), factory.noTemplates(),
        factory.noParameters()));

    // string size()
    ld->declarations.push_back(factory.subprogram(
        factory.integer(nullptr, false), _makeHifName("size", hifFormat), factory.noTemplates(),
        factory.noParameters()));

    // string substr(start, size)
    ld->declarations.push_back(factory.subprogram(
        factory.string(), _makeHifName("substr", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.integer(), "param1"),
         factory.parameter(factory.integer(), "param2", factory.intval(-1)))));

    // string& replace (size_t pos,  size_t len,  const string& str)
    ld->declarations.push_back(factory.subprogram(
        factory.string(), _makeHifName("replace", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.integer(), "param1"), factory.parameter(factory.integer(), "param2"),
         factory.parameter(factory.string(), "param3"))));

    // constructor
    ld->declarations.push_back(factory.subprogram(
        factory.string(), _makeHifName(hif::NameTable::getInstance()->hifConstructor(), hifFormat),
        factory.noTemplates(), (factory.parameter(factory.pointer(factory.character()), "param1"))));
    return ld;
}

LibraryDef *SystemCSemantics::getCStringPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("cstring", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // size_t
    ld->declarations.push_back(
        factory.typeDef(_makeHifName("size_t", hifFormat), factory.integer(nullptr, false, true), false, nullptr));

    // size_t strlen(const char * s)
    ld->declarations.push_back(_factory.subprogram(
        factory.typeRef(_makeHifName("size_t", hifFormat)), _makeHifName("strlen", hifFormat), _factory.noTemplates(),
        _factory.parameter(factory.pointer(_factory.character()), "param1")));

    // int strcmp(const char * s1, const char * s2)
    ld->declarations.push_back(_factory.subprogram(
        factory.integer(), _makeHifName("strcmp", hifFormat), _factory.noTemplates(),
        (_factory.parameter(factory.pointer(_factory.character()), "param1"),
         _factory.parameter(factory.pointer(_factory.character()), "param2"))));

    // char * strcpy(char * s1, const char * s2)
    ld->declarations.push_back(_factory.subprogram(
        factory.pointer(factory.character()), _makeHifName("strcpy", hifFormat), _factory.noTemplates(),
        (_factory.parameter(factory.pointer(_factory.character()), "param1"),
         _factory.parameter(factory.pointer(_factory.character()), "param2"))));

    return ld;
}

LibraryDef *SystemCSemantics::getCStdDefPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("cstddef", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());
    return ld;
}
LibraryDef *SystemCSemantics::getCStdIOPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("cstdio", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    Type *filePointer = factory.pointer(factory.file(factory.string()));

    // ///////////////////////////////////////////////////////////////////
    // variables
    // ///////////////////////////////////////////////////////////////////

    ld->declarations.push_back(factory.variable(
        hif::copy(filePointer), _makeHifName("stdin", hifFormat), factory.nullval(hif::copy(filePointer), false)));

    ld->declarations.push_back(factory.variable(
        hif::copy(filePointer), _makeHifName("stdout", hifFormat), factory.nullval(hif::copy(filePointer), false)));

    ld->declarations.push_back(factory.variable(
        hif::copy(filePointer), _makeHifName("stderr", hifFormat), factory.nullval(hif::copy(filePointer), false)));

    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // int printf(string format, string parameter)
    _addMultiparamFunction(ld, "printf", factory, hifFormat, factory.integer());

    // int fprintf(File * f, string format, string parameter)
    _addMultiparamFunction(ld, "fprintf", factory, hifFormat, factory.integer());

    // File * fdopen(int fd, char * mode)
    ld->declarations.push_back(factory.subprogram(
        hif::copy(filePointer), _makeHifName("fdopen", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.integer(), "param1"), factory.parameter(factory.string(), "param2"))));

    // int fflush(File * file)
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("fflush", hifFormat), factory.noTemplates(),
        factory.parameter(hif::copy(filePointer), "param1")));

    // int fclose(File * file);
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("fclose", hifFormat), factory.noTemplates(),
        factory.parameter(hif::copy(filePointer), "param1")));

    // File * fopen(char * filename, char * mode);
    ld->declarations.push_back(factory.subprogram(
        hif::copy(filePointer), _makeHifName("fopen", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.string(), "param1"), factory.parameter(factory.string(), "param2"))));

    // int fileno(File * file);
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("fileno", hifFormat), factory.noTemplates(),
        factory.parameter(hif::copy(filePointer), "param1")));

    // int feof(File * file);
    ld->declarations.push_back(factory.subprogram(
        factory.integer(), _makeHifName("feof", hifFormat), factory.noTemplates(),
        factory.parameter(hif::copy(filePointer), "param1")));

    // int scanf(string format, string parameter)
    _addMultiparamFunction(ld, "scanf", factory, hifFormat, factory.integer());

    // int fscanf(File * f, string format, string parameter)
    _addMultiparamFunction(ld, "fscanf", factory, hifFormat, factory.integer());

    delete filePointer;
    return ld;
}

LibraryDef *SystemCSemantics::getSystemcExtensionsPackage(bool hifFormat)
{
    LibraryDef *lib = new LibraryDef();
    lib->setName(_makeHifName("hif_systemc_extensions", hifFormat));
    lib->setLanguageID(hif::rtl);
    lib->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // hif_equals
    lib->declarations.push_back(_makeBinaryAttribute(
        "hif_equals", factory.boolean(), factory.typeRef("T"), nullptr, factory.typeRef("T"), nullptr, false,
        hifFormat));

    // hif_mod
    lib->declarations.push_back(_makeBinaryAttribute(
        "hif_mod", factory.integer(factory.range(63, 0)), factory.integer(factory.range(63, 0)), nullptr,
        factory.integer(factory.range(63, 0)), nullptr, false, hifFormat));

    // hif_xorrd
    lib->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("hif_xorrd", hifFormat), factory.templateTypeParameter(nullptr, "T"),
        factory.parameter(factory.typeRef("T"), "param1")));

    // HifAggregateArray
    {
        Array *t = factory.array(
            factory.range(
                factory.expression(new Identifier("size"), op_minus, factory.intval(1)), dir_downto, new IntValue(0)),
            factory.typeRef("T"));

        DesignUnit *du = _makeHifAggregateDU(factory, "HifAggregateArray", t, factory.typeRef("T"), hifFormat);
        lib->declarations.push_back(du);
    }

    // HifAggregateBitVector
    {
        Bitvector *t = factory.bitvector(
            factory.range(
                factory.expression(new Identifier("size"), op_minus, factory.intval(1)), dir_downto, new IntValue(0)),
            false, false, false);

        DesignUnit *du = _makeHifAggregateDU(factory, "HifAggregateBitVector", t, factory.boolean(), hifFormat);
        lib->declarations.push_back(du);
    }

    // HifAggregateLogicVector
    {
        Bitvector *t = factory.bitvector(
            factory.range(
                factory.expression(new Identifier("size"), op_minus, factory.intval(1)), dir_downto, new IntValue(0)),
            true, false, false);

        DesignUnit *du =
            _makeHifAggregateDU(factory, "HifAggregateLogicVector", t, factory.bit(true, false), hifFormat);
        lib->declarations.push_back(du);
    }

    // HifAggregateHlBv
    {
        Bitvector *t = factory.bitvector(
            factory.range(
                factory.expression(new Identifier("size"), op_minus, factory.intval(1)), dir_downto, new IntValue(0)),
            false, false, false);

        DesignUnit *du = _makeHifAggregateDU(factory, "HifAggregateHlBv", t, factory.boolean(), hifFormat);
        lib->declarations.push_back(du);
    }

    // HifAggregateHlLv
    {
        Bitvector *t = factory.bitvector(
            factory.range(
                factory.expression(new Identifier("size"), op_minus, factory.intval(1)), dir_downto, new IntValue(0)),
            true, false, false);

        DesignUnit *du = _makeHifAggregateDU(factory, "HifAggregateHlLv", t, factory.bit(true, false), hifFormat);
        lib->declarations.push_back(du);
    }

    // ArrayConcat
    {
        View *v = factory.view(
            "cpp",
            factory.contents(
                nullptr, factory.noDeclarations(), factory.noGenerates(), factory.noInstances(),
                factory.noStateTables(), factory.noLibraries()),
            nullptr, hif::cpp, factory.noDeclarations(), factory.noLibraries(),
            (factory.templateTypeParameter(nullptr, "T")), false);

        DesignUnit *du = factory.designUnit(_makeHifName("ArrayConcat", hifFormat), v);

        Function *ctor = factory.classConstructor(du, factory.noParameters(), factory.noTemplates());
        du->views.front()->getContents()->declarations.push_front(ctor);
        lib->declarations.push_back(du);

        Array *retTypeArrays = factory.array(
            factory.range(
                factory.expression(
                    factory.expression(factory.identifier("s1"), op_plus, factory.identifier("s2")), op_minus,
                    factory.intval(1)),
                dir_downto, new IntValue(0)),
            factory.typeRef("T"));

        // template<int Size1, int Size2, typename T1, typename T2>
        // RetElementType * concatArrays(T1 * p1, T2 * p2);
        v->getContents()->declarations.push_back(factory.subprogram(
            retTypeArrays, "concatArrays",
            (factory.templateValueParameter(factory.integer(), "s1"),
             factory.templateValueParameter(factory.integer(), "s2"), factory.templateTypeParameter(nullptr, "T1"),
             factory.templateTypeParameter(nullptr, "T2")),
            (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"))));

        Array *retTypeArrayWithValue =
            factory.array(factory.range(factory.identifier("s1"), dir_downto, new IntValue(0)), factory.typeRef("T"));

        // template<int Size1, typename T1, typename T2>
        // RetElementType * concatArrayWithValue(T1 * p1, T2 * p2);
        v->getContents()->declarations.push_back(factory.subprogram(
            retTypeArrayWithValue, "concatArrayWithValue",
            (factory.templateValueParameter(factory.integer(), "s1"), factory.templateTypeParameter(nullptr, "T1"),
             factory.templateTypeParameter(nullptr, "T2")),
            (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"))));

        Array *retTypeValueWithArray =
            factory.array(factory.range(factory.identifier("s2"), dir_downto, new IntValue(0)), factory.typeRef("T"));

        // template<int Size1, typename T1, typename T2>
        // RetElementType * concatValueWithArray(T1 * p1, T2 * p2);
        v->getContents()->declarations.push_back(factory.subprogram(
            retTypeValueWithArray, "concatValueWithArray",
            (factory.templateValueParameter(factory.integer(), "s2"), factory.templateTypeParameter(nullptr, "T1"),
             factory.templateTypeParameter(nullptr, "T2")),
            (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"))));
    }

    // hif_assign
    // template< typename T, typename S >
    // void hif2sc_assign( T target, S source,
    //                      unsigned int size = 0,
    //                      unsigned int left1 = 0, unsigned int right1 = 0
    //                      unsigned int left2 = 0, unsigned int right2 = 0 );
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hif_assign", hifFormat),
        (factory.templateTypeParameter(nullptr, "T"), factory.templateTypeParameter(nullptr, "S")),
        (factory.parameter(factory.typeRef("T"), "target"), factory.parameter(factory.typeRef("S"), "source"),
         factory.parameter(
             factory.integer(nullptr, false, true), "size", factory.intval(0, factory.integer(nullptr, false, true))),
         factory.parameter(
             factory.integer(nullptr, false, true), "left1", factory.intval(0, factory.integer(nullptr, false, true))),
         factory.parameter(
             factory.integer(nullptr, false, true), "right1", factory.intval(0, factory.integer(nullptr, false, true))),
         factory.parameter(
             factory.integer(nullptr, false, true), "left2", factory.intval(0, factory.integer(nullptr, false, true))),
         factory.parameter(
             factory.integer(nullptr, false, true), "right2",
             factory.intval(0, factory.integer(nullptr, false, true))))));

    // hif_lastValue
    // template< typename T >
    // T hif_lastValue(T & s, T & last, T & prev)
    lib->declarations.push_back(factory.subprogram(
        factory.typeRef("T"), _makeHifName("hif_lastValue", hifFormat), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("T"), "param1", nullptr),
         factory.parameter(factory.typeRef("T"), "param2", nullptr),
         factory.parameter(factory.typeRef("T"), "param3", nullptr))));

    // hif_lastValue_var
    // template< typename T >
    // T hif_lastValue(const T & s, T & last, T & prev)
    lib->declarations.push_back(factory.subprogram(
        factory.typeRef("T"), _makeHifName("hif_lastValue_var", hifFormat),
        (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("T"), "param1", nullptr),
         factory.parameter(factory.typeRef("T"), "param2", nullptr),
         factory.parameter(factory.typeRef("T"), "param3", nullptr))));

    // template< int size >
    // function SXT(ARG: STD_LOGIC_VECTOR) return STD_LOGIC_VECTOR<size>;
    {
        Range *retTypeSpan = factory.range(
            factory.expression(new Identifier("param2"), op_minus, factory.intval(1)), dir_downto, new IntValue(0));
        Bitvector *std_logic_vector = factory.bitvector(retTypeSpan, true, false);

        lib->declarations.push_back(factory.subprogram(
            std_logic_vector, _makeHifName("hif_sxt", hifFormat),
            (factory.templateValueParameter(factory.integer(), "param2"), factory.templateTypeParameter(nullptr, "T")),
            factory.parameter(factory.typeRef("T"), "param1", nullptr)));
    }

    // Relationals on logics
    {
        // sc_logic hif__op_lt(logic, logic)
        lib->declarations.push_back(factory.subprogram(
            factory.bit(true, false), _makeHifName("hif__op_lt", hifFormat), factory.noTemplates(),
            (factory.parameter(factory.bit(true, false), "param1"),
             factory.parameter(factory.bit(true, false), "param2"))));

        // sc_logic hif__op_gt(logic, logic)
        lib->declarations.push_back(factory.subprogram(
            factory.bit(true, false), _makeHifName("hif__op_gt", hifFormat), factory.noTemplates(),
            (factory.parameter(factory.bit(true, false), "param1"),
             factory.parameter(factory.bit(true, false), "param2"))));

        // sc_logic hif__op_le(logic, logic)
        lib->declarations.push_back(factory.subprogram(
            factory.bit(true, false), _makeHifName("hif__op_le", hifFormat), factory.noTemplates(),
            (factory.parameter(factory.bit(true, false), "param1"),
             factory.parameter(factory.bit(true, false), "param2"))));

        // sc_logic hif__op_ge(logic, logic)
        lib->declarations.push_back(factory.subprogram(
            factory.bit(true, false), _makeHifName("hif__op_ge", hifFormat), factory.noTemplates(),
            (factory.parameter(factory.bit(true, false), "param1"),
             factory.parameter(factory.bit(true, false), "param2"))));

        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_lt_signed", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, true),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, true), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_gt_signed", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, true),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, true), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_le_signed", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, true),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, true), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_ge_signed", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, true),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, true), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_lt_unsigned", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, false),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, false), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_gt_unsigned", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, false),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, false), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_le_unsigned", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, false),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, false), factory.noValue(), false,
            hifFormat));
        lib->declarations.push_back(_makeBinaryAttribute(
            "hif__op_ge_unsigned", factory.bit(true, false), factory.bitvector(nullptr, true, false, false, false),
            factory.noValue(), factory.bitvector(nullptr, true, false, false, false), factory.noValue(), false,
            hifFormat));
    }

    // template< typename T >
    // bool hif_arrayEquals(T param1[], T param2[]);
    // Note: bi-dimensional version is implicit!
    lib->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("hif_arrayEquals", hifFormat), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("T"), "param1", nullptr),
         factory.parameter(factory.typeRef("T"), "param2", nullptr))));

    {
        // template< typename T >
        // logic hif_logicEquals(T param, T param, bool sign);
        Function *f = dynamic_cast<Function *>(factory.subprogram(
            factory.bit(true, true), _makeHifName("hif_logicEquals", hifFormat),
            (factory.templateTypeParameter(nullptr, "T")),
            (factory.parameter(factory.typeRef("T"), "param1", nullptr),
             factory.parameter(factory.typeRef("T"), "param2", nullptr),
             factory.parameter(factory.boolean(), "param3", factory.boolval(true)))));
        lib->declarations.push_back(f);

        // template< typename T >
        // logic hif_logicEquals_hdtlib(T param, T param, bool sign);
        Function *f_hdtlib = getSuffixedCopy(f, "_hdtlib");
        lib->declarations.push_back(f_hdtlib);
    }

    // template< int size1, int size2 >
    // sc_lv<size1> hif_shift(sc_lv<size1> param1, sc_lv<size2> param2);
    lib->declarations.push_back(factory.subprogram(
        factory.typeRef("T1"), _makeHifName("hif_op_shift_left", hifFormat),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1", nullptr),
         factory.parameter(factory.typeRef("T2"), "param2", nullptr))));
    lib->declarations.push_back(factory.subprogram(
        factory.typeRef("T1"), _makeHifName("hif_op_shift_right_arith", hifFormat),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1", nullptr),
         factory.parameter(factory.typeRef("T2"), "param2", nullptr))));
    lib->declarations.push_back(factory.subprogram(
        factory.typeRef("T1"), _makeHifName("hif_op_shift_right_logic", hifFormat),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1", nullptr),
         factory.parameter(factory.typeRef("T2"), "param2", nullptr))));
    // hif_after
    // template< typename T, typename S >
    // void hif_after(T param1, S param2, sc_time param3);
    lib->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("hif_after", hifFormat),
        (factory.templateTypeParameter(nullptr, "T"), factory.templateTypeParameter(nullptr, "S")),
        (factory.parameter(factory.typeRef("T"), "param1"), factory.parameter(factory.typeRef("S"), "param2"),
         factory.parameter(factory.time(), "param3"))));

    // hif_vector_slice
    lib->declarations.push_back(factory.subprogram(
        factory.array(
            factory.range(factory.identifier("param2"), dir_downto, factory.identifier("param3")),
            factory.typeRef("T")),
        _makeHifName("hif_vector_slice", hifFormat),
        (factory.templateTypeParameter(nullptr, "T"), factory.templateValueParameter(factory.integer(), "left"),
         factory.templateValueParameter(factory.integer(), "right")),
        (factory.parameter(
             factory.array(
                 factory.range(factory.identifier("left"), dir_downto, factory.identifier("right")),
                 factory.typeRef("T")),
             "param1"),
         factory.parameter(factory.integer(), "param2"), factory.parameter(factory.integer(), "param3"))));

    // hif_caseXZ(bv1, bv2, bool)
    lib->declarations.push_back(factory.subprogram(
        factory.boolean(), _makeHifName("hif_caseXZ", hifFormat),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"),
         factory.parameter(factory.boolean(), "param3"),
         factory.parameter(factory.boolean(), "param4", _factory.boolval(true)))));

    // template< typename T >
    // T hif_reverse(T param1);
    lib->declarations.push_back(factory.subprogram(
        factory.typeRef("T"), _makeHifName("hif_reverse", hifFormat), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("T"), "param1", nullptr))));

    // void hif_setResourcePath(const std::string & path);
    lib->declarations.push_back(factory.subprogram(
        factory.noType(), _makeHifName("hif_setResourcePath", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.string(), "param1", nullptr))));
    // std::string hif_getResourcePath();
    lib->declarations.push_back(factory.subprogram(
        factory.string(), _makeHifName("hif_getResourcePath", hifFormat), factory.noTemplates(),
        factory.noParameters()));
    // std::string hif_getResourceFileName(const std::string & name);
    lib->declarations.push_back(factory.subprogram(
        factory.string(), _makeHifName("hif_getResourceFileName", hifFormat), factory.noTemplates(),
        (factory.parameter(factory.string(), "param1", nullptr))));

    return lib;
}
LibraryDef *SystemCSemantics::getHdtlibPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("hdtlib", hifFormat));
    ld->setLanguageID(hif::cpp);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // This trick avoids to re-define SystemC-like standard methods,
    // and therefore avoids ambiguity.
    Library *lib = factory.library(_makeHifName("sc_core", hifFormat), nullptr, "systemc", false, true);
    ld->libraries.push_back(lib);
    // ///////////////////////////////////////////////////////////////////
    // constants
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // attributes
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////
    // methods
    // ///////////////////////////////////////////////////////////////////

    // template< int W >
    // function range(int param1, int param2) return logic_vector<W>;
    // and
    // template< int W >
    // function range(int param1, int param2) return bit_vector<W>;
    {
        Range *retTypeSpan = factory.range(
            factory.expression(new Identifier("W"), op_minus, factory.intval(1)), dir_downto, new IntValue(0));
        Bitvector *std_logic_vector = factory.bitvector(retTypeSpan, true);
        Bitvector *bit_vector       = factory.bitvector(hif::copy(retTypeSpan), false);

        ld->declarations.push_back(factory.subprogram(
            std_logic_vector, _makeHifName("range", hifFormat), factory.templateValueParameter(factory.integer(), "W"),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.integer(), "param2", nullptr))));

        ld->declarations.push_back(factory.subprogram(
            bit_vector, _makeHifName("range", hifFormat), factory.templateValueParameter(factory.integer(), "W"),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.integer(), "param2", nullptr))));
    }
    // void set_bit(int i, bit value);
    // and
    // void set_bit(int i, hl_logic value);
    {
        ld->declarations.push_back(factory.subprogram(
            nullptr, _makeHifName("set_bit", hifFormat), factory.noTemplates(),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.bit(), "param2", nullptr))));

        ld->declarations.push_back(factory.subprogram(
            nullptr, _makeHifName("set_bit", hifFormat), factory.noTemplates(),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.bit(true, true), "param2", nullptr))));
    }

    // template <int WR>
    // hl_lv_t<W>& set_range(int hi, int lo, const hl_lv_t<WR>& rhs);
    // and
    // template <int WR>
    // hl_bv_t<W>& set_range(int hi, int lo, const hl_bv_t<WR>& rhs);
    {
        Range *retTypeSpan          = factory.range(new Identifier("WR"), dir_downto, new IntValue(0));
        Bitvector *std_logic_vector = factory.bitvector(retTypeSpan, true, false);
        Bitvector *bit_vector       = factory.bitvector(hif::copy(retTypeSpan), false, false);

        ld->declarations.push_back(factory.subprogram(
            nullptr, _makeHifName("set_range", hifFormat), factory.templateValueParameter(factory.integer(), "WR"),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.integer(), "param2", nullptr),
             factory.parameter(std_logic_vector, "param3", nullptr))));

        ld->declarations.push_back(factory.subprogram(
            nullptr, _makeHifName("set_range", hifFormat), factory.templateValueParameter(factory.integer(), "WR"),
            (factory.parameter(factory.integer(), "param1", nullptr),
             factory.parameter(factory.integer(), "param2", nullptr),
             factory.parameter(bit_vector, "param3", nullptr))));
    }

    return ld;
}

LibraryDef *SystemCSemantics::getIOStreamPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("iostream", hifFormat));
    ld->setLanguageID(hif::cpp);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // ///////////////////////////////////////////////////////////////////
    // types
    // ///////////////////////////////////////////////////////////////////
    {
        DesignUnit *ostreamDu = factory.designUnit(
            _makeHifName("ostream", hifFormat),
            factory.view(
                "class",
                factory.contents(
                    nullptr, factory.noDeclarations(), factory.noGenerates(), factory.noInstances(),
                    factory.noStateTables(), factory.noLibraries()),
                nullptr, hif::cpp, factory.noDeclarations(), factory.noLibraries(), factory.noTemplates(), false,
                factory.noViewReferences()));
        ld->declarations.push_back(ostreamDu);
        ostreamDu->views.front()->getContents()->declarations.push_front(
            factory.classConstructor(ostreamDu, factory.noParameters(), factory.noTemplates()));
        ostreamDu->views.front()->getContents()->declarations.push_back(factory.subprogram(
            factory.viewRef(ostreamDu->getName(), "class", nullptr, factory.noTemplateArguments()), "__systemc_op_sla",
            factory.templateTypeParameter(nullptr, "T"), factory.parameter(factory.typeRef("T"), "param1")));
    }
    // ///////////////////////////////////////////////////////////////////
    // variables
    // ///////////////////////////////////////////////////////////////////

    ld->declarations.push_back(factory.variable(
        factory.viewRef(_makeHifName("ostream", hifFormat), "class"), _makeHifName("cout", hifFormat),
        factory.classConstructorCall(
            NameTable::getInstance()->hifConstructor(), factory.viewRef(_makeHifName("ostream", hifFormat), "class"),
            factory.noParameterArguments(), factory.noTemplateArguments())));

    ld->declarations.push_back(factory.variable(
        factory.viewRef(_makeHifName("ostream", hifFormat), "class"), _makeHifName("clog", hifFormat),
        factory.classConstructorCall(
            NameTable::getInstance()->hifConstructor(), factory.viewRef(_makeHifName("ostream", hifFormat), "class"),
            factory.noParameterArguments(), factory.noTemplateArguments())));

    ld->declarations.push_back(factory.variable(
        factory.viewRef(_makeHifName("ostream", hifFormat), "class"), _makeHifName("cerr", hifFormat),
        factory.classConstructorCall(
            NameTable::getInstance()->hifConstructor(), factory.viewRef(_makeHifName("ostream", hifFormat), "class"),
            factory.noParameterArguments(), factory.noTemplateArguments())));

    ld->declarations.push_back(
        factory.constant(factory.character(), _makeHifName("endl", hifFormat), factory.charval('E')));

    return ld;
}

LibraryDef *SystemCSemantics::getDdtClibPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("ddtclib", hifFormat));
    ld->setLanguageID(hif::c);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // Mask functions
    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("getMask", hifFormat), nullptr,
        (factory.parameter(factory.integer(nullptr, false, true), "left"),
         factory.parameter(factory.integer(nullptr, false, true), "right"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("getMask0", hifFormat), nullptr,
        (factory.parameter(factory.integer(nullptr, false, true), "left"),
         factory.parameter(factory.integer(nullptr, false, true), "right"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("getSignedMask", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), true, true), "value"),
         factory.parameter(factory.integer(nullptr, false, true), "width"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("signExtend", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), false, true), "value"),
         factory.parameter(factory.integer(nullptr, false, true), "width"))));

    // Member assign functions
    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("memberAssign", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), false, true), "target"),
         factory.parameter(factory.integer(nullptr, false, true), "position"),
         factory.parameter(factory.boolean(true), "source"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("memberAssign2", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), false, true), "target"),
         factory.parameter(factory.integer(nullptr, false, true), "position"),
         factory.parameter(factory.integer(factory.range(63, 0), false, true), "source"),
         factory.parameter(factory.integer(nullptr, false, true), "position2"))));

    // Assign functions
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("uintToArrayAssign", hifFormat), nullptr,
        (factory.parameter(factory.pointer(factory.boolean(true)), "target"),
         factory.parameter(factory.integer(factory.range(63, 0), false, true), "source"),
         factory.parameter(factory.integer(nullptr, false, true), "tleft"),
         factory.parameter(factory.integer(nullptr, false, true), "tright"),
         factory.parameter(factory.integer(nullptr, false, true), "sright"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("arrayToUintAssign", hifFormat), nullptr,
        (factory.parameter(factory.pointer(factory.boolean(true)), "source"),
         factory.parameter(factory.integer(nullptr, false, true), "size"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), true, true), _makeHifName("arrayToIntAssign", hifFormat), nullptr,
        (factory.parameter(factory.pointer(factory.boolean(true)), "source"),
         factory.parameter(factory.integer(nullptr, false, true), "size"))));

    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("arrayArrayBoolToArrayUintAssign", hifFormat),
        (factory.templateTypeParameter(nullptr, "sourceType"), factory.templateTypeParameter(nullptr, "targetType")),
        (factory.parameter(factory.typeRef("sourceType"), "source"),
         factory.parameter(factory.typeRef("targetType"), "target"),
         factory.parameter(factory.integer(nullptr, false, true), "arraySize"),
         factory.parameter(factory.integer(nullptr, false, true), "subArraySize"))));

    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("arrayArrayBoolToArrayIntAssign", hifFormat),
        (factory.templateTypeParameter(nullptr, "sourceType"), factory.templateTypeParameter(nullptr, "targetType")),
        (factory.parameter(factory.typeRef("sourceType"), "source"),
         factory.parameter(factory.typeRef("targetType"), "target"),
         factory.parameter(factory.integer(nullptr, false, true), "arraySize"),
         factory.parameter(factory.integer(nullptr, false, true), "subArraySize"))));

    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("arrayUintToArrayArrayBoolAssign", hifFormat),
        (factory.templateTypeParameter(nullptr, "sourceType"), factory.templateTypeParameter(nullptr, "targetType")),
        (factory.parameter(factory.typeRef("sourceType"), "source"),
         factory.parameter(factory.typeRef("targetType"), "target"),
         factory.parameter(factory.integer(nullptr, false, true), "arraySize"),
         factory.parameter(factory.integer(nullptr, false, true), "subArraySize"))));

    ld->declarations.push_back(factory.subprogram(
        factory.integer(factory.range(63, 0), false, true), _makeHifName("aggregateToUint", hifFormat), nullptr,
        (factory.parameter(factory.boolean(true), "source"),
         factory.parameter(factory.integer(nullptr, false, true), "size"))));

    // Bitwise reduction functions
    ld->declarations.push_back(factory.subprogram(
        factory.boolean(true), _makeHifName("andReduce", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), false, true), "value"),
         factory.parameter(factory.integer(factory.range(31, 0), false, true), "width"))));

    ld->declarations.push_back(factory.subprogram(
        factory.boolean(true), _makeHifName("orReduce", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), false, true), "value"),
         factory.parameter(factory.integer(factory.range(31, 0), false, true), "width"))));

    ld->declarations.push_back(factory.subprogram(
        factory.boolean(true), _makeHifName("xorReduce", hifFormat), nullptr,
        (factory.parameter(factory.integer(factory.range(63, 0), false, true), "value"),
         factory.parameter(factory.integer(factory.range(31, 0), false, true), "width"))));

    return ld;
}

LibraryDef *SystemCSemantics::getStandardPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("standard", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::c);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // T * new(T)
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.typeRef("T")), _makeHifName("new", hifFormat),
        (factory.templateTypeParameter(nullptr, "T")), (factory.parameter(factory.typeRef("T"), "param1"))));

    // T * new()
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.typeRef("T")), _makeHifName("new", hifFormat),
        (factory.templateTypeParameter(nullptr, "T")), factory.noParameters()));

    // void delete (T*) ;
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName("delete", hifFormat), (factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.pointer(factory.typeRef("T")), "param1"))));

    // class void
    ld->declarations.push_back(factory.designUnit(_makeHifName("void", hifFormat), "class", hif::cpp));

    // size_t
    ld->declarations.push_back(factory.typeDef(_makeHifName("size_t", hifFormat), factory.integer(nullptr, false)));
    // sizeof
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef(_makeHifName("size_t", hifFormat)), _makeHifName("sizeof", hifFormat),
        factory.templateTypeParameter(nullptr, "T"), factory.parameter(factory.typeRef("T"), "param1")));

    // constructor for general type
    ld->declarations.push_back(factory.subprogram(
        factory.typeRef("T"), _makeHifName(hif::NameTable::getInstance()->hifConstructor(), hifFormat),
        factory.templateTypeParameter(nullptr, "T"), factory.noParameters()));

    // destructor for general type
    ld->declarations.push_back(factory.subprogram(
        nullptr, _makeHifName(hif::NameTable::getInstance()->hifDestructor(), hifFormat), factory.noTemplates(),
        factory.noParameters()));

    return ld;
}

LibraryDef *SystemCSemantics::getNewPackage(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("new", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::cpp);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // T * new(expr)(T) -- placement
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.typeRef("T")), _makeHifName("placement_new", hifFormat),
        (factory.templateTypeParameter(nullptr, "TExpr"), factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("TExpr"), "param1"), factory.parameter(factory.typeRef("T"), "param2"))));

    // T * new(expr)(T) -- placement
    ld->declarations.push_back(factory.subprogram(
        factory.pointer(factory.typeRef("T")), _makeHifName("placement_new", hifFormat),
        (factory.templateTypeParameter(nullptr, "TExpr"), factory.templateTypeParameter(nullptr, "T")),
        (factory.parameter(factory.typeRef("TExpr"), "param1"))));

    return ld;
}

LibraryDef *SystemCSemantics::getSystemVueModelBuilder(bool hifFormat)
{
    LibraryDef *ld = new LibraryDef();
    ld->setName(_makeHifName("SystemVueModelBuilder", hifFormat));
    ld->setLanguageID(hif::cpp);
    ld->setStandard(true);

    hif::HifFactory factory(this);
    if (hifFormat)
        factory.setSemantics(HIFSemantics::getInstance());

    // creating DECLARE_MODEL_INTERFACE procedure
    {
        SubProgram *dmi = factory.subprogram(
            nullptr, _makeHifName("DECLARE_MODEL_INTERFACE", hifFormat), factory.templateTypeParameter(nullptr, "T"),
            factory.parameter(factory.typeRef("T"), "param", nullptr));
        dmi->setKind(SubProgram::MACRO);
        ld->declarations.push_back(dmi);
    }

    // creating ADD_MODEL_INPUT and ADD_MODEL_OUTPUT procedures
    {
        SubProgram *in = factory.subprogram(
            nullptr, _makeHifName("ADD_MODEL_INPUT", hifFormat), factory.templateTypeParameter(nullptr, "T"),
            factory.parameter(factory.typeRef("T"), "param", nullptr));
        ld->declarations.push_back(in);

        SubProgram *out = factory.subprogram(
            nullptr, _makeHifName("ADD_MODEL_OUTPUT", hifFormat), factory.templateTypeParameter(nullptr, "T"),
            factory.parameter(factory.typeRef("T"), "param", nullptr));
        ld->declarations.push_back(out);
    }

    // creating DFModel
    {
        Contents *c = new Contents();
        c->setName("behav");

        Entity *e = new Entity();
        e->setName("behav");

        View *v = new View();
        v->setLanguageID(hif::cpp);
        v->setName("behav");
        v->setContents(c);
        v->setEntity(e);

        DesignUnit *dfmodel = new DesignUnit();
        dfmodel->setName(_makeHifName("DFModel", hifFormat));
        dfmodel->views.push_back(v);
        ld->declarations.push_back(dfmodel);
    }

    return ld;
}

LibraryDef *SystemCSemantics::getStandardLibrary(const std::string &n)
{
    if (n == "sc_core") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getScCorePackage(false);
        return lStandard;
    } else if (n == "sc_dt") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getScDtPackage(false);
        return lStandard;
    } else if (n == "tlm") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getTlmPackage(false);
        return lStandard;
    } else if (n == "cmath") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getCMathPackage(false);
        return lStandard;
    } else if (n == "ctime") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getCTimePackage(false);
        return lStandard;
    } else if (n == "cstdlib") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getCStdLibPackage(false);
        return lStandard;
    } else if (n == "hif_systemc_extensions") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getSystemcExtensionsPackage(false);
        return lStandard;
    } else if (n == "iostream") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getIOStreamPackage(false);
        return lStandard;
    } else if (n == "standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getStandardPackage(false);
        return lStandard;
    } else if (n == "new") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getNewPackage(false);
        return lStandard;
    } else if (n == "hdtlib") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getHdtlibPackage(false);
        return lStandard;
    } else if (n == "ddtclib") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getDdtClibPackage(false);
        return lStandard;
    } else if (n == "string") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getStringPackage(false);
        return lStandard;
    } else if (n == "cstring") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getCStringPackage(false);
        return lStandard;
    } else if (n == "cstddef") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getCStdDefPackage(false);
        return lStandard;
    } else if (n == "vector") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getVectorPackage(false);
        return lStandard;
    } else if (n == "cstdio") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getCStdIOPackage(false);
        return lStandard;
    } else if (n == "sca_eln") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getScAmsELNPackage(false);
        return lStandard;
    } else if (n == "SystemVueModelBuilder") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getSystemVueModelBuilder(false);
        return lStandard;
    } else if (n == "tlm_utils") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getTlmUtils(false);
        return lStandard;
    }

    return nullptr;
}

bool SystemCSemantics::isNativeLibrary(const std::string &n, bool hifFormat)
{
    if (n == _makeHifName("sc_core", hifFormat) || n == _makeHifName("sc_dt", hifFormat) ||
        n == _makeHifName("sca_eln", hifFormat) || n == _makeHifName("tlm", hifFormat) ||
        n == _makeHifName("cmath", hifFormat) || n == _makeHifName("ctime", hifFormat) ||
        n == _makeHifName("cstdlib", hifFormat) || n == _makeHifName("cstdio", hifFormat) ||
        n == _makeHifName("iostream", hifFormat) || n == _makeHifName("vector", hifFormat) ||
        n == _makeHifName("string", hifFormat) || n == _makeHifName("cstring", hifFormat) ||
        n == _makeHifName("cstddef", hifFormat) || n == _makeHifName("standard", hifFormat) ||
        n == _makeHifName("new", hifFormat) || n == _makeHifName("hif_systemc_extensions", hifFormat) ||
        n == _makeHifName("tlm_utils", hifFormat))
        return true;

    return false;
}

SystemCSemantics::MapCases SystemCSemantics::mapStandardSymbol(
    Declaration *decl,
    KeySymbol &key,
    ValueSymbol &value,
    ILanguageSemantics * /*srcSem*/)
{
    std::string libName;
    bool isMine = _isHifPrefixed(key.first, libName);
    // messageAssert(isMine, "Asked name not prefixed with 'hif_': " + unprefixed, nullptr, nullptr);
    if (isMine) {
        std::string symName;
        bool ok = _isHifPrefixed(key.second, symName);
        messageAssert(ok, "Found not prefixed symbol in prefixed library", decl, this);

        value.libraries.clear();
        value.libraries.push_back(libName);
        value.mappedSymbol = symName;
        // Libraries are always replaced, therefore internal symbols must be kept
        bool isLibrary     = (libName == symName);
        value.mapAction    = isLibrary ? MAP_DELETE : MAP_KEEP;
        return value.mapAction;
    }
    // custom cases
    // IDEA: methods that are mapped into methods which differs for parameter numbers,
    // or whose mapping cannot be decided only by the name (i.e. decl must be inspected).
    else if (key.first != key.second) {
        if (key.first == "hif_vhdl_ieee_std_math_real" && key.second == "hif_vhdl_log") {
            Function *f = dynamic_cast<Function *>(decl);
            messageAssert(f != nullptr, "Expected Function", decl, this);

            value.libraries.clear();
            if (f->parameters.size() == 2) {
                value.mappedSymbol = f->getName();
                value.mapAction    = MAP_KEEP;
            } else {
                value = _standardSymbols[key];
            }
            return value.mapAction;
        }
    }

    StandardSymbols::iterator it(_standardSymbols.find(key));
    if (it == _standardSymbols.end())
        return ILanguageSemantics::UNKNOWN;

    value = it->second;
    return value.mapAction;
}

Object *SystemCSemantics::getSimplifiedSymbol(KeySymbol &key, Object *s)
{
    FunctionCall *fc = dynamic_cast<FunctionCall *>(s);

    if (key.first == "hif_vhdl_ieee_std_logic_1164") {
        if (key.second == "hif_vhdl_to_x01z") {
            return fc->parameterAssigns.back()->setValue(nullptr);
        }
    }

    messageError("Found unsupported symbol", s, this);
}

bool SystemCSemantics::isStandardInclusion(const std::string &n, bool isLibInclusion)
{
    // Libs w/ standard flags
    if (isLibInclusion) {
        if (n == "standard")
            return true;
    } else {
        if (n == "standard")
            return true;
        else if (n == "cmath")
            return true;
        else if (n == "ctime")
            return true;
        else if (n == "cstdlib")
            return true;
        else if (n == "cstring")
            return true;
        else if (n == "cstddef")
            return true;
    }

    return false;
}

std::string SystemCSemantics::getEventMethodName(bool hifFormat) { return _makeHifName("event", hifFormat); }

bool SystemCSemantics::isEventCall(FunctionCall *call)
{
    return call->getInstance() != nullptr && call->getName() == getEventMethodName(false);
}

std::string SystemCSemantics::getLibraryNamespace(const std::string &n)
{
    if (n == "iostream")
        return "std";
    if (n == "vector")
        return "std";

    return n;
}

} // namespace semantics
} // namespace hif
