//==========================================================================
// CNEDLOADER.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cnedloader.h"
#include "nedelements.h"
#include "nederror.h"
#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"

#include "cproperty.h"
#include "cproperties.h"
#include "ccomponenttype.h"
#include "cexception.h"
#include "cenvir.h"
#include "cexpressionbuilder.h"
#include "cpar.h"
#include "clongpar.h"
#include "cdoublepar.h"
#include "globals.h"
#include "cdynamicmoduletype.h"
#include "cdynamicchanneltype.h"
#include "cdisplaystring.h"

cNEDLoader *cNEDLoader::instance_;

cNEDLoader *cNEDLoader::instance()
{
    if (!instance_)
        instance_ = new cNEDLoader();
    return instance_;
}

void cNEDLoader::clear()
{
    delete instance_;
    instance_ = NULL;
}

cNEDLoader::cNEDLoader()
{
    registerBuiltinDeclarations();
}

void cNEDLoader::registerBuiltinDeclarations()
{
    // NED code to define built-in types
    const char *nedcode =
        "channel withcppclass cBasicChannel\n"
        "{\n"
        "    bool disabled = false;\n"
        "    double delay = 0;\n"
        "    double error = 0;\n"
        "    double datarate = 0;\n"
        "}\n"

        "channel withcppclass cNullChannel\n";
        "{\n"
        "}\n"

        "interface IBidirectionalChannel\n"
        "{\n"
        "    gates:\n"
        "        inout a;\n"
        "        inout b;\n"
        "}\n"

        "interface IUnidirectionalChannel\n"
        "{\n"
        "    gates:\n"
        "        input i;\n"
        "        output o;\n"
        "}\n"
    ;

    NEDErrorStore errors;
    NEDParser parser(&errors);
    NEDElement *tree = parser.parseNEDText(nedcode);
    //FIXME check errors; run validation perhaps, etc!

    try
    {
        addFile("[builtin-declarations]", tree);
    }
    catch (NEDException *e)
    {
        throw new cRuntimeError("NED error: %s", e->errorMessage()); // FIXME or something
    }
}

void cNEDLoader::addComponent(const char *name, NEDElement *node)
{
    if (!areDependenciesResolved(node))
    {
        // we'll process it later
        pendingList.push_back(node);
        return;
    }

    // Note: base class (nedxml's NEDResourceCache) has already checked for duplicates, no need here
    cNEDDeclaration *decl = buildNEDDeclaration(node);
    components[name] = decl;

    printf("DBG: registered %s\n",name);//XXX remove

    // if module or channel, register corresponding object which can be used to instantiate it
    cComponentType *type = NULL;
    if (node->getTagCode()==NED_SIMPLE_MODULE)
        type = new cDynamicModuleType(name);
    else if (node->getTagCode()==NED_COMPOUND_MODULE)
        type = new cDynamicModuleType(name);
    else if (node->getTagCode()==NED_CHANNEL)
        type = new cDynamicChannelType(name);
    if (type)
        componentTypes.instance()->add(type);
}

cNEDDeclaration *cNEDLoader::lookup2(const char *name) const
{
    return dynamic_cast<cNEDDeclaration *>(NEDResourceCache::lookup(name));
}

NEDElement *cNEDLoader::parseAndValidateNedFile(const char *fname, bool isXML)
{
    // load file
    NEDElement *tree = 0;
    NEDErrorStore errors;
    errors.setPrintToStderr(true); //XXX
    if (isXML)
    {
        tree = parseXML(fname, &errors);
    }
    else
    {
        NEDParser parser(&errors);
        parser.setParseExpressions(true);
        parser.setStoreSource(false);
        tree = parser.parseNEDFile(fname);
    }
    if (errors.numErrors() > 0) // FIXME print errors; ignore warnings
    {
        delete tree;
        throw new cRuntimeError("errors while loading or parsing file `%s'", fname);
    }

    // DTD validation and additional basic validation
    NEDDTDValidator dtdvalidator(&errors);
    dtdvalidator.validate(tree);
    if (errors.numErrors() > 0) // FIXME print errors; ignore warnings
    {
        delete tree;
        throw new cRuntimeError("errors during DTD validation of file `%s'", fname);
    }

    NEDBasicValidator basicvalidator(true, &errors);
    basicvalidator.validate(tree);
    if (errors.numErrors() > 0) // FIXME print errors; ignore warnings
    {
        delete tree;
        throw new cRuntimeError("errors during validation of file `%s'", fname);
    }
    return tree;
}

void cNEDLoader::loadNedFile(const char *nedfname, bool isXML)
{
    // parse file
    NEDElement *tree = parseAndValidateNedFile(nedfname, isXML);

    try
    {
        addFile(nedfname, tree);
    }
    catch (NEDException *e)
    {
        throw new cRuntimeError("NED error: %s", e->errorMessage()); // FIXME or something
    }
}


