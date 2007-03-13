package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

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
		ImageSelectionDialog cellDialog = 
			new ImageSelectionDialog(cellEditorWindow.getShell(), (String)getValue());

		cellDialog.open(); 

        return cellDialog.getFirstResult();
	}

}
