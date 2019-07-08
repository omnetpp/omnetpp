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
  public:
    /**
     * Interface for extending cDynamicExpression with support for variables, member access,
     * (extra) function calls, and method calls.
     */
    class SIM_API IResolver {
      public:
        virtual ~IResolver();
        virtual IResolver *dup() const = 0;

        /** @name Evaluator methods. */
        //@{
        /** Return the value of a variable with the given name. */
        virtual cNedValue readVariable(Context *context, const char *name) = 0;

        /** Return the value of an element of an array variable with the given name. Expression syntax: name[index] */
        virtual cNedValue readVariable(Context *context, const char *name, intpar_t index) = 0;

        /** Return the value of a member of the given object. Expression syntax: object.name */
        virtual cNedValue readMember(Context *context, const cNedValue& object, const char *name) = 0;

        /** Return the value of an element of an array member of the given object. Expression syntax: object.name[index] */
        virtual cNedValue readMember(Context *context, const cNedValue& object, const char *name, intpar_t index) = 0;

        /** Evaluate a function call with the given arguments. Expression syntax: name(argv0, argv1,...) */
        virtual cNedValue callFunction(Context *context, const char *name, cNedValue argv[], int argc) = 0;

        /** Evaluate a method call on the given object with the given arguments. Expression syntax: object.name(argv0, argv1,...) */
        virtual cNedValue callMethod(Context *context, const cNedValue& object, const char *name, cNedValue argv[], int argc) = 0;
        //@}
    };

    /**
     * A base resolver implementation where all methods throws an "unknown variable/member/function/method" exception.
     */
    class SIM_API ResolverBase : public IResolver {
      public:
        virtual cNedValue readVariable(Context *context, const char *name) override;
        virtual cNedValue readVariable(Context *context, const char *name, intpar_t index) override;
        virtual cNedValue readMember(Context *context, const cNedValue& object, const char *name) override;
        virtual cNedValue readMember(Context *context, const cNedValue& object, const char *name, intpar_t index) override;
        virtual cNedValue callFunction(Context *context, const char *name, cNedValue argv[], int argc) override;
        virtual cNedValue callMethod(Context *context, const cNedValue& object, const char *name, cNedValue argv[], int argc) override;
    };

    /**
     * A resolver that serves variables from an std::map-based symbol table
     */
    class SIM_API SymbolTable : public ResolverBase {
      private:
        std::map<std::string, cNedValue> symbolTable;
      public:
        SymbolTable(const std::map<std::string, cNedValue>& symbolTable) : symbolTable(symbolTable) {}
        virtual SymbolTable *dup() const override {return new SymbolTable(symbolTable);}
        virtual cNedValue readVariable(Context *context, const char *name) override;
    };

  protected:
    common::Expression *expression = nullptr;
    IResolver *resolver = nullptr;

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
     * Interprets the string as a generic expression, and stores it.
     */
    virtual void parse(const char *text) override {parse(text, nullptr);}

    /**
     * Interprets the string as a generic expression, and stores it.
     * The resolver object allows variables, member accesses,
     * function calls and method calls to be handled in a custom way.
     * The expression object will take ownership of the resolver object.
     */
    virtual void parse(const char *text, IResolver *resolver);

    /**
     * Interprets the string as a generic expression, and stores it.
     * Values for variables used in the expression can be passed in the
     * symbolTable argument. This method is simply delegates to
     * parse(const char *text, IResolver *resolver) while using
     * cDynamicExpression::SymbolTable as resolver.
     */
    virtual void parse(const char *text, const std::map<std::string,cNedValue>& symbolTable) {parse(text, new SymbolTable(symbolTable));}

    /**
     * Interprets the string as a NED expression, and stores it.
     */
    virtual void parseNedExpr(const char *text, bool inSubcomponentScope, bool inInifile);

    /**
     * Evaluate the expression, and return the results as a cNedValue.
     * Evaluation errors result in exceptions.
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


