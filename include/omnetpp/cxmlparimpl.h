//==========================================================================
//   CXMLPARIMPL.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CXMLPARIMPL_H
#define __CXMLPARIMPL_H

#include "cparimpl.h"

NAMESPACE_BEGIN

/**
 * A cParImpl subclass that stores a module/channel parameter of type XML.
 *
 * @ingroup Internals
 */
class SIM_API cXMLParImpl : public cParImpl
{
  protected:
    // selector: flags & FL_ISEXPR
    cExpression *expr;
    cXMLElement *val;

  private:
    void copy(const cXMLParImpl& other);

  protected:
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cXMLParImpl();

    /**
     * Copy constructor.
     */
    cXMLParImpl(const cXMLParImpl& other) : cParImpl(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cXMLParImpl();

    /**
     * Assignment operator.
     */
    void operator=(const cXMLParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cXMLParImpl *dup() const  {return new cXMLParImpl(*this);}

    /**
     * Returns a multi-line description of the contained XML element.
     */
    virtual std::string detailedInfo() const;

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to XML.
     */
    virtual void setBoolValue(bool b);

    /**
     * Raises an error: cannot convert long to XML.
     */
    virtual void setLongValue(long l);

    /**
     * Raises an error: cannot convert double to XML.
     */
    virtual void setDoubleValue(double d);

    /**
     * Raises an error: cannot convert string to XML.
     */
    virtual void setStringValue(const char *s);

    /**
     * Sets the value to the given cXMLElement tree.
     */
    virtual void setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual void setExpression(cExpression *e);
    //@}

    /** @name Redefined cParImpl getter functions. */
    //@{

    /**
     * Raises an error: cannot convert XML to bool.
     */
    virtual bool boolValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert XML to long.
     */
    virtual long longValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert XML to double.
     */
    virtual double doubleValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert XML to string.
     */
    virtual const char *stringValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert XML to string.
     */
    virtual std::string stdstringValue(cComponent *context) const;

    /**
     * Returns the value of the parameter.
     */
    virtual cXMLElement *xmlValue(cComponent *context) const;

    /**
     * Returns pointer to the expression stored by the object, or NULL.
     */
    virtual cExpression *getExpression() const;
    //@}

    /** @name Type, prompt text, input flag, change flag. */
    //@{

    /**
     * Returns XML.
     */
    virtual Type getType() const;

    /**
     * Returns false.
     */
    virtual bool isNumeric() const;
    //@}

    /** @name Redefined cParImpl misc functions. */
    //@{

    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst(cComponent *context);

    /**
     * Returns the value in text form.
     */
    virtual std::string str() const;

    /**
     * Converts from text.
     */
    virtual void parse(const char *text);

    /**
     * Object comparison.
     */
    virtual int compare(const cParImpl *other) const;
    //@}
};

NAMESPACE_END


#endif


