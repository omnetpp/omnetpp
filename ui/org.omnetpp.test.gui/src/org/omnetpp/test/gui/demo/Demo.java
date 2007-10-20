package org.omnetpp.test.gui.demo;

import com.simulcraft.test.gui.access.*;
import com.simulcraft.test.gui.core.GUITestCase;

import org.eclipse.swt.SWT;

public class Demo extends GUITestCase {
    public void testPlay() {
        {
            //create project
            Access.findShellWithTitle("OMNeT\\+\\+ - OMNeT\\+\\+ IDE").chooseFromMainMenu("File|New\tAlt\\+Shift\\+N|OMNEST/OMNeT\\+\\+ Project");
            ShellAccess shell = Access.findShellWithTitle("New OMNeT\\+\\+ project");
            shell.findTextAfterLabel("Project name:").typeOver("demo");
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            TreeItemAccess treeItem = tree.findTreeItemByContent("demo");

            // set dependent projects
            treeItem.reveal().chooseFromContextMenu("Properties");
            ShellAccess shell2 = Access.findShellWithTitle("Properties for demo");
            TreeAccess tree2 = shell2.findTree();
            tree2.findTreeItemByContent("Project References").click();
            shell2.findTable().findTableItemByContent("queueinglib").click();
            tree2.findTreeItemByContent("NED Source Folders").click();
            shell2.findButtonWithLabel("OK").selectWithMouseClick();

            // open dependent project
            TreeItemAccess treeItem2 = tree.findTreeItemByContent("queueinglib");
            treeItem2.reveal().chooseFromContextMenu("Open Project");
        }
        // create new ned file
        {
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent("demo").reveal().chooseFromContextMenu("New|Network Description File \\(ned\\)");
            ShellAccess shell = Access.findShellWithTitle("New NED File");
            shell.findTree().findTreeItemByContent("demo").click();
            TextAccess text = shell.findTextAfterLabel("File name:");
            text.typeOver("demo");
            shell.findButtonWithLabel("A new toplevel Network").selectWithMouseClick();
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
        }
        
        // create demo network
        {
        }
        
        // create INI file
        {
            // create with wizard
            WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
            TreeAccess tree = workbenchWindow.findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent("demo").reveal().chooseFromContextMenu("New|Initialization File \\(ini\\)");
            ShellAccess shell = Access.findShellWithTitle("New Ini File");
            shell.findComboAfterLabel("NED Network:").selectItem("demo");
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            
            // add source parameters
            CTabFolderAccess cTabFolder = workbenchWindow.findEditorPartByTitle("omnetpp\\.ini").getComposite().findCTabFolder();
            ButtonAccess addKeysButton = cTabFolder.findButtonWithLabel("Add.*");
            addKeysButton.selectWithMouseClick();
            ShellAccess shell2 = Access.findShellWithTitle("Add Inifile Keys");
            shell2.findButtonWithLabel("Deselect All").selectWithMouseClick();
            TableAccess table = shell2.findTable();
            table.findTableItemByContent("\\*\\*\\.source\\.interArrivalTime").click();
            table.findTableItemByContent("\\*\\*\\.source\\.numJobs").click();
            shell2.findButtonWithLabel("OK").selectWithMouseClick();
            
            TreeAccess tree2 = cTabFolder.findTreeAfterLabel("HINT: Drag the icons to change the order of entries\\.");
            tree2.findTreeItemByContent("\\*\\*\\.source\\.numJobs").activateCellEditor().typeOver("60\n");
            tree2.findTreeItemByContent("\\*\\*\\.source\\.interArrivalTime").activateCellEditor().typeIn("0\n");
            
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
            ((TreeAccess)cTabFolder.findControlWithID("CategoryTree")).findTreeItemByContent("Output Files").click();
            TextAccess text = cTabFolder.findTextAfterLabel("Eventlog file:");
            text.clickAndTypeOver("demo.log");
            workbenchWindow.getShell().findToolItemWithTooltip("Save \\(Ctrl\\+S\\)");
            // or much cleaner:
            // workbenchWindow.findEditorPartByTitle("omnetpp\\.ini").saveWithHotKey();
        }
        
        // launch the simulation
        {
            Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Run|Open Run Dialog.*");
            ShellAccess shell = Access.findShellWithTitle("Run");
            
            shell.findTree().findTreeItemByContent("OMNeT\\+\\+ Simulation").click();
            shell.findToolItemWithTooltip("New launch configuration").click();
            shell.findTextAfterLabel("Name:").clickAndTypeOver("demo");
            CompositeAccess cTabFolder = shell.findCTabFolder();
            cTabFolder.findButtonWithLabel("Browse.*").selectWithMouseClick();

            ShellAccess shell2 = Access.findShellWithTitle("Select Executable File");
            TreeAccess tree = shell2.findTree();
            tree.findTreeItemByContent("queueinglib").click();
            tree.pressKey(SWT.ARROW_RIGHT);
            tree.findTreeItemByContent("queueinglib\\.exe").doubleClick();
            
            cTabFolder.findButtonWithLabel("Browse.*").selectWithMouseClick();

            ShellAccess shell3 = Access.findShellWithTitle("Select INI Files");
            TreeAccess tree2 = shell3.findTree();
            tree2.findTreeItemByContent("demo").click();
            tree2.pressKey(SWT.ARROW_RIGHT);
            tree2.findTreeItemByContent("omnetpp\\.ini").doubleClick();

            ComboAccess combo = cTabFolder.findComboAfterLabel("Configuration name:");
            combo.selectItem("General -- \\(network: demo\\)");
            shell.findButtonWithLabel("Apply").selectWithMouseClick();
            shell.findButtonWithLabel("Run").selectWithMouseClick();
            
        }
    }
}
