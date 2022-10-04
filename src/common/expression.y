/*===============================================================
 * File: expression.y
 *
 *  Grammar for generic arithmetic expressions.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

%define api.pure full
%param { yyscan_t scanner }
%parse-param {AstNode *&resultAstTree}

%code top {
#include <limits>
#include "commonutil.h"
#include "exception.h"
#include "stringutil.h"
#include "unitconversion.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <cstring>          /* YYVERBOSE needs it */
#endif
} // %code top

%code requires {
#include "expression.h"

typedef void* yyscan_t;

using namespace omnetpp;
using namespace omnetpp::common;
using namespace omnetpp::common::expression;

typedef Expression::AstNode AstNode;
typedef Expression::AstNode::Type AstNodeType;
} // %code requires

%code {
int yylex(YYSTYPE* yylvalp, yyscan_t scanner);
void yyerror(yyscan_t scanner, AstNode *&resultAstTree, const char *msg);

static char *join(const char *s1, const char *s2, const char *s3=nullptr)
{
    char *d = new char[strlen(s1) + strlen(s2) + strlen(s3?s3:"") + 4];
    strcpy(d, s1);
    strcat(d, " ");
    strcat(d, s2);
    if (s3) {strcat(d, " "); strcat(d, s3);}
    return d;
}

static char *concat(const char *s1, const char *s2, const char *s3=nullptr, const char *s4=nullptr)
{
    char *d = new char[strlen(s1) + strlen(s2) + strlen(s3?s3:"") + strlen(s4?s4:"") + 1];
    strcpy(d, s1);
    strcat(d, s2);
    if (s3) strcat(d, s3);
    if (s4) strcat(d, s4);
    return d;
}

static AstNode *newConstant(const ExprValue& c)
{
    return new AstNode(c);
}

static AstNode *newOp(const char *name, AstNode *child1, AstNode *child2=nullptr, AstNode *child3=nullptr)
{
    AstNode *node = new AstNode(AstNode::OP, name);
    node->children.push_back(child1);
    if (child2)
        node->children.push_back(child2);
    if (child3)
        node->children.push_back(child3);
    return node;
}

static bool isIntegerValued(double d)
{
    if (std::isnan(d) || d < (double)INT64_MIN || d > (double)INT64_MAX)  // just to avoid UndefinedBehaviorSanitizer message "<value> is outside the range of representable values of type 'long'"
            return false;

    // check that when converted to integer and back to double, it stays the same
    intval_t l = (intval_t)d;
    return d == l;
}

}  // %code

//TODO use c++ variants and std::string instead of %union, see https://www.gnu.org/software/bison/manual/html_node/C_002b_002b-Variants.html

%union {
    const char *str;
    AstNode *node;
}

/* Reserved words */
%token TRUE_ FALSE_ NAN_ INF_ UNDEFINED_ NULLPTR_ NULL_

/* Other tokens: identifiers, numeric literals, operators etc */
%token <str> NAME INTCONSTANT REALCONSTANT STRINGCONSTANT
%token EQ NE GE LE SPACESHIP
%token AND OR XOR
%token SHIFT_LEFT SHIFT_RIGHT
%token DOUBLECOLON

%token INVALID_CHAR   /* just to generate parse error */

/* Operator precedences (low to high) and associativity */
%right '?' ':'
%left OR
%left XOR
%left AND
%left EQ NE '='
%left '<' '>' LE GE
%left SPACESHIP
%left MATCH
%left '|'
%left '#'
%left '&'
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%right UMIN_ NEG_ NOT_
%left '!'
%left '.'

%start expression

%%

expression
        : expr
                { resultAstTree = $<node>1; }
        ;

expr
        : literal
        | variable
        | functioncall
        | object
        | array
        | '(' expr ')'
                { $<node>2->parenthesized = true; $<node>$ = $<node>2; }
        | operation
        | expr '.' methodcall
                { $<node>$ = $<node>3; $<node>$->children.insert($<node>$->children.begin(), $<node>1); }
        | expr '.' member
                { $<node>$ = $<node>3; $<node>$->children.insert($<node>$->children.begin(), $<node>1); }
        ;

