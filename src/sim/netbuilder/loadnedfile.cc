//==========================================================================
//   LOADNEDFILE.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#include "nedelements.h"
#include "nederror.h"
#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"

#include "loadnedfile.h"
#include "cneddeclaration.h"
#include "cproperty.h"
#include "cproperties.h"
#include "ccomponenttype.h"
#include "cexception.h"
#include "globals.h"
#include "opp_string.h"
#include "cenvir.h"
#include "cdynamicmodule.h"
#include "cdynamicchannel.h"
#include "cexpressionbuilder.h"
#include "cpar.h"
#include "clongpar.h"
#include "cdoublepar.h"


cNEDFileLoader::cNEDFileLoader()
{
}

cNEDFileLoader::~cNEDFileLoader()
{
    if (!pendingList.empty())
        done();
}

NEDElement *cNEDFileLoader::parseAndValidateNedFile(const char *fname, bool isXML)
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

/*XXX
    NEDBasicValidator basicvalidator(true);
    basicvalidator.validate(tree);
    if (errorsOccurred())
    {
        delete tree;
        throw new cRuntimeError("errors during validation of file `%s'", fname);
    }
*/
    return tree;
}

void cNEDFileLoader::loadNedFile(const char *nedfname, bool isXML)
{
    // parse file
    NEDElement *tree = parseAndValidateNedFile(nedfname, isXML);

    // pick interesting parts
    NEDElement *node;
    while ((node = tree->getFirstChild())!=NULL)
    {
        tree->removeChild(node);

        if (node->getTagCode()==NED_IMPORT ||
            node->getTagCode()==NED_PROPERTY_DECL ||
            node->getTagCode()==NED_PROPERTY)
        {
            // ignore
            delete node;
        }
        else if (node->getTagCode()==NED_SIMPLE_MODULE ||
                 node->getTagCode()==NED_COMPOUND_MODULE ||
                 node->getTagCode()==NED_MODULE_INTERFACE ||
                 node->getTagCode()==NED_CHANNEL ||
                 node->getTagCode()==NED_CHANNEL_INTERFACE)
        {
            if (areDependenciesResolved(node))
                buildNEDDeclaration(node);
            else
                pendingList.push_back(node);
        }
        else
        {
            opp_string tagname(node->getTagName());
            delete node;
            delete tree;
            throw new cRuntimeError("Error loading `%s': unsupported element", tagname.c_str());
        }
    }

    // and delete the rest
    delete tree;
}

bool cNEDFileLoader::areDependenciesResolved(NEDElement *node)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()!=NED_EXTENDS && child->getTagCode()!=NED_INTERFACE_NAME)
            continue;

        const char *name = child->getAttribute("name");
        cNEDDeclaration *decl = cNEDDeclaration::find(name);
        if (!decl)
            return false;
    }
    return true;
}

void cNEDFileLoader::tryResolvePendingDeclarations()
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

void cNEDFileLoader::buildNEDDeclaration(NEDElement *node)
{
    const char *name = node->getAttribute("name");

    // and replace existing one
    cNEDDeclaration *oldDecl = cNEDDeclaration::find(name);
    if (oldDecl)
    {
        ev.printf("Warning: replacing existing declaration for type `%s'\n", name);
        delete nedDeclarations.instance()->remove(oldDecl);
    }

    // create declaration object
    cNEDDeclaration *decl = new cNEDDeclaration(name);
    nedDeclarations.instance()->add(decl);

    // add "extends" and "like" names
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()==NED_EXTENDS)
            decl->addExtendsName(((ExtendsNode *)child)->getName());
        if (child->getTagCode()==NED_INTERFACE_NAME)
            decl->addInterfaceName(((InterfaceNameNode *)child)->getName());
    }

    // add inherited parameters and gates
    for (NEDElement *child=node->getFirstChildWithTag(NED_EXTENDS); child; child=child->getNextSiblingWithTag(NED_EXTENDS))
    {
        const char *superName = ((ExtendsNode *)child)->getName();
        cNEDDeclaration *superDecl = cNEDDeclaration::find(superName);
        ASSERT(superDecl);

        // add inherited parameters
        for (int i=0; i<superDecl->numPars(); i++)
        {
            const char *paramName = superDecl->paramName(i);
            if (decl->hasPar(paramName))
                throw new cRuntimeError("already exists"); //XXX improve

            decl->addPar(superDecl->paramDescription(i).deepCopy());
        }

        // add inherited gates
        for (int i=0; i<superDecl->numGates(); i++)
        {
            const char *gateName = superDecl->gateName(i);
            if (decl->hasGate(gateName))
                throw new cRuntimeError("already exists"); //XXX improve

            decl->addGate(superDecl->gateDescription(i).deepCopy());
        }
    }

    // parse new parameters
    NEDElement *parametersNode = node->getFirstChildWithTag(NED_PARAMETERS);
    if (parametersNode)
    {
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

            // assign parameter
            ExpressionNode *exprNode = paramNode->getFirstExpressionChild();
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, false);
            cPar *paramValue = new cDoublePar(); //FIXME of the right subclass!!!
            paramValue->setExpression(dynamicExpr);
            //FIXME add to declaration! decl->setParamValue(paramValue);
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

            // assign gatesize
            ExpressionNode *exprNode = gateNode->getFirstExpressionChild();
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, false);
            cPar *gatesize = new cLongPar();
            gatesize->setExpression(dynamicExpr);
            //FIXME add to declaration! decl->setGatesize(gatesize);
        }
    }

}

