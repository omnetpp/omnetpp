//==========================================================================
//   CSTRINGPAR.H  - part of
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

#ifndef __CSTRINGPAR_H
#define __CSTRINGPAR_H

#include "cpar.h"

/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * @ingroup SimCore
 */
class SIM_API cStringPar : public cPar
{
  protected:
    // only one of expr and val is active at a time; however, we cannot use
    // union here because std::string has constructor/destructor.
    // selector: flags & FL_ISEXPR
    cExpression *expr;
    std::string val;   // FIXME should be char*, and POOLED!!!

  protected:
    std::string evaluate() const;
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cStringPar();

    /**
     * Copy constructor.
     */
    cStringPar(const cStringPar& other) {operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cStringPar();

    /**
     * Assignment operator.
     */
    cStringPar& operator=(const cStringPar& otherpar);
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cStringPar *dup() const  {return new cStringPar(*this);}

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
     * Raises an error: cannot convert bool to string.
     */
    virtual cStringPar& setBoolValue(bool b);

    /**
     * Raises an error: cannot convert long to string.
     */
    virtual cStringPar& setLongValue(long l);

    /**
     * Raises an error: cannot convert double to string.
     */
    virtual cStringPar& setDoubleValue(double d);

    /**
     * Sets the value to the given string. NULL is accepted as "".
     */
    virtual cStringPar& setStringValue(const char *s);

    /**
     * Raises an error: cannot convert XML to long.
     */
    virtual cStringPar& setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual cStringPar& setExpression(cExpression *e);
    //@}

    /** @name Redefined cPar getter functions. */
    //@{

    /**
     * Raises an error: cannot convert string to bool.
     */
    virtual bool boolValue() const;

    /**
     * Raises an error: cannot convert string to long.
     */
    virtual long longValue() const;

    /**
     * Raises an error: cannot convert string to double.
     */
    virtual double doubleValue() const;

    /**
     * Returns the value of the parameter.
     */
    virtual std::string stringValue() const;

    /**
     * Raises an error: cannot convert string to XML.
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
     * Returns 'S' (for "string").
     */
    virtual char type() const;

    /**
     * Returns false.
     */
    virtual bool isNumeric() const;
    //@}

    /** @name Redefined cPar misc functions. */
    //@{

    /**
     * Reads the object value from the ini file or from the user.
     */
    virtual cStringPar& read();

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