operation
        : expr '+' expr
                { $<node>$ = newOp("+", $<node>1, $<node>3); }
        | expr '-' expr
                { $<node>$ = newOp("-", $<node>1, $<node>3); }
        | expr '*' expr
                { $<node>$ = newOp("*", $<node>1, $<node>3); }
        | expr '/' expr
                { $<node>$ = newOp("/", $<node>1, $<node>3); }
        | expr '%' expr
                { $<node>$ = newOp("%", $<node>1, $<node>3); }
        | expr '^' expr
                { $<node>$ = newOp("^", $<node>1, $<node>3); }

        | '-' expr
                %prec UMIN_
                {
                    AstNode *arg = $<node>2;
                    if (arg->type == AstNode::CONSTANT && arg->constant.getType() == ExprValue::DOUBLE) {
                        arg->constant.setPreservingUnit(-arg->constant.doubleValue());
                        $<node>$ = $<node>2;
                    }
                    else if (arg->type == AstNode::CONSTANT && arg->constant.getType() == ExprValue::INT) {
                        arg->constant.setPreservingUnit(-arg->constant.intValue());
                        $<node>$ = $<node>2;
                    }
                    else {
                        $<node>$ = newOp("-", arg);
                    }
                }

        | expr '=' expr
                { $<node>$ = newOp("=", $<node>1, $<node>3); }
        | expr EQ expr
                { $<node>$ = newOp("==", $<node>1, $<node>3); }
        | expr NE expr
                { $<node>$ = newOp("!=", $<node>1, $<node>3); }
        | expr '>' expr
                { $<node>$ = newOp(">", $<node>1, $<node>3); }
        | expr GE expr
                { $<node>$ = newOp(">=", $<node>1, $<node>3); }
        | expr '<' expr
                { $<node>$ = newOp("<", $<node>1, $<node>3); }
        | expr LE expr
                { $<node>$ = newOp("<=", $<node>1, $<node>3); }
        | expr SPACESHIP expr
                { $<node>$ = newOp("<=>", $<node>1, $<node>3); }
        | expr MATCH expr
                { $<node>$ = newOp("=~", $<node>1, $<node>3); }

        | expr AND expr
                { $<node>$ = newOp("&&", $<node>1, $<node>3); }
        | expr OR expr
                { $<node>$ = newOp("||", $<node>1, $<node>3); }
        | expr XOR expr
                { $<node>$ = newOp("##", $<node>1, $<node>3); }

        | '!' expr
                %prec NOT_
                { $<node>$ = newOp("!", $<node>2); }

        | expr '!'
                { $<node>$ = newOp("_!", $<node>1); /*!!!*/ }

        | expr '&' expr
                { $<node>$ = newOp("&", $<node>1, $<node>3); }
        | expr '|' expr
                { $<node>$ = newOp("|", $<node>1, $<node>3); }
        | expr '#' expr
                { $<node>$ = newOp("#", $<node>1, $<node>3); }

        | '~' expr
                %prec NEG_
                { $<node>$ = newOp("~", $<node>2); }
        | expr SHIFT_LEFT expr
                { $<node>$ = newOp("<<", $<node>1, $<node>3); }
        | expr SHIFT_RIGHT expr
                { $<node>$ = newOp(">>", $<node>1, $<node>3); }
        | expr '?' expr ':' expr
                { $<node>$ = newOp("?:", $<node>1, $<node>3, $<node>5); }
        ;

functioncall
        : NAME '(' opt_exprlist ')'
                { $<node>3->type = AstNode::FUNCTION; $<node>3->name = $1; delete [] $1; $<node>$ = $<node>3; }
        ;

methodcall
        : NAME '(' opt_exprlist ')'
                { $<node>3->type = AstNode::METHOD; $<node>3->name = $1; delete [] $1; $<node>$ = $<node>3; }
        ;

array
        : '[' ']'
                { $<node>$ = new AstNode(AstNode::ARRAY, ""); }
        | '[' exprlist ']'
                { $<node>2->type = AstNode::ARRAY; $<node>$ = $<node>2; }
        | '[' exprlist ',' ']'
                { $<node>2->type = AstNode::ARRAY; $<node>$ = $<node>2; }
        ;

object
        : '{' opt_keyvaluelist '}'
                { $<node>2->type = AstNode::OBJECT; $<node>$ = $<node>2; }
        | qname '{' opt_keyvaluelist '}'
                { $<node>3->type = AstNode::OBJECT; $<node>3->name = $<str>1; delete [] $<str>1; $<node>$ = $<node>3; }
        ;

qname
        : NAME DOUBLECOLON qname
                { $<str>$ = concat($<str>1, "::", $<str>3); delete [] $<str>1; delete [] $<str>3; }
        | NAME
                { $<str>$ = $<str>1; }
        ;

opt_exprlist  /* note: do not move optional final ',' into this rule, as it is also used as function arg lists */
        : exprlist
        | %empty
                { $<node>$ = new AstNode(); }
        ;

exprlist
        : exprlist ',' expr
                { $<node>1->children.push_back($<node>3); $<node>$ = $<node>1; }
        | expr
                { $<node>$ = new AstNode(); $<node>$->children.push_back($<node>1); }
        ;

opt_keyvaluelist
        : keyvaluelist
        | keyvaluelist ','
        | %empty
                { $<node>$ = new AstNode(); }
        ;

keyvaluelist
        : keyvaluelist ',' keyvalue
                { $<node>1->children.push_back($<node>3); $<node>$ = $<node>1; }
        | keyvalue
                { $<node>$ = new AstNode(); $<node>$->children.push_back($<node>1); }
        ;

keyvalue
        : key ':' expr
                { $<node>$ = new AstNode(AstNode::KEYVALUE, $<str>1); $<node>$->children.push_back($<node>3); delete [] $<str>1; }
        ;

