//==========================================================================
//   LOADNEDFILE.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2003 Andras Varga

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

#include "ctypes.h"
#include "cexception.h"
#include "globals.h"
#include "cdynamicmodule.h"
#include "cdynamicnetwork.h"


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
        else if (node->getTagCode()==NED_COMPOUND_MODULE)
        {
            // register dynamic module type
            CompoundModuleNode *compoundmodulenode = (CompoundModuleNode *)node;
            const char *name = compoundmodulenode->getName();

            // replace existing registration object
            cModuleType *oldmodtype = findModuleType(name);
            if (oldmodtype)
                delete oldmodtype;

            // create dynamic module type object
            cModuleType *modtype = new cDynamicModuleType(name, compoundmodulenode);
            modtype->setOwner(&modtypes);
            // FIXME must create cModuleInterface, too!
        }
        else if (node->getTagCode()==NED_NETWORK)
        {
            // register dynamic network type
            NetworkNode *networknode = (NetworkNode *)node;
            const char *name = networknode->getName();

            // replace existing registration object
            cNetworkType *oldnetworktype = findNetwork(name);
            if (oldnetworktype)
                delete oldnetworktype;

            // create dynamic network type object
            cNetworkType *networktype = new cDynamicNetworkType(name, networknode);
            networktype->setOwner(&networks);
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


