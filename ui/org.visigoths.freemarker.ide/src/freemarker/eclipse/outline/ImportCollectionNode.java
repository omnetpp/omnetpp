package freemarker.eclipse.outline;

import java.util.Arrays;
import java.util.List;

import freemarker.core.LibraryLoad;
import freemarker.template.Template;

public class ImportCollectionNode {

    private Object[] children = {};
    private static final String TEXT = "import declarations";

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public ImportCollectionNode(Template t) {
        List imports = t.getImports();
        children = new Object[imports.size()];
        LibraryLoad ll;
        for(int i = 0;i<imports.size();i++) {
            ll = (LibraryLoad)imports.get(i);
            children[i] = new ImportNode(ll,this);
        }
        Arrays.sort(children,OutlineContentProvider.COMPARATOR);
    }

    public Object[] getChildren() {
        return children;
    }

    public boolean hasChildren() {
        return (children.length>0);
    }

    public String toString() {
        return TEXT;
    }
}
