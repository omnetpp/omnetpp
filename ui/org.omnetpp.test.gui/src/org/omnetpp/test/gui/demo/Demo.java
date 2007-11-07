package org.omnetpp.test.gui.demo;

import com.simulcraft.test.gui.access.*;
import com.simulcraft.test.gui.core.AnimationEffects;
import com.simulcraft.test.gui.core.GUITestCase;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Rectangle;

import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.eventlogtable.EventLogTablePlugin;
import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.scave.charting.VectorChart;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.widgets.SequenceChart;
import org.omnetpp.test.gui.access.BrowseDataPageAccess;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.DatasetsAndChartsPageAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.InputsPageAccess;
import org.omnetpp.test.gui.access.NedEditorAccess;
import org.omnetpp.test.gui.access.ScaveEditorAccess;
import org.omnetpp.test.gui.access.SequenceChartAccess;
import org.omnetpp.test.gui.scave.ScaveEditorUtils;

public class Demo extends GUITestCase {
    protected String name = "demo";
    protected String logFileName = name + ".log";
    protected boolean delay = true;
    protected int readingSpeed = 20;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        workbenchWindow.closeAllEditorPartsWithHotKey();
        workbenchWindow.assertNoOpenEditorParts();
//        WorkspaceUtils.ensureProjectNotExists(name);
        System.clearProperty("com.simulcraft.test.running");
//      WorkspaceUtils.ensureFileNotExists("/demo/demo.ned");
      WorkspaceUtils.ensureFileNotExists("/demo/omnetpp.ini");
      WorkspaceUtils.ensureFileNotExists("/demo/demo.anf");
//      setMouseMoveDuration(1000);
//      setTimeScale(0.0);
      setDelayAfterMouseMove(0);
      setDelayBeforeMouseMove(0);

    }

    void sleep(double time) {
        Access.sleep(time);
    }

    void showMessage(String msg, int lines) {
        showMessage(msg, lines, 0);
    }

    void showMessage(String msg, int lines, int verticalDisplacement) {
        if (readingSpeed == 0) return;

        int width = 600;
        int height = lines * 28 + 20;
        Rectangle bounds = Access.getWorkbenchWindow().getShell().getAbsoluteBounds();
        int x = bounds.x + bounds.width/2 - width/2;
        int y = bounds.y + bounds.height/3;
        msg = msg.replace("\n", "<br/>");
        if (delay)
            AnimationEffects.showMessage(msg, x, y + verticalDisplacement, width, height, msg.length()*Access.rescaleTime(readingSpeed));        
    }

    public void testPlay() throws Throwable {
        // open perspective
//        showMessage("Welcome to the OMNeT++/OMNEST video demo.\n" +
//                "This demo will show you, how to create, " +
//                "setup, run and analyze a closed queuing network " +
//                "using the OMNeT++ IDE", 3);
//
        openPerspective();
//        createProject();
//        createNedFile();
//        createDemoNetwork();
        createIniFile();
        createLaunchConfigAndLaunch();
        refreshNavigator();
        analyseResults();
        showSequenceChart();
    }

    private void openPerspective() {
        showMessage("First of all, we switch to the OMNeT++ perspective", 1);
        Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Window|Open Perspective|OMNeT\\+\\+");
    }

    private void refreshNavigator() {
        TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
        TreeItemAccess treeItem = tree.findTreeItemByContent(name);
        treeItem.reveal().chooseFromContextMenu("Refresh");
    }

    private void createProject() {
        showMessage("Now we are able to create a new OMNeT++ simulation project using " +
                "the OMNeT++ project wizard", 2);
        //create project
        Access.getWorkbenchWindow().chooseFromMainMenu("File|New\tAlt\\+Shift\\+N|OMNEST/OMNeT\\+\\+ Project");
        ShellAccess shell = Access.findShellWithTitle("New OMNeT\\+\\+ project");
        sleep(1);
        shell.findTextAfterLabel("Project name:").clickAndTypeOver(name);
        sleep(2);
        shell.findButtonWithLabel("Finish").selectWithMouseClick();

        sleep(1);
        TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
        TreeItemAccess treeItem = tree.findTreeItemByContent(name);
        treeItem.ensureExpanded();

        // set dependent projects
        showMessage("We will use simple modules already defined in the 'queueinglib' project, " +
                "so we have to include it in our dependencies", 3);
        treeItem.reveal().chooseFromContextMenu("Properties");
        ShellAccess shell2 = Access.findShellWithTitle("Properties for demo");
        TreeAccess tree2 = shell2.findTree();
        sleep(1);
        tree2.findTreeItemByContent("Project References").click();
        sleep(1);
        shell2.findTable().findTableItemByContent("queueinglib").ensureChecked(true);

        sleep(2);
        shell2.findButtonWithLabel("OK").selectWithMouseClick();

        sleep(3);
    }

    private void createNedFile() {
        showMessage("The next step is to create a new NED file with an empty network", 2);
        TreeAccess tree = Access.getWorkbenchWindow().findViewPartByTitle("Navigator").getComposite().findTree();
        tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Network Description File \\(ned\\)");
        sleep(1);
        ShellAccess shell = Access.findShellWithTitle("New NED File");
        shell.findTree().findTreeItemByContent(name).click();
        sleep(1);
        TextAccess text = shell.findTextAfterLabel("File name:");
        text.clickAndTypeOver(name);
        showMessage("We choose an empty network template to begin with", 1);
        shell.findButtonWithLabel("A new toplevel Network").selectWithMouseClick();
        sleep(1);
        shell.findButtonWithLabel("Finish").selectWithMouseClick();
        sleep(2);
    }

    private void createDemoNetwork() {
        showMessage("Let's build a closed queuing network with a single source and three queues connected in a ring", 2);
        NedEditorAccess editor = (NedEditorAccess)Access.getWorkbenchWindow().findEditorPartByTitle("demo\\.ned");
        GraphicalNedEditorAccess graphEd = editor.ensureActiveGraphicalEditor();

        CompoundModuleEditPartAccess compoundModule = graphEd.findCompoundModule("demo");
        
        compoundModule.createSubModuleWithPalette("Queue.*", "queue1", 180, 150);
        showMessage("Module types are available in the palette at the right side of the graphical editor", 2, 100);
        compoundModule.createSubModuleWithPalette("Queue.*", "queue2", 300, 210);
        sleep(1);
        compoundModule.createSubModuleWithPalette("Queue.*", "queue3", 300, 90);
        sleep(1);
        compoundModule.createSubModuleWithPalette("Source.*", null, 60, 150);

        showMessage("Now we have to connect the submodules with the connection tool", 2, -50);
        compoundModule.createConnectionWithPalette("source", "queue1", ".*");
        showMessage("When connecting two submodules we have to choose from a popup menu, which gates should be connected", 2, 100);
        compoundModule.createConnectionWithPalette("queue1", "queue2", ".*");
        sleep(1);
        compoundModule.createConnectionWithPalette("queue3", "queue1", ".*");
        sleep(1);

        showMessage("Let's check the created NED file in text form and add a connection", 2, 100);
        StyledTextAccess styledText = editor.ensureActiveTextEditor().findStyledText();
        styledText.moveCursorAfter(".*queue1\\.in\\+\\+;");
        sleep(1);
        styledText.pressEnter();
        styledText.pressKey('q');
        styledText.pressKey(' ', SWT.CTRL);
        sleep(1);
        Access.findContentAssistPopup().chooseWithKeyboard("queue2 - submodule");
        styledText.pressKey('.');
        sleep(1);
        Access.findContentAssistPopup().chooseWithMouse("out - gate");
        sleep(1);
        styledText.typeIn(" --> q");
        sleep(1);
        styledText.pressKey(' ', SWT.CTRL);
        sleep(1);
        Access.findContentAssistPopup().chooseWithKeyboard("queue3 - submodule");
        sleep(1);
        styledText.typeIn(".in++;");
        sleep(3);

        editor.ensureActiveGraphicalEditor();
        sleep(3);

        Access.getWorkbenchWindow().getShell().findToolItemWithTooltip("Save.*").click();
    }

    private void createIniFile() {
        showMessage("To run the simulation we should create an INI file and set the model parameters", 2);
        // create with wizard
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        TreeAccess tree = workbenchWindow.findViewPartByTitle("Navigator").getComposite().findTree();
        showMessage("We can create an INI file using an OMNeT++ wizard", 1);
        tree.findTreeItemByContent(name).reveal().chooseFromContextMenu("New|Initialization File \\(ini\\)");
        ShellAccess shell = Access.findShellWithTitle("New Ini File");
        showMessage("Setting the network to be simulated", 1);
        shell.findComboAfterLabel("NED Network:").selectItem(name);
        sleep(1);
        shell.findButtonWithLabel("Finish").selectWithMouseClick();
        sleep(2);

        // add source parameters
        showMessage("Our next task is to specify the model parameters that do not have a default value", 2);
        MultiPageEditorPartAccess iniEditor = (MultiPageEditorPartAccess)workbenchWindow.findEditorPartByTitle("omnetpp\\.ini");
        CompositeAccess form = (CompositeAccess)iniEditor.getActivePageControl();
        ButtonAccess addKeysButton = form.findButtonWithLabel("Add.*");
        addKeysButton.selectWithMouseClick();
        showMessage("First we set the interArrivalTime and the number of jobs in the Source submodule", 2);
        ShellAccess shell2 = Access.findShellWithTitle("Add Inifile Keys");
        shell2.findButtonWithLabel("Deselect All").selectWithMouseClick();
        sleep(1);
        TableAccess table = shell2.findTable();
        TableItemAccess tableItem = table.findTableItemByContent("\\*\\*\\.source\\.interArrivalTime");
        tableItem.ensureChecked(true);
        sleep(0.5);
        tableItem = table.findTableItemByContent("\\*\\*\\.source\\.numJobs");
        tableItem.ensureChecked(true);
        sleep(2);
        shell2.findButtonWithLabel("OK").selectWithMouseClick();
        sleep(2);

        // we should use table item access here
        TreeAccess tree2 = form.findTreeAfterLabel("HINT: Drag the icons to change the order of entries\\.");
        showMessage("InterArrivalTime will be 0 meaning all jobs will be immediately injected to the queuing network", 2);
        tree2.findTreeItemByContent("\\*\\*\\.source\\.interArrivalTime").clickAndTypeOver(1, "0\n");                        
        sleep(1);
        showMessage("Let's run the simulation with 30 and then with 60 initial jobs. " +
                "To do this we specify a 'running' parameter value using the ${jobs=30,60} syntax", 3, -40);
        tree2.findTreeItemByContent("\\*\\*\\.source\\.numJobs").clickAndTypeOver(1, "${jobs=30,60}\n");
        sleep(2);

        // add queue parameters from dialog
        showMessage("Now specify the service time for all queues in the system", 1);
        addKeysButton.selectWithMouseClick();
        sleep(2);
        ShellAccess shell3 = Access.findShellWithTitle("Add Inifile Keys");
        shell3.findButtonWithLabel("Parameter name only.*").selectWithMouseClick();
        sleep(1);
        showMessage("We want to specify only parameters without default value defined in the NED files", 2);
        shell3.findButtonWithLabel("Skip parameters that have a default value").selectWithMouseClick();
        sleep(1);
        shell3.findTable().findTableItemByContent("\\*\\*\\.serviceTime").ensureChecked(true);
        sleep(2);
        shell3.findButtonWithLabel("OK").selectWithMouseClick();
        sleep(2);

        // add queue parameter values in table
        showMessage("We want to simulate the network with exponential serviceTime with means 1,2 and 3. " +
                "We can specify this running parameter as: exponential(${serviceMean=1..3 step 1})", 4);
        TextAccess cellEditor = tree2.findTreeItemByContent("\\*\\*\\.serviceTime").activateCellEditor(1);
        sleep(2);
        cellEditor.typeOver("exp");
        cellEditor.pressKey(' ', SWT.CTRL);
        sleep(2);
        Access.findContentAssistPopup().chooseWithKeyboard("exponential.*continuous.*");
        cellEditor.pressKey(SWT.BS);
        cellEditor.pressKey(SWT.BS);
        cellEditor.pressKey(SWT.BS);
        cellEditor.pressKey(SWT.BS);
        cellEditor.pressKey(SWT.BS);
        cellEditor.typeIn("${serviceMean=1..3 step 1})\n");
        sleep(3);

        // set event logging file
        showMessage("Turning on the log file generation allows us to analyze the interaction " +
                "between the modules later", 2);
        ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Output Files").click();
        sleep(2);
        TextAccess text = form.findTextAfterLabel("Eventlog file:");
        text.clickAndTypeOver(logFileName);
        sleep(2);

        // set simulation time limit
        showMessage("Specify how long each simulation has to run (in simulation time)", 2);
        ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("General.*").click();
        sleep(2);
        text = form.findTextAfterLabel("Simulation.*");
        text.clickAndTypeOver("20000");

        // set simulation time limit
        ((TreeAccess)form.findControlWithID("CategoryTree")).findTreeItemByContent("Cmdenv.*").click();
        sleep(2);
        form.findLabel("Status frequency.*").click();
        form.findTextAfterLabel("Status frequency.*").typeIn("500");

        sleep(3);

        showMessage("Let's save our new INI file", 2);
        iniEditor.activatePageEditor("Text");
        workbenchWindow.getShell().findToolItemWithTooltip("Save.*").click();
        sleep(2);
    }

    private void createLaunchConfigAndLaunch() {
        showMessage("Now we have a network defined in a NED file and an " +
                "INI file defining the open parameters for the system. " +
                "As a next step we want to run the simulation from the IDE. " +
                "We have to create a Launch Configuration to do this", 4);
        Access.getWorkbenchWindow().getShell().chooseFromMainMenu("Run|Open Run Dialog.*");
        sleep(2);
        ShellAccess shell = Access.findShellWithTitle("Run");

        shell.findTree().findTreeItemByContent("OMNeT\\+\\+ Simulation").click();
        sleep(1);
        shell.findToolItemWithTooltip("New launch configuration").click();
        sleep(1);

        showMessage("The simulation program itself is already compiled and can be found in the 'queueinglib' project", 2);
        CompositeAccess cTabFolder = shell.findCTabFolder();
        cTabFolder.findTextAfterLabel("Simulation Program:").click();
        shell.pressKey(SWT.TAB);
        shell.pressKey(' ');
        sleep(2);
        TreeAccess tree = Access.findShellWithTitle("Select Executable File").findTree();
        tree.pressKey(SWT.ARROW_RIGHT);
        tree.findTreeItemByContent("queueinglib\\.exe").doubleClick();

        showMessage("We can specify also which INI file should be used with the simulation", 2);

        cTabFolder.findTextAfterLabel("Initialization file\\(s\\):").click();
        shell.pressKey(SWT.TAB);
        shell.pressKey(' ');
        sleep(2);
        TreeAccess tree2 = Access.findShellWithTitle("Select INI Files").findTree();
        tree2.findTreeItemByContent(name).click();
        tree2.pressKey(SWT.ARROW_RIGHT);
        sleep(1);
        tree2.findTreeItemByContent("omnetpp\\.ini").doubleClick();

        showMessage("We must also chose a configuration from the INI file", 1);

        ComboAccess combo = cTabFolder.findComboAfterLabel("Configuration name:");
        combo.selectItem("General.*");
        showMessage("We will use the command line environment to run the simulation", 2);
        shell.findButtonWithLabel("Command.*").selectWithMouseClick();

        showMessage("In the INI file we have specified that the simulation should be run with " +
                "30 and 60 jobs and queue service times should be exponential with means 1,2 and 3. " +
                "This would result a total of 6 runs. We want to run each of them so we specify * (all) as a run number ", 5, -50);
        shell.findTextAfterLabel("Run number.*").clickAndTypeOver("*");

        showMessage("This is a Dual Core machine so we will gain speed if more than one simulation " +
                "can run at the same time. We set the maximum number of simulations running at the same time " +
                "to 2", 4,-50);
        shell.pressKey(SWT.TAB);
        shell.pressKey(SWT.DEL);
        shell.pressKey('2');
        showMessage("Now we are ready to execute our simulation batch. Just press run", 2);
        shell.findButtonWithLabel("Run").selectWithMouseClick();
        WorkbenchUtils.ensureViewActivated("General", "Progress.*"); 
        showMessage("And watch the progress of the simulation batch in the progress view", 2);

        Access.sleep(20);
    }

    private void analyseResults() {
        showMessage("We can analyze the generated results now. Let's display the length " +
                "of each queue over the time", 2);
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        workbenchWindow.getShell().chooseFromMainMenu("File|New\tAlt\\+Shift\\+N|Analysis File \\(anf\\)");
        ShellAccess shell1 = Access.findShellWithTitle("New Analysis File");
        shell1.findTree().findTreeItemByContent("demo").click();
        TextAccess text1 = shell1.findTextAfterLabel("File name:");
        text1.clickAndTypeOver("demo");
        shell1.findButtonWithLabel("Finish").selectWithMouseClick();

        // create an analysis file
        showMessage("Add all generated result files to the analysis", 1);
        ScaveEditorAccess scaveEditor = ScaveEditorUtils.findScaveEditor("demo\\.anf");
        InputsPageAccess ip = scaveEditor.ensureInputsPageActive();
        ip.findButtonWithLabel("Wildcard.*").selectWithMouseClick();
        Access.findShellWithTitle("Add files with wildcard").findButtonWithLabel("OK").selectWithMouseClick();

        // browse data
        showMessage("Now we will select all queue length data in run#4", 1);
        BrowseDataPageAccess bdp = scaveEditor.ensureBrowseDataPageActive();
        bdp.ensureVectorsSelected();
        bdp.getRunNameFilter().selectItem("General-4.*");
        bdp.getDataNameFilter().selectItem("length");

        showMessage("Let's select all vectors and plot them on a single chart", 1);
        bdp.click();
        bdp.pressKey('a', SWT.CTRL);
        workbenchWindow.getShell().findToolItemWithTooltip("Plot.*").click();

        // show and play with the chart
        showMessage("We can apply a 'mean' function so we will get a smoothed out version of the charts", 2);
        CompositeAccess chart = (CompositeAccess)scaveEditor.ensureActivePage("Chart: temp1");
        sleep(3);
        ControlAccess canvas = ((ControlAccess)Access.createAccess(chart.findDescendantControl(VectorChart.class)));
        canvas.chooseFromContextMenu("Apply|Mean");
        sleep(3);

        // create a dataset from the chart
        showMessage("We can store this chart as a recipe, so next time when the simulation is re-run " +
                "the chart can be recreated automatically", 3);
        canvas.chooseFromContextMenu("Convert to Dataset.*");
        ShellAccess shell = Access.findShellWithTitle("Create chart template");
        shell.findTextAfterLabel("Dataset name:").typeOver("queue length mean");
        shell.findTextAfterLabel("Chart name:").clickAndTypeOver("queue length");
        shell.findButtonWithLabel("OK").selectWithMouseClick();

//      // manipulate the dataset
        DatasetsAndChartsPageAccess dp = scaveEditor.ensureDatasetsPageActive();

        sleep(3);
        workbenchWindow.getShell().findToolItemWithTooltip("Save.*").click();
        sleep(10);
    }

    private void showSequenceChart() {
        IResource resource = ResourcesPlugin.getWorkspace().getRoot().getProject(name).getFile(logFileName);
        new PersistentResourcePropertyManager(CommonPlugin.PLUGIN_ID).removeProperty(resource, EventLogInput.STATE_PROPERTY);
        new PersistentResourcePropertyManager(SequenceChartPlugin.PLUGIN_ID).removeProperty(resource, SequenceChart.STATE_PROPERTY);
        new PersistentResourcePropertyManager(EventLogTablePlugin.PLUGIN_ID).removeProperty(resource, EventLogTable.STATE_PROPERTY);

        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        ShellAccess workbenchShell = workbenchWindow.getShell();

        showMessage("Take a look at the sequence chart\n", 1);

        TreeAccess tree = WorkbenchUtils.ensureViewActivated("General", "Navigator").getComposite().findTree();
        TreeItemAccess treeItem = tree.findTreeItemByContent(name);
        treeItem.ensureExpanded();
        tree.findTreeItemByContent(logFileName).doubleClick();

        showMessage("Here you can see the initial 60 messages being pushed into one of the queues.\n", 2);

        showMessage("Go to where the first message is first processed by a queue\n", 2);

        EditorPartAccess editorPart = workbenchWindow.findEditorPartByTitle(logFileName);
        SequenceChartAccess sequenceChart = (SequenceChartAccess)Access.createAccess(Access.findDescendantControl(editorPart.getComposite().getControl(), SequenceChart.class));
        Rectangle r = sequenceChart.getAbsoluteBounds();
        sequenceChart.activateContextMenuWithMouseClick(2).activateMenuItemWithMouse("Sending.*cMessage.*").activateMenuItemWithMouse("Goto Consequence.*");
        
        showMessage("Zoom out to see more\n", 1);

        ToolItemAccess toolItem = workbenchShell.findToolItemWithTooltip("Zoom Out");
        for (int i = 0; i < 5; i++)
            toolItem.click();

        showMessage("Switch to linear timeline mode and see the initial messages being sent in zero simulation time\n", 2);
        
        toolItem = workbenchShell.findToolItemWithTooltip("Timeline Mode");
        toolItem.activateDropDownMenu().activateMenuItemWithMouse("Linear");

        sequenceChart.dragMouse(Access.LEFT_MOUSE_BUTTON, 1, r.height / 2, r.width / 4, r.height / 2);

        showMessage("Filter for the first message to see how it goes around and around in the closed network...\n", 2);

        workbenchShell.findToolItemWithTooltip("Filter").click();
        ShellAccess filterShell = Access.findShellWithTitle("Filter event log");
        treeItem = filterShell.findTree().findTreeItemByPath("Message filter/by name");
        treeItem.click();
        treeItem.pressKey(' ');
        TableAccess table = filterShell.findTable();
        table.findTableItemByContent("source 1").click();
        table.pressKey(' ');
        filterShell.findButtonWithLabel("OK").selectWithMouseClick();

        sleep(5);
        showMessage("Switch to non linear to see the message going around several times at once\n", 2);

        toolItem = workbenchShell.findToolItemWithTooltip("Timeline Mode");
        toolItem.activateDropDownMenu().activateMenuItemWithMouse("Nonlinear");

        toolItem = workbenchShell.findToolItemWithTooltip("Zoom Out");
        for (int i = 0; i < 3; i++)
            toolItem.click();

        sequenceChart.dragMouse(Access.LEFT_MOUSE_BUTTON, r.width - 1, r.height / 2, 1, r.height / 2);
        
        showMessage("Show where message objects are reused by resending them\n", 2);

        sequenceChart.activateContextMenuWithMouseClick().activateMenuItemWithMouse("Show Reuse.*");

        sleep(3);
    }
}
