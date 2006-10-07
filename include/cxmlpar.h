//==========================================================================
//   CXMLPAR.H  - part of
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

#ifndef __CXMLPAR_H
#define __CXMLPAR_H

#include "cpar.h"

/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * @ingroup SimCore
 */
class SIM_API cXMLPar : public cPar
{
  protected:
    // selector: flags & FL_ISEXPR
    cExpression *expr;
    cXMLElement *val;

  protected:
    cXMLElement *evaluate() const;
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cXMLPar();

    /**
     * Copy constructor.
     */
    cXMLPar(const cXMLPar& other) {operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cXMLPar();

    /**
     * Assignment operator.
     */
    cXMLPar& operator=(const cXMLPar& otherpar);
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cXMLPar *dup() const  {return new cXMLPar(*this);}

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
     * Raises an error: cannot convert bool to XML.
     */
    virtual cXMLPar& setBoolValue(bool b);

    /**
     * Raises an error: cannot convert long to XML.
     */
    virtual cXMLPar& setLongValue(long l);

    /**
     * Raises an error: cannot convert double to XML.
     */
    virtual cXMLPar& setDoubleValue(double d);

    /**
     * Raises an error: cannot convert string to XML.
     */
    virtual cXMLPar& setStringValue(const char *s);

    /**
     * Sets the value to the given cXMLElement tree.
     */
    virtual cXMLPar& setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual cXMLPar& setExpression(cExpression *e);
    //@}

    /** @name Redefined cPar getter functions. */
    //@{

    /**
     * Raises an error: cannot convert XML to bool.
     */
    virtual bool boolValue() const;

    /**
     * Raises an error: cannot convert XML to long.
     */
    virtual long longValue() const;

    /**
     * Raises an error: cannot convert XML to double.
     */
    virtual double doubleValue() const;

    /**
     * Raises an error: cannot convert XML to string.
     */
    virtual std::string stringValue() const;

    /**
     * Returns the value of the parameter.
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
     * Returns 'X' (for "XML").
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
    virtual cXMLPar& read();

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