cNEDDeclaration::ParamDescription cNEDFileLoader::extractParamDescription(ParamNode *paramNode)
{
    cNEDDeclaration::ParamDescription desc;
    desc.name = paramNode->getName();
    int t = paramNode->getType();
    desc.type = t==NED_PARTYPE_DOUBLE ? cPar::DOUBLE :
                t==NED_PARTYPE_INT ? cPar::LONG :
                t==NED_PARTYPE_STRING ? cPar::STRING :
                t==NED_PARTYPE_BOOL ? cPar::BOOL :
                t==NED_PARTYPE_XML ? cPar::XML :
                (cPar::Type)-1;
    desc.isVolatile = paramNode->getIsVolatile();
    desc.properties = extractProperties(paramNode);
    return desc;
}

cNEDDeclaration::GateDescription cNEDFileLoader::extractGateDescription(GateNode *gateNode)
{
    cNEDDeclaration::GateDescription desc;
    desc.name = gateNode->getName();
    int t = gateNode->getType();
    desc.type = t==NED_GATETYPE_INPUT ? cGate::INPUT :
                t==NED_GATETYPE_OUTPUT ? cGate::OUTPUT :
                t==NED_GATETYPE_INOUT ? cGate::INOUT :
                (cGate::Type)-1;
    desc.isVector = gateNode->getIsVector();
    desc.properties = extractProperties(gateNode);
    return desc;
}

cProperties *cNEDFileLoader::extractProperties(NEDElement *parent)
{
    cProperties *props = NULL;
    for (NEDElement *child=parent->getFirstChildWithTag(NED_PROPERTY); child; child=child->getNextSiblingWithTag(NED_PROPERTY))
    {
        if (!props)
            props = new cProperties();
        cProperty *prop = extractProperty((PropertyNode *)child);
        props->add(prop);

    }
    return props;
}

cProperty *cNEDFileLoader::extractProperty(PropertyNode *propNode)
{
    cProperty *prop = new cProperty(propNode->getName());
    prop->setIsImplicit(propNode->getIsImplicit());
    prop->setIndex(atoi(propNode->getIndex()));

    for (NEDElement *child=propNode->getFirstChildWithTag(NED_PROPERTY_KEY); child; child=child->getNextSiblingWithTag(NED_PROPERTY_KEY))
    {
        PropertyKeyNode *propKeyNode = (PropertyKeyNode *)child;

        // collect values
        std::vector<const char *> valueVector;
        for (NEDElement *child2=propKeyNode->getFirstChildWithTag(NED_LITERAL); child2; child2=child2->getNextSiblingWithTag(NED_LITERAL))
        {
            LiteralNode *literalNode = (LiteralNode *)child2;
            const char *value = literalNode->getValue(); //XXX what about unitType()?
            valueVector.push_back(value);
        }
        prop->setValues(propKeyNode->getName(), valueVector);
    }
    return prop;
}

void cNEDFileLoader::done()
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


