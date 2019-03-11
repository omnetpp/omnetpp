//==========================================================================
//   CDYNAMICEXPRESSION.H  - part of
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

#ifndef __OMNETPP_CDYNAMICEXPRESSION_H
#define __OMNETPP_CDYNAMICEXPRESSION_H

#include "cnedvalue.h"
#include "cexpression.h"
#include "cstringpool.h"

namespace omnetpp {

namespace common { class Expression; }

class cXMLElement;
class cPar;
class cNedMathFunction;
class cNedFunction;

/**
 * @brief A stack-based expression evaluator class, for dynamically created
 * expressions.
 *
 * @ingroup SimSupport
 */
class SIM_API cDynamicExpression : public cExpression
{
  protected:
    common::Expression *expression = nullptr;

  private:
    void copy(const cDynamicExpression& other);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cDynamicExpression();

    /**
     * Copy constructor.
     */
    cDynamicExpression(const cDynamicExpression& other) : cExpression(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cDynamicExpression();

    /**
     * Assignment operator.
     */
    cDynamicExpression& operator=(const cDynamicExpression& other);
    //@}

    /** @name Redefined cObject functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cDynamicExpression *dup() const override  {return new cDynamicExpression(*this);}

    /**
     * Converts the expression to string.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    // Note: parsimPack()/parsimUnpack() de-inherited in cExpression.
    //@}

    /** @name Setter and evaluator methods. */
    //@{
    /**
     * Evaluate the expression, and return the results as a cNedValue.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual cNedValue evaluate(Context *context) const override;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(Context *context) const override;

    /**
     * Evaluate the expression and convert the result to intpar_t if possible;
     * throw an error if conversion from that type is not supported, or
     * the value of out of the range of intpar_t.
     */
    virtual intpar_t intValue(Context *context, const char *expectedUnit=nullptr) const override;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(Context *context, const char *expectedUnit=nullptr) const override;

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(Context *context) const override;

    /**
     * Evaluate the expression and convert the result to an XML tree if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual cXMLElement *xmlValue(Context *context) const override;

    using cExpression::evaluate;
    using cExpression::boolValue;
    using cExpression::intValue;
    using cExpression::doubleValue;
    using cExpression::stringValue;
    using cExpression::xmlValue;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Interprets the string as an expression, and stores it.
     */
    virtual void parse(const char *text) override {parse(text, true, true);}

    /**
     * Interprets the string as an expression, and stores it.
     */
    virtual void parse(const char *text, bool inSubcomponentScope, bool inInifile);

    /**
     * Compares two expressions.
     */
    virtual int compare(const cExpression *other) const override;

    /**
     * Returns true if the expression is just a literal (or equivalent to one,
     * like "2+2"). This can be used for optimization.
     */
    virtual bool isAConstant() const override;

    /**
     * Convert the given number into the target unit (e.g. milliwatt to watt).
     * Throws an exception if conversion is not possible (unknown/unrelated units).
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);

    //@}
};


}  // namespace omnetpp


#endif


