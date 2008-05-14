//==========================================================================
// nedtools.h -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDTOOLS_H
#define __NEDTOOLS_H

#include "nedelements.h"

NAMESPACE_BEGIN

/**
 * Misc NED utilities
 *
 * @ingroup Validation
 */
class NEDXML_API NEDTools
{
  public:
    /**
     * Tries to makes a NEDElement tree comply with the DTD by gradually
     * discarding elements that fail DTD validation. Success is not guaranteed.
     */
    static void repairNEDElementTree(NEDElement *tree);

    /**
     * Splits each NEDFile under FilesElement to several files, to place
     * every module or interface into its own file.
     */
    static void splitToFiles(FilesElement *tree);
};

NAMESPACE_END


#endif

