package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
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
	private Label messageLabel;
	private Control viewControl;

	public void createPartControl(Composite parent) {
		GridLayout layout = new GridLayout();
		layout.marginWidth = layout.marginHeight = 0;
		parent.setLayout(layout);
	
		messageLabel = new Label(parent, SWT.WRAP);
		messageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		setVisible(messageLabel, false);
		
		viewControl = createViewControl(parent);
		viewControl.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
	}

	/**
	 * Create and return the control that displays the content of the view. 
	 * This will typically be a table or a tree widget. 
	 */
	abstract protected Control createViewControl(Composite parent);

	/**
	 * Sets the focus. Override if the focus should be set to a child of viewControl.
	 */
	@Override
	public void setFocus() {
		viewControl.setFocus();
	}

	/**
	 * Display a message (such as "Nothing to show") instead of the contents.
	 */
	protected void displayMessage(String text) {
		messageLabel.setText(text);
		if (isVisible(viewControl)) {
			setVisible(messageLabel, true);
			setVisible(viewControl, false);
		}
	}

	protected boolean isDisposed() {
		return messageLabel.isDisposed() || viewControl.isDisposed();
	}
	
	/**
	 * Displays the part control.
	 */
	protected void hideMessage() {
		if (!isVisible(viewControl)) {
			setVisible(messageLabel, false);
			setVisible(viewControl, true);
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
}
