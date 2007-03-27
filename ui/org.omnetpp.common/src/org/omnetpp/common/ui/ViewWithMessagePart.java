package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.part.ViewPart;

/**
 * Base class for Views that may need to display an informative message 
 * instead of the normal contents (like "No data available."). 
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
	
		hookListeners();
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
	 * Install listeners (PartListener, SelectionChangeListener, etc)
	 * for events that need to update the view. 
	 */
	protected abstract void hookListeners();

	/**
	 * Remove listeners installed in hookListeners(). 
	 * This gets called on view shutdown (dispose()).
	 */
	protected abstract void unhookListeners();

	
	@Override
	public void dispose() {
		unhookListeners();
		super.dispose();
	}

	/**
	 * Display a message (such as "Nothing to show") instead of the contents.
	 */
	protected void displayMessage(String text) {
		if (viewControl.isVisible()) {
			messageLabel.setText(text);
			setVisible(messageLabel, true);
			setVisible(viewControl, false);
		}
	}

	/**
	 * Displays the part control.
	 */
	protected void hideMessage() {
		if (!viewControl.isVisible()) {
			setVisible(messageLabel, false);
			setVisible(viewControl, true);
		}
	}

	/**
	 * Utility function
	 */
	private void setVisible(Control control, boolean visible) {
		Assert.isTrue(control.getParent().getLayout() instanceof GridLayout); // if not, set it!
		GridData gridData = (GridData)control.getLayoutData();
		if (gridData == null) {
			gridData = new GridData();
			control.setLayoutData(gridData);
		}
		gridData.exclude = !visible;
		control.setVisible(visible);
		control.getParent().layout(true, true);
	}
}
