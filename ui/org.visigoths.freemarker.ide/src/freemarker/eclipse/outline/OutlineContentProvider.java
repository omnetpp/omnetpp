package freemarker.eclipse.outline;

import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

import org.eclipse.jface.text.BadPositionCategoryException;
import org.eclipse.jface.text.DefaultPositionUpdater;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IPositionUpdater;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;

import freemarker.core.Macro;
import freemarker.eclipse.editors.FreemarkerEditor;
import freemarker.eclipse.editors.ReconcilingStrategy;
import freemarker.template.Template;

/**
 * @version $Id: OutlineContentProvider.java,v 1.3 2003/08/15 19:49:08 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
@SuppressWarnings("rawtypes")
public class OutlineContentProvider implements ITreeContentProvider {

    public static final Comparator COMPARATOR = new Comparator() {
        public int compare(Object arg0, Object arg1) {
            return (arg0.toString().compareTo(
                arg1.toString()));
        }
    };
    public static final String FREEMARKER_TEMPLATE = "__ftl_template";

    private FreemarkerEditor fEditor;
    private IReconcilingStrategy fReconcilingStrategy;
    private IPositionUpdater fPositionUpdater;

    public OutlineContentProvider(FreemarkerEditor anEditor) {
        fEditor = anEditor;
        fPositionUpdater = new DefaultPositionUpdater(FREEMARKER_TEMPLATE);
        fReconcilingStrategy = new ReconcilingStrategy(fEditor);
    }

    public void inputChanged(
        Viewer aViewer,
        Object anOldInput,
        Object aNewInput) {
        if (anOldInput != aNewInput) {
            if (anOldInput != null) {
                IDocument document =
                    fEditor.getDocumentProvider().getDocument(anOldInput);
                if (document != null) {
                    try {
                        document.removePositionCategory(FREEMARKER_TEMPLATE);
                    } catch (BadPositionCategoryException e) {
                    }
                    document.removePositionUpdater(fPositionUpdater);
                }
            }

            if (aNewInput != null) {
                IDocument document =
                    fEditor.getDocumentProvider().getDocument(aNewInput);
                if (document != null) {
                    document.addPositionCategory(FREEMARKER_TEMPLATE);
                    document.addPositionUpdater(fPositionUpdater);
                }
            }
        }
    }

    public boolean isDeleted(Object anElement) {
        return false;
    }

    public void dispose() {
    }

    @SuppressWarnings("unchecked")
    public Object[] getElements(Object inputElement) {
        Template t = fEditor.getReconcilingStrategy().getTemplate();
        if (null == t) {
            return new Object[0];
        } else {
            Vector temp = new Vector();

            Map macros = t.getMacros();
            Iterator iter = macros.keySet().iterator();
            Object key = null;
            Macro m = null;
            while (iter.hasNext()) {
                key = iter.next();
                m = (Macro) macros.get(key);
                temp.add(new MacroNode(m));
            }
            Collections.sort(temp,OutlineContentProvider.COMPARATOR);
            temp.add(0,new ImportCollectionNode(t));

            return temp.toArray();
        }
    }

    public Object[] getChildren(Object anElement) {
        return (anElement instanceof ImportCollectionNode)
            ? ((ImportCollectionNode) anElement).getChildren()
            : new Object[0];
    }

    public Object getParent(Object anElement) {
        return (anElement instanceof ImportNode)
            ? ((ImportNode) anElement).getParent()
            : null;
    }

    public boolean hasChildren(Object anElement) {
        return (anElement instanceof ImportCollectionNode)
            ? ((ImportCollectionNode) anElement).hasChildren()
            : false;
    }

}
