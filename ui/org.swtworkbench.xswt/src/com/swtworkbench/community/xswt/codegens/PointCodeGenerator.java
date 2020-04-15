package com.swtworkbench.community.xswt.codegens;

import org.eclipse.swt.graphics.Point;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;

public class PointCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        Point p = (Point) o;
        return "new Point(" + p.x + ", " + p.y + ")";
    }
}