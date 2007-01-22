// $ANTLR : "CppFormulaGenerator.g" -> "CppFormulaGenerator.java"$

/*
 *   Copyright (c) 1999-2004 eVelopers Corporation. All rights reserved.
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA.
 */
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
