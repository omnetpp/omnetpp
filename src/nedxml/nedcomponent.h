//==========================================================================
// NEDCOMPONENT.H -
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


#ifndef __NEDCOMPONENT_H
#define __NEDCOMPONENT_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"


/**
 * Wraps a NEDElement tree of a NED declaration (module, channel,
 * module interface or channel interface)
 *
 * @ingroup NEDCompiler
 */
class NEDComponent
{
  protected:
    NEDElement *tree;

  public:
    NEDComponent(NEDElement *tree);
    ~NEDComponent();
    NEDElement *getTree();
};

#endif

