//==========================================================================
//   CDOUBLEPAR.H  - part of
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

#ifndef __CDOUBLEPAR_H
#define __CDOUBLEPAR_H

#include "cparvalue.h"

NAMESPACE_BEGIN

/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * @ingroup Internals
 */
class SIM_API cDoublePar : public cParValue
{
  protected:
    // selector: flags & FL_ISEXPR
    union {
      cExpression *expr;
      double val;
    };

  protected:
    double evaluate(cComponent *context) const;
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cDoublePar();

    /**
     * Copy constructor.
     */
    cDoublePar(const cDoublePar& other) {setName(other.name()); operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cDoublePar();

    /**
     * Assignment operator.
     */
    void operator=(const cDoublePar& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cDoublePar *dup() const  {return new cDoublePar(*this);}

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cParValue setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to double.
     */
    virtual void setBoolValue(bool b);

    /**
     * Converts from long.
     */
    virtual void setLongValue(long l);

    /**
     * Sets the value to the given constant.
     */
    virtual void setDoubleValue(double d);

    /**
     * Raises an error: cannot convert string to double.
     */
    virtual void setStringValue(const char *s);

    /**
     * Raises an error: cannot convert XML to double.
     */
    virtual void setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual void setExpression(cExpression *e);
    //@}

    /** @name Redefined cParValue getter functions. */
    //@{

    /**
     * Raises an error: cannot convert double to bool.
     */
    virtual bool boolValue(cComponent *context) const;

    /**
     * Converts the value to long.
     */
    virtual long longValue(cComponent *context) const;

    /**
     * Returns the value of the parameter.
     */
    virtual double doubleValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert double to string.
     */
    virtual const char *stringValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert double to string.
     */
    virtual std::string stdstringValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert double to XML.
     */
    virtual cXMLElement *xmlValue(cComponent *context) const;

    /**
     * Returns pointer to the expression stored by the object, or NULL.
     */
    virtual cExpression *expression() const;
    //@}

    /** @name Type, prompt text, input flag, change flag. */
    //@{

    /**
     * Returns DOUBLE.
     */
    virtual Type type() const;

    /**
     * Returns true.
     */
    virtual bool isNumeric() const;
    //@}

    /** @name Redefined cParValue misc functions. */
    //@{

    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst(cComponent *context);

    /**
     * Returns the value in text form.
     */
    virtual std::string toString() const;

    /**
     * Converts from text.
     */
    virtual void parse(const char *text);

    /**
     * Object comparison.
     */
    virtual int compare(const cParValue *other) const;
    //@}
};

NAMESPACE_END


#endif


