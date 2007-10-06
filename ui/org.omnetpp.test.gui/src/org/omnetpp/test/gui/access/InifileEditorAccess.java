package org.omnetpp.test.gui.access;

import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.form.InifileFormEditor;
import org.omnetpp.inifile.editor.text.InifileTextEditor;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;

public class InifileEditorAccess
    extends MultiPageEditorPartAccess
{

	public InifileEditorAccess(InifileEditor multiPageNedEditor) {
		super(multiPageNedEditor);
	}
	
    @Override
	public InifileEditor getPart() {
	    return (InifileEditor)workbenchPart;
    }

    public TextEditorAccess getTextEditor() {
        return (TextEditorAccess) createAccess(getPart().getTextEditor());
    }

    public InifileFormEditorAccess getFormEditor() {
        return (InifileFormEditorAccess) createAccess(getPart().getFormEditor());
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

    public CompositeAccess ensureActiveFormPage(String category) {
        return ensureActiveFormEditor().ensureActiveCategoryPage(category);
    }

}
