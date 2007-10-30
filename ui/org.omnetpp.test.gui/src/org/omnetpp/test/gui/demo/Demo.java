package org.omnetpp.test.gui.demo;

import com.simulcraft.test.gui.access.*;
import com.simulcraft.test.gui.core.AnimationEffects;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Rectangle;

import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.util.StringUtils;
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
    protected boolean delay = true;
    
    @Override
    protected void setUp() throws Exception {
        super.setUp();
        Access.getWorkbenchWindow().closeAllEditorPartsWithHotKey();
        WorkspaceUtils.ensureProjectNotExists(name);
//      WorkspaceUtils.ensureFileNotExists("/demo/omnetpp.ini");
//      WorkspaceUtils.ensureFileNotExists("/demo/demo.anf");
    }
    
    void showMessage(String msg, int lines) {
        int width = 600;
        int height = lines * 28 + 20;
        Rectangle bounds = Access.getWorkbenchWindow().getShell().getAbsoluteBounds();
        int x = bounds.x + bounds.width/2 - width/2;
        int y = bounds.y + bounds.height/3;
        msg = msg.replace("\n", "<br/>");
        if (delay)
            AnimationEffects.showMessage(msg, x, y, width, height, msg.length()*70);        
    }
    
    public void testPlay() throws Throwable {
        // open perspective
        showMessage("Welcome to the OMNeT++/OMNEST video demo.\n" +
            		"This demo will show you, how to create, " +
            		"setup, run and analyze a closed queuing network " +
            		"using the OMNeT++ IDE", 3);
        
        openPerspective();
        createProject();
        createNedFile();
        createDemoNetwork();
        createIniFile();
        createLaunchConfigAndLaunch();
        refreshNavigator();
        analyseResults();
    }

    private void openPerspective() {
        showMessage("First of all, we switch our IDE to the OMNeT++ perspective", 1);
        Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Window|Open Perspective|OMNeT\\+\\+");
    }

    private void refreshNavigator() {
        {
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            TreeItemAccess treeItem = tree.findTreeItemByContent(name);
            treeItem.reveal().chooseFromContextMenu("Refresh");
        }
    }

    private void createProject() {
        {
            showMessage("Now we are able to create a new OMNeT++ simulation project", 2);
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
    
            if (delay) Access.sleep(3);
        }
    }

    private void createNedFile() {
        {
            showMessage("Let's create a new NED file with an empty network in it.", 1);
            TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Network Description File \\(ned\\)");
            ShellAccess shell = Access.findShellWithTitle("New NED File");
            shell.findTree().findTreeItemByContent(name).click();
            TextAccess text = shell.findTextAfterLabel("File name:");
            text.clickAndTypeOver(name);
            shell.findButtonWithLabel("A new toplevel Network").selectWithMouseClick();
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            if (delay) Access.sleep(2);
        }
    }

    private void createDemoNetwork() {
        {
            showMessage("Set up a network with a single source and three queues connected in a ring", 2);
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
    }

    private void createIniFile() {
        {
            showMessage("To run the simulation we should create an INI file.", 1);
            // create with wizard
            WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
            TreeAccess tree = workbenchWindow.findViewPartByTitle("Navigator").getComposite().findTree();
            tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Initialization File \\(ini\\)");
            ShellAccess shell = Access.findShellWithTitle("New Ini File");
            if (delay) Access.sleep(1);
            shell.findComboAfterLabel("NED Network:").selectItem(name);
            if (delay) Access.sleep(1);
            shell.findButtonWithLabel("Finish").selectWithMouseClick();
            if (delay) Access.sleep(2);
            
            // add source parameters
            MultiPageEditorPartAccess iniEditor = (MultiPageEditorPartAccess)workbenchWindow.findEditorPartByTitle("omnetpp\\.ini");
            CompositeAccess form = (CompositeAccess)iniEditor.getActivePageControl();
            ButtonAccess addKeysButton = form.findButtonWithLabel("Add.*");
            addKeysButton.selectWithMouseClick();
            if (delay) Access.sleep(1);
            ShellAccess shell2 = Access.findShellWithTitle("Add Inifile Keys");
            shell2.findButtonWithLabel("Deselect All").selectWithMouseClick();
            if (delay) Access.sleep(1);
            TableAccess table = shell2.findTable();
            TableItemAccess tableItem = table.findTableItemByContent("\\*\\*\\.source\\.interArrivalTime");
            tableItem.ensureChecked(true);
            if (delay) Access.sleep(0.5);
            tableItem = table.findTableItemByContent("\\*\\*\\.source\\.numJobs");
            tableItem.ensureChecked(true);
            if (delay) Access.sleep(2);
            shell2.findButtonWithLabel("OK").selectWithMouseClick();
            if (delay) Access.sleep(2);
            
            // we should use table item access here
            TreeAccess tree2 = form.findTreeAfterLabel("HINT: Drag the icons to change the order of entries\\.");
            tree2.findTreeItemByContent("\\*\\*\\.source\\.numJobs").clickAndTypeOver(1, "${jobs=30,60}\n");
            if (delay) Access.sleep(2);
            tree2.findTreeItemByContent("\\*\\*\\.source\\.interArrivalTime").clickAndTypeOver(1, "0\n");                        
            if (delay) Access.sleep(1);
    
            // add queue parameters from dialog
            addKeysButton.selectWithMouseClick();
            if (delay) Access.sleep(2);
            ShellAccess shell3 = Access.findShellWithTitle("Add Inifile Keys");
            shell3.findButtonWithLabel("Parameter name only.*").selectWithMouseClick();
            if (delay) Access.sleep(1);
            shell3.findButtonWithLabel("Skip parameters that have a default value").selectWithMouseClick();
            if (delay) Access.sleep(1);
            shell3.findTable().findTableItemByContent("\\*\\*\\.serviceTime").ensureChecked(true);
            if (delay) Access.sleep(2);
            shell3.findButtonWithLabel("OK").selectWithMouseClick();
            if (delay) Access.sleep(2);
            
            // add queue parameter values in table
            TextAccess cellEditor = tree2.findTreeItemByContent("\\*\\*\\.serviceTime").activateCellEditor(1);
            if (delay) Access.sleep(2);
            cellEditor.typeOver("exp");
            cellEditor.pressKey(' ', SWT.CTRL);
            if (delay) Access.sleep(2);
            Access.findContentAssistPopup().chooseWithKeyboard("exponential.*continuous.*");
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.pressKey(SWT.BS);
            cellEditor.typeIn("${serviceMean=1..3 step 1})\n");
            if (delay) Access.sleep(3);
            
            // set event logging file
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Output Files").click();
            if (delay) Access.sleep(2);
            TextAccess text = form.findTextAfterLabel("Eventlog file:");
            text.clickAndTypeOver("demo.log");
            if (delay) Access.sleep(2);
    
            // set simulation time limit
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("General.*").click();
            if (delay) Access.sleep(2);
            text = form.findTextAfterLabel("Simulation.*");
            text.clickAndTypeOver("20000");
    
            // set simulation time limit
            ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Cmdenv.*").click();
            if (delay) Access.sleep(2);
            form.findLabel("Status frequency.*").click();
            form.findTextAfterLabel("Status frequency.*").typeIn("500");
    
            if (delay) Access.sleep(3);
            
            iniEditor.activatePageEditor("Text");
            workbenchWindow.getShell().findToolItemWithToolTip("Save.*").click();
            if (delay) Access.sleep(2);
        }
    }

    private void createLaunchConfigAndLaunch() {
        {
            showMessage("Let's create a launch configuration, so we will be able to start the simulation from the IDE", 2);
            Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Run|Open Run Dialog.*");
            if (delay) Access.sleep(2);
            ShellAccess shell = Access.findShellWithTitle("Run");
            
            shell.findTree().findTreeItemByContent("OMNeT\\+\\+ Simulation").click();
            if (delay) Access.sleep(1);
            shell.findToolItemWithToolTip("New launch configuration").click();
            if (delay) Access.sleep(1);
    
            CompositeAccess cTabFolder = shell.findCTabFolder();
            cTabFolder.findTextAfterLabel("Simulation Program:").click();
            shell.pressKey(SWT.TAB);
            shell.pressKey(' ');
            if (delay) Access.sleep(2);
            TreeAccess tree = Access.findShellWithTitle("Select Executable File").findTree();
            tree.pressKey(SWT.ARROW_RIGHT);
            tree.findTreeItemByContent("queueinglib\\.exe").doubleClick();
            if (delay) Access.sleep(2);
            
            cTabFolder.findTextAfterLabel("Initialization file\\(s\\):").click();
            shell.pressKey(SWT.TAB);
            shell.pressKey(' ');
            if (delay) Access.sleep(2);
            TreeAccess tree2 = Access.findShellWithTitle("Select INI Files").findTree();
            tree2.findTreeItemByContent(name).click();
            tree2.pressKey(SWT.ARROW_RIGHT);
            if (delay) Access.sleep(1);
            tree2.findTreeItemByContent("omnetpp\\.ini").doubleClick();
    
            if (delay) Access.sleep(3);
            ComboAccess combo = cTabFolder.findComboAfterLabel("Configuration name:");
            combo.selectItem("General.*");
            if (delay) Access.sleep(2);
            shell.findButtonWithLabel("Command.*").selectWithMouseClick();
            if (delay) Access.sleep(1);
            shell.findTextAfterLabel("Run number.*").clickAndTypeOver("*");
            if (delay) Access.sleep(1);
            shell.pressKey(SWT.TAB);
            shell.pressKey(SWT.DEL);
            shell.pressKey('2');
            if (delay) Access.sleep(1);
            shell.findButtonWithLabel("Run").selectWithMouseClick();
            WorkbenchUtils.ensureViewActivated("General", "Progress.*"); 
            Access.sleep(25);
        }
    }

    private void analyseResults() {
        {
            showMessage("We can analyze the generated results now", 1);
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
            if (delay) Access.sleep(3);
            ControlAccess canvas = ((ControlAccess)Access.createAccess(chart.findDescendantControl(VectorChart.class)));
            canvas.chooseFromContextMenu("Apply|Mean");
            if (delay) Access.sleep(3);

            // create a dataset from the chart
//          canvas.chooseFromContextMenu("Convert to Dataset.*");
//          ShellAccess shell = Access.findShellWithTitle("Create chart template");
//          shell.findTextAfterLabel("Dataset name:").typeOver("queue length mean");
//          shell.findTextAfterLabel("Chart name:").clickAndTypeOver("queue length");
//          shell.findButtonWithLabel("OK").selectWithMouseClick();

//          // manipulate the dataset
//          DatasetsAndChartsPageAccess dp = scaveEditor.ensureDatasetsPageActive();

            if (delay) Access.sleep(3);
            workbenchWindow.getShell().findToolItemWithToolTip("Save.*").click();
            Access.sleep(10);


        }
    }
}
