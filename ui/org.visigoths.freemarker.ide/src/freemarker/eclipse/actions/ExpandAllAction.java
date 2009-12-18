package freemarker.eclipse.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.AbstractTreeViewer;

import freemarker.eclipse.ImageManager;

/**
 * @version $Id:$
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class ExpandAllAction extends Action {

	private AbstractTreeViewer fViewer;

	public ExpandAllAction(AbstractTreeViewer aViewer) {
		fViewer = aViewer;
		setImageDescriptor(ImageManager.getImageDescriptor("icons/expandall.gif"));
        setToolTipText("Expand all nodes");
								  
        
	}

	/**
	 * @see org.eclipse.jface.action.IAction#run()
	 */
	public void run() {
		fViewer.expandAll();
	}

}
