package org.omnetpp.inifile.editor.views;

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
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;

/**
 * Content outline page for the inifile editor.
 */
public class InifileContentOutlinePage extends ContentOutlinePage implements IInifileChangeListener {
	protected IInifileDocument inifileDocument;
	protected ITextEditor textEditor;

	/**
	 * Creates a content outline page using the given provider and the given editor.
	 * @param editor the editor
	 */
	public InifileContentOutlinePage(ITextEditor editor) {
		super();
		textEditor = editor;
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage
	 */
	public void createControl(Composite parent) {
		super.createControl(parent);

		getTreeViewer().setLabelProvider(new LabelProvider() {
			@Override
			public Image getImage(Object element) {
				return ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER); //XXX
			}

			@Override
			public String getText(Object element) {
				return "["+(String)element+"]";
			}
		});
		getTreeViewer().setContentProvider(new ITreeContentProvider() {
			public void dispose() {
			}
			public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			}
			public Object[] getChildren(Object parentElement) {
				if (parentElement instanceof IInifileDocument) {
					IInifileDocument doc = (IInifileDocument) parentElement;
					return doc.getSectionNames();
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
		
		Assert.isTrue(inifileDocument!=null);
		getTreeViewer().setInput(inifileDocument);
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
			textEditor.resetHighlightRange();
		}
		else {
			// if an InifileLine is selected in the outline, highlight it in the text editor
			TreeViewer viewer = getTreeViewer();
			viewer.refresh();
			Object sel = ((IStructuredSelection) selection).getFirstElement();
			if (sel instanceof String) {
				String sectionName = (String) sel;

				LineInfo line = inifileDocument.getSectionLineDetails(sectionName);
				IDocument docu = textEditor.getDocumentProvider().getDocument(textEditor.getEditorInput());
				try {
					int startOffset = docu.getLineOffset(line.getLineNumber()-1);
					int endOffset = docu.getLineOffset(line.getLineNumber())-1;
					textEditor.setHighlightRange(startOffset, endOffset-startOffset, true);
				} catch (BadLocationException e) {
				}
				catch (IllegalArgumentException x) {
					textEditor.resetHighlightRange();
				}
			}
		}
	}

	@Override
	public void dispose() {
		if (inifileDocument != null)
			inifileDocument.getListeners().remove(this);
		super.dispose();
	}
	
	/**
	 * Sets the input of the outline page
	 * 
	 * @param input the input of this outline page
	 */
	public void setInput(Object input) {
		// unhook from old input object
		if (inifileDocument != null)
			inifileDocument.getListeners().remove(this);
		Assert.isTrue(input instanceof IInifileDocument || input == null);
		inifileDocument = (IInifileDocument) input;
		// Note: when first invoked, treeViewer==null yet
		if (getTreeViewer() != null) 
			getTreeViewer().setInput(inifileDocument);
		if (inifileDocument != null)
			inifileDocument.getListeners().add(this);
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
