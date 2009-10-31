package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class CharacterCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        return "'" + o.toString() + "'";
    }
}