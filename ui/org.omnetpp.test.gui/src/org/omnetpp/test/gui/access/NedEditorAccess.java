package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.text.TextualNedEditor;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

public class NedEditorAccess
    extends MultiPageEditorPartAccess
{

	public NedEditorAccess(NedEditor multiPageNedEditor) {
		super(multiPageNedEditor);
	}
	
	public NedEditor getMultiPageNedEditor() {
	    return (NedEditor)workbenchPart;
    }

    public TextualNedEditor getTextualNedEditor() {
        return getMultiPageNedEditor().getTextEditor();
    }

    public TextEditorAccess activateTextEditor() {
        return (TextEditorAccess) activatePageEditor("Text");
    }

    public GraphicalNedEditorAccess activateGraphicalEditor() {
        return (GraphicalNedEditorAccess) activatePageEditor("Graphical");
    }

    public TextEditorAccess ensureActiveTextEditor() {
        return (TextEditorAccess)ensureActiveEditor("Text");
    }

    public GraphicalNedEditorAccess ensureActiveGraphicalEditor() {
        return (GraphicalNedEditorAccess)ensureActiveEditor("Graphical");
    }
}
