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
#include "omnetpp/cproperty.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/globals.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cdynamicexpression.h"
#include "cnedloader.h"
#include "nedxml/errorstore.h"
#include "cdynamicmoduletype.h"
#include "cdynamicchanneltype.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

cNedLoader *cNedLoader::inst;

EXECUTE_ON_SHUTDOWN(cNedLoader::clear());

cNedLoader::~cNedLoader()
{
    for (auto it : cachedExpresssions)
        delete it.second;
}

cNedLoader *cNedLoader::getInstance()
{
    if (!inst) {
        inst = new cNedLoader();
        inst->registerBuiltinDeclarations();
    }
    return inst;
}

void cNedLoader::clear()
{
    delete inst;
    inst = nullptr;
}

void cNedLoader::registerNedType(const char *qname, bool isInnerType, NedElement *node)
{
    // wrap, and add to the table (Note: we cannot reuse base class impl, because it creates a NedTypeInfo)
    cNedDeclaration *decl = new cNedDeclaration(this, qname, isInnerType, node);
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

cNedDeclaration *cNedLoader::getDecl(const char *qname) const
{
    cNedDeclaration *decl = dynamic_cast<cNedDeclaration *>(NedResourceCache::getDecl(qname));
    ASSERT(decl);
    return decl;
}

cDynamicExpression *cNedLoader::getCompiledExpression(const ExprRef& key)
{
    auto it = cachedExpresssions.find(key);
    if (it != cachedExpresssions.end())
        return it->second;
    cDynamicExpression *expr = new cDynamicExpression();
    expr->parseNedExpr(key.getExprText());
    cachedExpresssions[key] = expr;
    return expr;
}

}  // namespace omnetpp

