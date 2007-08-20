package org.omnetpp.ned.editor.text.outline;



import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.model.NEDTreeUtil;

/**
 * XXX notify after every run of the reconciler
 *
 * @author rhornig
 */
public class NedContentOutlinePage extends ContentOutlinePage {
	protected IFileEditorInput fInput;
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

        getTreeViewer().setContentProvider(NEDTreeUtil.getNedModelContentProvider());
        getTreeViewer().setLabelProvider(NEDTreeUtil.getNedModelLabelProvider());
        getTreeViewer().addSelectionChangedListener(this);
        update();
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
			INEDElement node = (INEDElement) ((IStructuredSelection) selection).getFirstElement();
			//System.out.println("selected: "+node);
			NEDSourceRegion region = node.getSourceRegion();
			if (region!=null) {
				IDocument docu = fDocumentProvider.getDocument(fInput);
				try {
					int startOffset = docu.getLineOffset(region.getStartLine()-1)+region.getStartColumn();
					int endOffset = docu.getLineOffset(region.getEndLine()-1)+region.getEndColumn();
					fTextEditor.setHighlightRange(startOffset, endOffset-startOffset, true);
				} catch (BadLocationException e) {
				}
				catch (IllegalArgumentException x) {
					fTextEditor.resetHighlightRange();
				}
			}
		}
	}

	/**
	 * Sets the input of the outline page
	 *
	 * @param input the input of this outline page
	 */
	public void setInput(Object input) {
		Assert.isTrue(input instanceof IFileEditorInput || input == null);
		fInput = (IFileEditorInput) input;
//		System.out.println(this+".setInput( " + input+ ") called");
		update();
	}

	/**
	 * Updates the outline page.
	 */
	public void update() {
//		System.out.println(this+".update() called");
        if (fInput == null)
            return;

		TreeViewer viewer = getTreeViewer();
		if (viewer != null) {
			Control control = viewer.getControl();
			if (control != null && !control.isDisposed()) {
				control.setRedraw(false);

				// set file contents as input
				IFile file = fInput.getFile();
				INEDElement tree = NEDResourcesPlugin.getNEDResources().getNEDFileModel(file);
                viewer.setInput(tree);

				//viewer.expandAll();
				control.setRedraw(true);
			}
		}
	}
}
