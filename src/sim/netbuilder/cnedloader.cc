//==========================================================================
// CNEDLOADER.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "common/fileutil.h"
#include "nedxml/nedelements.h"
#include "nedxml/nederror.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cpar.h"
#include "omnetpp/clongparimpl.h"
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/globals.h"
#include "omnetpp/cdisplaystring.h"
#include "cnedloader.h"
#include "cexpressionbuilder.h"
#include "cdynamicmoduletype.h"
#include "cdynamicchanneltype.h"

using namespace omnetpp::common;

namespace omnetpp {

cNEDLoader *cNEDLoader::inst;

EXECUTE_ON_SHUTDOWN(cNEDLoader::clear());

cNEDLoader *cNEDLoader::getInstance()
{
    if (!inst) {
        inst = new cNEDLoader();
        inst->registerBuiltinDeclarations();
    }
    return inst;
}

void cNEDLoader::clear()
{
    delete inst;
    inst = nullptr;
}

void cNEDLoader::registerNedType(const char *qname, bool isInnerType, NEDElement *node)
{
    // wrap, and add to the table (Note: we cannot reuse base class impl, because it creates a NEDTypeInfo)
    cNEDDeclaration *decl = new cNEDDeclaration(this, qname, isInnerType, node);
    nedTypes[qname] = decl;
    nedTypeNames.clear();  // invalidate

    // if module or channel, register corresponding object which can be used to instantiate it
    cComponentType *type = nullptr;
    if (node->getTagCode() == NED_SIMPLE_MODULE || node->getTagCode() == NED_COMPOUND_MODULE)
        type = new cDynamicModuleType(qname);
    else if (node->getTagCode() == NED_CHANNEL)
        type = new cDynamicChannelType(qname);
    if (type)
        componentTypes.getInstance()->add(type);
}

cNEDDeclaration *cNEDLoader::getDecl(const char *qname) const
{
    cNEDDeclaration *decl = dynamic_cast<cNEDDeclaration *>(NEDResourceCache::getDecl(qname));
    ASSERT(decl);
    return decl;
}

}  // namespace omnetpp

