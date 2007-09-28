package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;

public class MultiPageNedEditorAccess
    extends MultiPageEditorPartAccess
{

	public MultiPageNedEditorAccess(NedEditor multiPageNedEditor) {
		super(multiPageNedEditor);
	}
	
	public NedEditor getMultiPageNedEditor() {
	    return (NedEditor)workbenchPart;
    }

    public TextualNedEditor getTextualNedEditor() {
        return getMultiPageNedEditor().getTextEditor();
    }
}
