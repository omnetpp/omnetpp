//==========================================================================
// neddtdvalidatorbase.h -
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


#ifndef __OMNETPP_NEDXML_NEDDTDVALIDATORBASE_H
#define __OMNETPP_NEDXML_NEDDTDVALIDATORBASE_H

#include "nedvalidator.h"

namespace omnetpp {
namespace nedxml {

/**
 * Adds utility methods for DTD validation to NEDValidatorBase.
 *
 * @ingroup Validation
 */
class NEDXML_API NEDDTDValidatorBase : public NEDValidatorBase
{
  protected:
    struct Choice {
        int tags[20]; // array terminated by NED_NULL (increase size if needed)
        char mult;
    };

    // helper function
    void tryCheckChoice(ASTNode *node, ASTNode *&curchild, int tags[], char mult);

    /** @name Utility functions */
    //@{
    void checkSequence(ASTNode *node, int tags[], char mult[]);
    void checkChoice(ASTNode *node, int tags[], char mult);
    void checkSeqOfChoices(ASTNode *node, Choice choices[], int n);
    void checkEmpty(ASTNode *node);
    void checkRequiredAttribute(ASTNode *node, const char *attr);
    void checkEnumeratedAttribute(ASTNode *node, const char *attr, const char *vals[], int n);
    void checkNameAttribute(ASTNode *node, const char *attr);
    void checkCommentAttribute(ASTNode *node, const char *attr);
    //@}
  public:
    NEDDTDValidatorBase(ErrorStore *e) : NEDValidatorBase(e) {}
};

} // namespace nedxml
}  // namespace omnetpp


#endif

