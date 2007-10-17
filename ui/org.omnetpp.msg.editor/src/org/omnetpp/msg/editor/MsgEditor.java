package org.omnetpp.msg.editor;

import org.eclipse.ui.editors.text.TextEditor;



public class MsgEditor extends TextEditor {

	public MsgEditor() {
		super();
        setSourceViewerConfiguration(new MsgSourceViewerConfiguration());
	}
}
