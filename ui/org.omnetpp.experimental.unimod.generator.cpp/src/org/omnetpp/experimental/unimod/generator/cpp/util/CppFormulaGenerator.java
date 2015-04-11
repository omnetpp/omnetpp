/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp.util;
import com.evelopers.unimod.core.stateworks.Action;
import com.evelopers.unimod.parser.IdentNode;


import antlr.TreeParser;
import antlr.Token;
import antlr.collections.AST;
import antlr.RecognitionException;
import antlr.ANTLRException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.collections.impl.BitSet;
import antlr.ASTPair;
import antlr.collections.impl.ASTArray;


public class CppFormulaGenerator extends antlr.TreeParser       implements CppFormulaGeneratorTokenTypes
 {

    private static CppFormulaGenerator instance;

    public static String generate(AST ast) throws RecognitionException {
        CppFormulaGenerator generator = new CppFormulaGenerator();
        StringBuffer buffer = new StringBuffer();
        generator.formula(ast, buffer);
        return buffer.toString();
    }
public CppFormulaGenerator() {
    tokenNames = _tokenNames;
}

    public final void formula(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST formula_AST_in = (_t == ASTNULL) ? null : (AST)_t;

        try {      // for error handling
            if (_t==null) _t=ASTNULL;
            switch ( _t.getType()) {
            case OR:
            {
                dis(_t,formulaString);
                _t = _retTree;
                break;
            }
            case TRUE:
            case FALSE:
            case AND:
            case NOT:
            case EQUAL:
            case NEQUAL:
            case GE:
            case GT:
            case LE:
            case LT:
            case IDENT:
            case CONST_NUM:
            {
                term(_t,formulaString);
                _t = _retTree;
                break;
            }
            default:
            {
                throw new NoViableAltException(_t);
            }
            }
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void dis(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST dis_AST_in = (_t == ASTNULL) ? null : (AST)_t;

            int currentTerm = 0;


        try {      // for error handling
            AST __t29 = _t;
            AST tmp1_AST_in = (AST)_t;
            match(_t,OR);
            _t = _t.getFirstChild();
            {
            int _cnt31=0;
            _loop31:
            do {
                if (_t==null) _t=ASTNULL;
                if ((_tokenSet_0.member(_t.getType()))) {
                    formulaString.append(currentTerm++ == 0 ? "" : " || ");
                    formula(_t,formulaString);
                    _t = _retTree;
                }
                else {
                    if ( _cnt31>=1 ) { break _loop31; } else {throw new NoViableAltException(_t);}
                }

                _cnt31++;
            } while (true);
            }
            _t = __t29;
            _t = _t.getNextSibling();
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void term(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST term_AST_in = (_t == ASTNULL) ? null : (AST)_t;

        try {      // for error handling
            if (_t==null) _t=ASTNULL;
            switch ( _t.getType()) {
            case AND:
            {
                con(_t,formulaString);
                _t = _retTree;
                break;
            }
            case TRUE:
            case FALSE:
            case NOT:
            case EQUAL:
            case NEQUAL:
            case GE:
            case GT:
            case LE:
            case LT:
            case IDENT:
            case CONST_NUM:
            {
                lit(_t,formulaString);
                _t = _retTree;
                break;
            }
            default:
            {
                throw new NoViableAltException(_t);
            }
            }
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void con(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST con_AST_in = (_t == ASTNULL) ? null : (AST)_t;

            int currentLiteral = 0;


        try {      // for error handling
            AST __t33 = _t;
            AST tmp2_AST_in = (AST)_t;
            match(_t,AND);
            _t = _t.getFirstChild();
            {
            int _cnt36=0;
            _loop36:
            do {
                if (_t==null) _t=ASTNULL;
                if ((_tokenSet_0.member(_t.getType()))) {
                    formulaString.append(currentLiteral++ == 0 ? "" : " && ");
                    {
                    if (_t==null) _t=ASTNULL;
                    switch ( _t.getType()) {
                    case OR:
                    {
                        formulaString.append("(");
                        dis(_t,formulaString);
                        _t = _retTree;
                        formulaString.append(")");
                        break;
                    }
                    case TRUE:
                    case FALSE:
                    case AND:
                    case NOT:
                    case EQUAL:
                    case NEQUAL:
                    case GE:
                    case GT:
                    case LE:
                    case LT:
                    case IDENT:
                    case CONST_NUM:
                    {
                        term(_t,formulaString);
                        _t = _retTree;
                        break;
                    }
                    default:
                    {
                        throw new NoViableAltException(_t);
                    }
                    }
                    }
                }
                else {
                    if ( _cnt36>=1 ) { break _loop36; } else {throw new NoViableAltException(_t);}
                }

                _cnt36++;
            } while (true);
            }
            _t = __t33;
            _t = _t.getNextSibling();
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void lit(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST lit_AST_in = (_t == ASTNULL) ? null : (AST)_t;

        try {      // for error handling
            if (_t==null) _t=ASTNULL;
            switch ( _t.getType()) {
            case NOT:
            {
                neg(_t,formulaString);
                _t = _retTree;
                break;
            }
            case EQUAL:
            case NEQUAL:
            case GE:
            case GT:
            case LE:
            case LT:
            {
                cmp(_t,formulaString);
                _t = _retTree;
                break;
            }
            case TRUE:
            case FALSE:
            case IDENT:
            case CONST_NUM:
            {
                val(_t,formulaString);
                _t = _retTree;
                break;
            }
            default:
            {
                throw new NoViableAltException(_t);
            }
            }
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void neg(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST neg_AST_in = (_t == ASTNULL) ? null : (AST)_t;

        try {      // for error handling
            AST __t38 = _t;
            AST tmp3_AST_in = (AST)_t;
            match(_t,NOT);
            _t = _t.getFirstChild();
            {
            formulaString.append("!");
            {
            if (_t==null) _t=ASTNULL;
            switch ( _t.getType()) {
            case OR:
            {
                formulaString.append("(");
                dis(_t,formulaString);
                _t = _retTree;
                formulaString.append(")");
                break;
            }
            case AND:
            {
                formulaString.append("(");
                con(_t,formulaString);
                _t = _retTree;
                formulaString.append(")");
                break;
            }
            case TRUE:
            case FALSE:
            case NOT:
            case EQUAL:
            case NEQUAL:
            case GE:
            case GT:
            case LE:
            case LT:
            case IDENT:
            case CONST_NUM:
            {
                lit(_t,formulaString);
                _t = _retTree;
                break;
            }
            default:
            {
                throw new NoViableAltException(_t);
            }
            }
            }
            }
            _t = __t38;
            _t = _t.getNextSibling();
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void cmp(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST cmp_AST_in = (_t == ASTNULL) ? null : (AST)_t;

        try {      // for error handling
            if (_t==null) _t=ASTNULL;
            switch ( _t.getType()) {
            case EQUAL:
            {
                AST __t42 = _t;
                AST tmp4_AST_in = (AST)_t;
                match(_t,EQUAL);
                _t = _t.getFirstChild();
                val(_t,formulaString);
                _t = _retTree;
                formulaString.append(" == ");
                val(_t,formulaString);
                _t = _retTree;
                _t = __t42;
                _t = _t.getNextSibling();
                break;
            }
            case NEQUAL:
            {
                AST __t43 = _t;
                AST tmp5_AST_in = (AST)_t;
                match(_t,NEQUAL);
                _t = _t.getFirstChild();
                val(_t,formulaString);
                _t = _retTree;
                formulaString.append(" != ");
                val(_t,formulaString);
                _t = _retTree;
                _t = __t43;
                _t = _t.getNextSibling();
                break;
            }
            case GE:
            {
                AST __t44 = _t;
                AST tmp6_AST_in = (AST)_t;
                match(_t,GE);
                _t = _t.getFirstChild();
                val(_t,formulaString);
                _t = _retTree;
                formulaString.append(" >= ");
                val(_t,formulaString);
                _t = _retTree;
                _t = __t44;
                _t = _t.getNextSibling();
                break;
            }
            case GT:
            {
                AST __t45 = _t;
                AST tmp7_AST_in = (AST)_t;
                match(_t,GT);
                _t = _t.getFirstChild();
                val(_t,formulaString);
                _t = _retTree;
                formulaString.append(" > ");
                val(_t,formulaString);
                _t = _retTree;
                _t = __t45;
                _t = _t.getNextSibling();
                break;
            }
            case LE:
            {
                AST __t46 = _t;
                AST tmp8_AST_in = (AST)_t;
                match(_t,LE);
                _t = _t.getFirstChild();
                val(_t,formulaString);
                _t = _retTree;
                formulaString.append(" <= ");
                val(_t,formulaString);
                _t = _retTree;
                _t = __t46;
                _t = _t.getNextSibling();
                break;
            }
            case LT:
            {
                AST __t47 = _t;
                AST tmp9_AST_in = (AST)_t;
                match(_t,LT);
                _t = _t.getFirstChild();
                val(_t,formulaString);
                _t = _retTree;
                formulaString.append(" < ");
                val(_t,formulaString);
                _t = _retTree;
                _t = __t47;
                _t = _t.getNextSibling();
                break;
            }
            default:
            {
                throw new NoViableAltException(_t);
            }
            }
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }

    public final void val(AST _t,
        StringBuffer formulaString
    ) throws RecognitionException {

        AST val_AST_in = (_t == ASTNULL) ? null : (AST)_t;
        AST i = null;
        AST c = null;

        try {      // for error handling
            if (_t==null) _t=ASTNULL;
            switch ( _t.getType()) {
            case IDENT:
            {
                i = (AST)_t;
                match(_t,IDENT);
                _t = _t.getNextSibling();

                    Action action = ((IdentNode) i).getAction();
                    formulaString.append("i").append(action.getObject().getName())
                .append(".").append(action.getActionName()).append("L()");

                break;
            }
            case CONST_NUM:
            {
                c = (AST)_t;
                match(_t,CONST_NUM);
                _t = _t.getNextSibling();
                formulaString.append(c.getText());
                break;
            }
            case TRUE:
            {
                AST tmp10_AST_in = (AST)_t;
                match(_t,TRUE);
                _t = _t.getNextSibling();
                formulaString.append("TRUE");
                break;
            }
            case FALSE:
            {
                AST tmp11_AST_in = (AST)_t;
                match(_t,FALSE);
                _t = _t.getNextSibling();
                formulaString.append("FALSE");
                break;
            }
            default:
            {
                throw new NoViableAltException(_t);
            }
            }
        }
        catch (RecognitionException ex) {
            reportError(ex);
            if (_t!=null) {_t = _t.getNextSibling();}
        }
        _retTree = _t;
    }


    public static final String[] _tokenNames = {
        "<0>",
        "EOF",
        "<2>",
        "NULL_TREE_LOOKAHEAD",
        "TRUE",
        "FALSE",
        "OR",
        "AND",
        "NOT",
        "LPAREN",
        "RPAREN",
        "EQUAL",
        "NEQUAL",
        "GE",
        "GT",
        "LE",
        "LT",
        "IDENT",
        "CONST_BOOL",
        "CONST_NUM",
        "WS",
        "DIGIT",
        "LETTER",
        "NAME",
        "CONST_BOOL_OR_IDENT"
    };

    private static final long[] mk_tokenSet_0() {
        long[] data = { 784880L, 0L};
        return data;
    }
    public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
    }

