package freemarker.eclipse.outline;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;

import freemarker.eclipse.editors.FreemarkerEditor;

/**
 * @version $Id: OutlinePage.java,v 1.3 2003/08/15 19:49:08 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class OutlinePage extends ContentOutlinePage {
	private FreemarkerEditor fEditor;
	private Object fInput;
	private String fSelectedNodeID;
	private OutlineLabelProvider fLabelProvider;
	
	/**
	 * Creates a content outline page using the given editor.
	 */
	public OutlinePage(FreemarkerEditor anEditor) {
		fEditor = anEditor;
	}

	/**
	 * @see org.eclipse.ui.part.IPart#createControl(Composite)
	 */
	public void createControl(Composite aParent) {
		super.createControl(aParent);

		fLabelProvider = new OutlineLabelProvider();

		// Init tree viewer
		TreeViewer viewer = getTreeViewer();
		viewer.setContentProvider(new OutlineContentProvider(fEditor));
		viewer.setLabelProvider(fLabelProvider);
		viewer.addSelectionChangedListener(this);
		if (fInput != null) {
			viewer.setInput(fInput);
		}
		// Refresh outline according to initial cursor position
		update();
	}

	/**
	 * @see org.eclipse.jface.viewers.ISelectionProvider#selectionChanged(SelectionChangedEvent)
	 */
	public void selectionChanged(SelectionChangedEvent anEvent) {
		super.selectionChanged(anEvent);
		ISelection selection = anEvent.getSelection();

		if (!selection.isEmpty()) {
			Object tmp = ((IStructuredSelection) selection).getFirstElement();
			if (tmp instanceof MacroNode) {
				MacroNode node = (MacroNode) tmp;
				fEditor.selectTextRange(
					node.getBeginLine(),
					node.getBeginColumn(),
					1);
			}
		}
	}

	/**
	 * Sets the input of the outline page.
	 */
	public void setInput(Object aInput) {
		fInput = aInput;
		update();
	}

	/**
	 * Updates the outline page.
	 */
	public void update() {
		TreeViewer viewer = getTreeViewer();
		if (viewer != null) {
			Control control = viewer.getControl();
			if (control != null && !control.isDisposed()) {
				viewer.removeSelectionChangedListener(this);
				control.setRedraw(false);
				viewer.setInput(fInput);
				control.setRedraw(true);
				viewer.addSelectionChangedListener(this);
			}
		}
	}

	/**
	 * @see org.eclipse.ui.part.Page#dispose()
	 */
	public void dispose() {
		super.dispose();
	}

}
