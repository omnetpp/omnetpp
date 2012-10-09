package freemarker.eclipse.outline;

import freemarker.core.Macro;

public class MacroNode {

    int beginLine = 0;
    int beginColumn = 0;
    boolean isFunction = false;
    String name = "";

    public MacroNode(Macro m) {
        beginLine = m.getBeginLine();
        beginColumn = m.getBeginColumn();
        name = m.getName();
        isFunction = m.isFunction();
    }

    public int getBeginLine() {
        return beginLine;
    }

    public int getBeginColumn() {
        return beginColumn;
    }

    public String toString() {
        return name;
    }

    public boolean isFunction() {
        return isFunction;
    }
}
