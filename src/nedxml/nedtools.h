//==========================================================================
// nedtools.h -
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


#ifndef __OMNETPP_NEDXML_NEDTOOLS_H
#define __OMNETPP_NEDXML_NEDTOOLS_H

#include "nedelements.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Misc NED utilities
 *
 * @ingroup Validation
 */
class NEDXML_API NedTools
{
  public:
    /**
     * Tries to makes an ASTNode tree comply with the DTD by gradually
     * discarding elements that fail DTD validation. Success is not guaranteed.
     */
    static void repairNedAST(ASTNode *tree);

    /**
     * Tries to makes an ASTNode tree comply with the DTD by gradually
     * discarding elements that fail DTD validation. Success is not guaranteed.
     */
    static void repairMsgAST(ASTNode *tree);

    /**
     * Splits each NedFile under FilesElement to several files so that
     * every module or interface is placed into its own file.
     */
    static void splitNedFiles(FilesElement *tree);
};

} // namespace nedxml
}  // namespace omnetpp


#endif

