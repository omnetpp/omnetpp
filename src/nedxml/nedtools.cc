//==========================================================================
// nedtools.cc -
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

#include "nederror.h"
#include "neddtdvalidator.h"


void NEDTools::repairNEDElementTree(NEDElement *tree)
{
    while (true)
    {
        // try DTD validation, and find first problem
        NEDErrorStore errors;
        NEDDTDValidator dtdvalidator(errors);
        dtdvalidator.validate(tree);
        if (errors->empty())
            break; // we're done
        NEDElement *errnode = errors.errorContext(0);
        if (!errnode)
            break; // this shouldn't happen, but if it does we can't go on
        if (!errnode->getParent())
            break; // we can't help if root node is wrong

        // throw out problem node, and try again
        errnode->getParent()->removeChild(errnode);
    }
}



