package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class StringCodeGenerator implements ICodeGenerator {
    private static StringCodeGenerator singleton = null;

    public static StringCodeGenerator getDefault() {
        if (singleton == null)
            new StringCodeGenerator();
        return singleton;
    }

    public StringCodeGenerator() {
        singleton = this;
    }

    public String getCode(Object o, String source) {
        return "\"" + o.toString() + "\"";
    }
}