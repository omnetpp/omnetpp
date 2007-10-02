package org.omnetpp.test.gui.access;

import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.form.InifileFormEditor;
import org.omnetpp.inifile.editor.text.InifileTextEditor;

import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

public class InifileEditorAccess
    extends MultiPageEditorPartAccess
{

	public InifileEditorAccess(InifileEditor multiPageNedEditor) {
		super(multiPageNedEditor);
	}
	
	public InifileEditor getInifileEditor() {
	    return (InifileEditor)workbenchPart;
    }

    public InifileTextEditor getTextEditor() {
        return getInifileEditor().getTextEditor();
    }

    public InifileFormEditor getFormEditor() {
        return getInifileEditor().getFormEditor();
    }

    public TextEditorAccess activateTextEditor() {
        return (TextEditorAccess) activatePageEditor("Text");
    }

    public InifileFormEditorAccess activateFormEditor() {
        return (InifileFormEditorAccess) activatePageControl("Form");
    }

    public TextEditorAccess ensureActiveTextEditor() {
        return (TextEditorAccess)ensureActiveEditor("Text");
    }

    public InifileFormEditorAccess ensureActiveFormEditor() {
        return (InifileFormEditorAccess)ensureActivePage("Form");
    }
    
}
