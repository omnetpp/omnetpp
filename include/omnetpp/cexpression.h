//==========================================================================
//   CEXPRESSION.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CEXPRESSION_H
#define __OMNETPP_CEXPRESSION_H

#include "cobject.h"
#include "cexception.h"
#include "cnedvalue.h"

namespace omnetpp {

class cXMLElement;

/**
 * @brief Abstract base class for expression evaluators.
 *
 * @see cPar
 * @ingroup SimSupport
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
    cExpression(const cExpression& other) : cObject(other) {}

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
    virtual cExpression *dup() const override {copyNotSupported(); return nullptr;}

    /**
     * Converts the expression to string.
     */
    virtual std::string str() const override {return "";}

    /**
     * Redefined to "de-inherit" it.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override  {throw cRuntimeError(this, E_CANTPACK);}

    /**
     * Redefined to "de-inherit" it.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override  {throw cRuntimeError(this, E_CANTPACK);}
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Evaluate the expression and return the result in a cNEDValue.
     */
    virtual cNEDValue evaluate(cComponent *context=nullptr) const = 0;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(cComponent *context=nullptr) = 0;

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     * Also throws an error if the actual unit does not match the expected unit.
     */
    virtual long longValue(cComponent *context=nullptr, const char *expectedUnit=nullptr) = 0;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     * Also throws an error if the actual unit does not match the expected unit.
     */
    virtual double doubleValue(cComponent *context=nullptr, const char *expectedUnit=nullptr) = 0;

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(cComponent *context=nullptr) = 0;

    /**
     * Evaluate the expression and convert the result to an XML tree if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual cXMLElement *xmlValue(cComponent *context=nullptr) = 0;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
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

    /**
     * Returns true if this expression contains const subexpressions.
     */
    virtual bool containsConstSubexpressions() const = 0;

    /**
     * Evaluates const subexpressions, and replaces them with their values.
     * See cDynamicExpression::Elem::CONSTSUBEXPR.
     */
    virtual void evaluateConstSubexpressions(cComponent *context) = 0;
    //@}
};

}  // namespace omnetpp


#endif


