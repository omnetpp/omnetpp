//==========================================================================
//   CEXPRESSION.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CEXPRESSION_H
#define __CEXPRESSION_H

#include "cobject.h"

NAMESPACE_BEGIN

class cXMLElement;

/**
 * Abstract base class for expression evaluators.
 *
 * @see cPar
 * @ingroup SimCore
 */
class SIM_API cExpression : public cObject
{
  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cExpression() {}

    /**
     * Copy constructor.
     */
    cExpression(const cExpression& other) {}

    /**
     * Destructor.
     */
    virtual ~cExpression() {}

    /**
     * Assignment operator.
     */
    cExpression& operator=(const cExpression& other) {return *this;}
    //@}

    /** @name Redefined cObject functions */
    //@{
    /**
     * Duplication not supported, this method is redefined to throw an error.
     */
    virtual cExpression *dup() const {copyNotSupported(); return NULL;}

    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const {return "";}

    /**
     * Redefined to "de-inherit" it.
     */
    virtual void netPack(cCommBuffer *buffer)  {throw cRuntimeError(this, eCANTPACK);}

    /**
     * Redefined to "de-inherit" it.
     */
    virtual void netUnpack(cCommBuffer *buffer)  {throw cRuntimeError(this, eCANTPACK);}
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(cComponent *context=NULL) = 0;

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     * Also throws an error if the actual unit does not match the expected unit.
     */
    virtual long longValue(cComponent *context=NULL, const char *expectedUnit=NULL) = 0;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     * Also throws an error if the actual unit does not match the expected unit.
     */
    virtual double doubleValue(cComponent *context=NULL, const char *expectedUnit=NULL) = 0;

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(cComponent *context=NULL) = 0;

    /**
     * Evaluate the expression and convert the result to an XML tree if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual cXMLElement *xmlValue(cComponent *context=NULL) = 0;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Converts the expression to string.
     */
    virtual std::string toString() const = 0;

    /**
     * Interprets the string as an expression, and store it.
     * If the text cannot be parsed, an exception is thrown, which
     * can be caught as std::runtime_error& if necessary.
     * An exception is also thrown if the particular cExpression subclass
     * does not support parsing.
     */
    virtual void parse(const char *text) = 0;

    /**
     * Compares two expressions. Makes it possible to use cExpression
     * as (part of) a key in std::map or std::set.
     */
    virtual int compare(const cExpression *other) const = 0;
    //@}
};

NAMESPACE_END


#endif


