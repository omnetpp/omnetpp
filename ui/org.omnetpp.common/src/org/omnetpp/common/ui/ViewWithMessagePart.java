package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.eclipse.ui.part.ViewPart;

/**
 * Base class for Views that may occasionally need to display an informative message
 * (like "No data available") instead of the normal contents . 
 * Subclasses can call displayMessage()/hideMessage() for this functionality.
 * 
 * @author Andras
 */
public abstract class ViewWithMessagePart extends ViewPart {
	private Control viewControl;

	public void createPartControl(Composite parent) {
		GridLayout layout = new GridLayout();
		layout.marginWidth = layout.marginHeight = 0;
		parent.setLayout(layout);
	
		viewControl = createViewControl(parent);
		viewControl.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
	}

	/**
	 * Create and return the control that displays the content of the view. 
	 * This will typically be a table or a tree widget. 
	 */
	abstract protected Control createViewControl(Composite parent);

	/**
	 * By default, Sets the focus to the message label. Override in subclasses
	 * to set the focus to the appropriate table or tree control.
	 */
	@Override
	public void setFocus() {
		//XXX choose a widget that can own the focus, even if a message is shown instead of viewControl
		// Note: looks like labels cannot have focus. Use a readonly Text instead of Label? (as in InputDialog)
	}

	/**
	 * Display a message (such as "Nothing to show") instead of the contents.
	 */
	protected void showMessage(String text) {
		setContentDescription(text);
		if (isVisible(viewControl))
			setVisible(viewControl, false);
	}

	/**
	 * Returns when the message is showing instead of the view contents.
	 * @see showMessage() 
	 */
	public boolean isShowingMessage() {
		return !isVisible(viewControl);
	}
	
	protected boolean isDisposed() {
		return viewControl.isDisposed();
	}
	
	/**
	 * Displays the part control.
	 */
	protected void hideMessage() {
		if (!isVisible(viewControl)) {
			setVisible(viewControl, true);
			setContentDescription(""); // clear any previous message
		}
	}

	/**
	 * Utility function
	 */
	private boolean isVisible(Control control) {
		GridData gridData = (GridData)control.getLayoutData();
		return !gridData.exclude;
	}

	/**
	 * Utility function
	 */
	private void setVisible(Control control, boolean visible) {
		Assert.isTrue(control.getParent().getLayout() instanceof GridLayout); // if not, set it!
		GridData gridData = (GridData)control.getLayoutData();
		gridData.exclude = !visible;
		control.setVisible(visible);
		control.getParent().layout(true, true);
	}
	
	/**
	 * Utility method: Returns the active editor, or null.
	 */
	protected IEditorPart getActiveEditor() {
		IWorkbenchPartSite site = getSite();
		IWorkbenchPage activePage = site==null ? null : site.getWorkbenchWindow().getActivePage();
		return activePage==null ? null : activePage.getActiveEditor();
	}
	
	/**
	 * Utility method: Returns the active part, or null.
	 */
	protected IWorkbenchPart getActivePart() {
		IWorkbenchPartSite site = getSite();
		IWorkbenchPage activePage = site==null ? null : site.getWorkbenchWindow().getActivePage();
		return activePage==null ? null : activePage.getActivePart();
	}

	/**
	 * Utility method: Return the active editor's selection, or null.
	 */
	protected ISelection getActiveEditorSelection() {
		IEditorPart editor = getActiveEditor();
		ISelectionProvider selectionProvider = editor==null ? null : editor.getSite().getSelectionProvider();
		return selectionProvider==null ? null : selectionProvider.getSelection();
	}
	
	/**
	 * Utility method: Return the selection of the active 
	 * workbench part (editor OR view!), or null.
	 */
	protected ISelection getWorkbenchSelection() {
		return getSite()==null ? null : getSite().getWorkbenchWindow().getSelectionService().getSelection();
	}

	/**
	 * Utility method: Activates the given editor (unless it's already the active one).
	 */
	protected void activateEditor(IEditorPart editor) {
		if (getActiveEditor() != editor) {
			IWorkbenchPartSite site = getSite();
			IWorkbenchPage activePage = site==null ? null : site.getWorkbenchWindow().getActivePage();
			if (activePage != null)
				activePage.activate(editor);
		}
	}
}
