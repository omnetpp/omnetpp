package org.omnetpp.inifile.editor.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Item;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Content outline page for the inifile editor.
 */
//XXX use workbench selection service instead of manually calling setInput()
//XXX also: treeView to follow editor's selection
//XXX show error markers like in SectionsPage
public class InifileContentOutlinePage extends ContentOutlinePage implements IInifileChangeListener {
	protected IInifileDocument inifileDocument;
	protected InifileEditor inifileEditor;

	/**
	 * Creates a content outline page using the given provider and the given editor.
	 * @param editor the editor
	 */
	public InifileContentOutlinePage(InifileEditor editor) {
		super();
		inifileEditor = editor;
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage
	 */
	@Override
	public void createControl(Composite parent) {
		super.createControl(parent);

		getTreeViewer().setLabelProvider(new LabelProvider() {
			@Override
			public Image getImage(Object element) {
				String sectionName = (String)element;
				InifileAnalyzer inifileAnalyzer = inifileEditor.getEditorData().getInifileAnalyzer();
				return InifileUtils.getSectionImage(sectionName, inifileAnalyzer);
			}

			@Override
			public String getText(Object element) {
				return (String)element;
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
		
 		// add tooltip support
 		new HoverSupport().adapt(getTreeViewer().getTree(), new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
				Item item = getTreeViewer().getTree().getItem(new Point(x,y));
				String section = (String) (item==null ? null : item.getData());
				return section==null ? null : InifileHoverUtils.getSectionHoverText(section, inifileDocument, null, true);
			}
 		});
		
	}
	
	/* (non-Javadoc)
	 * Method declared on ContentOutlinePage; called back from treeviewer.
	 */
	@Override
	public void selectionChanged(SelectionChangedEvent event) {
		super.selectionChanged(event);
		//Debug.println(this+".selectionChanged( " + event + ") called");

		// make text editor to follow outline's selection
		ISelection selection = event.getSelection();
		if (selection.isEmpty()) {
			inifileEditor.gotoSection(null, IGotoInifile.Mode.AUTO);
		}
		else {
			// if a line is selected in the outline, highlight it in the text editor
			TreeViewer viewer = getTreeViewer();
			viewer.refresh();
			Object sel = ((IStructuredSelection) selection).getFirstElement();
			if (sel instanceof String) {
				String sectionName = (String) sel;
				inifileEditor.gotoSection(sectionName, IGotoInifile.Mode.AUTO);
			}
		}
	}

	@Override
	public void dispose() {
		if (inifileDocument != null)
			inifileDocument.removeInifileChangeListener(this);
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
			inifileDocument.removeInifileChangeListener(this);
		Assert.isTrue(input instanceof IInifileDocument || input == null);
		inifileDocument = (IInifileDocument) input;
		// Note: when first invoked, treeViewer==null yet
		if (getTreeViewer() != null && !getTreeViewer().getTree().isDisposed()) 
			getTreeViewer().setInput(inifileDocument);
		if (inifileDocument != null)
			inifileDocument.addInifileChangeListener(this);
	}
	
	/**
	 * Updates the outline page.
	 */
	public void update() {
		final TreeViewer viewer = getTreeViewer();
		if (viewer != null) {
			Display.getDefault().asyncExec(new Runnable() {  //XXX why asyncExec?
				public void run() {
				    if (!viewer.getTree().isDisposed()) // because of asyncExec
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
