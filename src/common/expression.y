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

/* Reserved words */
%token TRUE_ FALSE_ NAN_ INF_ UNDEFINED_

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME INTCONSTANT REALCONSTANT STRINGCONSTANT
%token EQ NE GE LE SPACESHIP
%token AND OR XOR
%token SHIFT_LEFT SHIFT_RIGHT

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
%left BANG_

%start expression

%{

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "commonutil.h"
#include "expressionyydefs.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <cstring>         /* YYVERBOSE needs it */
#endif

#include "expression.h"
#include "exception.h"
#include "stringutil.h"
#include "unitconversion.h"

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

typedef Expression::AstNode AstNode;
typedef Expression::AstNode::Type AstNodeType;

#define yyin expressionyyin
#define yyout expressionyyout
#define yyrestart expressionyyrestart
#define yy_scan_string expressionyy_scan_string
#define yy_delete_buffer expressionyy_delete_buffer
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);
void yyrestart(FILE *);
int yylex();
void yyerror (const char *s);

LineColumn xpos, xprevpos;

static std::vector<AstNode*> astNodes;

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

static void addNode(AstNode *node, int numChildren)
{
    node->children.resize(numChildren);
    for (int i=0; i<numChildren; i++)
        node->children[i] = astNodes[astNodes.size()-numChildren+i];
    astNodes.resize(astNodes.size()-numChildren);
    astNodes.push_back(node);
}

static void addConstant(const ExprValue& c)
{
    addNode(new AstNode(c), 0);
}

static void addNode(AstNodeType type, const char *name, int numChildren)
{
    addNode(new AstNode(type, name), numChildren);
}

static void addOp(const char *name, int numArgs)
{
    addNode(new AstNode(AstNode::OP, name), numArgs);
}

static void addFunction(const char *name, int numArgs)
{
    addNode(new AstNode(AstNode::FUNCTION, name), numArgs);
}

static void addMethod(const char *name, int numArgs)
{
    addNode(new AstNode(AstNode::METHOD, name), 1 + numArgs);
}

static void addIdent(const char *name, bool withIndex)
{
    addNode(new AstNode(withIndex ? AstNode::IDENT_W_INDEX : AstNode::IDENT, name), withIndex ? 1 : 0);
}

static void addMember(const char *name, bool withIndex)
{
    addNode(new AstNode(withIndex ? AstNode::MEMBER_W_INDEX : AstNode::MEMBER, name), withIndex ? 2 : 1);
}

static double parseQuantity(const char *text, std::string& unit)
{
    try {
        // evaluate quantities like "5s 230ms"
        return UnitConversion::parseQuantity(text, unit);
    }
    catch (std::exception& e) {
        yyerror(e.what());
        return 0;
    }
}

%}

%%

expression
        : expr
        ;

expr
        : basicexpr
        | literal
        | functioncall
        | '(' expr ')'
        | operation
        ;

operation
        : expr '+' expr
                { addOp("+", 2); }
        | expr '-' expr
                { addOp("-", 2); }
        | expr '*' expr
                { addOp("*", 2); }
        | expr '/' expr
                { addOp("/", 2); }
        | expr '%' expr
                { addOp("%", 2); }
        | expr '^' expr
                { addOp("^", 2); }

        | '-' expr
                %prec UMIN_
                {
                  AstNode *last = astNodes.back();
                  if (last->type == AstNode::CONSTANT && last->constant.getType() == ExprValue::DOUBLE)
                      last->constant.setPreservingUnit(-last->constant.doubleValue());
                  else if (last->type == AstNode::CONSTANT && last->constant.getType() == ExprValue::INT)
                      last->constant.setPreservingUnit(-last->constant.intValue());
                  else
                      addOp("-", 1);
                }

        | expr '=' expr
                { addOp("=", 2); }
        | expr EQ expr
                { addOp("==", 2); }
        | expr NE expr
                { addOp("!=", 2); }
        | expr '>' expr
                { addOp(">", 2); }
        | expr GE expr
                { addOp(">=", 2); }
        | expr '<' expr
                { addOp("<", 2); }
        | expr LE expr
                { addOp("<=", 2); }
        | expr SPACESHIP expr
                { addOp("<=>", 2); }
        | expr MATCH expr
                { addOp("=~", 2); }

        | expr AND expr
                { addOp("&&", 2); }
        | expr OR expr
                { addOp("||", 2); }
        | expr XOR expr
                { addOp("##", 2); }

        | '!' expr
                %prec NOT_
                { addOp("!", 1); }

        | expr '!'
                %prec BANG_
                { addOp("_!", 1); /*!!!*/ }

        | expr '&' expr
                { addOp("&", 2); }
        | expr '|' expr
                { addOp("|", 2); }
        | expr '#' expr
                { addOp("#", 2); }

        | '~' expr
                %prec NEG_
                { addOp("~", 1); }
        | expr SHIFT_LEFT expr
                { addOp("<<", 2); }
        | expr SHIFT_RIGHT expr
                { addOp(">>", 2); }
        | expr '?' expr ':' expr
                { addOp("?:", 3); }
        ;

