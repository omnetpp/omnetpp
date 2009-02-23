//==========================================================================
//   CMSGPAR.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMSGPAR_H
#define __CMSGPAR_H

#include "cownedobject.h"
#include "cmathfunction.h"

NAMESPACE_BEGIN

class  cStatistic;
class  cXMLElement;


/**
 * cMsgPar objects store a (string, bool, double, etc) value, and
 * can be attached to cMessage objects.
 *
 * <b>Note:</b> Before version 3.0, the cPar class was used for both
 * message parameters and module parameters. As of 4.0, cPar only
 * serves as module/channel parameter, and other uses of cPar in
 * existing 3.x code should be either eliminated (see next note),
 * or changed to cMsgPar.
 *
 * <b>Note2:</b> This is an obsolete class, retained to support legacy code.
 * The preferred way of adding data to cMessages since the 2.3 version is
 * via subclassing, using <tt>.msg</tt> files and the <i>opp_msgc</i> tool.
 *
 * cMsgPar supports several data types. Data types are identified by type
 * characters. The current data type is returned by getType().
 *     - basic types: <b>S</b> string, <b>B</b> bool, <b>L</b> long, <b>D</b> double
 *     - <b>F</b> math function (MathFuncNoArgs,MathFunc1Args,etc),
 *     - <b>T</b> a statistic object (subclassed from cStatistic)
 *     - <b>P</b> pointer to cOwnedObject,
 *     - <b>M</b> XML element (pointer to a cXMLElement)
 *
 * @ingroup SimCore
 */
class SIM_API cMsgPar : public cOwnedObject
{
  protected:
    static const char *possibletypes;

  private:
    enum { SHORTSTR_MAXLEN = 27 };

    char typechar;     // S/B/L/D/F/T/P/O
    bool changedflag;
    bool tkownership;

    union {
       struct { bool sht; char *str;  } ls;   // S:long string
       struct { bool sht; char str[SHORTSTR_MAXLEN+1]; } ss;  // S:short str
       struct { long val;             } lng;  // L:long,B:bool
       struct { double val;           } dbl;  // D:double
       struct { MathFunc f; int argc;
                double p1,p2,p3,p4;   } func; // F:math function
       struct { cStatistic *res;      } dtr;  // T:distribution
       struct { void *ptr;
                VoidDelFunc delfunc;
                VoidDupFunc dupfunc;
                size_t itemsize;      } ptr;  // P:void* pointer
       struct { cOwnedObject *obj;    } obj;  // O:object pointer
       struct { cXMLElement *node;    } xmlp; // M:XML element pointer
    };

  private:
    // helper func: destruct old value
    void deleteOld();

    // helper func: rand.num with given distr.(T)
    double getFromstat();

    // setFromText() helper func.
    bool setfunction(char *w);

  protected:
    /** @name Event hooks */
    //@{

    /**
     * Called each time before the value of this object changes.
     * It can be used for tracking parameter changes.
     * This default implementation does nothing.
     */
    virtual void beforeChange();

    /**
     * Called each time after the value of this object changed.
     * It can be used for tracking parameter changes.
     * This default implementation does nothing.
     */
    virtual void afterChange();
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor, creates an exact copy of the argument.
     */
    cMsgPar(const cMsgPar& other);

    /**
     * Constructor, creates a cMsgPar with the given name and long
     * ('L') as default type.
     */
    explicit cMsgPar(const char *name=NULL);

    /**
     * Constructor, creates a copy of the second argument with another
     * name.
     */
    explicit cMsgPar(const char *name, cMsgPar& other);

    /**
     * Destructor.
     */
    virtual ~cMsgPar();

