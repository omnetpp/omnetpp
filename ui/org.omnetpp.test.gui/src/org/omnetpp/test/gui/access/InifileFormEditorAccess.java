package org.omnetpp.test.gui.access;

import org.omnetpp.inifile.editor.TestSupport;
import org.omnetpp.inifile.editor.form.FormPage;
import org.omnetpp.inifile.editor.form.InifileFormEditor;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.core.NotInUIThread;
import com.simulcraft.test.gui.util.Predicate;

public class InifileFormEditorAccess
    extends CompositeAccess
{

	public InifileFormEditorAccess(InifileFormEditor formEditor) {
		super(formEditor);
	}
	
	public InifileFormEditor getFormEditor() {
	    return (InifileFormEditor)getControl();
    }

	public TreeAccess getCategoryTree() {
	    return (TreeAccess)createAccess(
	            findDescendantControl(
	                    getFormEditor(),
	                    Predicate.hasID(TestSupport.CATEGORY_TREE)));
	}

    @NotInUIThread
    public CompositeAccess activateCategoryPage(String category) {
        getCategoryTree().findTreeItemByContent(category).click();
        Access.sleep(1); //XXX no idea why this is needed. should work without it too (because click() is a "step" which includes the processing of the event, and there's no asyncExec in the inifile code here) 
        return (CompositeAccess)createAccess(getFormEditor().getActiveCategoryPage());
    }

    @NotInUIThread
    public CompositeAccess ensureActiveCategoryPage(String category) {
        FormPage activeFormPage = getFormEditor().getActiveCategoryPage();
        if (activeFormPage.getPageCategory().equals(category))
            return (CompositeAccess)createAccess(activeFormPage);
        else 
            return activateCategoryPage(category);
    }

}
