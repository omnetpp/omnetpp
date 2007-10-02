package org.omnetpp.test.gui.inifileeditor.sectionspage;

import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

public class SectionDialogTest extends InifileEditorTestCase {

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private InifileFormEditorAccess getFormEditor() {
        return findInifileEditor().ensureActiveFormEditor();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    private String makeSectionContent(String sectionName, String description, String baseSection, String networkName) {
        String result = "[" + (sectionName.equals("General") ? "" : "Config ") + sectionName + "]\n";
        if (baseSection != null)
            result += "extends = " + baseSection + "\n";
        if (description != null)
            result += "description = \"" + description + "\"\n";
        if (networkName != null)
            result += "network = " + networkName + "\n";
        return result;
    }

    public void testCreateSection1() throws Exception {
        prepareTest("");
        getFormEditor().createSectionByDialog("Foo", null, null, null);
        assertTextEditorContentMatches("[Config Foo]\n");
        //FIXME fails: cannot close editor at the end. Why on earth...?
    }
    
    public void testCreateSection2() throws Exception {
        prepareTest("[Config Bar]\n");
        getFormEditor().createSectionByDialog("Foo", "some foo", "Bar", "FooNetwork");
        assertTextEditorContentMatches("[Config Bar]\n" + makeSectionContent("Foo", "some foo", "Bar", "FooNetwork"));
    }

    public void testCreateGeneralSection1() throws Exception {
        prepareTest("");
        getFormEditor().createSectionByDialog("General", null, null, null);
        assertTextEditorContentMatches(makeSectionContent("General", null, null, null));
    }
    
    public void testCreateGeneralSection2() throws Exception {
        prepareTest("");
        getFormEditor().createSectionByDialog("General", "something general", null, "GeneralNetwork");
        assertTextEditorContentMatches(makeSectionContent("General", "something general", null, "GeneralNetwork"));
    }

    public void testEditSection1() throws Exception {
        // fill in fields
        prepareTest("[Config Foo]\n[Config Bar]");
        getFormEditor().editSectionByDialog("Foo", true, null, "some foo", "Bar", "FooNetwork");
        assertTextEditorContentMatches(makeSectionContent("Foo", "some foo", "Bar", "FooNetwork")+"[Config Bar]\n");
    }

    public void testEditSection2() throws Exception {
        // remove (blank out) fields
        //FIXME fails: cannot null out base section in the dialog, combo doesn't contain blank entry!!!
        prepareTest(makeSectionContent("Foo", "some foo", "Bar", "FooNetwork")+"[Config Bar]\n");
        getFormEditor().editSectionByDialog("Foo", true, null, "", "", "");
        assertTextEditorContentMatches("[Config Foo]\n[Config Bar]\n");
    }

    public void testRenameSection1() throws Exception {
        // name must be adjusted in other sections' "extends=" line as well
        prepareTest(
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\nextends = Foo\n" +
                "[Config Baz]\nextends = Foo\n");
        getFormEditor().editSectionByDialog("Foo", false, "NewFoo", null, null, null);
        assertTextEditorContentMatches(
                "[Config NewFoo]\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\nextends = NewFoo\n" +
                "[Config Baz]\nextends = NewFoo\n");
    }

    public void testRenameToGeneral() throws Exception {
        prepareTest(
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\nextends = Foo\n" +
                "[Config Baz]\nextends = Foo\n");
        getFormEditor().editSectionByDialog("Foo", false, "General", null, null, null);
        assertTextEditorContentMatches(
                "[General]\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\n" +
                "[Config Baz]\n");
        //FIXME fails: "extends=" line don't get removed
    }

    public void testRenameGeneral() throws Exception {
        prepareTest(
                "[General]\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\n" +
                "[Config Baz]\n");
        getFormEditor().editSectionByDialog("General", false, "Foo", null, null, null);
        assertTextEditorContentMatches(
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\nextends = Foo\n" +
                "[Config Baz]\nextends = Foo\n");
        //FIXME fails: extends= doesn't get added to sections
    }
    
    //TODO test that sections causing cycles are not offered
    //TODO test that duplicate names are not allowed (OK button gets disabled)
    //TODO test that content assist works in network editfield
}
