package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;
import com.swtworkbench.community.xswt.layoutbuilder.ObjectStub;

public class ObjectStubCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        ObjectStub stub = (ObjectStub) o;
        return stub.sourceName;
    }
}