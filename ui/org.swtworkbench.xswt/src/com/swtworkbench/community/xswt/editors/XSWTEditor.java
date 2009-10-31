package com.swtworkbench.community.xswt.editors;

import org.eclipse.ui.editors.text.TextEditor;

public class XSWTEditor extends TextEditor {
    private ColorManager colorManager;

    public XSWTEditor() {
        this.colorManager = new ColorManager();
        setSourceViewerConfiguration(new XMLConfiguration(this.colorManager));
        setDocumentProvider(new XMLDocumentProvider());
    }

    public void dispose() {
        this.colorManager.dispose();
        super.dispose();
    }
}