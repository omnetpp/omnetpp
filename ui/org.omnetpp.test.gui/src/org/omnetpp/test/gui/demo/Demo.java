package org.omnetpp.test.gui.demo;

import com.simulcraft.test.gui.access.*;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.eclipse.swt.SWT;

import org.omnetpp.scave.charting.VectorChart;
import org.omnetpp.test.gui.access.BrowseDataPageAccess;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.DatasetsAndChartsPageAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.InputsPageAccess;
import org.omnetpp.test.gui.access.NedEditorAccess;
import org.omnetpp.test.gui.access.ScaveEditorAccess;
import org.omnetpp.test.gui.scave.ScaveEditorUtils;

public class Demo extends GUITestCase {
    protected String name = "demo";
    protected boolean delayed = false;
    
    @Override
    protected void setUp() throws Exception {
        super.setUp();
        Access.getWorkbenchWindow().closeAllEditorPartsWithHotKey();
        WorkspaceUtils.ensureProjectNotExists(name);
//      WorkspaceUtils.ensureFileNotExists("/demo/omnetpp.ini");
//      WorkspaceUtils.ensureFileNotExists("/demo/demo.anf");
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

            if (delayed) Access.sleep(3);
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
            
            Access.getWorkbenchWindow().getShell().findToolItemWithToolTip("Save.*").click();
        }
        
        // create INI file
        {
            // create with wizard
            WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
            TreeAccess tree = workbenchWindow.findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Initialization File \\(ini\\)");
            ShellAccess shell = Access.findShellWithTitle("New Ini File");
            if (delayed) Access.sleep(1);
            shell.findComboAfterLabel("NED Network:").selectItem(name);
            if (delayed) Access.sleep(1);
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            if (delayed) Access.sleep(2);
            
            // add source parameters
            MultiPageEditorPartAccess iniEditor = (MultiPageEditorPartAccess)workbenchWindow.findEditorPartByTitle("omnetpp\\.ini");
            CompositeAccess form = (CompositeAccess)iniEditor.getActivePageControl();
            ButtonAccess addKeysButton = form.findButtonWithLabel("Add.*");
            addKeysButton.selectWithMouseClick();
            if (delayed) Access.sleep(1);
            ShellAccess shell2 = Access.findShellWithTitle("Add Inifile Keys");
            shell2.findButtonWithLabel("Deselect All").selectWithMouseClick();
            if (delayed) Access.sleep(1);
            TableAccess table = shell2.findTable();
            TableItemAccess tableItem = table.findTableItemByContent("\\*\\*\\.source\\.interArrivalTime");
            tableItem.ensureChecked(true);
            if (delayed) Access.sleep(0.5);
            tableItem = table.findTableItemByContent("\\*\\*\\.source\\.numJobs");
            tableItem.ensureChecked(true);
            if (delayed) Access.sleep(2);
            shell2.findButtonWithLabel("OK").selectWithMouseClick();
            if (delayed) Access.sleep(2);
            
            // we should use table item access here
            TreeAccess tree2 = form.findTreeAfterLabel("HINT: Drag the icons to change the order of entries\\.");
            tree2.findTreeItemByContent("\\*\\*\\.source\\.numJobs").clickAndTypeOver(1, "${jobs=30,60}\n");
            if (delayed) Access.sleep(2);
            tree2.findTreeItemByContent("\\*\\*\\.source\\.interArrivalTime").clickAndTypeOver(1, "0\n");                        
            if (delayed) Access.sleep(1);

            // add queue parameters from dialog
            addKeysButton.selectWithMouseClick();
            if (delayed) Access.sleep(2);
            ShellAccess shell3 = Access.findShellWithTitle("Add Inifile Keys");
            shell3.findButtonWithLabel("Parameter name only.*").selectWithMouseClick();
            if (delayed) Access.sleep(1);
            shell3.findButtonWithLabel("Skip parameters that have a default value").selectWithMouseClick();
            if (delayed) Access.sleep(1);
            shell3.findTable().findTableItemByContent("\\*\\*\\.serviceTime").ensureChecked(true);
            if (delayed) Access.sleep(2);
            shell3.findButtonWithLabel("OK").selectWithMouseClick();
            if (delayed) Access.sleep(2);
            
            // add queue parameter values in table
            TextAccess cellEditor = tree2.findTreeItemByContent("\\*\\*\\.serviceTime").activateCellEditor(1);
            if (delayed) Access.sleep(2);
            cellEditor.typeOver("exp");
            cellEditor.pressKey(' ', SWT.CTRL);
            if (delayed) Access.sleep(2);
            Access.findContentAssistPopup().chooseWithKeyboard("exponential.*continuous.*");
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.typeIn("${serviceMean=1..3 step 1})\n");
            if (delayed) Access.sleep(3);
            
            // set event logging file
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Output Files").click();
            if (delayed) Access.sleep(2);
            TextAccess text = form.findTextAfterLabel("Eventlog file:");
            text.clickAndTypeOver("demo.log");
            if (delayed) Access.sleep(2);

            // set simulation time limit
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("General.*").click();
            if (delayed) Access.sleep(2);
            text = form.findTextAfterLabel("Simulation.*");
            text.clickAndTypeOver("20000");

            // set simulation time limit
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Cmdenv.*").click();
            if (delayed) Access.sleep(2);
            form.findLabel("Status frequency.*").click();
            form.findTextAfterLabel("Status frequency.*").typeIn("500");

            if (delayed) Access.sleep(3);
            
            iniEditor.activatePageEditor("Text");
            workbenchWindow.getShell().findToolItemWithToolTip("Save.*").click();
            if (delayed) Access.sleep(2);
        }
        
