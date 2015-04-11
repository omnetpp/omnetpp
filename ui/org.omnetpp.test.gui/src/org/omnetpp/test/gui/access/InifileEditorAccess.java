/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.omnetpp.inifile.editor.editors.InifileEditor;

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
    public InifileEditor getWorkbenchPart() {
        return (InifileEditor)workbenchPart;
    }

    public TextEditorAccess getTextEditor() {
        return (TextEditorAccess) createAccess(getWorkbenchPart().getTextEditor());
    }

    public InifileFormEditorAccess getFormEditor() {
        return (InifileFormEditorAccess) createAccess(getWorkbenchPart().getFormEditor());
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