bool cNEDLoader::areDependenciesResolved(NEDElement *node)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()!=NED_EXTENDS && child->getTagCode()!=NED_INTERFACE_NAME)
            continue;

        const char *name = child->getAttribute("name");
        cNEDDeclaration *decl = lookup2(name);
        if (!decl)
            return false;
    }
    return true;
}

void cNEDLoader::tryResolvePendingDeclarations()
{
    bool again = true;
    while (again)
    {
        again = false;
        for (int i=0; i<pendingList.size(); i++)
        {
            NEDElement *node = pendingList[i];
            if (areDependenciesResolved(node))
            {
                buildNEDDeclaration(node);
                again = true;
            }
        }
    }
}

cNEDDeclaration *cNEDLoader::buildNEDDeclaration(NEDElement *node)
{
    const char *name = node->getAttribute("name");

    // create declaration object
    cNEDDeclaration *decl = new cNEDDeclaration(name, node);

    // add "extends" and "like" names
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()==NED_EXTENDS)
            decl->addExtendsName(((ExtendsNode *)child)->getName());
        if (child->getTagCode()==NED_INTERFACE_NAME)
            decl->addInterfaceName(((InterfaceNameNode *)child)->getName());
    }

    if (node->getTagCode()==NED_SIMPLE_MODULE || node->getTagCode()==NED_CHANNEL)
        decl->setImplementationClassName(name);

    // add inherited parameters and gates
    //FIXME copy "interfaces" from parents too!!!!
    for (NEDElement *child=node->getFirstChildWithTag(NED_EXTENDS); child; child=child->getNextSiblingWithTag(NED_EXTENDS))
    {
        const char *superName = ((ExtendsNode *)child)->getName();
        cNEDDeclaration *superDecl = lookup2(superName);
        ASSERT(superDecl!=NULL); // because areDependenciesResolved() got passed

        // propagate ultimate super class as C++ class name for simple modules and channels
        decl->setImplementationClassName(superDecl->implementationClassName());

        // take over properties
        decl->setProperties(superDecl->properties()->dup());

        // add inherited parameters
        for (int i=0; i<superDecl->numPars(); i++)
        {
            const char *paramName = superDecl->parName(i);
            if (decl->hasPar(paramName))
                throw new cRuntimeError("already exists"); //XXX improve msg

            decl->addPar(superDecl->paramDescription(i).deepCopy());
        }

        // add inherited gates
        for (int i=0; i<superDecl->numGates(); i++)
        {
            const char *gateName = superDecl->gateName(i);
            if (decl->hasGate(gateName))
                throw new cRuntimeError("already exists"); //XXX improve msg

            decl->addGate(superDecl->gateDescription(i).deepCopy());
        }
    }

    // parse new properties & parameters
    NEDElement *parametersNode = node->getFirstChildWithTag(NED_PARAMETERS);
    if (parametersNode)
    {
        // parse properties
        updateProperties(parametersNode, decl->properties());

        // parse parameters
        for (NEDElement *child=parametersNode->getFirstChildWithTag(NED_PARAM); child; child=child->getNextSiblingWithTag(NED_PARAM))
        {
            ParamNode *paramNode = (ParamNode *)child;
            const char *paramName = paramNode->getName();
            if (!decl->hasPar(paramName))
            {
                // new parameter -- add it
                cNEDDeclaration::ParamDescription desc = extractParamDescription(paramNode);
                desc.declaredOn = name;
                decl->addPar(desc);
            }

            const cNEDDeclaration::ParamDescription& desc = decl->paramDescription(paramName);
            updateProperties(paramNode, desc.properties);

            // assign parameter
            ExpressionNode *exprNode = paramNode->getFirstExpressionChild();
            if (exprNode)
            {
                cNEDDeclaration::ParamDescription& desc = const_cast<cNEDDeclaration::ParamDescription&>(decl->paramDescription(paramName));
                cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, false);
                desc.value->setExpression(dynamicExpr);
/*FIXME
                if (paramNode->getIsDefault())
                    desc.value->markAsUnset();
*/
            }
        }
    }

    // parse new gates
    NEDElement *gatesNode = node->getFirstChildWithTag(NED_GATES);
    if (gatesNode)
    {
        for (NEDElement *child=gatesNode->getFirstChildWithTag(NED_GATE); child; child=child->getNextSiblingWithTag(NED_GATE))
        {
            GateNode *gateNode = (GateNode *)child;
            const char *gateName = gateNode->getName();
            if (!decl->hasGate(gateName))
            {
                // new gate -- add it
                cNEDDeclaration::GateDescription desc = extractGateDescription(gateNode);
                desc.declaredOn = name;
                decl->addGate(desc);
            }

            const cNEDDeclaration::GateDescription& desc = decl->gateDescription(gateName);
            updateProperties(gateNode, desc.properties);

            // assign gatesize
            ExpressionNode *exprNode = gateNode->getFirstExpressionChild();
            if (exprNode)
            {
                cDynamicExpression *gatesize = cExpressionBuilder().process(exprNode, false);
                decl->setGateSize(gateName, gatesize);
            }
        }
    }
    return decl;
}

