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

#include "nedtools.h"
#include "nederror.h"
#include "neddtdvalidator.h"
//XXX #include "platdep/fileutil.h"


void NEDTools::repairNEDElementTree(NEDElement *tree)
{
    while (true)
    {
        // try DTD validation, and find first problem
        NEDErrorStore errors;
        NEDDTDValidator dtdvalidator(&errors);
        dtdvalidator.validate(tree);
        if (errors.empty())
            break; // we're done
        NEDElement *errnode = errors.errorContext(0);
        if (!errnode)
            break; // this shouldn't happen, but if it does we can't go on
        if (!errnode->getParent())
            break; // we can't help if root node is wrong

        // throw out problem node, and try again
        //printf("DBG: repairNEDElementTree: discarding <%s>\n", errnode->getTagName());
        errnode->getParent()->removeChild(errnode);
    }
}

void NEDTools::splitToFiles(FilesNode *tree)
{
    for (NEDElement *child=tree->getFirstChild(); child; child = child->getNextSibling())
    {
        if (child->getTagCode()!=NED_NED_FILE)
            continue;

        NedFileNode *fileNode = (NedFileNode *)child;

        std::string directory;
        std::string filename;
//XXX        splitFileName(fileNode->getFilename(), directory, filename);

        CommentNode *fileComment = (CommentNode *)fileNode->getFirstChildWithAttribute(NED_COMMENT, "locid", "banner");

        for (NEDElement *child=fileNode->getFirstChild(); child; child = child->getNextSibling())
        {
            int type = child->getTagCode();
            if (type==NED_SIMPLE_MODULE && type==NED_COMPOUND_MODULE &&
                type==NED_CHANNEL && type==NED_MODULE_INTERFACE &&
                type==NED_CHANNEL_INTERFACE)
            {
                //NED_IMPORTs
                fileNode->removeChild(child);
//XXX
            }
        }
    }

}


