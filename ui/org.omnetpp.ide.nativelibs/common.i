%module Common

%{
#include "stringutil.h"
#include "patternmatcher.h"
#include "unitconversion.h"
#include "bigdecimal.h"
%}

%include "commondefs.i"

%exception {
    try {
        $action
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}

%include "std_string.i"

// hide export/import macros from swig
#define COMMON_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

%rename(parseQuotedString)   ::opp_parsequotedstr;
%rename(quoteString)         ::opp_quotestr;
%rename(needsQuotes)         ::opp_needsquotes;
%rename(quoteStringIfNeeded) ::opp_quotestr_ifneeded;

std::string opp_parsequotedstr(const char *txt);
std::string opp_quotestr(const char *txt);
bool opp_needsquotes(const char *txt);
std::string opp_quotestr_ifneeded(const char *txt);

%ignore UnitConversion::parseQuantity(const char *, std::string&);

%include "patternmatcher.h"
%include "unitconversion.h"


/* -------------------- BigDecimal -------------------------- */

%ignore BigDecimal::BigDecimal();
%ignore BigDecimal::str(char*);
%ignore BigDecimal::parse(const char*,const char*&);
%ignore BigDecimal::ttoa;
SWIG_JAVABODY_METHODS(public, public, BigDecimal)
%include "bigdecimal.h"



