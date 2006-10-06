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

#include "cpar.h"

/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * @ingroup SimCore
 */
class SIM_API cDoublePar : public cPar
{
  protected:
    // selector: flags & FL_ISEXPR
    union {
      cExpression *expr;
      double val;
    };

  protected:
    double evaluate() const;
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
    cDoublePar(const cDoublePar& other) {operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cDoublePar();

    /**
     * Assignment operator.
     */
    cDoublePar& operator=(const cDoublePar& otherpar);
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cDoublePar *dup() const  {return new cDoublePar(*this);}

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
     * Raises an error: cannot convert bool to double.
     */
    virtual cDoublePar& setBoolValue(bool b);

    /**
     * Converts from long.
     */
    virtual cDoublePar& setLongValue(long l);

    /**
     * Sets the value to the given constant.
     */
    virtual cDoublePar& setDoubleValue(double d);

    /**
     * Raises an error: cannot convert string to double.
     */
    virtual cDoublePar& setStringValue(const char *s);

    /**
     * Raises an error: cannot convert XML to double.
     */
    virtual cDoublePar& setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual cDoublePar& setExpression(cExpression *e);
    //@}

    /** @name Redefined cPar getter functions. */
    //@{

    /**
     * Raises an error: cannot convert double to bool.
     */
    virtual bool boolValue() const;

    /**
     * Converts the value to long.
     */
    virtual long longValue() const;

    /**
     * Returns the value of the parameter.
     */
    virtual double doubleValue() const;

    /**
     * Raises an error: cannot convert double to string.
     */
    virtual std::string stringValue() const;

    /**
     * Raises an error: cannot convert double to XML.
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
     * Returns 'D' (for "double").
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
     * Reads the object value from the ini file or from the user.
     */
    virtual cDoublePar& read();

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