    /**
     * Assignment operator. The name member doesn't get copied; see cOwnedObject's
     * operator=() for more details.
     *
     * The behavior with redirected cMsgPar objects is the following. This function
     * copies the contents of the other object (whether it is redirected or not)
     * into this object, <b>or,</b> if this object is redirected, into the object
     * this object refers to. This means that if you want to overwrite this
     * very object (and not the one it points to), you have to use
     * cancelRedirection() first.
     */
    cMsgPar& operator=(const cMsgPar& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cMsgPar *dup() const  {return new cMsgPar(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Produces a multi-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string detailedInfo() const;

    /**
     * Calls v->visit(this) for the contained object, if there's any.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    cMsgPar& setBoolValue(bool b);

    /**
     * Sets the value to the given long value.
     */
    cMsgPar& setLongValue(long l);

    /**
     * Sets the value to the given string value.
     * The cMsgPar will make its own copy of the string. NULL is also accepted
     * and treated as an empty string.
     */
    cMsgPar& setStringValue(const char *s);

    /**
     * Sets the value to the given double value.
     */
    cMsgPar& setDoubleValue(double d);

    /**
     * Sets the value to the given distribution.
     * Every time the cMsgPar's value is asked a random number produced
     * by res.random() will be returned.
     */
    cMsgPar& setDoubleValue(cStatistic *res);

    /**
     * Sets the value to the given math function with no arguments.
     * Every time the cMsgPar's value is asked the function will be called.
     */
    cMsgPar& setDoubleValue(MathFuncNoArg f);

    /**
     * Sets the value to the given math function with one argument.
     * Every time the cMsgPar's value is asked the function will be called
     * with p1 as an argument.
     */
    cMsgPar& setDoubleValue(MathFunc1Arg  f, double p1);

    /**
     * Sets the value to the given math function with two arguments.
     * Every time the cMsgPar's value is asked the function will be called
     * with p1 and p2 as an arguments.
     */
    cMsgPar& setDoubleValue(MathFunc2Args f, double p1, double p2);

    /**
     * Sets the value to the given math function with three arguments.
     * Every time the cMsgPar's value is asked the function will be called
     * with p1, p2 and p3 as an arguments.
     */
    cMsgPar& setDoubleValue(MathFunc3Args f, double p1, double p2, double p3);

    /**
     * Sets the value to the given math function with four arguments.
     * Every time the cMsgPar's value is asked the function will be called
     * with p1, p2, p3 and p4 as an arguments.
     */
    cMsgPar& setDoubleValue(MathFunc4Args f, double p1, double p2, double p3, double p4);

    /**
     * Sets the value to the given pointer. The ownership of the block
     * pointer to can be controlled using configPointer().
     *
     * @see configPointer
     */
    cMsgPar& setPointerValue(void *ptr);

    /**
     * Sets the value to the given object. Whether cMsgPar will take the
     * ownership of the object depends on the getTakeOwnership() flag.
     */
    cMsgPar& setObjectValue(cOwnedObject *obj);

    /**
     * Sets the value to the given cXMLElement.
     */
    cMsgPar& setXMLValue(cXMLElement *node);

    /**
     * Configures memory management for the void* pointer ('P') type.
     * Similar to cLinkedList::configPointer().
     *
     * <TABLE BORDER=1>
     *   <TR>
     *     <TD><b>delfunc</b></TD><TD><b>dupfunc.</b></TD><TD><b>itemsize</b></TD><TD><b>behavior</b></TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL</TD><TD>NULL</TD><TD>0</TD><TD>Pointer is treated as mere pointer - no memory management. Duplication copies the pointer, and deletion does nothing.</TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL</TD><TD>NULL</TD><TD>!=0</TD><TD>Plain memory management. Duplication is done with new char[size]+memcpy(), and deletion is done via delete.</TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL or user's delete func.</TD><TD>user's dupfunc.</TD><TD>indifferent</TD><TD WIDTH=317>Sophisticated memory management. Duplication is done by calling the user-supplied duplication function, which should do the allocation and the appropriate copying. Deletion is done by calling the user-supplied delete function, or the delete operator if it is not supplied.</TD>
     *   </TR>
     * </TABLE>
     */
    void configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc, size_t itemsize=0);

    /**
     * Sets the flag that determines whether setObjectValue(cOwnedObject *) and
     * setDoubleValue(cStatistic *) should automatically take ownership of
     * the objects.
     */
    void setTakeOwnership(bool tk) {tkownership=tk;}

    /**
     * Returns the takeOwnership flag, see getTakeOwnership().
     */
    bool getTakeOwnership() const   {return tkownership;}
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cMsgPar type must be bool (B) or a numeric type.
     */
    bool boolValue();

    /**
     * Returns value as long. The cMsgPar type must be types long (L),
     * double (D), Boolean (B), function (F), distribution (T).
     */
    long longValue();

    /**
     * Returns value as const char *. Only for string (S) type.
     */
    const char *stringValue();

    /**
     * Returns value as double. The cMsgPar type must be types long (L),
     * double (D), function (F), Boolean (B), distribution (T).
     */
    double doubleValue();

    /**
     * Returns value as a void * pointer. The cMsgPar type must be pointer (P).
     */
    void *pointerValue();

    /**
     * Returns value as pointer to cOwnedObject. The cMsgPar type must be pointer (O).
     */
    cOwnedObject *getObjectValue();

    /**
     * Returns value as pointer to cXMLElement. The cMsgPar type must be XML (M).
     */
    cXMLElement *xmlValue();
    //@}

    /** @name Type, change flag. */
    //@{

    /**
     * Returns type character.
     */
    char getType() const;

    /**
     * Returns true if the stored value is of a numeric type.
     */
    bool isNumeric() const;

    /**
     * Returns true if the value is constant. It returns true if
     * the type is 'L', 'D', 'B', or 'S', that is, the object stores
     * a long, double, boolean or string constant.
     */
    bool isConstant() const;

    /**
     * Returns true if the value has changed since the last hasChanged() call.
     * Side effect: clears the 'changed' flag, so a next call will return
     * false.
     */
    bool hasChanged();
    //@}

    /** @name Utility functions. */
    //@{
    /**
     * Replaces the object value with its evaluation (a double).
     * Equivalent to <tt>setValue('D', this->doubleValue())</tt>.
     */
    void convertToConst();

    /**
     * Compares the stored values. The two objects must have the same type character
     * and the same value to be equal.
     */
    bool equalsTo(cMsgPar *par);
    //@}

    /** @name Convert to/from text representation. */
    //@{
    /**
     * Returns the value in text form.
     */
    virtual std::string str() const;

    /**
     * DEPRECATED. Same as str().
     */
    _OPPDEPRECATED std::string getAsText() const  {return str();}

    /**
     * This function tries to interpret the argument text as a type
     * typed value (L=long, S=string, etc). type=='?' (the default)
     * means that the type is to be auto-selected. On success,
     * cMsgPar is updated with the new value and true is returned,
     * otherwise the function returns false. No error message is generated.
     */
    virtual bool parse(const char *text, char type='?');

    /**
     * DEPRECATED. Same as parse().
     */
    _OPPDEPRECATED bool setFromText(const char *text, char type='?')  {return parse(text, '?');}
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to setBoolValue().
     */
    cMsgPar& operator=(bool b)  {return setBoolValue(b);}

    /**
     * Equivalent to setStringValue().
     */
    cMsgPar& operator=(const char *s)  {return setStringValue(s);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(char c)  {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(unsigned char c) {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(int i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(unsigned int i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(short i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(unsigned short i)  {return setLongValue((long)i);}

    /**
     * Equivalent to setLongValue().
     */
    cMsgPar& operator=(long l)  {return setLongValue(l);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMsgPar& operator=(unsigned long l) {return setLongValue((long)l);}

    /**
     * Equivalent to setDoubleValue().
     */
    cMsgPar& operator=(double d)  {return setDoubleValue(d);}

    /**
     * Converts the argument to double, and calls setDoubleValue().
     */
    cMsgPar& operator=(long double d)  {return setDoubleValue((double)d);}

    /**
     * Equivalent to setPointerValue().
     */
    cMsgPar& operator=(void *ptr)  {return setPointerValue(ptr);}

    /**
     * Equivalent to setObjectValue().
     */
    cMsgPar& operator=(cOwnedObject *obj)  {return setObjectValue(obj);}

    /**
     * Equivalent to setXMLValue().
     */
    cMsgPar& operator=(cXMLElement *node) {return setXMLValue(node);}

    /**
     * Equivalent to boolValue().
     */
    operator bool()  {return boolValue();}

    /**
     * Equivalent to stringValue().
     */
    operator const char *()  {return stringValue();}

    /**
     * Calls longValue() and converts the result to char.
     */
    operator char()  {return (char)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned char.
     */
    operator unsigned char() {return (unsigned char)longValue();}

    /**
     * Calls longValue() and converts the result to int.
     */
    operator int()  {return (int)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned int.
     */
    operator unsigned int()  {return (unsigned int)longValue();}

    /**
     * Calls longValue() and converts the result to short.
     */
    operator short()  {return (short)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned short.
     */
    operator unsigned short()  {return (unsigned short)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator long()  {return longValue();}

    /**
     * Calls longValue() and converts the result to unsigned long.
     */
    operator unsigned long() {return longValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator double()  {return doubleValue();}

    /**
     * Calls doubleValue() and converts the result to long double.
     */
    operator long double()  {return doubleValue();}

    /**
     * Equivalent to pointerValue().
     */
    operator void *()  {return pointerValue();}

    /**
     * Equivalent to getObjectValue().
     */
    operator cOwnedObject *()  {return getObjectValue();}

    /**
     * Equivalent to xmlValue().
     */
    operator cXMLElement *()  {return xmlValue();}
    //@}
};

NAMESPACE_END


#endif


