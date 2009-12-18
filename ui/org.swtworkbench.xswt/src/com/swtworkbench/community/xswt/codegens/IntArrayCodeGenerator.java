package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class IntArrayCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        int[] intarray = (int[]) o;
        StringBuffer result = new StringBuffer("new int[] {");
        if (intarray.length >= 1) {
            result.append(intarray[0]);
        }
        if (intarray.length >= 2) {
            for (int i = 1; i < intarray.length; ++i) {
                result.append(", ");
                result.append(intarray[i]);
            }
        }
        result.append("}");
        return result.toString();
    }
}