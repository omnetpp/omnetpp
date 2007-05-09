package org.omnetpp.common.properties;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.image.ImageSelectionDialog;

public class ImageCellEditor extends TextCellEditorEx {

    public ImageCellEditor() {
		super();
	}

	public ImageCellEditor(Composite parent, int style) {
		super(parent, style);
	}

	protected ImageCellEditor(Composite parent) {
		super(parent);
	}

	@Override
	protected Object openDialogBox(Control cellEditorWindow) {
		ImageSelectionDialog cellDialog = 
			new ImageSelectionDialog(cellEditorWindow.getShell(), (String)getValue());

		if (cellDialog.open() == Dialog.OK) 
		    return cellDialog.getImageId();

		// dialog cancelled
		return null;
	}
	
}