functioncall
        : NAME '(' ')'
                { addFunction($1,0); delete [] $1; }
        | NAME '(' expr ')'
                { addFunction($1,1); delete [] $1; }
        | NAME '(' expr ',' expr ')'
                { addFunction($1,2); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ')'
                { addFunction($1,3); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,4); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,5); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,6); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,7); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,8); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,9); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,10); delete [] $1; }
        ;

basicexpr
        : variable '.' members
        | functioncall '.' members
        | '(' expr ')' '.' members
        | variable
        ;

members
        :  member '.' members
        |  member
        ;

variable
        : NAME
                { addIdent($1, false); delete [] $1; }
        | NAME '[' expr ']'
                { addIdent($1, true); delete [] $1; }
        ;

member
        : NAME
                { addMember($1, false); delete [] $1; }
        | NAME '[' expr ']'
                { addMember($1, true); delete [] $1; }
        | NAME '(' ')'
                { addMethod($1,0); delete [] $1; }
        | NAME '(' expr ')'
                { addMethod($1,1); delete [] $1; }
        | NAME '(' expr ',' expr ')'
                { addMethod($1,2); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ')'
                { addMethod($1,3); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,4); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,5); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,6); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,7); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,8); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,9); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
                { addMethod($1,10); delete [] $1; }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        ;

stringliteral
        : STRINGCONSTANT
                { addConstant(opp_parsequotedstr($1)); delete [] $1; }
        ;

boolliteral
        : TRUE_
                { addConstant(true); }
        | FALSE_
                { addConstant(false); }
        ;

numliteral
        : INTCONSTANT
                { addConstant((intval_t)opp_atoll($1)); delete [] $1; }
        | REALCONSTANT
                { addConstant(opp_atof($1)); delete [] $1; }
        | NAN_
                { addConstant(std::nan("")); }
        | INF_
                { addConstant(1/0.0); }
        | UNDEFINED_
                { addConstant(ExprValue()); }
        | quantity
                {
                  std::string unitstr;
                  double d = parseQuantity($1, unitstr);
                  const char *unit = ExprValue::getPooled(unitstr.c_str());
                  intval_t l = (intval_t)d;
                  if (d == l)
                      addConstant(ExprValue(l, unit));
                  else
                      addConstant(ExprValue(d, unit));
                  delete [] $1;
                }
        ;

quantity
        : quantity qnumber NAME
                { $$ = join($1, $2, $3); delete [] $1; delete [] $2; delete [] $3; }
        | qnumber NAME
                { $$ = join($1, $2); delete [] $1; delete [] $2; }
        ;

qnumber
        : INTCONSTANT
        | REALCONSTANT
        | NAN_
                { $$ = opp_strdup("nan"); }
        | INF_
                { $$ = opp_strdup("inf"); }
        ;
%%

//----------------------------------------------------------------------

AstNode *Expression::parseToAst(const char *text) const
{
    NONREENTRANT_PARSER();

    // reset the lexer
    xpos.co = 0;
    xpos.li = 1;
    xprevpos = xpos;

    yyin = nullptr;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(text);
    if (!handle)
        throw std::runtime_error("Parser is unable to allocate work memory");

    for (AstNode *node : astNodes)
        delete node;
    astNodes.clear();

    // parse
    try
    {
        yyparse();
    }
    catch (std::exception& e)
    {
        yy_delete_buffer(handle);
        for (AstNode *node : astNodes)
            delete node;
        astNodes.clear();
        throw;
    }
    yy_delete_buffer(handle);

    Assert(astNodes.size() == 1);
    AstNode *result = astNodes[0];
    astNodes.clear();
    return result;
}

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    throw std::runtime_error(buf);
}
