package com.swtworkbench.community.xswt.codegens;

import com.swtworkbench.community.xswt.codegen.ICodeGenerator;
import org.eclipse.swt.graphics.Point;

public class PointCodeGenerator implements ICodeGenerator {
    public String getCode(Object o, String source) {
        Point p = (Point) o;
        return "new Point(" + p.x + ", " + p.y + ")";
    }
}