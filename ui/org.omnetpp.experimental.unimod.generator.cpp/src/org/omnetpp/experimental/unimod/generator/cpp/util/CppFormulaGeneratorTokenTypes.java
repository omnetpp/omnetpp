/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp.util;
import com.evelopers.unimod.core.stateworks.Action;
import com.evelopers.unimod.parser.IdentNode;


public interface CppFormulaGeneratorTokenTypes {
    int EOF = 1;
    int NULL_TREE_LOOKAHEAD = 3;
    int TRUE = 4;
    int FALSE = 5;
    int OR = 6;
    int AND = 7;
    int NOT = 8;
    int LPAREN = 9;
    int RPAREN = 10;
    int EQUAL = 11;
    int NEQUAL = 12;
    int GE = 13;
    int GT = 14;
    int LE = 15;
    int LT = 16;
    int IDENT = 17;
    int CONST_BOOL = 18;
    int CONST_NUM = 19;
    int WS = 20;
    int DIGIT = 21;
    int LETTER = 22;
    int NAME = 23;
    int CONST_BOOL_OR_IDENT = 24;
}
