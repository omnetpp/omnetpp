//==========================================================================
// neddtdvalidatorbase.h -
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


#ifndef __NEDDTDVALIDATORBASE_H
#define __NEDDTDVALIDATORBASE_H

#include "nedvalidator.h"

/**
 * Adds utility methods for DTD validation to NEDValidatorBase.
 *
 * @ingroup Validation
 */
class NEDDTDValidatorBase : public NEDValidatorBase
{
  protected:
    /** @name Utility functions */
    //@{
    void checkSequence(NEDElement *node, int tags[], char mult[], int n);
    void checkChoice(NEDElement *node, int tags[], int n, char mult);
    void checkEmpty(NEDElement *node);
    void checkRequiredAttribute(NEDElement *node, const char *attr);
    void checkEnumeratedAttribute(NEDElement *node, const char *attr, const char *vals[], int n);
    void checkNameAttribute(NEDElement *node, const char *attr);
    void checkCommentAttribute(NEDElement *node, const char *attr);
    void checkNMTokenAttribute(NEDElement *node, const char *attr);
    //@}
};

#endif

