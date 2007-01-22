package org.omnetpp.scave.editors.ui;


/**
 * Additional dialog constants used in the Scave editor.
 *
 * @author tomi
 */
public interface IDialogConstants extends
		org.eclipse.jface.dialogs.IDialogConstants {
	
	int START = org.eclipse.jface.dialogs.IDialogConstants.CLIENT_ID;

	/**
	 * Button id for an "Apply" button.
	 */
	public int APPLY_ID = START + 1;

	/**
	 * The label for apply buttons.
	 */
	public String APPLY_LABEL = "Apply";
}