cNEDDeclaration::ParamDescription cNEDLoader::extractParamDescription(ParamNode *paramNode)
{
    cNEDDeclaration::ParamDescription desc;
    int t = paramNode->getType();
    if (t==NED_PARTYPE_NONE)
        throw new cRuntimeError("undeclared parameter `%s'", paramNode->getName()); //XXX improve msg

    cPar::Type type = t==NED_PARTYPE_DOUBLE ? cPar::DOUBLE :
                      t==NED_PARTYPE_INT ? cPar::LONG :
                      t==NED_PARTYPE_STRING ? cPar::STRING :
                      t==NED_PARTYPE_BOOL ? cPar::BOOL :
                      t==NED_PARTYPE_XML ? cPar::XML :
                      (cPar::Type)-1;
    ASSERT(type!=-1);
    desc.value = cParValue::createWithType(type); // gets created with isSet()==false
    desc.value->setName(paramNode->getName());
    desc.value->setIsVolatile(paramNode->getIsVolatile());
    return desc;
}

cNEDDeclaration::GateDescription cNEDLoader::extractGateDescription(GateNode *gateNode)
{
    cNEDDeclaration::GateDescription desc;
    desc.name = gateNode->getName();
    int t = gateNode->getType();
    desc.type = t==NED_GATETYPE_INPUT ? cGate::INPUT :
                t==NED_GATETYPE_OUTPUT ? cGate::OUTPUT :
                t==NED_GATETYPE_INOUT ? cGate::INOUT :
                (cGate::Type)-1;
    desc.isVector = gateNode->getIsVector();
    return desc;
}

void cNEDLoader::updateProperties(NEDElement *parent, cProperties *props)
{
    for (NEDElement *child=parent->getFirstChildWithTag(NED_PROPERTY); child; child=child->getNextSiblingWithTag(NED_PROPERTY))
    {
        if (!props)
            props = new cProperties();
        PropertyNode *propNode = (PropertyNode *)child;
        const char *propName = propNode->getName();
        const char *propIndex = propNode->getIndex();
        cProperty *prop = props->get(propName, propIndex);
        if (!prop)
            props->add(prop = new cProperty(propName, propIndex));
        if (!strcmp(propName, "display"))
            updateDisplayProperty(propNode, prop);
        else
            updateProperty(propNode, prop);
    }
}

void cNEDLoader::updateProperty(PropertyNode *propNode, cProperty *prop)
{
    prop->setIsImplicit(propNode->getIsImplicit());

    for (NEDElement *child=propNode->getFirstChildWithTag(NED_PROPERTY_KEY); child; child=child->getNextSiblingWithTag(NED_PROPERTY_KEY))
    {
        PropertyKeyNode *propKeyNode = (PropertyKeyNode *)child;
        const char *key = propKeyNode->getName();
        if (!prop->hasKey(key))
            prop->addKey(key);

        // collect values
        int k = 0;
        for (NEDElement *child2=propKeyNode->getFirstChildWithTag(NED_LITERAL); child2; child2=child2->getNextSiblingWithTag(NED_LITERAL), k++)
        {
            LiteralNode *literalNode = (LiteralNode *)child2;
            const char *value = literalNode->getValue(); //XXX what about unitType()?
            if (value && *value)
            {
                if (!strcmp(value, "-"))  // "anti-value"
                    prop->setValue(key, k, "");
                else
                    prop->setValue(key, k, value);
            }
        }
    }
}

void cNEDLoader::updateDisplayProperty(PropertyNode *propNode, cProperty *prop)
{
    // @display() has to be treated specially
    // find new display string
    PropertyKeyNode *propKeyNode = (PropertyKeyNode *)propNode->getFirstChildWithTag(NED_PROPERTY_KEY);
    if (!propKeyNode)
        return;
    LiteralNode *literalNode = (LiteralNode *)propKeyNode->getFirstChildWithTag(NED_LITERAL);
    if (!literalNode)
        return;
    const char *newdisplaystring = literalNode->getValue();

    // if old display string is empty, just set it
    if (!prop->hasKey(cProperty::DEFAULTKEY))
        prop->addKey(cProperty::DEFAULTKEY);
    if (prop->numValues(cProperty::DEFAULTKEY)==0)
    {
        prop->setValue(cProperty::DEFAULTKEY, 0, newdisplaystring);
        return;
    }

    // merge
    const char *olddisplaystring = prop->value(cProperty::DEFAULTKEY, 0);
    cDisplayString d(olddisplaystring);
    cDisplayString dnew(newdisplaystring);
    d.updateWith(dnew);
    prop->setValue(cProperty::DEFAULTKEY, 0, d.toString());
}

void cNEDLoader::done()
{
    // we've loaded all NED files now, try resolving those which had missing dependencies
    tryResolvePendingDeclarations();

    for (int i=0; i<pendingList.size(); i++)
    {
        NEDElement *tree = pendingList[i];
        //FIXME if not resolved, print error...
        delete tree;
    }
}

