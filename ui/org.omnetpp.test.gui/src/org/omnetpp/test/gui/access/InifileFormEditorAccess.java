package org.omnetpp.test.gui.access;

import org.omnetpp.common.util.Predicate;
import org.omnetpp.inifile.editor.TestSupport;
import org.omnetpp.inifile.editor.form.FormPage;
import org.omnetpp.inifile.editor.form.InifileFormEditor;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.UIStep;
import com.simulcraft.test.gui.core.InBackgroundThread;

//XXX not sure we really need this class. All method could just be added into InifileEditorAccess
public class InifileFormEditorAccess
    extends CompositeAccess
{

	public InifileFormEditorAccess(InifileFormEditor formEditor) {
		super(formEditor);
	}
	
    @Override
	public InifileFormEditor getControl() {
	    return (InifileFormEditor)widget;
    }

	public TreeAccess getCategoryTree() {
	    return (TreeAccess)createAccess(
	            findDescendantControl(
	                    getControl(),
	                    Predicate.hasID(TestSupport.CATEGORY_TREE)));
	}

    @UIStep
    public CompositeAccess getActiveCategoryPage() {
        return (CompositeAccess)createAccess(getControl().getActiveCategoryPage());
    }

    @InBackgroundThread
    public CompositeAccess activateCategoryPage(String category) {
        getCategoryTree().findTreeItemByContent(category).click();
        Access.sleep(1); //XXX no idea why this is needed. should work without it too (because click() is a "step" which includes the processing of the event, and there's no asyncExec in the inifile code here) 
        return (CompositeAccess)createAccess(getControl().getActiveCategoryPage());
    }

    @InBackgroundThread
    public CompositeAccess ensureActiveCategoryPage(String category) {
        FormPage activeFormPage = getControl().getActiveCategoryPage();
        if (activeFormPage.getPageCategory().equals(category))
            return (CompositeAccess)createAccess(activeFormPage);
        else 
            return activateCategoryPage(category);
    }

    @InBackgroundThread
    public void createSectionByDialog(String sectionName, String description, String baseSection, String networkName) {
        CompositeAccess sectionsPage = ensureActiveCategoryPage("Sections");
        sectionsPage.findButtonWithLabel("New.*").click();
        ShellAccess dialog = WorkbenchWindowAccess.findShellWithTitle("New Section");
        fillSectionDialog(dialog, sectionName, description, baseSection, networkName);
    }

    @InBackgroundThread
    public void editSectionByDialog(String sectionName, boolean usingContextMenu, 
            String newSectionName, String description, String baseSection, String networkName) {
        CompositeAccess sectionsPage = ensureActiveCategoryPage("Sections");
        TreeAccess sectionsTree = sectionsPage.findTree();
        String sectionLabel = (sectionName.equals("General") ? sectionName : "Config "+sectionName) + "\\b.*";
        if (usingContextMenu) {
            sectionsTree.findTreeItemByContent(sectionLabel).chooseFromContextMenu("Edit.*");
        }
        else {
            sectionsTree.findTreeItemByContent(sectionLabel).click();
            sectionsPage.findButtonWithLabel("Edit.*").click();
        }
        ShellAccess dialog = WorkbenchWindowAccess.findShellWithTitle(".*Edit.* Section");
        fillSectionDialog(dialog, newSectionName, description, baseSection, networkName);
    }

    @InBackgroundThread
    public static void fillSectionDialog(ShellAccess dialog, String sectionName, String description, String baseSection, String networkName) {
        if (sectionName != null)
            dialog.findTextAfterLabel("Section Name.*").clickAndTypeOver(sectionName);
        if (description != null)
            dialog.findTextAfterLabel("Description.*").clickAndTypeOver(description);
        if (baseSection != null)
            dialog.findComboAfterLabel("Fall back.*").selectItem(baseSection.equals("") ? "" : ".*\\b"+baseSection);
        if (networkName != null)
            dialog.findComboAfterLabel("NED Network.*").clickAndTypeOver(networkName);
        dialog.findButtonWithLabel("OK").click();
    }

}
