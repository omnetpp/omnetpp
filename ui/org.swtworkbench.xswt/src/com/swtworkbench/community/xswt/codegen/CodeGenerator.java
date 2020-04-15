package com.swtworkbench.community.xswt.codegen;

import java.util.HashMap;

@SuppressWarnings({"rawtypes", "unchecked"})
public class CodeGenerator implements ICodeGenerator {
    private static CodeGenerator generator = null;

    private static HashMap constantGenerators = new HashMap();

    public static CodeGenerator getDefault() {
        if (generator == null)
            generator = new CodeGenerator();
        return generator;
    }

    public static void registerGenerator(Class klass, ICodeGenerator generator) {
        constantGenerators.put(klass, generator);
    }

    public String getCode(Object o, String source) {
        ICodeGenerator generator = (ICodeGenerator) constantGenerators.get(o.getClass());

        if (generator == null)
            return o.toString();
        return generator.getCode(o, source);
    }
}