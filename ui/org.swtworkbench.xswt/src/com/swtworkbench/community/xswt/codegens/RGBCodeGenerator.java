package com.swtworkbench.community.xswt.codegens;

import org.eclipse.swt.graphics.RGB;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class RGBCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        RGB rgb = (RGB) o;
        return "new RGB(" + rgb.red + ", " + rgb.green + ", " + rgb.blue + ")";
    }
}