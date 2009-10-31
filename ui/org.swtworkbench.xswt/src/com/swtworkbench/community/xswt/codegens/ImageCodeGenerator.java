package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;
import java.util.StringTokenizer;

public class ImageCodeGenerator implements ICodeGenerator {
    private String dequalify(String className) {
        int lastPeriod = className.lastIndexOf(".");
        if (lastPeriod >= 0)
            className = className.substring(lastPeriod + 1);
        return className;
    }

    public String getCode(Object o, String source) {
        StringTokenizer stringTokenizer = new StringTokenizer(source, " \t\r\n");
        String classname = dequalify(stringTokenizer.nextToken());
        String relativePath = stringTokenizer.nextToken();

        return "new Image(Display.getCurrent(), " + classname + ".class.getResource(\"" + relativePath + "\").openStream())";
    }
}