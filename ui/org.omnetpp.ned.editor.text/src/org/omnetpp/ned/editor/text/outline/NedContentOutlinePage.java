package org.omnetpp.ned.editor.text.outline;


import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;

/**
 */
public class NedContentOutlinePage extends ContentOutlinePage {


	protected Object fInput;
	protected IDocumentProvider fDocumentProvider;
	protected ITextEditor fTextEditor;

	/**
	 * Creates a content outline page using the given provider and the given editor.
	 * 
	 * @param provider the document provider
	 * @param editor the editor
	 */
	public NedContentOutlinePage(IDocumentProvider provider, ITextEditor editor) {
		super();
		fDocumentProvider= provider;
		fTextEditor= editor;
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage
	 */
	public void createControl(Composite parent) {

		super.createControl(parent);

		TreeViewer viewer= getTreeViewer();
		viewer.setContentProvider(new WorkbenchContentProvider());
		viewer.setLabelProvider(new WorkbenchLabelProvider());
		viewer.addSelectionChangedListener(this);
        viewer.setInput(null);  //XXX 
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage
	 */
	public void selectionChanged(SelectionChangedEvent event) {

		super.selectionChanged(event);

//		ISelection selection= event.getSelection();
//		if (selection.isEmpty())
//			fTextEditor.resetHighlightRange();
//		else {
//			TopLevelKeyword segment= (TopLevelKeyword) ((IStructuredSelection) selection).getFirstElement();
//			int start= segment.position.getOffset();
//			int length= segment.position.getLength();
//			try {
//				fTextEditor.setHighlightRange(start, length, true);
//			} catch (IllegalArgumentException x) {
//				fTextEditor.resetHighlightRange();
//			}
//		}
	}
	
	/**
	 * Sets the input of the outline page
	 * 
	 * @param input the input of this outline page
	 */
	public void setInput(Object input) {
		fInput= input;
		System.out.println(this+".setInput( " + input+ ") called");
		update();
	}
	
	/**
	 * Updates the outline page.
	 */
	public void update() {
		TreeViewer viewer= getTreeViewer();

		if (viewer != null) {
			Control control= viewer.getControl();
			if (control != null && !control.isDisposed()) {
				control.setRedraw(false);
				System.out.println(this+".update() called");
				//viewer.setInput(hp.getContentOutline(null)); XXX
				viewer.expandAll();
				control.setRedraw(true);
			}
		}
	}
}
