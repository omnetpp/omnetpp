//==========================================================================
//   LOADNEDFILE.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

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

#include "macros.h"
#include "ctypes.h"
#include "cexception.h"
#include "globals.h"
#include "cdynamicmodule.h"
#include "cdynamicnetwork.h"


cModuleInterface *createModuleInterfaceFrom(NEDElement *modulenode)
{
    ASSERT(modulenode->getTagCode()==NED_SIMPLE_MODULE || modulenode->getTagCode()==NED_COMPOUND_MODULE);

    cModuleInterface *modif = new cModuleInterface(modulenode->getAttribute("name"));

    // loop through parameters and add them
    ParamsNode *params = (ParamsNode *) modulenode->getFirstChildWithTag(NED_PARAMS);
    if (params)
    {
        modif->allocateParamDecls(params->getNumChildrenWithTag(NED_PARAM));
        for (ParamNode *par=params->getFirstParamChild(); par; par=par->getNextParamNodeSibling())
        {
            const char *parname = par->getName();
            const char *datatype = par->getDataType();
            const char *typecode = NULL;
            if (strcmp(datatype,"numeric"))
               typecode = ParType_Numeric;
            if (strcmp(datatype,"numeric const"))
               typecode = ParType_Numeric ParType_Const;
            if (strcmp(datatype,"string"))
               typecode = ParType_String;
            if (strcmp(datatype,"bool"))
               typecode = ParType_Bool;
            if (strcmp(datatype,"any"))
               typecode = ParType_Any;
            else
               throw new cException("createModuleInterface(): unknown parameter type `%s'", datatype);
            modif->addParamDecl(parname, typecode);
        }
    }

    // loop through gates and add them
    GatesNode *gates = (GatesNode *) modulenode->getFirstChildWithTag(NED_GATES);
    if (gates)
    {
        modif->allocateGateDecls(gates->getNumChildrenWithTag(NED_GATE));
        for (GateNode *gate=gates->getFirstGateChild(); gate; gate=gate->getNextGateNodeSibling())
        {
            const char *gatename = gate->getName();
            int gatedir = gate->getDirection();
            bool isvec = gate->getIsVector();
            modif->addGateDecl(gatename, gatedir==NED_GATEDIR_INPUT ? 'I' : 'O', isvec);
        }
    }
    return modif;
}


void loadNedFile(const char *fname, bool isXML)
{
    // load file
    NEDElement *tree = 0;
    clearErrors();
    if (isXML)
    {
        tree = parseXML(fname);
    }
    else
    {
        NEDParser parser;
        parser.setParseExpressions(true);
        parser.setStoreSource(false);
        parser.parseFile(fname);
        tree = parser.getTree();
    }
    if (errorsOccurred())
    {
        delete tree;
        throw new cException("errors while loading or parsing file `%s'", fname);
    }

    // DTD validation and additional basic validation
    NEDDTDValidator dtdvalidator;
    dtdvalidator.validate(tree);
    if (errorsOccurred())
    {
        delete tree;
        throw new cException("errors during DTD validation of file `%s'", fname);
    }

    NEDBasicValidator basicvalidator(true);
    basicvalidator.validate(tree);
    if (errorsOccurred())
    {
        delete tree;
        throw new cException("errors during validation of file `%s'", fname);
    }

    // pick interesting parts
    NEDElement *node;
    while ((node = tree->getFirstChild())!=NULL)
    {
        tree->removeChild(node);

        if (node->getTagCode()==NED_IMPORT)
        {
            // ignore
            delete node;
        }
        else if (node->getTagCode()==NED_SIMPLE_MODULE)
        {
            SimpleModuleNode *simplemodulenode = (SimpleModuleNode *)node;
            const char *name = simplemodulenode->getName();

            // throw exception if the code for the simple module is not present?
            // it's enough if error occurs when module is actually needed
            //cModuleType *modtype = findModuleType(name);
            //if (!modtype) throw new cException("");

            // create module interface object
            cModuleInterface *modif = createModuleInterfaceFrom(node);

            // and replace existing one
            cModuleInterface *oldmodif = findModuleInterface(name);
            if (oldmodif)
                delete modinterfaces.instance()->remove(oldmodif);
            modinterfaces.instance()->add(modif);

        }
        else if (node->getTagCode()==NED_COMPOUND_MODULE)
        {
            CompoundModuleNode *compoundmodulenode = (CompoundModuleNode *)node;
            const char *name = compoundmodulenode->getName();

            // create module interface object
            cModuleInterface *modif = createModuleInterfaceFrom(node);

            // and replace existing one
            cModuleInterface *oldmodif = findModuleInterface(name);
            if (oldmodif)
                delete modinterfaces.instance()->remove(oldmodif);
            modinterfaces.instance()->add(modif);

            // replace existing registration object
            cModuleType *oldmodtype = findModuleType(name);
            if (oldmodtype)
                delete modtypes.instance()->remove(oldmodtype);

            // create dynamic module type object
            cModuleType *modtype = new cDynamicModuleType(name, compoundmodulenode);
            modtypes.instance()->add(modtype);
        }
        else if (node->getTagCode()==NED_NETWORK)
        {
            // register dynamic network type
            NetworkNode *networknode = (NetworkNode *)node;
            const char *name = networknode->getName();

            // replace existing registration object
            cNetworkType *oldnetworktype = findNetwork(name);
            if (oldnetworktype)
                delete networks.instance()->remove(oldnetworktype);

            // create dynamic network type object
            cNetworkType *networktype = new cDynamicNetworkType(name, networknode);
            networks.instance()->add(networktype);
        }
        else
        {
            delete tree;
            throw new cException("error loading `%s': only compund modules and networks are "
                                 "supported in dynamically loaded NED files, they may not contain "
                                 "simple modules, channels, etc.", fname);
        }
    }

    // and delete the rest
    delete tree;
}


