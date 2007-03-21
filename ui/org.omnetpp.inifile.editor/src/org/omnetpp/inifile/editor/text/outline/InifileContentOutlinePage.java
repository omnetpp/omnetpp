package org.omnetpp.inifile.editor.text.outline;



import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.omnetpp.inifile.editor.model.InifileContents;

/**
 * Content outline page for the inifile editor.
 */
//XXX notify after every run of the reconciler
public class InifileContentOutlinePage extends ContentOutlinePage {
	protected Object fInput;
	protected IDocumentProvider fDocumentProvider;
	protected ITextEditor fTextEditor;


	/**
	 * Creates a content outline page using the given provider and the given editor.
	 * 
	 * @param provider the document provider
	 * @param editor the editor
	 */
	public InifileContentOutlinePage(IDocumentProvider provider, ITextEditor editor) {
		super();
		fDocumentProvider = provider;
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
				if (parentElement instanceof InifileContents) {
					InifileContents ini = (InifileContents) parentElement;
					return ini.getSections();
				}
				return null;
			}

			public Object getParent(Object element) {
				return null; //XXX
			}

			public boolean hasChildren(Object element) {
				return (element instanceof InifileContents);
			}

			public Object[] getElements(Object inputElement) {
				return getChildren(inputElement);
			}
		});
		
		Assert.isTrue(fInput!=null);
		getTreeViewer().setInput(fInput);
		getTreeViewer().addSelectionChangedListener(this);
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage; called back from treeviewer.
	 */
	public void selectionChanged(SelectionChangedEvent event) {
		super.selectionChanged(event);
		//System.out.println(this+".selectionChanged( " + event + ") called");

		ISelection selection = event.getSelection();
		if (selection.isEmpty())
			fTextEditor.resetHighlightRange();
		else {
			TreeViewer viewer = getTreeViewer();
			viewer.refresh();
			
//XXX			
//			NEDElement node = (NEDElement) ((IStructuredSelection) selection).getFirstElement();
//			//System.out.println("selected: "+node);
//			NEDSourceRegion region = node.getSourceRegion();
//			if (region!=null) {
//				IDocument docu = fDocumentProvider.getDocument(fInput);
//				try {
//					int startOffset = docu.getLineOffset(region.startLine-1)+region.startColumn;
//					int endOffset = docu.getLineOffset(region.endLine-1)+region.endColumn;
//					fTextEditor.setHighlightRange(startOffset, endOffset-startOffset, true);
//				} catch (BadLocationException e) {
//				}
//				catch (IllegalArgumentException x) {
//					fTextEditor.resetHighlightRange();
//				}
//			}
		}
	}
	
	/**
	 * Sets the input of the outline page
	 * 
	 * @param input the input of this outline page
	 */
	public void setInput(Object input) {
		// Note: treeViewer==null on first invocation, so we need fInput and cannot set it immediately
		fInput = input;
		if (getTreeViewer() != null) 
			getTreeViewer().setInput(fInput);
	}
	
	/**
	 * Updates the outline page.
	 */
	public void update() {
		System.out.println(this+".update() called");
		TreeViewer viewer = getTreeViewer();
		if (viewer != null) {
			viewer.refresh();
		}		
	}
}
