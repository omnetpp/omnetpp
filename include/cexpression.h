//==========================================================================
//   CABSTRACTEXPR.H  - part of
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

#include "cpolymorphic.h"

class cXMLElement;


/**
 * Abstract base class for expression evaluators.
 *
 * FIXME create cCompiledExpressionBase etc!!
 *
 * @see cPar
 * @ingroup SimCore
 */
class SIM_API cExpression : public cPolymorphic
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

    /** @name Redefined cPolymorphic functions */
    //@{
    /**
     * Duplication not supported, this method is redefined to throw an error.
     */
    virtual cExpression *dup() const {copyNotSupported(); return NULL;}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const {return "";}

    /**
     * Redefined to "de-inherit" it.
     */
    virtual void netPack(cCommBuffer *buffer) {} //FIXME exception?

    /**
     * Redefined to "de-inherit" it.
     */
    virtual void netUnpack(cCommBuffer *buffer) {} //FIXME exception?
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
//FIXME perhaps rename these methods to evaluateXXX()
    virtual bool boolValue(cComponent *context=NULL) = 0;

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual long longValue(cComponent *context=NULL) = 0;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(cComponent *context=NULL) = 0;

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
     * Reads the object value from the ini file or from the user.
     * FIXME clarify this....FIXME do we really need this function HERE? not only in cPar?
     */
    virtual cExpression& read() = 0;

    /**
     * Converts the expression to string.
     */
    virtual std::string toString() const = 0;

    /**
     * Interprets the string as an expression, and store it.
     * This may not be possible with all subclasses.
     */
    virtual bool parse(const char *text) = 0;
    //@}
};

#endif


