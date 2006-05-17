package org.omnetpp.scave2.editors;

import org.eclipse.ui.editors.text.TextEditor;

public class ScaveEditor extends TextEditor {

	private ColorManager colorManager;

	public ScaveEditor() {
		super();
		colorManager = new ColorManager();
		setSourceViewerConfiguration(new XMLConfiguration(colorManager));
		setDocumentProvider(new XMLDocumentProvider());
	}
	public void dispose() {
		colorManager.dispose();
		super.dispose();
	}

}