        // launch the simulation
        {
            Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Run|Open Run Dialog.*");
            if (delayed) Access.sleep(2);
            ShellAccess shell = Access.findShellWithTitle("Run");
            
            shell.findTree().findTreeItemByContent("OMNeT\\+\\+ Simulation").click();
            if (delayed) Access.sleep(1);
            shell.findToolItemWithToolTip("New launch configuration").click();
            if (delayed) Access.sleep(1);
            shell.findTextAfterLabel("Name:").clickAndTypeOver(name);
            if (delayed) Access.sleep(1);

            CompositeAccess cTabFolder = shell.findCTabFolder();
            cTabFolder.findTextAfterLabel("Simulation Program:").click();
            shell.pressKey(SWT.TAB);
            shell.pressKey(' ');
            if (delayed) Access.sleep(2);
            TreeAccess tree = Access.findShellWithTitle("Select Executable File").findTree();
            tree.pressKey(SWT.ARROW_RIGHT);
            tree.findTreeItemByContent("queueinglib\\.exe").doubleClick();
            if (delayed) Access.sleep(2);
            
            cTabFolder.findTextAfterLabel("Initialization file\\(s\\):").click();
            shell.pressKey(SWT.TAB);
            shell.pressKey(' ');
            if (delayed) Access.sleep(2);
            TreeAccess tree2 = Access.findShellWithTitle("Select INI Files").findTree();
            tree2.findTreeItemByContent(name).click();
            tree2.pressKey(SWT.ARROW_RIGHT);
            if (delayed) Access.sleep(1);
            tree2.findTreeItemByContent("omnetpp\\.ini").doubleClick();

            if (delayed) Access.sleep(3);
            ComboAccess combo = cTabFolder.findComboAfterLabel("Configuration name:");
            combo.selectItem("General.*");
            if (delayed) Access.sleep(2);
            shell.findButtonWithLabel("Command.*").selectWithMouseClick();
            if (delayed) Access.sleep(1);
            shell.findTextAfterLabel("Run number.*").clickAndTypeOver("*");
            if (delayed) Access.sleep(1);
            shell.pressKey(SWT.TAB);
            shell.pressKey(SWT.DEL);
            shell.pressKey('2');
            if (delayed) Access.sleep(1);
            shell.findButtonWithLabel("Run").selectWithMouseClick();
            WorkbenchUtils.ensureViewActivated("General", "Progress.*"); 
            Access.sleep(25);
        }

        // refresh the workspace to contionue with analysis
        {
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            TreeItemAccess treeItem = tree.findTreeItemByContent(name);
            treeItem.reveal().chooseFromContextMenu("Refresh");
        }
        
