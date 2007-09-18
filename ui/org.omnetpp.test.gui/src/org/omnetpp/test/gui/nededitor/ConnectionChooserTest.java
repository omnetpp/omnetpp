package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.FlyoutPaletteCompositeAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.MenuAccess;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;


/**
 * Tests the ConnectionChooser popup menu. Tests whether all possible gate combinations are offered. 
 * Used gates should be displayed with a disabled menu item. If no gate is defined notification should 
 * be shown. 
 * @author rhornig
 */
public class ConnectionChooserTest
    extends NedFileTestCase
{
    private GraphicalNedEditorAccess graphicalNedEditor;

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
        graphicalNedEditor = (GraphicalNedEditorAccess)findMultiPageEditor().activatePage("Graphical");
    }
    
    // used gates cannot be part of a new connection (they should be disabled in the menu)
    public void testDisabledItemsInConnectionChooser() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1; inout g2; }\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        createConnection("test1", "test2", "test1\\.g1 <--> test2\\.g1");        
        assertConnectionChooserItemsEnabledState("test1", "test2", 
                new String[] {"test1.g2 <--> test2.g2"},
                new String[] {"test1.g1 <--> test2.g2", "test1.g2 <--> test2.g1", "test1.g1 <--> test2.g1"});
    }

    public void testNoGatesMessageInConnectionChooser() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1; }\nsimple TestSimpleModuleNoGates {}\nmodule TestCompoundModule { submodules: test: TestSimpleModule; testNoGate1: TestSimpleModuleNoGates; testNoGate2: TestSimpleModuleNoGates;}");
        assertConnectionChooserItemsEnabledState("test", "testNoGate1", 
                new String[] {},
                new String[] {"testNoGate1 has no gates"});
        assertConnectionChooserItemsEnabledState("testNoGate1", "test", 
                new String[] {},
                new String[] {"testNoGate1 has no gates"});
        assertConnectionChooserItemsEnabledState("testNoGate1", "testNoGate2", 
                new String[] {},
                new String[] {"testNoGate1 and testNoGate2 have no gates"});
    }

    private void createConnection(String moduleName1, String moduleName2, String menuItem) {
        createConnection("Connection", moduleName1, moduleName2, menuItem);
    }

    private void createConnection(String channel, String moduleName1, String moduleName2, String menuItem) {
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        compoundModuleEditPart.createConnectionWithPalette(channel, moduleName1, moduleName2, menuItem);
    }
    
    @NotInUIThread
    private void assertConnectionChooserItemsEnabledState(String moduleName1, String moduleName2, String[] enabledItems, String[] disabledItems) {
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        FlyoutPaletteCompositeAccess flyoutPaletteComposite = compoundModuleEditPart.getFlyoutPaletteComposite();
        flyoutPaletteComposite.clickButtonFigureWithLabel("Connection");
        compoundModuleEditPart.clickSubmoduleFigureWithName(moduleName1);
        compoundModuleEditPart.clickSubmoduleFigureWithName(moduleName2);
        assertMenuItemsEnabled(enabledItems);
        assertMenuItemsDisabled(disabledItems);
        MenuAccess.closeMenus();
     }

    @InUIThread
    private void assertMenuItemsDisabled(String[] labels) {
        new MenuAccess(Access.getDisplay().getActiveShell().getMenu()).assertMenuItemsDisabled(labels);
    }

    @InUIThread
    private void assertMenuItemsEnabled(String[] labels) {
        new MenuAccess(Access.getDisplay().getActiveShell().getMenu()).assertMenuItemsEnabled(labels);
    }
    
}
