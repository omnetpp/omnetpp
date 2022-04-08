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

#include <map>
#include "cvalue.h"
#include "cexpression.h"
#include "cownedobject.h"
#include "fileline.h"

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
 * @ingroup Expressions
 */
class SIM_API cDynamicExpression : public cExpression
{
  public:
    /**
     * Interface for extending cDynamicExpression with support for variables, member access,
     * (extra) function calls, and method calls. If a variable/member/etc was not found,
     * the corresponding method should return cValue(), i.e. the "undefined" value.
     */
    class SIM_API IResolver {
      public:
        virtual ~IResolver() {}
        /**
         * Create and return an exact copy of this object.
         */
        virtual IResolver *dup() const = 0;

        /** @name Evaluator methods. */
        //@{
        /** Return the value of a variable with the given name. */
        virtual cValue readVariable(Context *context, const char *name) {return cValue();}

        /** Return the value of an element of an array variable with the given name. Expression syntax: name[index] */
        virtual cValue readVariable(Context *context, const char *name, intval_t index) {return cValue();}

        /** Return the value of a member of the given object. Expression syntax: object.name */
        virtual cValue readMember(Context *context, const cValue& object, const char *name) {return cValue();}

        /** Return the value of an element of an array member of the given object. Expression syntax: object.name[index] */
        virtual cValue readMember(Context *context, const cValue& object, const char *name, intval_t index) {return cValue();}

        /** Evaluate a function call with the given arguments. Expression syntax: name(argv0, argv1,...) */
        virtual cValue callFunction(Context *context, const char *name, cValue argv[], int argc) {return cValue();}

        /** Evaluate a method call on the given object with the given arguments. Expression syntax: object.name(argv0, argv1,...) */
        virtual cValue callMethod(Context *context, const cValue& object, const char *name, cValue argv[], int argc) {return cValue();}
        //@}
    };

    typedef IResolver ResolverBase;

    /**
     * A resolver that serves variables from an std::map-based symbol table
     */
    class SIM_API SymbolTable : public IResolver {
      private:
        std::map<std::string, cValue> variables;
        std::map<std::string, std::vector<cValue>> arrays;
      public:
        SymbolTable() {}
        SymbolTable(const std::map<std::string, cValue>& variables) : variables(variables) {}
        SymbolTable(const std::map<std::string, cValue>& variables, const std::map<std::string, std::vector<cValue>>& arrays) : variables(variables), arrays(arrays) {}
        virtual SymbolTable *dup() const override {return new SymbolTable(variables, arrays);}
        virtual cValue readVariable(Context *context, const char *name) override;
        virtual cValue readVariable(Context *context, const char *name, intval_t index) override;
    };

  protected:
    common::Expression *expression = nullptr;
    IResolver *resolver = nullptr;
    FileLine sourceLoc;

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

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cDynamicExpression *dup() const override  {return new cDynamicExpression(*this);}

    /**
     * Converts the expression to string.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name Setter and evaluator methods. */
    //@{
    /**
     * Interprets the string as a generic expression, and stores it.
     */
    virtual void parse(const char *text) override;

    /**
     * Interprets the string as a generic expression, and stores it.
     * The resolver object allows variables, member accesses,
     * function calls and method calls to be handled in a custom way.
     * The expression object will take ownership of the resolver object.
     */
    virtual void parse(const char *text, IResolver *resolver) {parse(text); setResolver(resolver);}

    /**
     * Interprets the string as a generic expression, and stores it.
     * Values for variables used in the expression can be passed in the
     * symbolTable argument. This method is simply delegates to
     * parse(const char *text, IResolver *resolver) while using
     * cDynamicExpression::SymbolTable as resolver.
     */
    virtual void parse(const char *text, const std::map<std::string,cValue>& symbolTable) {parse(text); setResolver(new SymbolTable(symbolTable));}

    /**
     * Allows changing the resolver after a parse() call.
     */
    virtual void setResolver(IResolver *resolver);

    /**
     * Returns the resolver.
     */
    virtual IResolver *getResolver() const {return resolver;}

    /**
     * Interprets the string as a NED expression, and stores it.
     */
    virtual void parseNedExpr(const char *text);

    /**
     * Evaluate the expression, and return the results as a cValue.
     * Evaluation errors result in exceptions.
     */
    virtual cValue evaluate(Context *context) const override;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(Context *context) const override;

    /**
     * Evaluate the expression and convert the result to intval_t if possible;
     * throw an error if conversion from that type is not supported, or
     * the value of out of the range of intval_t.
     */
    virtual intval_t intValue(Context *context, const char *expectedUnit=nullptr) const override;

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
     * Compares two expressions.
     */
    virtual int compare(const cExpression *other) const override;

    /**
     * Returns true if the expression is just a literal (or equivalent to one,
     * like "2+2"). This can be used for optimization.
     */
    virtual bool isAConstant() const override;

    /**
     * Returns the file:line info where this expression was parsed from.
     * Returns empty string if such info is not available.
     */
    void setSourceLocation(FileLine loc) {sourceLoc = loc;}

    /**
     * Returns the file:line info where this expression was parsed from.
     * Returns empty string if such info is not available.
     */
    virtual std::string getSourceLocation() const override {return sourceLoc.str();}

    /**
     * Convert the given number into the target unit (e.g. milliwatt to watt).
     * Throws an exception if conversion is not possible (unknown/unrelated units).
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);
    //@}
};

/**
 * @brief An "owned" version cDynamicExpression to allow it to be assigned to module
 * parameters of the type "object".
 *
 * @ingroup Expressions
 */
class cOwnedDynamicExpression : public cOwnedObject, public cDynamicExpression
{
  private:
    virtual void parsimPack(cCommBuffer *) const override {throw cRuntimeError(this, E_CANTPACK);}
    virtual void parsimUnpack(cCommBuffer *) override {throw cRuntimeError(this, E_CANTPACK);}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cOwnedDynamicExpression(const char *name=nullptr) : cOwnedObject(name) {}

    /**
     * Copy constructor.
     */
    explicit cOwnedDynamicExpression(const cOwnedDynamicExpression& other) : cOwnedObject(other), cDynamicExpression(other) {}

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject::operator=() for details.
     * Contained objects that are owned by cOwnedDynamicExpression will be duplicated
     * so that the new cOwnedDynamicExpression will have its own copy of them.
     */
    cOwnedDynamicExpression& operator=(const cOwnedDynamicExpression& other) = default;
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cOwnedDynamicExpression *dup() const override  {return new cOwnedDynamicExpression(*this);}

    /**
     * Converts the expression to string.
     * See cObject for more details.
     */
    virtual std::string str() const override { return cDynamicExpression::str(); }
};

}  // namespace omnetpp


#endif


