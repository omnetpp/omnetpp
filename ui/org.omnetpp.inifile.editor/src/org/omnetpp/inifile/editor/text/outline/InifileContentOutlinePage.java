package org.omnetpp.inifile.editor.text.outline;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.old.InifileLine;

/**
 * Content outline page for the inifile editor.
 */
public class InifileContentOutlinePage extends ContentOutlinePage implements IInifileChangeListener {
	protected IInifileDocument fInput;
	protected ITextEditor fTextEditor;

	/**
	 * Creates a content outline page using the given provider and the given editor.
	 * 
	 * @param provider the document provider
	 * @param editor the editor
	 */
	public InifileContentOutlinePage(IDocumentProvider provider, ITextEditor editor) {
		super();
		fTextEditor = editor;
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage
	 */
	public void createControl(Composite parent) {
		super.createControl(parent);

		getTreeViewer().setLabelProvider(new LabelProvider()); //XXX for now
		getTreeViewer().setContentProvider(new ITreeContentProvider() {
			public void dispose() {
			}
			public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			}
			public Object[] getChildren(Object parentElement) {
				if (parentElement instanceof IInifileDocument) {
					IInifileDocument ini = (IInifileDocument) parentElement;
					return ini.getSectionNames();
				}
				return null;
			}
			public Object getParent(Object element) {
				return null; //XXX
			}
			public boolean hasChildren(Object element) {
				return (element instanceof IInifileDocument);
			}
			public Object[] getElements(Object inputElement) {
				return getChildren(inputElement);
			}
		});
		
		Assert.isTrue(fInput!=null);
		getTreeViewer().setInput(fInput);
		//XXX needed? it's there in the base class too!!!: getTreeViewer().addSelectionChangedListener(this);
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage; called back from treeviewer.
	 */
	public void selectionChanged(SelectionChangedEvent event) {
		super.selectionChanged(event);
		//System.out.println(this+".selectionChanged( " + event + ") called");

		// make text editor to follow outline's selection
		ISelection selection = event.getSelection();
		if (selection.isEmpty()) {
			fTextEditor.resetHighlightRange();
		}
		else {
			// if an InifileLine is selected in the outline, highlight it in the text editor
			TreeViewer viewer = getTreeViewer();
			viewer.refresh();
			Object sel = ((IStructuredSelection) selection).getFirstElement();
			if (sel instanceof InifileLine) {  //XXX String!!!
				InifileLine line = (InifileLine) sel;

				IDocument docu = fTextEditor.getDocumentProvider().getDocument(fTextEditor.getEditorInput());
				try {
					int startOffset = docu.getLineOffset(line.getLineNumber()-1);
					int endOffset = docu.getLineOffset(line.getLineNumber())-1;
					fTextEditor.setHighlightRange(startOffset, endOffset-startOffset, true);
				} catch (BadLocationException e) {
				}
				catch (IllegalArgumentException x) {
					fTextEditor.resetHighlightRange();
				}
			}
		}
	}

	@Override
	public void dispose() {
		if (fInput != null)
			fInput.getListeners().remove(this);
		super.dispose();
	}
	
	/**
	 * Sets the input of the outline page
	 * 
	 * @param input the input of this outline page
	 */
	public void setInput(Object input) {
		// unhook from old input object
		if (fInput != null)
			fInput.getListeners().remove(this);
		Assert.isTrue(input instanceof IInifileDocument || input == null);
		fInput = (IInifileDocument) input;
		// Note: when first invoked, treeViewer==null yet
		if (getTreeViewer() != null) 
			getTreeViewer().setInput(fInput);
		if (fInput != null)
			fInput.getListeners().add(this);
	}
	
	/**
	 * Updates the outline page.
	 */
	public void update() {
		System.out.println(this+".update() called");
		final TreeViewer viewer = getTreeViewer();
		if (viewer != null) {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {
					viewer.refresh();
				}
			});
		}		
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.inifile.editor.model.IInifileChangeListener#modelChanged()
	 */
	public void modelChanged() {
		update();
	}
}
