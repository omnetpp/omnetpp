package org.omnetpp.test.gui.access;

import org.omnetpp.inifile.editor.form.FormPage;
import org.omnetpp.inifile.editor.form.InifileFormEditor;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.core.InUIThread;

public class InifileFormEditorAccess
    extends CompositeAccess
{

	public InifileFormEditorAccess(InifileFormEditor formEditor) {
		super(formEditor);
	}
	
	public InifileFormEditor getFormEditor() {
	    return (InifileFormEditor)getControl();
    }

    @InUIThread
    public CompositeAccess activateCategoryPage(String category) {
        //FIXME by clicking!!!!
        return (CompositeAccess)createAccess(getFormEditor().showCategoryPage(category));
    }

    @InUIThread
    public CompositeAccess ensureActiveCategoryPage(String category) {
        FormPage activeFormPage = getFormEditor().getActiveFormPage();
        if (activeFormPage.getPageCategory().equals(category))
            return (CompositeAccess)createAccess(activeFormPage);
        else 
            return activateCategoryPage(category);
    }

}