key
        : STRINGCONSTANT
                { $<str>$ = opp_strdup(opp_parsequotedstr($1,0).c_str()); delete [] $1; }
        | NAME
        | INTCONSTANT
        | REALCONSTANT
        | quantity
        | '-' INTCONSTANT
                { $<str>$ = concat("-", $<str>2); delete [] $<str>2; }
        | '-' REALCONSTANT
                { $<str>$ = concat("-", $<str>2); delete [] $<str>2; }
        | '-' quantity
                { $<str>$ = concat("-", $<str>2); delete [] $<str>2; }
        | NAN_
                { $<str>$ = opp_strdup("nan"); }
        | INF_
                { $<str>$ = opp_strdup("inf"); }
        | '-' INF_
                { $<str>$ = opp_strdup("-inf"); }
        | TRUE_
                { $<str>$ = opp_strdup("true"); }
        | FALSE_
                { $<str>$ = opp_strdup("false"); }
        | NULL_
                { $<str>$ = opp_strdup("null"); }
        | NULLPTR_
                { $<str>$ = opp_strdup("nullptr"); }
        ;

variable
        : NAME
                { $<node>$ = new AstNode(AstNode::IDENT, $1); delete [] $1; }
        | NAME '[' expr ']'
                { $<node>$ = new AstNode(AstNode::IDENT_W_INDEX, $1); delete [] $1; $<node>$->children.push_back($<node>3); }
        ;

member
        : NAME
                { $<node>$ = new AstNode(AstNode::MEMBER, $1); delete [] $1; }
        | NAME '[' expr ']'
                { $<node>$ = new AstNode(AstNode::MEMBER_W_INDEX, $1); delete [] $1; $<node>$->children.push_back($<node>3); }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        | otherliteral
        ;

stringliteral
        : STRINGCONSTANT
                { $<node>$ = newConstant(opp_parsequotedstr($1,0)); delete [] $1; }
        ;

boolliteral
        : TRUE_
                { $<node>$ = newConstant(true); }
        | FALSE_
                { $<node>$ = newConstant(false); }
        ;

numliteral
        : INTCONSTANT
                { $<node>$ = newConstant((intval_t)opp_atoll($1)); delete [] $1; }
        | REALCONSTANT
                { $<node>$ = newConstant(opp_atof($1)); delete [] $1; }
        | NAN_
                { $<node>$ = newConstant(std::nan("")); }
        | INF_
                { $<node>$ = newConstant(std::numeric_limits<double>::infinity()); }
        | quantity
                {
                  double d = 0;
                  std::string unit;
                  try {
                      // evaluate quantities like "5s 230ms"
                      d = UnitConversion::parseQuantity($<str>1, unit);
                  }
                  catch (std::exception& e) {
                      yyerror(scanner, resultAstTree, e.what());
                  }
                  if (isIntegerValued(d))
                      $<node>$ = newConstant(ExprValue((intval_t)d, unit.c_str()));
                  else
                      $<node>$ = newConstant(ExprValue(d, unit.c_str()));
                  delete [] $<str>1;
                }
        ;

otherliteral
        : UNDEFINED_
                { $<node>$ = newConstant(ExprValue()); }
        | NULL_
                { $<node>$ = newConstant(any_ptr(nullptr)); }
        | NULLPTR_
                { $<node>$ = newConstant(any_ptr(nullptr)); }
        ;

quantity
        : quantity qnumber NAME
                { $<str>$ = join($<str>1, $<str>2, $<str>3); delete [] $<str>1; delete [] $<str>2; delete [] $<str>3; }
        | qnumber NAME
                { $<str>$ = join($<str>1, $<str>2); delete [] $<str>1; delete [] $<str>2; }
        ;

qnumber
        : INTCONSTANT
        | REALCONSTANT
        | NAN_
                { $<str>$ = opp_strdup("nan"); }
        | INF_
                { $<str>$ = opp_strdup("inf"); }
        ;
%%

#include "expression.lex.h"


AstNode *Expression::parseToAst(const char *text) const
{
    yyscan_t scanner;
    expressionyylex_init(&scanner);

    yy_buffer_state *handle = expressionyy_scan_string(text, scanner);

    // optional: yyset_debug(1, scanner);
    // optional: yydebug = 1;

    expressionyyset_lineno(1,scanner);
    expressionyyset_column(0,scanner);

    AstNode *result;
    try {
        expressionyyparse(scanner, result);
    }
    catch (std::exception& e) {
        expressionyy_delete_buffer(handle, scanner);
        expressionyylex_destroy(scanner);
        throw;
    }
    expressionyy_delete_buffer(handle, scanner);
    expressionyylex_destroy(scanner);

    return result;
}

void yyerror(yyscan_t scanner, AstNode *&resultAstTree, const char* msg)
{
    throw std::runtime_error(opp_removeend(msg, "\n").c_str());
}
