package freemarker.eclipse.outline;

import freemarker.core.LibraryLoad;

public class ImportNode {

    Object parent = null;
    String text = null;

    public ImportNode(LibraryLoad lib, Object parent) {
        this.parent = parent;
        text = lib.getTemplateName();
    }

    public Object getParent() {
        return parent;
    }

    public String toString() {
        return text;
    }
}
