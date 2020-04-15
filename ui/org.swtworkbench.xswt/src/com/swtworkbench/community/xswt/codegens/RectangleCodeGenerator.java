package com.swtworkbench.community.xswt.codegens;

import org.eclipse.swt.graphics.Rectangle;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class RectangleCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        Rectangle r = (Rectangle) o;
        return "new Rectangle(" + r.x + ", " + r.y + ", " + r.width + ", " + r.height + ")";
    }
}