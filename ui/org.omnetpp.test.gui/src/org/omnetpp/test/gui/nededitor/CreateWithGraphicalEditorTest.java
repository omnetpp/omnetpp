package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.util.WorkspaceUtils;


public class CreateWithGraphicalEditorTest
    extends NedFileTestCase
{
    private GraphicalNedEditorAccess graphicalNedEditor;

    public void testCreateSimpleModule() throws Exception {
        prepareTest();
        graphicalNedEditor.createSimpleModuleWithPalette("TestSimpleModule");
        assertFileExistsWithContent(".*simple TestSimpleModule.*");
    }

    public void testCreateModuleInterface() throws Exception {
        prepareTest();
        graphicalNedEditor.createModuleInterfaceWithPalette("TestModuleInterface");
        assertFileExistsWithContent(".*moduleinterface TestModuleInterface.*");
    }
    
    public void testCreateChannel() throws Exception {
        prepareTest();
        graphicalNedEditor.createChannelWithPalette("TestChannel");
        assertFileExistsWithContent(".*channel TestChannel.*");
    }
    
    public void testCreateChannelInterface() throws Exception {
        prepareTest();
        graphicalNedEditor.createChannelInterfaceWithPalette("TestChannelInterface");
        assertFileExistsWithContent(".*channelinterface TestChannelInterface.*");
    }

    public void testCreateCompoundModule() throws Exception {
        prepareTest();
        graphicalNedEditor.createCompoundModuleWithPalette("TestCompoundModule");
        assertFileExistsWithContent(".*module TestCompoundModule.*");
    }
    
    public void testCreateSubmodule() throws Exception {
        prepareTest("simple TestSimpleModule {}\nmodule TestCompoundModule {}");
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        compoundModuleEditPart.createSubModuleWithPalette("TestSimpleModule", "test", 100, 100);
        assertFileExistsWithContent(".*submodules.*test.*TestSimpleModule.*");
    }
    
    public void testCreateSubmoduleToSubmoduleConnection() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g; }\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        testCreateConnection("test1", "test2");
        assertFileExistsWithContent(".*connections.*test1.g.*test2.g.*");
    }

    public void testCreateSubmoduleSelfConnection() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g; }\nmodule TestCompoundModule { submodules: test: TestSimpleModule; }");
        testCreateConnection("test", "test");
        assertFileExistsWithContent(".*connections.*test.g.*test.g.*");
    }
    
    public void testCreateSubmoduleToParentConnection() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g; }\nmodule TestCompoundModule { gates: inout g; submodules: test: TestSimpleModule; }");
        testCreateConnection("test", "TestCompoundModule");
        assertFileExistsWithContent(".*connections.*test.g.*g.*");
    }

    public void testCreateSubmoduleToSubmoduleConnectionWithChannel() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g; }\nchannel TestChannel {}\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        testCreateConnection(".*TestChannel.*", "test1", "test2");
        assertFileExistsWithContent(".*connections.*test1.g.*TestChannel.*test2.g.*");
    }

    private void prepareTest() throws Exception {
        prepareTest("");
    }

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
        graphicalNedEditor = findNedEditor().ensureActiveGraphicalEditor();
    }
    
    private void testCreateConnection(String moduleName1, String moduleName2) {
        testCreateConnection("Connection", moduleName1, moduleName2);
    }

    private void testCreateConnection(String channel, String moduleName1, String moduleName2) {
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        compoundModuleEditPart.createConnectionWithPalette(channel, moduleName1, moduleName2, ".*g.*");
    }
    
    private void assertFileExistsWithContent(String regexpContent) throws Exception {
        Access.getWorkbenchWindowAccess().findEditorPartByTitle(fileName).saveWithHotKey();
        WorkspaceUtils.assertFileExistsWithRegexpContent(filePath, regexpContent);
    }
}
