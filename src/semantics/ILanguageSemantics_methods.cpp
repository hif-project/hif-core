/// @file ILanguageSemantics_methods.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{
class StandardPackagesVisitor : public hif::GuideVisitor
{
public:
    StandardPackagesVisitor(System *s, hif::semantics::ILanguageSemantics *sem);
    virtual ~StandardPackagesVisitor();

    int visitSystem(System &o);
    int visitLibraryDef(LibraryDef &o);
    int visitView(View &o);
    int visitContents(Contents &o);

private:
    StandardPackagesVisitor(const StandardPackagesVisitor &);
    StandardPackagesVisitor operator=(const StandardPackagesVisitor &);

    void _checkLibraries(BList<Library> &libs);

    hif::semantics::ILanguageSemantics *_sem;
    System *_system;
};

StandardPackagesVisitor::StandardPackagesVisitor(System *s, hif::semantics::ILanguageSemantics *sem)
    : hif::GuideVisitor()
    , _sem(sem)
    , _system(s)
{
    // ntd
}

StandardPackagesVisitor::~StandardPackagesVisitor()
{
    // ntd
}

void StandardPackagesVisitor::_checkLibraries(BList<Library> &libs)
{
    for (BList<Library>::iterator i = libs.begin(); i != libs.end(); ++i) {
        LibraryDef *ld = _sem->getStandardLibrary((*i)->getName());
        if (ld == nullptr)
            continue;
        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.equalsOptions.checkOnlyNames = true;
        hif::manipulation::addUniqueObject(ld, _system->libraryDefs, addOpt);
    }
}

int StandardPackagesVisitor::visitSystem(System &o)
{
    GuideVisitor::visitSystem(o);
    _checkLibraries(o.libraries);
    return 0;
}

int StandardPackagesVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    GuideVisitor::visitLibraryDef(o);
    _checkLibraries(o.libraries);
    return 0;
}

int StandardPackagesVisitor::visitView(View &o)
{
    if (o.isStandard())
        return 0;
    GuideVisitor::visitView(o);
    _checkLibraries(o.libraries);
    return 0;
}

int StandardPackagesVisitor::visitContents(Contents &o)
{
    GuideVisitor::visitContents(o);
    _checkLibraries(o.libraries);
    return 0;
}

} // namespace
std::string ILanguageSemantics::_makeHifName(const std::string &reqName, bool hifFormat) const
{
    if (!hifFormat)
        return reqName;
    const std::string &n = getName();
    if (n == "hif")
        return reqName;
    return "hif_" + getName() + "_" + reqName;
}

TypeDef *
ILanguageSemantics::_makeEnum(const char *enumName, const char *values[], std::size_t size, bool hifFormat)
{
    std::string en(_makeHifName(enumName, hifFormat));

    TypeDef *td = new TypeDef();
    td->setName(en);
    td->setOpaque(true);
    Enum *e = new Enum();
    td->setType(e);

    for (size_t i = 0; i < size; ++i) {
        TypeReference *tr = new TypeReference();
        tr->setName(en);
        EnumValue *ev = new EnumValue();
        ev->setName(_makeHifName(values[i], hifFormat));
        ev->setType(tr);
        e->values.push_back(ev);
    }

    return td;
}

