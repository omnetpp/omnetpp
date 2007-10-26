package org.omnetpp.test.gui.demo;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.NedEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ButtonAccess;
import com.simulcraft.test.gui.access.ComboAccess;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.TableItemAccess;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.access.TreeItemAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class Demo extends GUITestCase {
    protected String name = "demo";

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        WorkspaceUtils.ensureProjectNotExists(name);
    }
    
    public void testPlay() throws Throwable {
        // open perspective
      Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Window|Open Perspective|OMNeT\\+\\+");
        
        {
            //create project
            Access.getWorkbenchWindow().chooseFromMainMenu("File|New\tAlt\\+Shift\\+N|OMNEST/OMNeT\\+\\+ Project");
            ShellAccess shell = Access.findShellWithTitle("New OMNeT\\+\\+ project");
            shell.findTextAfterLabel("Project name:").typeOver(name);
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            TreeItemAccess treeItem = tree.findTreeItemByContent(name);

            // set dependent projects
            treeItem.reveal().chooseFromContextMenu("Properties");
            ShellAccess shell2 = Access.findShellWithTitle("Properties for demo");
            TreeAccess tree2 = shell2.findTree();
            tree2.findTreeItemByContent("Project References").click();
            shell2.findTable().findTableItemByContent("queueinglib").ensureChecked(true);
            
            tree2.findTreeItemByContent("NED Source Folders").click();
            shell2.findButtonWithLabel("OK").selectWithMouseClick();

            // open dependent project
            // TreeItemAccess treeItem2 = tree.findTreeItemByContent("queueinglib");
            // treeItem2.reveal().chooseFromContextMenu("Open Project");
            Access.sleep(3);
        }
        
        // create new ned file
        {
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Network Description File \\(ned\\)");
            ShellAccess shell = Access.findShellWithTitle("New NED File");
            shell.findTree().findTreeItemByContent(name).click();
            TextAccess text = shell.findTextAfterLabel("File name:");
            text.clickAndTypeOver(name);
            shell.findButtonWithLabel("A new toplevel Network").selectWithMouseClick();
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
        }
        
        // create demo network
        {
            NedEditorAccess editor = (NedEditorAccess)Access.getWorkbenchWindow().findEditorPartByTitle("demo\\.ned");
            GraphicalNedEditorAccess graphEd = editor.ensureActiveGraphicalEditor();

            CompoundModuleEditPartAccess compoundModule = graphEd.findCompoundModule("demo");
            compoundModule.createSubModuleWithPalette("Queue.*", "queue1", 180, 150);
            compoundModule.createSubModuleWithPalette("Queue.*", "queue2", 300, 210);
            compoundModule.createSubModuleWithPalette("Queue.*", "queue3", 300, 90);
            compoundModule.createSubModuleWithPalette("Source.*", null, 60, 150);
            compoundModule.createConnectionWithPalette("source", "queue1", ".*");
            compoundModule.createConnectionWithPalette("queue1", "queue2", ".*");
            compoundModule.createConnectionWithPalette("queue3", "queue1", ".*");
            compoundModule.createConnectionWithPalette("queue2", "queue3", ".*");
            
            editor.saveWithHotKey();
        }
        
        
        // create INI file
        {
            // create with wizard
            WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
            WorkspaceUtils.ensureFileNotExists("/demo/omnetpp.ini");
            TreeAccess tree = workbenchWindow.findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Initialization File \\(ini\\)");
            ShellAccess shell = Access.findShellWithTitle("New Ini File");
            shell.findComboAfterLabel("NED Network:").selectItem(name);
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            
            // add source parameters
            MultiPageEditorPartAccess iniEditor = (MultiPageEditorPartAccess)workbenchWindow.findEditorPartByTitle("omnetpp\\.ini");
            CompositeAccess form = (CompositeAccess)iniEditor.getActivePageControl();
            ButtonAccess addKeysButton = form.findButtonWithLabel("Add.*");
            addKeysButton.selectWithMouseClick();
            ShellAccess shell2 = Access.findShellWithTitle("Add Inifile Keys");
            shell2.findButtonWithLabel("Deselect All").selectWithMouseClick();
            TableAccess table = shell2.findTable();
            TableItemAccess tableItem = table.findTableItemByContent("\\*\\*\\.source\\.interArrivalTime");
            tableItem.ensureChecked(true);
            tableItem = table.findTableItemByContent("\\*\\*\\.source\\.numJobs");
            tableItem.ensureChecked(true);
            shell2.findButtonWithLabel("OK").selectWithMouseClick();
            
            // we should use table item access here
            TreeAccess tree2 = form.findTreeAfterLabel("HINT: Drag the icons to change the order of entries\\.");
            tree2.findTreeItemByContent("\\*\\*\\.source\\.numJobs").activateCellEditor().typeOver("60\n");
            tree2.findTreeItemByContent("\\*\\*\\.source\\.interArrivalTime").activateCellEditor().typeIn("0\n");                        

            TableAccess table2 = iniEditor.findTable();
            table2.findTableItemByContent(1, "\\*\\*\\.source\\.numJobs").activateCellEditor().typeOver("60\n");
            table2.findTableItemByContent(1,"\\*\\*\\.source\\.interArrivalTime").activateCellEditor().typeIn("0\n");                        
            
            // add queue parameters from dialog
            addKeysButton.selectWithMouseClick();
            ShellAccess shell3 = Access.findShellWithTitle("Add Inifile Keys");
            shell3.findButtonWithLabel("Parameter name only.*").selectWithMouseClick();
            shell3.findButtonWithLabel("Deselect All").selectWithMouseClick();
            shell3.findTable().findTableItemByContent("\\*\\*\\.serviceTime").click();
            shell3.findButtonWithLabel("Skip parameters that have a default value").selectWithMouseClick();
            shell3.findButtonWithLabel("OK").selectWithMouseClick();
            
            // add queue parameter values in table
            TextAccess cellEditor = tree2.findTreeItemByContent("\\*\\*\\.serviceTime").activateCellEditor();
            cellEditor.typeOver("exp");
            cellEditor.pressKey(' ', SWT.CTRL);
            Access.findContentAssistPopup().chooseWithKeyboard("exponential.*continuous.*");
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.typeIn("2)\n");
            
            // set event logging file
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Output Files").click();
            TextAccess text = form.findTextAfterLabel("Eventlog file:");
            text.clickAndTypeOver("demo.log");
            workbenchWindow.getShell().findToolItemWithToolTip("Save \\(Ctrl\\+S\\)");
            // or much cleaner:
            // workbenchWindow.findEditorPartByTitle("omnetpp\\.ini").saveWithHotKey();
        }
        
        // launch the simulation
        {
            Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Run|Open Run Dialog.*");
            ShellAccess shell = Access.findShellWithTitle("Run");
            
            shell.findTree().findTreeItemByContent("OMNeT\\+\\+ Simulation").click();
            shell.findToolItemWithToolTip("New launch configuration").click();
            shell.findTextAfterLabel("Name:").clickAndTypeOver(name);

            CompositeAccess cTabFolder = shell.findCTabFolder();
            Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Run|Open Run Dialog.*");
            cTabFolder.findTextAfterLabel("Simulation Program:").click();
            shell.pressKey(SWT.TAB);
            shell.pressKey(' ');
            TreeAccess tree = Access.findShellWithTitle("Select Executable File").findTree();
            tree.pressKey(SWT.ARROW_RIGHT);
            tree.findTreeItemByContent("queueinglib\\.exe").doubleClick();
            
            cTabFolder.findTextAfterLabel("Initialization file\\(s\\):").click();
            shell.pressKey(SWT.TAB);
            shell.pressKey(' ');
            TreeAccess tree2 = Access.findShellWithTitle("Select INI Files").findTree();
            tree2.findTreeItemByContent(name).click();
            tree2.pressKey(SWT.ARROW_RIGHT);
            tree2.findTreeItemByContent("omnetpp\\.ini").doubleClick();
            
            ComboAccess combo = cTabFolder.findComboAfterLabel("Configuration name:");
            combo.selectItem("General -- \\(network: demo\\)");
            shell.findButtonWithLabel("Apply").selectWithMouseClick();
            shell.findButtonWithLabel("Run").selectWithMouseClick();
            
        }
    }
}
