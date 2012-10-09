package freemarker.eclipse.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.AbstractTreeViewer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;

/**
 * @version $Id:$
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class CollapseBranchAction extends Action {

    private AbstractTreeViewer fViewer;

    public CollapseBranchAction(AbstractTreeViewer aViewer) {
        fViewer = aViewer;
        setText("Collapse");
    }

    /**
     * @see org.eclipse.jface.action.IAction#run()
     */
    public void run() {
        ISelection selection = fViewer.getSelection();
        if (!selection.isEmpty()) {
            fViewer.collapseToLevel(((IStructuredSelection)selection).getFirstElement(),AbstractTreeViewer.ALL_LEVELS);
        }
    }

}
