package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.MultiPageNedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;

public class MultiPageNedEditorAccess
    extends MultiPageEditorPartAccess
{

	public MultiPageNedEditorAccess(MultiPageNedEditor multiPageNedEditor) {
		super(multiPageNedEditor);
	}
	
	public MultiPageNedEditor getMultiPageNedEditor() {
	    return (MultiPageNedEditor)workbenchPart;
    }

    public TextualNedEditor getTextualNedEditor() {
        return getMultiPageNedEditor().getTextEditor();
    }
}