      // results analysis
      {
          WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
          workbenchWindow.getShell().chooseFromMainMenu("File|New\tAlt\\+Shift\\+N|Analysis File \\(anf\\)");
          ShellAccess shell1 = Access.findShellWithTitle("New Analysis File");
          shell1.findTree().findTreeItemByContent("demo").click();
          TextAccess text1 = shell1.findTextAfterLabel("File name:");
          text1.clickAndTypeOver("demo");
          shell1.findButtonWithLabel("Finish").selectWithMouseClick();

          // create an analysis file
          ScaveEditorAccess scaveEditor = ScaveEditorUtils.findScaveEditor("demo\\.anf");
          InputsPageAccess ip = scaveEditor.ensureInputsPageActive();
          ip.findButtonWithLabel("Wildcard.*").selectWithMouseClick();
          Access.findShellWithTitle("Add files with wildcard").findButtonWithLabel("OK").selectWithMouseClick();
          
          // browse data
          BrowseDataPageAccess bdp = scaveEditor.ensureBrowseDataPageActive();
          bdp.ensureVectorsSelected();
          bdp.getRunNameFilter().selectItem("General-4.*");
          bdp.getDataNameFilter().selectItem("length");
          
          bdp.click();
          bdp.pressKey('a', SWT.CTRL);
          workbenchWindow.getShell().findToolItemWithToolTip("Plot.*").click();
          
          // show and play with the chart
          CompositeAccess chart = (CompositeAccess)scaveEditor.ensureActivePage("Chart: temp1");
          if (delayed) Access.sleep(3);
          ControlAccess canvas = ((ControlAccess)Access.createAccess(chart.findDescendantControl(VectorChart.class)));
          canvas.chooseFromContextMenu("Apply|Mean");
          if (delayed) Access.sleep(3);
          
          // create a dataset from the chart
          canvas.chooseFromContextMenu("Convert to Dataset.*");
          ShellAccess shell = Access.findShellWithTitle("Create chart template");
          shell.findTextAfterLabel("Dataset name:").typeOver("queue length mean");
          shell.findTextAfterLabel("Chart name:").clickAndTypeOver("queue length");
          shell.findButtonWithLabel("OK").selectWithMouseClick();

          // manipulate the dataset
          DatasetsAndChartsPageAccess dp = scaveEditor.ensureDatasetsPageActive();
          
          // create dataset
//          TableAccess table = bdp.getSelectedTable();
//          MenuAccess menu = table.activateContextMenuWithMouseClick();
//          menu.activateMenuItemWithMouse("Add [fF]ilter.*");
//
//          ShellAccess dialogShell = Access.findShellWithTitle("Select.*[dD]ataset.*"); 
//          dialogShell.findButtonWithLabel("New.*").selectWithMouseClick();
//          ShellAccess dialogShell2 = Access.findShellWithTitle("New [dD]ataset.*");
//          TextAccess text = dialogShell2.findTextAfterLabel("Enter.*name.*");
//          text.typeIn("queue length");
//          dialogShell2.findButtonWithLabel("OK").selectWithMouseClick();
//          dialogShell.findButtonWithLabel("OK").selectWithMouseClick();
          
//          Access.clickAbsolute(36, 14); //TODO unrecognized click - revise
//          Access.clickAbsolute(249, 7); //TODO unrecognized click - revise
//          Access.findShellWithTitle("New Object").findButtonWithLabel("Finish").selectWithMouseClick();
//          Access.clickAbsolute(62, 12); //TODO unrecognized click - revise
//          ShellAccess newObjectShell = Access.findShellWithTitle("New Object");
//          TabFolderAccess tabFolder = newObjectShell.findTabFolder();
//          TextAccess text = tabFolder.findTextAfterLabel("Chart name:");
//          text.typeIn("queue length");
//          tabFolder.click();
//          tabFolder.findTextAfterLabel("X axis title:").clickAndTypeOver("time");
//          tabFolder.findTextAfterLabel("Y axis title:").clickAndTypeOver("queue length");
//          newObjectShell.findButtonWithLabel("Finish").selectWithMouseClick();
//          composite.findWidgetWithID("Datasets").findTreeItemByContent("line chart queue length").chooseFromContextMenu("Open");
          
          if (delayed) Access.sleep(3);
          workbenchWindow.getShell().findToolItemWithToolTip("Save.*").click();
          Access.sleep(10);
          
          
      }
    }
}
