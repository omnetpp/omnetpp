package org.omnetpp.test.gui.nededitor.graphical;

import java.util.regex.Pattern;

import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.FlyoutPaletteCompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;


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
        graphicalNedEditor = findNedEditor().ensureActiveGraphicalEditor();
    }
    
    // used gates cannot be part of a new connection (they should be disabled in the menu)
    public void testSubmodulesConnectionEnablement() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1; inout g2; }\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        createConnection("test1", "test2", q("test1.g1 <--> test2.g1"));        
        assertConnectionChooserItemsEnabledState("test1", "test2", 
                new String[] {q("test1.g2 <--> test2.g2")},
                new String[] {q("test1.g1 <--> test2.g2"), q("test1.g2 <--> test2.g1"), q("test1.g1 <--> test2.g1")},
                true);
    }

    public void testSubmoduleCompoundConnectionEnablement() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1; inout g2; }\n" +
                    "module TestCompoundModule { gates: inout g1; inout g2; submodules: test: TestSimpleModule; }");
        createConnection("TestCompoundModule", "test", q("g1 <--> test.g1"));        
        assertConnectionChooserItemsEnabledState("TestCompoundModule", "test", 
                new String[] {q("g2 <--> test.g2")},
                new String[] {q("g2 <--> test.g1"), q("g1 <--> test.g2"), q("g1 <--> test.g1")}, true);
    }

    public void testSubmoduleWithUnknownGateArraySizeConnectionEnablement() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1[]; inout g2; }\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        createConnection("test1", "test2", q("test1.g1++ <--> test2.g2"));        
        assertConnectionChooserItemsEnabledState("test1", "test2", 
                new String[] {q("test1.g1++ <--> test2.g1++"), q("test1.g2 <--> test2.g1++")},
                new String[] {q("test1.g1++ <--> test2.g2"), q("test1.g2 <--> test2.g2")},
                true);
    }

    public void testSubmoduleWithKnownGateArraySizeConnectionEnablement() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1[2]; inout g2; }\nmodule TestCompoundModule { submodules: test1: TestSimpleModule; test2: TestSimpleModule; }");
        createConnection("test1", "test2", q("test1.g1[0] <--> test2.g2"));        
        assertConnectionChooserItemsEnabledState("test1", "test2", 
                new String[] {q("test1.g1[0] <--> test2.g1[0]"), q("test1.g2 <--> test2.g1[0]")},
                new String[] {q("test1.g1[0] <--> test2.g2"), q("test1.g2 <--> test2.g2")},
                true);
    }

    public void testSubmoduleWithUnknownArraySizeConnectionEnablement() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1; inout g2; }\nmodule TestCompoundModule { submodules: test1[3]: TestSimpleModule; test2: TestSimpleModule; }");
        createConnection(q("test1[3]"), "test2", q("test1[0].g1 <--> test2.g2"));        
        assertConnectionChooserItemsEnabledState(q("test1[3]"), "test2", 
                new String[] {q("test1[0].g1 <--> test2.g1"), q("test1[0].g2 <--> test2.g1")},
                new String[] {q("test1[0].g1 <--> test2.g2"), q("test1[0].g2 <--> test2.g2")},
                true);
    }
    
    public void testNoGatesMessageInConnectionChooser() throws Exception {
        prepareTest("simple TestSimpleModule { gates: inout g1; }\nsimple TestSimpleModuleNoGates {}\nmodule TestCompoundModule { submodules: test: TestSimpleModule; testNoGate1: TestSimpleModuleNoGates; testNoGate2: TestSimpleModuleNoGates;}");
        assertConnectionChooserItemsEnabledState("test", "testNoGate1", 
                new String[] {},
                new String[] {q("testNoGate1 has no gates")}, true);
        assertConnectionChooserItemsEnabledState("testNoGate1", "test", 
                new String[] {},
                new String[] {q("testNoGate1 has no gates")}, true);
        assertConnectionChooserItemsEnabledState("testNoGate1", "testNoGate2", 
                new String[] {},
                new String[] {q("testNoGate1 and testNoGate2 have no gates")}, true);
    }

    // helpers ----
    private void createConnection(String moduleName1, String moduleName2, String menuItem) {
        createConnection("Connection", moduleName1, moduleName2, menuItem);
    }

    private void createConnection(String channel, String moduleName1, String moduleName2, String menuItem) {
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        compoundModuleEditPart.createConnectionWithPalette(channel, moduleName1, moduleName2, menuItem);
    }
    
    @NotInUIThread
    private void assertConnectionChooserItemsEnabledState(String moduleName1, String moduleName2, String[] enabledItems, String[] disabledItems, boolean doNotAllowExtraItems) {
        CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.findCompoundModule("TestCompoundModule");
        FlyoutPaletteCompositeAccess flyoutPaletteComposite = compoundModuleEditPart.getFlyoutPaletteComposite();
        flyoutPaletteComposite.clickButtonFigureWithLabel("Connection");
        compoundModuleEditPart.clickSubmoduleFigureWithName(moduleName1);
        compoundModuleEditPart.clickSubmoduleFigureWithName(moduleName2);
        assertMenuState(enabledItems, disabledItems, doNotAllowExtraItems);
        MenuAccess.closeMenus();
     }

    @InUIThread
    private void assertMenuState(String[] enabledLabels, String[] disabledLabels, boolean doNotAllowExtraItems) {
        MenuAccess menuAccess = new MenuAccess(Access.getDisplay().getActiveShell().getMenu());
        menuAccess.assertMenuItemsEnabled(enabledLabels);
        menuAccess.assertMenuItemsDisabled(disabledLabels);
        if (doNotAllowExtraItems)
            assertTrue("Menu contains extra items", menuAccess.getMenu().getItemCount() == enabledLabels.length + disabledLabels.length);
    }

    private String q(String txt) {
        return Pattern.quote(txt);
    }
}
