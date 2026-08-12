/// @file bindOpenPortAssigns.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/bindOpenPortAssigns.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

/// This function builds this name: prefix_suffix[_xyzu]
std::string buildFreshName(const std::string &prefix, const std::string &suffix)
{
    return NameTable::getInstance()->getFreshName(prefix + "_" + suffix);
}

/// Fixes binding of open-port assigns in Instances
class BindOpenPortAssignVisitor : public GuideVisitor
{
public:
    BindOpenPortAssignVisitor(hif::semantics::ILanguageSemantics *sem);
    ~BindOpenPortAssignVisitor();

    int visitInstance(Instance &instance);

private:
    BindOpenPortAssignVisitor(const BindOpenPortAssignVisitor &);
    BindOpenPortAssignVisitor operator=(const BindOpenPortAssignVisitor &);
    hif::semantics::ILanguageSemantics *_sem;
};
BindOpenPortAssignVisitor::BindOpenPortAssignVisitor(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
{
}
BindOpenPortAssignVisitor::~BindOpenPortAssignVisitor() {}
int BindOpenPortAssignVisitor::visitInstance(Instance &instance)
{
    Entity *en = instantiate(&instance, _sem);
    if (en == nullptr)
        return 0;

    // TODO: At the moment declarations are not allowed inside generates.
    Contents *contents = hif::getNearestParent<Contents>(&instance);
    if (contents == nullptr)
        return 0;

    for (BList<Port>::iterator i = en->ports.begin(); i != en->ports.end(); ++i) {
        PortAssign *pa = nullptr;
        for (BList<PortAssign>::iterator j = instance.portAssigns.begin(); j != instance.portAssigns.end(); ++j) {
            if ((*i)->getName() != (*j)->getName())
                continue;
            pa = *j;
            break;
        }

        // Can be either: missing bind or bind with no value
        if (pa != nullptr && pa->getValue() != nullptr)
            continue;

        std::string name     = buildFreshName((*i)->getName(), "open");
        // build the signal
        Signal *dummy_signal = new Signal();
        dummy_signal->setType(hif::copy((*i)->getType()));
        dummy_signal->setValue(_sem->getTypeDefaultValue((*i)->getType(), nullptr));
        dummy_signal->setName(name);
        // add the signal to the declarations

        contents->declarations.push_back(dummy_signal);
        // build the portassign
        if (pa == nullptr) {
            pa = new PortAssign();
            pa->setName((*i)->getName());
        }
        pa->setValue(new Identifier(name));

        // add the portassign to the instance
        if (!pa->isInBList())
            instance.portAssigns.push_back(pa);

            // This fix is necessary to set direction of non-bound ports.
#ifdef NDEBUG
        hif::semantics::getDeclaration(pa, _sem);
#else
        Port *port = hif::semantics::getDeclaration(pa, _sem);
        messageDebugAssert(port != nullptr, "Port assign declaration not found", pa, _sem);
#endif
    }

    return 0;
}

} // namespace

void bindOpenPortAssigns(Object &o, hif::semantics::ILanguageSemantics *sem)
{
    BindOpenPortAssignVisitor vis(sem);
    o.acceptVisitor(vis);
}

} // namespace manipulation
} // namespace hif