void ILanguageSemantics::_makeAttributeParameter(
    SubProgram *scope,
    Type *paramType,
    Value *paramValue,
    const std::string &paramIndex,
    bool hifFormat)
{
    if (paramType != nullptr) {
        Parameter *p = new Parameter();
        p->setName("param" + paramIndex);
        p->setType(paramType);
        scope->parameters.push_back(p);

        if ((scope->templateParameters.empty() ||
             dynamic_cast<TypeTP *>(scope->templateParameters.front()) == nullptr) &&
            dynamic_cast<TypeReference *>(paramType) != nullptr) {
            TypeReference *tr = static_cast<TypeReference *>(paramType);
            if (tr->getName() == "T") {
                TypeTP *t = new TypeTP();
                t->setName("T");
                scope->templateParameters.push_front(t);
            }
        } else if (
            dynamic_cast<Bitvector *>(paramType) != nullptr || dynamic_cast<Signed *>(paramType) != nullptr ||
            dynamic_cast<Unsigned *>(paramType) != nullptr) {
            Range *span = typeGetSpan(paramType, this);
            if (span == nullptr) {
                Range *r =
                    new Range(new Identifier("left" + paramIndex), new Identifier("right" + paramIndex), dir_downto);
                typeSetSpan(paramType, r, this);

                hif::HifFactory fact(this);
                if (hifFormat)
                    fact.setSemantics(HIFSemantics::getInstance());

                scope->templateParameters.push_back(
                    fact.templateValueParameter(fact.integer(), ("left" + paramIndex).c_str()));
                scope->templateParameters.push_back(
                    fact.templateValueParameter(fact.integer(), ("right" + paramIndex).c_str()));
            }
        }

        p->setValue(paramValue);
    }
}

void ILanguageSemantics::_addMultiparamFunction(
    LibraryDef *ld,
    const char *name,
    HifFactory &factory,
    bool hifFormat,
    Type *ret)
{
    ld->declarations.push_back(
        factory.subprogram(ret, _makeHifName(name, hifFormat).c_str(), factory.noTemplates(), factory.noParameters()));
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret), _makeHifName(name, hifFormat).c_str(), factory.templateTypeParameter(nullptr, "T1"),
        factory.parameter(factory.typeRef("T1"), "param1")));
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret), _makeHifName(name, hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"))));
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret), _makeHifName(name, hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2"),
         factory.templateTypeParameter(nullptr, "T3")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"),
         factory.parameter(factory.typeRef("T3"), "param3"))));
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret), _makeHifName(name, hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2"),
         factory.templateTypeParameter(nullptr, "T3"), factory.templateTypeParameter(nullptr, "T4")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"),
         factory.parameter(factory.typeRef("T3"), "param3"), factory.parameter(factory.typeRef("T4"), "param4"))));
    ld->declarations.push_back(factory.subprogram(
        hif::copy(ret), _makeHifName(name, hifFormat).c_str(),
        (factory.templateTypeParameter(nullptr, "T1"), factory.templateTypeParameter(nullptr, "T2"),
         factory.templateTypeParameter(nullptr, "T3"), factory.templateTypeParameter(nullptr, "T4"),
         factory.templateTypeParameter(nullptr, "T5")),
        (factory.parameter(factory.typeRef("T1"), "param1"), factory.parameter(factory.typeRef("T2"), "param2"),
         factory.parameter(factory.typeRef("T3"), "param3"), factory.parameter(factory.typeRef("T4"), "param4"),
         factory.parameter(factory.typeRef("T5"), "param5"))));
    // TODO add overloading for N parameters...
}

Array *ILanguageSemantics::_makeTemplateArray(const std::string &index, Type *t)
{
    Array *ret = new Array();
    ret->setType(t);
    Range *r = new Range(new Identifier("left" + index), new Identifier("left" + index), hif::dir_downto);
    ret->setSpan(r);
    return ret;
}
SubProgram *ILanguageSemantics::_makeBinaryAttribute(
    const std::string &n,
    Type *retType,
    Type *param1Type,
    Value *param1Value,
    Type *param2Type,
    Value *param2Value,
    bool unsupported,
    bool hifFormat)
{
    SubProgram *sp = _makeAttribute(n, retType, param1Type, param1Value, unsupported, hifFormat);

    _makeAttributeParameter(sp, param2Type, param2Value, "2", hifFormat);

    return sp;
}

