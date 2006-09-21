package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;

public class ImageCellEditor extends DialogCellEditor {

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
		// TODO Auto-generated method stub
		FileDialog cellDialog = new FileDialog(cellEditorWindow.getShell());
        String value = (String)getValue();
        if (value != null) {
			cellDialog.setFileName(value);
		}
        return cellDialog.open();
	}

}
