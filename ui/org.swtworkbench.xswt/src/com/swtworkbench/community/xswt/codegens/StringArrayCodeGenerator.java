package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class StringArrayCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        String[] strarray = (String[]) o;

        StringBuffer result = new StringBuffer("new String[] {");
        if (strarray.length >= 1) {
            result.append(StringCodeGenerator.getDefault().getCode(strarray[0], strarray[0]));
        }
        if (strarray.length >= 2) {
            for (int i = 1; i < strarray.length; ++i) {
                result.append(", ");
                result.append(StringCodeGenerator.getDefault().getCode(strarray[i], strarray[i]));
            }
        }
        result.append("}");
        return result.toString();
    }
}