SubProgram *ILanguageSemantics::_makeTernaryAttribute(
    const std::string &n,
    Type *retType,
    Type *param1Type,
    Value *param1Value,
    Type *param2Type,
    Value *param2Value,
    Type *param3Type,
    Value *param3Value,
    bool unsupported,
    bool hifFormat)
{
    SubProgram *sp =
        _makeBinaryAttribute(n, retType, param1Type, param1Value, param2Type, param2Value, unsupported, hifFormat);

    _makeAttributeParameter(sp, param3Type, param3Value, "3", hifFormat);

    return sp;
}

SubProgram *ILanguageSemantics::_makeAttribute(
    const std::string &n,
    Type *retType,
    Type *paramType,
    Value *paramValue,
    bool unsupported,
    bool hifFormat)
{
    messageDebugAssert(
        (paramValue == nullptr || paramType != nullptr), "Unexpected param value without paramType", nullptr, nullptr);

    Function *f = new Function();
    f->setName(_makeHifName(n, hifFormat));

    if (retType == nullptr) {
        // Function is template on this
        TypeTP *t = new TypeTP();
        t->setName("T");
        f->templateParameters.push_back(t);

        TypeReference *tr = new TypeReference();
        tr->setName(t->getName());
        f->setType(tr);
    } else {
        f->setType(retType);
    }

    _makeAttributeParameter(f, paramType, paramValue, "1", hifFormat);

    if (unsupported) {
        f->addProperty(PROPERTY_UNSUPPORTED);
    }

    return f;
}

bool ILanguageSemantics::_isHifPrefixed(const std::string &n, std::string &unprefixed)
{
    unprefixed = n;
    std::string hifPrefix("hif_" + getName() + "_");
    bool isHif = (hifPrefix == unprefixed.substr(0, hifPrefix.size()));

    if (isHif)
        unprefixed = unprefixed.substr(hifPrefix.size());
    return isHif;
}

ILanguageSemantics::KeySymbol ILanguageSemantics::_makeKey(const char *library, const char *symbol)
{
    return std::make_pair(library, symbol);
}

ILanguageSemantics::ValueSymbol ILanguageSemantics::_makeValue(
    const char *library[],
    unsigned int size,
    const char *symbol,
    ILanguageSemantics::MapCases action)
{
    ILanguageSemantics::ValueSymbol ret;
    for (unsigned int i = 0; i < size; ++i) {
        ret.libraries.push_back(library[i]);
    }
    ret.mappedSymbol = symbol;
    ret.mapAction    = action;
    return ret;
}

void ILanguageSemantics::addStandardPackages(System *s)
{
    hif::application_utils::initializeLogHeader(
        hif::application_utils::getApplicationName(), "performStandardPackagesRefinement");

    StandardPackagesVisitor v(s, this);
    s->acceptVisitor(v);

    hif::application_utils::restoreLogHeader();
}

std::string ILanguageSemantics::mapStandardFilename(const std::string &n)
{
    StandardLibraryFiles::iterator it = _standardFilenames.find(n);
    if (it == _standardFilenames.end())
        return "";
    return it->second;
}

ILanguageSemantics::ValueSymbol::ValueSymbol()
    : libraries()
    , mappedSymbol()
    , mapAction(UNKNOWN)
{
    //ntd
}

ILanguageSemantics::ValueSymbol::~ValueSymbol()
{
    //ntd
}

ILanguageSemantics::ValueSymbol::ValueSymbol(const ValueSymbol &other)
    : libraries(other.libraries)
    , mappedSymbol(other.mappedSymbol)
    , mapAction(other.mapAction)
{
    //ntd
}

ILanguageSemantics::ValueSymbol &ILanguageSemantics::ValueSymbol::operator=(const ValueSymbol &other)
{
    if (&other == this)
        return *this;
    libraries    = other.libraries;
    mappedSymbol = other.mappedSymbol;
    mapAction    = other.mapAction;

    return *this;
}

bool ILanguageSemantics::isStandardInclusion(const std::string & /*n*/, bool /*isLibInclusion*/) { return false; }

} // namespace semantics
} // namespace hif
