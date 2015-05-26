%module Common

// covariant return type warning disabled
#pragma SWIG nowarn=822

%{
#include "common/stringutil.h"
#include "common/patternmatcher.h"
#include "common/unitconversion.h"
#include "common/bigdecimal.h"
#include "common/rwlock.h"
#include "common/expression.h"
#include "common/stringtokenizer2.h"

USING_NAMESPACE
%}

%include "defs.i"
%include "loadlib.i"
%include "pefileversion.i"
%include "std_string.i"
%include "std_vector.i"

namespace std {
   %template(StringVector) vector<string>;
   %template(PStringVector) vector<const char *>;
}

#define THREADED

// hide export/import macros from swig
#define COMMON_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

#define NAMESPACE_BEGIN
#define NAMESPACE_END
#define USING_NAMESPACE

%include "std_set.i"     // our custom version
namespace std {
   %template(StringSet) set<string>;
};


%rename(parseQuotedString)   ::opp_parsequotedstr;
%rename(quoteString)         ::opp_quotestr;
%rename(needsQuotes)         ::opp_needsquotes;
%rename(quoteStringIfNeeded) ::opp_quotestr_ifneeded;

std::string opp_parsequotedstr(const char *txt);
std::string opp_quotestr(const char *txt);
bool opp_needsquotes(const char *txt);
std::string opp_quotestr_ifneeded(const char *txt);
int strdictcmp(const char *s1, const char *s2);
//int getPEVersion(const char *fileName);

%ignore UnitConversion::parseQuantity(const char *, std::string&);

%include "common/patternmatcher.h"
%include "common/unitconversion.h"


/* -------------------- BigDecimal -------------------------- */


%ignore _I64_MAX_DIGITS;
%ignore BigDecimal::BigDecimal();
%ignore BigDecimal::str(char*);
%ignore BigDecimal::parse(const char*,const char*&);
%ignore BigDecimal::ttoa;
%ignore BigDecimal::Nil;
%ignore BigDecimal::isNil;
%ignore BigDecimal::operator=;
%ignore BigDecimal::operator+=;
%ignore BigDecimal::operator-=;
%ignore BigDecimal::operator*=;
%ignore BigDecimal::operator/=;
%ignore BigDecimal::operator!=;
%ignore operator+;
%ignore operator-;
%ignore operator*;
%ignore operator/;
%ignore operator<<;
%immutable BigDecimal::Zero;
%immutable BigDecimal::NaN;
%immutable BigDecimal::PositiveInfinity;
%immutable BigDecimal::NegativeInfinity;
%rename(equals) BigDecimal::operator==;
%rename(less) BigDecimal::operator<;
%rename(greater) BigDecimal::operator>;
%rename(lessOrEqual) BigDecimal::operator<=;
%rename(greaterOrEqual) BigDecimal::operator>=;
%rename(toString) BigDecimal::str;
%rename(doubleValue) BigDecimal::dbl;

%extend BigDecimal {
   const BigDecimal add(const BigDecimal& x) {return *self + x;}
   const BigDecimal subtract(const BigDecimal& x) {return *self - x;}
}

SWIG_JAVABODY_METHODS(public, public, BigDecimal)

%typemap(javacode) BigDecimal %{
    public boolean equals(Object other) {
       return other instanceof BigDecimal && equals((BigDecimal)other);
    }

    public int hashCode() {
       return (int)getIntValue();
    }

    public java.math.BigDecimal toBigDecimal() {
       long intVal = getIntValue();
       int scale = getScale();
       java.math.BigDecimal d = new java.math.BigDecimal(intVal);
       return (scale == 0 ? d : d.movePointRight(scale));
    }
%}

%include "common/bigdecimal.h"


/* -------------------- rwlock.h -------------------------- */
%ignore ReaderMutex;
%ignore WriterMutex;
SWIG_JAVABODY_METHODS(public, public, ILock)

%include "common/rwlock.h"

class SimpleResolver;

%extend Expression {
	void parse(const char* text, std::set<std::string> vars) {
	  SimpleResolver resolver(vars);
	  self->parse(text, &resolver);
	}
}


%{
class SimpleVar : public Expression::Variable {
  private:
    std::string name;
  public:
    SimpleVar(const char *name) : name(name) {}
    virtual Functor *dup() const { return new SimpleVar(name.c_str()); }
    virtual const char *getName() const { return name.c_str(); }
    virtual char getReturnType() const { return (char)Expression::Value::DBL; }
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) { return Expression::Value(); }
};

class SimpleResolver : public Expression::Resolver {
  private:
	std::set<std::string> vars;
  public:
    SimpleResolver(const std::set<std::string> &vars) : vars(vars) {}
    virtual Expression::Functor *resolveVariable(const char *varname) {
       if (vars.find(varname) == vars.end())
         throw opp_runtime_error("Undefined variable: %s", varname);
       return new SimpleVar(varname);
    }
    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount) { return new MathFunction(funcname); }
};
%}

%ignore MathFunction;
%ignore Expression::evaluate;  // swig does not support nested classes (Expression::Value)
%include "common/expression.h"

/* -------------------- stringtokenizer2.h -------------------------- */
%define CHECK_STRINGTOKENIZER_EXCEPTION(METHOD)
%exception METHOD {
    try {
        $action
    } catch (StringTokenizerException& e) {
        jclass clazz = jenv->FindClass("org/omnetpp/common/engineext/StringTokenizerException");
        jmethodID methodId = jenv->GetMethodID(clazz, "<init>", "(Ljava/lang/String;)V");
        jthrowable exception = (jthrowable)(jenv->NewObject(clazz, methodId, jenv->NewStringUTF(e.what())));
        jenv->Throw(exception);
        return $null;
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}
%enddef

CHECK_STRINGTOKENIZER_EXCEPTION(StringTokenizer2::StringTokenizer2)
CHECK_STRINGTOKENIZER_EXCEPTION(StringTokenizer2::setParentheses)
CHECK_STRINGTOKENIZER_EXCEPTION(StringTokenizer2::nextToken)

%ignore StringTokenizerException;

%include "common/stringtokenizer2.h"
