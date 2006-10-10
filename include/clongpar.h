//==========================================================================
//   CLONGPAR.H  - part of
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

#ifndef __CLONGPAR_H
#define __CLONGPAR_H

#include "cpar.h"

/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * @ingroup SimCore
 */
class SIM_API cLongPar : public cPar
{
  protected:
    // selector: flags & FL_ISEXPR
    union {
      cExpression *expr;
      long val;
    };

  protected:
    long evaluate() const;
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cLongPar();

    /**
     * Copy constructor.
     */
    cLongPar(const cLongPar& other) {operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cLongPar();

    /**
     * Assignment operator.
     */
    cLongPar& operator=(const cLongPar& otherpar);
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cLongPar *dup() const  {return new cLongPar(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cPar setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to long.
     */
    virtual cLongPar& setBoolValue(bool b);

    /**
     * Sets the value to the given constant.
     */
    virtual cLongPar& setLongValue(long l);

    /**
     * Converts from double.
     */
    virtual cLongPar& setDoubleValue(double d);

    /**
     * Raises an error: cannot convert string to long.
     */
    virtual cLongPar& setStringValue(const char *s);

    /**
     * Raises an error: cannot convert XML to long.
     */
    virtual cLongPar& setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual cLongPar& setExpression(cExpression *e);
    //@}

    /** @name Redefined cPar getter functions. */
    //@{

    /**
     * Raises an error: cannot convert long to bool.
     */
    virtual bool boolValue() const;

    /**
     * Returns the value of the parameter.
     */
    virtual long longValue() const;

    /**
     * Converts the value to double.
     */
    virtual double doubleValue() const;

    /**
     * Raises an error: cannot convert long to string.
     */
    virtual std::string stringValue() const;

    /**
     * Raises an error: cannot convert long to XML.
     */
    virtual cXMLElement *xmlValue() const;

    /**
     * Returns pointer to the expression stored by the object, or NULL.
     */
    virtual cExpression *expression() const;
    //@}

    /** @name Type, prompt text, input flag, change flag. */
    //@{

    /**
     * Returns 'L' (for "long").
     */
    virtual char type() const;

    /**
     * Returns true.
     */
    virtual bool isNumeric() const;
    //@}

    /** @name Redefined cPar misc functions. */
    //@{

    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst();

    /**
     * Returns the value in text form.
     */
    virtual std::string toString() const;

    /**
     * Converts from text.
     */
    virtual bool parse(const char *text);
    //@}
};

#endif


