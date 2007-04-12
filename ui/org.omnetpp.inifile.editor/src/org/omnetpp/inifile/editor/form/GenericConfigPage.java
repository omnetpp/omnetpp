package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_ANIMATION_ENABLED;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_ANIMATION_MSGCLASSNAMES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_ANIMATION_MSGCOLORS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_ANIMATION_MSGNAMES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_ANIMATION_SPEED;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_ANIM_METHODCALLS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_AUTOFLUSH;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_EXTRA_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CONFIGURATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CPU_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DEBUG_ON_ERRORS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_DEFAULT_RUN;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EVENTLOG_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_EVENT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_EVENT_BANNER_DETAILS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_EXPRESSMODE_AUTOUPDATE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_EXPRESS_MODE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_FINGERPRINT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_FNAME_APPEND_HOST;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_IMAGE_PATH;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_INI_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_LOAD_LIBS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_MESSAGE_TRACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_METHODCALLS_DELAY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_MODULE_MESSAGES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_NEXT_EVENT_MARKERS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NUM_RNGS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUTSCALARMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUTVECTORMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_OUTPUT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUT_SCALAR_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUT_SCALAR_PRECISION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUT_VECTORS_MEMORY_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUT_VECTOR_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUT_VECTOR_PRECISION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARALLEL_SIMULATION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_COMMUNICATIONS_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_DEBUG;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_FILECOMM_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_FILECOMM_PRESERVE_READ;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_FILECOMM_READ_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_NAMEDPIPECOMM_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PARSIM_SYNCHRONIZATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_PENGUIN_MODE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_PERFORMANCE_DISPLAY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PERFORM_GC;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_PLUGIN_PATH;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_PRINT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRINT_UNDISPOSED;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_REALTIMESCHEDULER_SCALING;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_RNG_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_RUNS_TO_EXECUTE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SCHEDULER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_SENDDIRECT_ARROWS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_SHOW_BUBBLES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_SHOW_LAYOUTING;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SIMTIME_SCALE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SIM_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_SLOWEXEC_DELAY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SNAPSHOTMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SNAPSHOT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_STATUS_FREQUENCY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_EXTRA_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TOTAL_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_UPDATE_FREQ_EXPRESS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_UPDATE_FREQ_FAST;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_USE_MAINWINDOW;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_USE_NEW_LAYOUTER;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_WARNINGS;

import java.util.ArrayList;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Makeshift form page, just throws in all field editors.
 * TODO design proper forms instead, and throw this out
 * 
 * @author Andras
 */
public class GenericConfigPage extends FormPage {
	private static Font titleFont = new Font(null, "Arial", 10, SWT.BOLD);
	private ArrayList<FieldEditor> fieldEditors = new ArrayList<FieldEditor>();
	
    public static final String CAT_GENERAL = "General";
    public static final String CAT_ADVANCED = "Advanced";
    public static final String CAT_RANDOMNUMBERS = "Random Numbers";
    public static final String CAT_OUTPUTFILES = "Output Files";
    public static final String CAT_CMDENV = "Cmdenv";
    public static final String CAT_TKENV = "Tkenv";
    public static final String CAT_EXTENSIONS = "Extensions";
    public static final String CAT_PARSIM = "Parallel Simulation";
    //public static final String CAT_OUTPUTVECTORS = "Output Vectors";

    public static String[] getCategoryNames() {
    	return new String[] {
    			CAT_GENERAL, 
    			CAT_ADVANCED,
    			CAT_RANDOMNUMBERS,
    			CAT_OUTPUTFILES,
    			CAT_CMDENV,
    			CAT_TKENV,
    			CAT_EXTENSIONS,
    			CAT_PARSIM,
    	};
    }
    
