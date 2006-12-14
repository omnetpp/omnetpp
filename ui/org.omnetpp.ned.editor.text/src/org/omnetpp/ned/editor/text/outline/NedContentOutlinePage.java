package org.omnetpp.ned.editor.text.outline;


import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.eclipse.ui.texteditor.ITextEditor;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * XXX notify after every run of the reconciler
 */
public class NedContentOutlinePage extends ContentOutlinePage {
	protected IFileEditorInput fInput;
	protected IDocumentProvider fDocumentProvider;
	protected ITextEditor fTextEditor;


	class ViewContentProvider implements IStructuredContentProvider, ITreeContentProvider {

		public void inputChanged(Viewer v, Object oldInput, Object newInput) {
		}

		public void dispose() {
		}

		public Object[] getElements(Object parent) {
			return getChildren(parent);
		}

		public Object getParent(Object child) {
			return ((NEDElement)child).getParent();
		}

		public Object[] getChildren(Object parent) {
			ArrayList<NEDElement> list = new ArrayList<NEDElement>();
			for (NEDElement child : (NEDElement)parent)
				list.add(child);
			return list.toArray();
		}

		public boolean hasChildren(Object parent) {
			return ((NEDElement)parent).getFirstChild()!=null;
		}
	}

	class ViewLabelProvider extends LabelProvider {

		public String getText(Object obj) { //XXX refine; factor out common part with graphical editor?
			NEDElement node = (NEDElement)obj;
			if (node.lookupAttribute("name")!=-1 && node.lookupAttribute("type")!=-1)
				return node.getTagName()+": "+node.getAttribute("type")+" "+node.getAttribute("name"); 
			else if (node.lookupAttribute("name")!=-1)
				return node.getTagName()+": "+node.getAttribute("name"); 
			else if (node.lookupAttribute("text")!=-1)
				return node.getTagName()+": "+node.getAttribute("text"); 
			else 
				return ((NEDElement)obj).getTagName();
		}

		public Image getImage(Object obj) {
			return ImageFactory.getImage(ImageFactory.TOOLBAR_IMAGE_SIMPLE); //XXX refine; factor out common part with graphical editor?
		}
	}

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
		viewer.setContentProvider(new ViewContentProvider());
		viewer.setLabelProvider(new ViewLabelProvider());
		viewer.addSelectionChangedListener(this);
        viewer.setInput(null);  //XXX 
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
			NEDElement node = (NEDElement) ((IStructuredSelection) selection).getFirstElement();
			//System.out.println("selected: "+node);
			NEDSourceRegion region = node.getSourceRegion();
			if (region!=null) {
				IDocument docu = fDocumentProvider.getDocument(fInput);
				try {
					int startOffset = docu.getLineOffset(region.startLine-1)+region.startColumn;
					int endOffset = docu.getLineOffset(region.endLine-1)+region.endColumn;
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
		System.out.println(this+".setInput( " + input+ ") called");
		update();
	}
	
	/**
	 * Updates the outline page.
	 */
	public void update() {
		System.out.println(this+".update() called");

		TreeViewer viewer = getTreeViewer();
		if (viewer != null) {
			Control control = viewer.getControl();
			if (control != null && !control.isDisposed()) {
				control.setRedraw(false);

				// set file contents as input
                // FIXME fInput might be NULL if called durnign dispose
				IFile file = fInput.getFile();
				NEDElement tree = NEDResourcesPlugin.getNEDResources().getNEDFileContents(file);
				viewer.setInput(tree);

				//viewer.expandAll();
				control.setRedraw(true);
			}
		}
	}
}
