package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.InifileFormEditorAccess;

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

    public void testCreateBareSection() throws Exception {
        prepareTest("");
        getFormEditor().createSectionByDialog("Foo", null, null, null);
        assertTextEditorContentMatches(makeSectionContent("Foo", null, null, null));
    }
    
    public void testCreateFullSection() throws Exception {
        prepareTest("[Config Bar]\n");
        getFormEditor().createSectionByDialog("Foo", "some foo", "Bar", "FooNetwork");
        assertTextEditorContentMatches("[Config Bar]\n" + makeSectionContent("Foo", "some foo", "Bar", "FooNetwork"));
    }

    public void testCreateBareGeneralSection() throws Exception {
        prepareTest("");
        getFormEditor().createSectionByDialog("General", null, null, null);
        assertTextEditorContentMatches(makeSectionContent("General", null, null, null));
    }
    
    public void testCreateFullGeneralSection() throws Exception {
        prepareTest("");
        getFormEditor().createSectionByDialog("General", "something general", null, "GeneralNetwork");
        assertTextEditorContentMatches(makeSectionContent("General", "something general", null, "GeneralNetwork"));
    }
    
    //TODO edit section
    //TODO rename section
    //TODO test that sections causing cycles are not offered
    //TODO test that duplicate names are not allowed
    //TODO test content assist in network editfield
}
