header {
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
package com.evelopers.unimod.generator.symbian.util;
import com.evelopers.unimod.core.stateworks.Action;
import com.evelopers.unimod.parser.IdentNode;

}

class CppFormulaGenerator extends TreeParser;
options {
	importVocab=Expr;
	buildAST = false;
}

{
	private static CppFormulaGenerator instance;
	private ModelTool modelTool;

	public static String generate(AST ast, ModelTool modelTool) throws RecognitionException {
		CppFormulaGenerator generator = new CppFormulaGenerator();
		StringBuffer buffer = new StringBuffer();
		generator.modelTool = modelTool;
		generator.formula(ast, buffer);
		return buffer.toString();
	}
}


formula [StringBuffer formulaString]
  : dis[formulaString]
  | term[formulaString]
  ;

term [StringBuffer formulaString]
  : con[formulaString]
  | lit[formulaString]
  ;

lit [StringBuffer formulaString]
  : neg[formulaString]
  | cmp[formulaString]
  | val[formulaString]
  ;

dis [StringBuffer formulaString] {
	int currentTerm = 0;
} : #(OR
		({formulaString.append(currentTerm++ == 0 ? "" : " || ");}
        formula[formulaString])+
     )
  ;

con [StringBuffer formulaString] {
	int currentLiteral = 0;
} : #(AND
        ({formulaString.append(currentLiteral++ == 0 ? "" : " && ");}
         ({formulaString.append("(");}
          dis[formulaString]
          {formulaString.append(")");}
         |
         term[formulaString]))+
      )
  ;

neg [StringBuffer formulaString]
  : #(NOT
        ({formulaString.append("!");}
         ({formulaString.append("(");}
          dis[formulaString]
		  {formulaString.append(")");}
         |
          {formulaString.append("(");}
          con[formulaString]
		  {formulaString.append(")");}
         |
          lit[formulaString])
        )
     )
  ;

cmp [StringBuffer formulaString]
  : #(EQUAL
        val[formulaString]
        {formulaString.append(" == ");}
        val[formulaString])
  | #(NEQUAL
        val[formulaString]
        {formulaString.append(" != ");}
        val[formulaString])
  | #(GE
        val[formulaString]
        {formulaString.append(" >= ");}
        val[formulaString])
  | #(GT
        val[formulaString]
        {formulaString.append(" > ");}
        val[formulaString])
  | #(LE
        val[formulaString]
        {formulaString.append(" <= ");}
        val[formulaString])
  | #(LT
        val[formulaString]
        {formulaString.append(" < ");}
        val[formulaString])
  ;

val [StringBuffer formulaString]
  : i:IDENT {
	Action action = ((IdentNode) i).getAction();
	formulaString.append(modelTool.getAction(action));
  	}
  | c:CONST_NUM        {formulaString.append(c.getText());}
  |   TRUE             {formulaString.append("TRUE");}
  |   FALSE            {formulaString.append("FALSE");}
  ;