	public GenericConfigPage(Composite parent, String category, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		GridLayout gridLayout = new GridLayout(1,false);
		gridLayout.verticalSpacing = 0;
		setLayout(gridLayout);
		
		createTitle(category);
		addSpacer();

		// populate with field editors
		IInifileDocument doc = getInifileDocument();
		if (category.equals(CAT_GENERAL)) {
			addField(new TextFieldEditor(this, CFGID_NETWORK, doc, "Network to simulate"));
			addSpacer();
			Group group1 = createGroup(this, "Setup");
			addField(new TextFieldEditor(group1, CFGID_PRELOAD_NED_FILES, doc, "NED files to load"));
			addField(new TextFieldEditor(group1, CFGID_USER_INTERFACE, doc, "User interface"));
			addSpacer();
			Group group2 = createGroup(this, "Stopping condition");
			addField(new TextFieldEditor(group2, CFGID_SIM_TIME_LIMIT, doc, "Simulation time limit"));
			addField(new TextFieldEditor(group2, CFGID_CPU_TIME_LIMIT, doc, "CPU time limit"));
			//addField(new TextFieldEditor(this, SWT.NONE, CFGID_DESCRIPTION, doc, "Description")); -- should go into the Run editor
		}
		else if (category.equals(CAT_ADVANCED)) {
			Group group1 = createGroup(this, "Debugging");
			addField(new CheckboxFieldEditor(group1, CFGID_DEBUG_ON_ERRORS, doc, "Debug on errors"));
			addField(new CheckboxFieldEditor(group1, CFGID_PRINT_UNDISPOSED, doc, "Dump names of undisposed objects"));
			addSpacer();
			Group group2 = createGroup(this, "Warnings");
			addField(new CheckboxFieldEditor(group2, CFGID_WARNINGS, doc, "Warnings")); //XXX
			addField(new CheckboxFieldEditor(group2, CFGID_INI_WARNINGS, doc, "Ini warnings")); //XXX
			addSpacer();
			addField(new TextFieldEditor(this, CFGID_SIMTIME_SCALE, doc, "Simtime scale exponent"));
			addField(new TextFieldEditor(this, CFGID_TOTAL_STACK_KB, doc, "Total activity() stack (Kb)"));
			addField(new TextFieldEditor(this, CFGID_FINGERPRINT, doc, "Fingerprint to verify"));
			addField(new CheckboxFieldEditor(this, CFGID_PERFORM_GC, doc, "Delete leaked objects on network cleanup"));
		}
		else if (category.equals(CAT_RANDOMNUMBERS)) {
			Group group1 = createGroup(this, "Random Number Generators");
			addField(new TextFieldEditor(group1, CFGID_NUM_RNGS, doc, "Number of RNGs"));
			addField(new TextFieldEditor(group1, CFGID_RNG_CLASS, doc, "RNG class"));
		}
		else if (category.equals(CAT_OUTPUTFILES)) {
			Group group1 = createGroup(this, "Result collection");
			addField(new TextFieldEditor(group1, CFGID_OUTPUT_VECTOR_FILE, doc, "Output vector file"));
			addField(new TextFieldEditor(group1, CFGID_OUTPUT_VECTOR_PRECISION, doc, "Output vector precision"));
			addField(new TextFieldEditor(group1, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, doc, "Output vectors memory limit"));
			addField(new TextFieldEditor(group1, CFGID_OUTPUT_SCALAR_FILE, doc, "Output scalar file"));
			addField(new TextFieldEditor(group1, CFGID_OUTPUT_SCALAR_PRECISION, doc, "Output scalar precision"));
			addSpacer();
			addField(new TextFieldEditor(this, CFGID_SNAPSHOT_FILE, doc, "Snapshot file"));
			addField(new TextFieldEditor(this, CFGID_EVENTLOG_FILE, doc, "Eventlog file"));
			addField(new CheckboxFieldEditor(this, CFGID_FNAME_APPEND_HOST, doc, "Append host name to filenames"));
		}
		else if (category.equals(CAT_EXTENSIONS)) {
			addField(new TextFieldEditor(this, CFGID_LOAD_LIBS, doc, "Shared libraries to load"));
			addSpacer();
			Group group1 = createGroup(this, "Extension classes");
			addField(new TextFieldEditor(group1, CFGID_CONFIGURATION_CLASS, doc, "Configuration class"));
			addField(new TextFieldEditor(group1, CFGID_SCHEDULER_CLASS, doc, "Scheduler class"));
			addField(new TextFieldEditor(group1, CFGID_REALTIMESCHEDULER_SCALING, doc, "Real-Time scheduler scaling"));
			addField(new TextFieldEditor(group1, CFGID_OUTPUTVECTORMANAGER_CLASS, doc, "Output vector manager class"));
			addField(new TextFieldEditor(group1, CFGID_OUTPUTSCALARMANAGER_CLASS, doc, "Output scalar manager class"));
			addField(new TextFieldEditor(group1, CFGID_SNAPSHOTMANAGER_CLASS, doc, "Snapshot manager class"));
		}
		else if (category.equals(CAT_CMDENV)) {
			addField(new TextFieldEditor(this, CFGID_CMDENV_RUNS_TO_EXECUTE, doc, "Runs to execute"));
			addField(new CheckboxFieldEditor(this, CFGID_CMDENV_EXPRESS_MODE, doc, "Run in Express mode"));
			addSpacer();
			Group group1 = createGroup(this, "Express mode");
			addField(new CheckboxFieldEditor(group1, CFGID_CMDENV_PERFORMANCE_DISPLAY, doc, "Display performance data"));
			addField(new TextFieldEditor(group1, CFGID_CMDENV_STATUS_FREQUENCY, doc, "Status frequency"));
			addSpacer();
			Group group2 = createGroup(this, "Normal (non-Express) mode");
			addField(new CheckboxFieldEditor(group2, CFGID_CMDENV_MODULE_MESSAGES, doc, "Print module messages"));
			addField(new CheckboxFieldEditor(group2, CFGID_CMDENV_EVENT_BANNERS, doc, "Print event banners"));
			addField(new CheckboxFieldEditor(group2, CFGID_CMDENV_EVENT_BANNER_DETAILS, doc, "Detailed event banners"));
			addField(new CheckboxFieldEditor(group2, CFGID_CMDENV_MESSAGE_TRACE, doc, "Message trace"));
			addSpacer();
			addField(new TextFieldEditor(this, CFGID_CMDENV_EXTRA_STACK_KB, doc, "Cmdenv extra stack (Kb)"));
			addField(new TextFieldEditor(this, CFGID_CMDENV_OUTPUT_FILE, doc, "Log file"));
			addField(new CheckboxFieldEditor(this, CFGID_CMDENV_AUTOFLUSH, doc, "Autoflush output files"));
		}
		else if (category.equals(CAT_TKENV)) {
			addField(new TextFieldEditor(this, CFGID_TKENV_DEFAULT_RUN, doc, "Default run"));
			addSpacer();
			Group group4 = createGroup(this, "Paths");
			addField(new TextFieldEditor(group4, CFGID_TKENV_IMAGE_PATH, doc, "Image path"));
			addField(new TextFieldEditor(group4, CFGID_TKENV_PLUGIN_PATH, doc, "Plugin path"));
			addSpacer();
			addField(new TextFieldEditor(this, CFGID_TKENV_EXTRA_STACK_KB, doc, "Tkenv extra stack (Kb)"));
			addSpacer();
			addMessage(ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING),
					"NOTE: The following settings are only first-time defaults, and settings in .tkenvrc (created by Tkenv\n" +
					"automatically) will override them. Delete .tkenvrc for these settings to take effect.");
			addSpacer();
			Group group5 = createGroup(this, "Execution");
			addField(new TextFieldEditor(group5, CFGID_TKENV_SLOWEXEC_DELAY, doc, "Slow-exec delay"));
			addField(new TextFieldEditor(group5, CFGID_TKENV_UPDATE_FREQ_FAST, doc, "Update frequency for Fast mode"));
			addField(new TextFieldEditor(group5, CFGID_TKENV_UPDATE_FREQ_EXPRESS, doc, "Update frequency for Express mode"));
			addField(new CheckboxFieldEditor(group5, CFGID_TKENV_EXPRESSMODE_AUTOUPDATE, doc, "Auto-update in Express mode"));
			addSpacer();
			Group group1 = createGroup(this, "Animation options");
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_ENABLED, doc, "Enable animation"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_NEXT_EVENT_MARKERS, doc, "Show next event markers"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_SENDDIRECT_ARROWS, doc, "Show SendDirect arrows"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_ANIM_METHODCALLS, doc, "Animate method calls"));
			addField(new TextFieldEditor(group1, CFGID_TKENV_METHODCALLS_DELAY, doc, "Delay for method call animation"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_MSGNAMES, doc, "Show message names in animation"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_MSGCLASSNAMES, doc, "Show message class names in animation"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_MSGCOLORS, doc, "Colorize messages by message kind"));
			addField(new CheckboxFieldEditor(this, CFGID_TKENV_SHOW_BUBBLES, doc, "Show bubbles"));
			addField(new CheckboxFieldEditor(group1, CFGID_TKENV_PENGUIN_MODE, doc, "Penguin mode"));
			addSpacer();
			Group group2 = createGroup(this, "Network layouting");
			addField(new CheckboxFieldEditor(group2, CFGID_TKENV_SHOW_LAYOUTING, doc, "Show layouting process"));
			addField(new CheckboxFieldEditor(group2, CFGID_TKENV_USE_NEW_LAYOUTER, doc, "Use new layouter"));
			addField(new TextFieldEditor(this, CFGID_TKENV_ANIMATION_SPEED, doc, "Animation speed"));
			addSpacer();
			Group group3 = createGroup(this, "Logging");
			addField(new CheckboxFieldEditor(group3, CFGID_TKENV_PRINT_BANNERS, doc, "Print banners"));
			addField(new CheckboxFieldEditor(group3, CFGID_TKENV_USE_MAINWINDOW, doc, "Use main window"));
		}
		else if (category.equals(CAT_PARSIM)) {
			Group group1 = createGroup(this, "General");
			addField(new CheckboxFieldEditor(group1, CFGID_PARALLEL_SIMULATION, doc, "Parallel simulation"));
			addField(new TextFieldEditor(group1, CFGID_PARSIM_COMMUNICATIONS_CLASS, doc, "Communications class"));
			addField(new TextFieldEditor(group1, CFGID_PARSIM_SYNCHRONIZATION_CLASS, doc, "Synchronization class"));
			addField(new CheckboxFieldEditor(group1, CFGID_PARSIM_DEBUG, doc, "Debug parallel simulation"));
			addSpacer();
			Group group2 = createGroup(this, "Communications");
			addField(new TextFieldEditor(group2, CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, doc, "MPI communications: MPI buffer"));
			addField(new TextFieldEditor(group2, CFGID_PARSIM_NAMEDPIPECOMM_PREFIX, doc, "Named Pipe communications: prefix"));
			addField(new TextFieldEditor(group2, CFGID_PARSIM_FILECOMM_PREFIX, doc, "File-based communications: prefix"));
			addField(new TextFieldEditor(group2, CFGID_PARSIM_FILECOMM_READ_PREFIX, doc, "File-based communications: read prefix"));
			addField(new CheckboxFieldEditor(group2, CFGID_PARSIM_FILECOMM_PRESERVE_READ, doc, "File-based communications: preserve read files"));
			addSpacer();
			Group group3 = createGroup(this, "Protocol-specific settings");
			addField(new TextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, doc, "Null Message Protocol: lookahead class"));
			addField(new TextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, doc, "Null Message Protocol: laziness"));
			addField(new TextFieldEditor(group3, CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, doc, "Ideal Simulation Protocol: table size"));
		}
		else {
			throw new IllegalArgumentException("no such category: "+category);
		}
		
	}

	private void createTitle(String category) {
		// title area (XXX a bit ugly -- re-think layout)
		Composite titleArea = new Composite(this, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 40;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.asColor("white"));
		titleArea.setLayout(new GridLayout(2, false));
		
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER)); //XXX 
		imageLabel.setBackground(ColorFactory.asColor("white"));
		
		Label title = new Label(titleArea, SWT.NONE);
		title.setText(category);
		title.setFont(titleFont);
		title.setBackground(ColorFactory.asColor("white"));
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
	}

	private void addMessage(Image image, String text) {
		Composite warnPanel = new Composite(this, SWT.NONE);
		warnPanel.setLayout(new GridLayout(2,false));
		Label warnImageLabel = new Label(warnPanel, SWT.NONE);
		warnImageLabel.setImage(image); //XXX 
		Label warnLabel = new Label(warnPanel, SWT.NONE);
		warnLabel.setText(text);
	}

	private void addSpacer() {
		new Label(this, SWT.NONE);
	}

	private Group createGroup(Composite parent, String groupLabel) {
		Group group = new Group(this, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setText(groupLabel);

		GridLayout gridLayout = new GridLayout(1, false);
		gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 5;
		group.setLayout(gridLayout);
		return group;
	}
	
	private void addField(FieldEditor editor) {
		fieldEditors.add(editor);		
		editor.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
	}

	@Override
	public void reread() {
		for (FieldEditor e : fieldEditors)
			e.reread();
	}

	@Override
	public void commit() {
		for (FieldEditor e : fieldEditors)
			e.commit();
	}
}
