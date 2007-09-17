package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_AUTOFLUSH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_CONFIG_NAME;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EVENT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EVENT_BANNER_DETAILS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EXPRESS_MODE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EXTRA_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_MESSAGE_TRACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_MODULE_MESSAGES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_OUTPUT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_PERFORMANCE_DISPLAY;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_RUNS_TO_EXECUTE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_STATUS_FREQUENCY;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONFIGURATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONSTRAINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CPU_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUG_ON_ERRORS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXPERIMENT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FINGERPRINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FNAME_APPEND_HOST;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_INI_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_LOAD_LIBS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MEASUREMENT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NUM_RNGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUTSCALARMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUTVECTORMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_SCALAR_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_SCALAR_FILE_APPEND;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_SCALAR_PRECISION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_VECTORS_MEMORY_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_VECTOR_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_VECTOR_PRECISION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARALLEL_SIMULATION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_COMMUNICATIONS_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_DEBUG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMM_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMM_PRESERVE_READ;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMM_READ_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NAMEDPIPECOMM_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_SYNCHRONIZATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PERFORM_GC;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NED_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PRINT_UNDISPOSED;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REALTIMESCHEDULER_SCALING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REPEAT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REPLICATION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RNG_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SCHEDULER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SEED_SET;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIMTIME_SCALE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIM_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SNAPSHOTMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SNAPSHOT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_ANIMATION_ENABLED;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_ANIMATION_MSGCLASSNAMES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_ANIMATION_MSGCOLORS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_ANIMATION_MSGNAMES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_ANIMATION_SPEED;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_ANIM_METHODCALLS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_DEFAULT_CONFIG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_DEFAULT_RUN;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_EXPRESSMODE_AUTOUPDATE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_EXTRA_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_IMAGE_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_METHODCALLS_DELAY;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_NEXT_EVENT_MARKERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_PENGUIN_MODE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_PLUGIN_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_PRINT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_SENDDIRECT_ARROWS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_SHOW_BUBBLES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_SHOW_LAYOUTING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_SLOWEXEC_DELAY;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_UPDATE_FREQ_EXPRESS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_UPDATE_FREQ_FAST;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_USE_MAINWINDOW;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_USE_NEW_LAYOUTER;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TOTAL_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Form page for configuration entries.
 * 
 * @author Andras
 */
public class GenericConfigPage extends ScrolledFormPage {
	private ArrayList<FieldEditor> fieldEditors = new ArrayList<FieldEditor>();
	private String category;
	
    public static final String CAT_GENERAL = "General";
    public static final String CAT_ADVANCED = "Advanced";
    public static final String CAT_SCENARIO = "Scenarios";
    public static final String CAT_RANDOMNUMBERS = "Random Numbers";
    public static final String CAT_OUTPUTFILES = "Output Files";
    public static final String CAT_CMDENV = "Cmdenv";
    public static final String CAT_TKENV = "Tkenv";
    public static final String CAT_EXTENSIONS = "Extensions";
    public static final String CAT_PARSIM = "Parallel Simulation";
    //XXX public static final String CAT_OUTPUTVECTORS = "Output Vectors";

	public static final Image ICON_WARNING = InifileEditorPlugin.getCachedImage("icons/full/obj16/Warning.png"); //XXX
    
    public static String[] getCategoryNames() {
    	return new String[] {
    			CAT_GENERAL, 
    			CAT_ADVANCED,
    			CAT_SCENARIO,
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
		this.category = category;
		
		GridLayout gridLayout = new GridLayout(1,false);
		gridLayout.verticalSpacing = 0;
		form.setLayout(gridLayout);
		
		createTitleArea(form, category);
		addSpacer(form);

		createFieldEditors(form, category);
		layoutForm();
		
		// obtain focus if background is clicked
		form.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				setFocus();
			}
		});
	}

	private void createFieldEditors(Composite form, String category) {
		if (category.equals(CAT_GENERAL)) {
			addTextFieldEditor(form, CFGID_NETWORK, "Network to simulate");
			addSpacer(form);
			Group group1 = createGroup(form, "Setup");
			addTextFieldEditor(group1, CFGID_NED_PATH, "NED file path");
			addTextFieldEditor(group1, CFGID_USER_INTERFACE, "User interface");
			addSpacer(form);
			Group group2 = createGroup(form, "Stopping condition");
			addTextFieldEditor(group2, CFGID_SIM_TIME_LIMIT, "Simulation time limit");
			addTextFieldEditor(group2, CFGID_CPU_TIME_LIMIT, "CPU time limit");
		}
		else if (category.equals(CAT_ADVANCED)) {
			Group group1 = createGroup(form, "Debugging");
			addCheckboxFieldEditor(group1, CFGID_DEBUG_ON_ERRORS, "Debug on errors");
			addCheckboxFieldEditor(group1, CFGID_PRINT_UNDISPOSED, "Dump names of undisposed objects");
			addSpacer(form);
			Group group2 = createGroup(form, "Warnings");
			addCheckboxFieldEditor(group2, CFGID_WARNINGS, "Warnings"); //XXX
			addCheckboxFieldEditor(group2, CFGID_INI_WARNINGS, "Ini warnings"); //XXX
			addSpacer(form);
			addTextFieldEditor(form, CFGID_SIMTIME_SCALE, "Simtime scale exponent");
			addTextFieldEditor(form, CFGID_TOTAL_STACK_KB, "Total activity() stack (Kb)");
			addTextFieldEditor(form, CFGID_FINGERPRINT, "Fingerprint to verify");
			addCheckboxFieldEditor(form, CFGID_PERFORM_GC, "Delete leaked objects on network cleanup");
		}
		else if (category.equals(CAT_RANDOMNUMBERS)) {
			Group group1 = createGroup(form, "Random Number Generators");
			addTextFieldEditor(group1, CFGID_NUM_RNGS, "Number of RNGs");
			addTextFieldEditor(group1, CFGID_RNG_CLASS, "RNG class");
			addSpacer(form);
			Group group2 = createGroup(form, "Automatic Seeds");
			addTextFieldEditor(group2, CFGID_SEED_SET, "Seed set");
			//Group group3 = createGroup(form, "Manual Seeds");
			//XXX todo: seed-%-mt, seed-%-mt-p%, seed-%-lcg32
		}
		else if (category.equals(CAT_SCENARIO)) {
			Group group1 = createGroup(form, "Run labeling");
			addTextFieldEditor(group1, CFGID_EXPERIMENT, "Experiment label");
			addTextFieldEditor(group1, CFGID_MEASUREMENT, "Measurement label");
			addTextFieldEditor(group1, CFGID_REPLICATION, "Replication label");
			addSpacer(form);
			Group group2 = createGroup(form, "Scenario generation");
			addTextFieldEditor(group2, CFGID_REPEAT, "Repeat count");
			addTextFieldEditor(group2, CFGID_CONSTRAINT, "Constraint");
		}
		else if (category.equals(CAT_OUTPUTFILES)) {
			Group group1 = createGroup(form, "Result collection");
			addTextFieldEditor(group1, CFGID_OUTPUT_VECTOR_FILE, "Output vector file");
			addTextFieldEditor(group1, CFGID_OUTPUT_VECTOR_PRECISION, "Output vector precision");
			addTextFieldEditor(group1, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, "Output vectors memory limit");
			addSpacer(group1);
			addTextFieldEditor(group1, CFGID_OUTPUT_SCALAR_FILE, "Output scalar file");
			addTextFieldEditor(group1, CFGID_OUTPUT_SCALAR_PRECISION, "Output scalar precision");
			addCheckboxFieldEditor(group1, CFGID_OUTPUT_SCALAR_FILE_APPEND, "Append to existing scalar file");
			addSpacer(form);
			addTextFieldEditor(form, CFGID_SNAPSHOT_FILE, "Snapshot file");
			addTextFieldEditor(form, CFGID_EVENTLOG_FILE, "Eventlog file");
			addTextFieldEditor(form, CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "Details to record");
			addSpacer(form);
			addCheckboxFieldEditor(form, CFGID_FNAME_APPEND_HOST, "Append host name to filenames");
		}
		else if (category.equals(CAT_EXTENSIONS)) {
			addTextFieldEditor(form, CFGID_LOAD_LIBS, "Shared libraries to load");
			addSpacer(form);
			Group group1 = createGroup(form, "Extension classes");
			addTextFieldEditor(group1, CFGID_CONFIGURATION_CLASS, "Configuration class");
			addTextFieldEditor(group1, CFGID_SCHEDULER_CLASS, "Scheduler class");
			addTextFieldEditor(group1, CFGID_REALTIMESCHEDULER_SCALING, "Real-Time scheduler scaling");
			addTextFieldEditor(group1, CFGID_OUTPUTVECTORMANAGER_CLASS, "Output vector manager class");
			addTextFieldEditor(group1, CFGID_OUTPUTSCALARMANAGER_CLASS, "Output scalar manager class");
			addTextFieldEditor(group1, CFGID_SNAPSHOTMANAGER_CLASS, "Snapshot manager class");
		}
		else if (category.equals(CAT_CMDENV)) {
			Group group0 = createGroup(form, "When no command-line options are present, run the following:");
			addTextFieldEditor(group0, CFGID_CMDENV_CONFIG_NAME, "Config name");
			addTextFieldEditor(group0, CFGID_CMDENV_RUNS_TO_EXECUTE, "Run numbers");
			addSpacer(form);
			addCheckboxFieldEditor(form, CFGID_CMDENV_EXPRESS_MODE, "Run in Express mode");
			addSpacer(form);
			Group group1 = createGroup(form, "Express mode");
			addCheckboxFieldEditor(group1, CFGID_CMDENV_PERFORMANCE_DISPLAY, "Display performance data");
			addTextFieldEditor(group1, CFGID_CMDENV_STATUS_FREQUENCY, "Status frequency");
			addSpacer(form);
			Group group2 = createGroup(form, "Normal (non-Express) mode");
			addCheckboxFieldEditor(group2, CFGID_CMDENV_MODULE_MESSAGES, "Print module messages");
			addCheckboxFieldEditor(group2, CFGID_CMDENV_EVENT_BANNERS, "Print event banners");
			addCheckboxFieldEditor(group2, CFGID_CMDENV_EVENT_BANNER_DETAILS, "Detailed event banners");
			addCheckboxFieldEditor(group2, CFGID_CMDENV_MESSAGE_TRACE, "Message trace");
			addSpacer(form);
			addTextFieldEditor(form, CFGID_CMDENV_EXTRA_STACK_KB, "Cmdenv extra stack (Kb)");
			addTextFieldEditor(form, CFGID_CMDENV_OUTPUT_FILE, "Redirect stdout to file");
			addCheckboxFieldEditor(form, CFGID_CMDENV_AUTOFLUSH, "Auto-flush output files");
		}
		else if (category.equals(CAT_TKENV)) {
			Group group0 = createGroup(form, "On startup, set up the following simulation:");
			addTextFieldEditor(group0, CFGID_TKENV_DEFAULT_CONFIG, "Config name");
			addTextFieldEditor(group0, CFGID_TKENV_DEFAULT_RUN, "Run number");
			addSpacer(form);
			Group group4 = createGroup(form, "Paths");
			addTextFieldEditor(group4, CFGID_TKENV_IMAGE_PATH, "Image path");
			addTextFieldEditor(group4, CFGID_TKENV_PLUGIN_PATH, "Plugin path");
			addSpacer(form);
			addTextFieldEditor(form, CFGID_TKENV_EXTRA_STACK_KB, "Tkenv extra stack (Kb)");
			addSpacer(form);
			addMessage(form, ICON_WARNING,
					"NOTE: The following settings are only first-time defaults, and settings in .tkenvrc (created by Tkenv\n" +
					"automatically) will override them. Delete .tkenvrc for these settings to take effect.");
			addSpacer(form);
			Group group5 = createGroup(form, "Execution");
			addTextFieldEditor(group5, CFGID_TKENV_ANIMATION_SPEED, "Animation speed");
			addCheckboxFieldEditor(group5, CFGID_TKENV_EXPRESSMODE_AUTOUPDATE, "Auto-update in Express mode");
			addTextFieldEditor(group5, CFGID_TKENV_SLOWEXEC_DELAY, "Slow-exec delay");
			addTextFieldEditor(group5, CFGID_TKENV_UPDATE_FREQ_FAST, "Update frequency for Fast mode");
			addTextFieldEditor(group5, CFGID_TKENV_UPDATE_FREQ_EXPRESS, "Update frequency for Express mode");
			addSpacer(form);
			Group group1 = createGroup(form, "Animation options");
			addCheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_ENABLED, "Enable animation");
			addCheckboxFieldEditor(group1, CFGID_TKENV_NEXT_EVENT_MARKERS, "Show next event markers");
			addCheckboxFieldEditor(group1, CFGID_TKENV_SENDDIRECT_ARROWS, "Show SendDirect arrows");
			addCheckboxFieldEditor(group1, CFGID_TKENV_ANIM_METHODCALLS, "Animate method calls");
			addTextFieldEditor(group1, CFGID_TKENV_METHODCALLS_DELAY, "Delay for method call animation");
			addCheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_MSGNAMES, "Show message names in animation");
			addCheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_MSGCLASSNAMES, "Show message class names in animation");
			addCheckboxFieldEditor(group1, CFGID_TKENV_ANIMATION_MSGCOLORS, "Colorize messages by message kind");
			addCheckboxFieldEditor(group1, CFGID_TKENV_SHOW_BUBBLES, "Show bubbles");
			addCheckboxFieldEditor(group1, CFGID_TKENV_PENGUIN_MODE, "Penguin mode");
			addSpacer(form);
			Group group2 = createGroup(form, "Network layouting");
			addCheckboxFieldEditor(group2, CFGID_TKENV_SHOW_LAYOUTING, "Show layouting process");
			addCheckboxFieldEditor(group2, CFGID_TKENV_USE_NEW_LAYOUTER, "Use new layouter");
			addSpacer(form);
			Group group3 = createGroup(form, "Logging");
			addCheckboxFieldEditor(group3, CFGID_TKENV_PRINT_BANNERS, "Print banners");
			addCheckboxFieldEditor(group3, CFGID_TKENV_USE_MAINWINDOW, "Use main window");
		}
		else if (category.equals(CAT_PARSIM)) {
			addCheckboxFieldEditor(form, CFGID_PARALLEL_SIMULATION, "Enable parallel simulation");
			Group group1 = createGroup(form, "General");
			addTextFieldEditor(group1, CFGID_PARSIM_COMMUNICATIONS_CLASS, "Communications class");
			addTextFieldEditor(group1, CFGID_PARSIM_SYNCHRONIZATION_CLASS, "Synchronization class");
			addCheckboxFieldEditor(group1, CFGID_PARSIM_DEBUG, "Debug parallel simulation");
			addSpacer(form);
			Group group2 = createGroup(form, "Communications");
			addTextFieldEditor(group2, CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, "MPI communications: MPI buffer");
			addTextFieldEditor(group2, CFGID_PARSIM_NAMEDPIPECOMM_PREFIX, "Named Pipe communications: prefix");
			addTextFieldEditor(group2, CFGID_PARSIM_FILECOMM_PREFIX, "File-based communications: prefix");
			addTextFieldEditor(group2, CFGID_PARSIM_FILECOMM_READ_PREFIX, "File-based communications: read prefix");
			addCheckboxFieldEditor(group2, CFGID_PARSIM_FILECOMM_PRESERVE_READ, "File-based communications: preserve read files");
			addSpacer(form);
			Group group3 = createGroup(form, "Protocol-specific settings");
			addTextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, "Null Message Protocol: lookahead class");
			addTextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, "Null Message Protocol: laziness");
			addTextFieldEditor(group3, CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, "Ideal Simulation Protocol: table size");
		}
		else {
			throw new IllegalArgumentException("no such category: "+category);
		}
	}

	private void addMessage(Composite parent, Image image, String text) {
		Composite warnPanel = new Composite(parent, SWT.NONE);
		warnPanel.setLayout(new GridLayout(2,false));
		Label warnImageLabel = new Label(warnPanel, SWT.NONE);
		warnImageLabel.setImage(image); //XXX 
		Label warnLabel = new Label(warnPanel, SWT.NONE);
		warnLabel.setText(text);
	}

	private void addSpacer(Composite parent) {
		new Label(parent, SWT.NONE);
	}

	private Group createGroup(Composite parent, String groupLabel) {
		final Group group = new Group(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setText(groupLabel);

		GridLayout gridLayout = new GridLayout(1, false);
		gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 5;
		group.setLayout(gridLayout);
		
		// when group background is clicked, transfer focus to first field editor in it
		group.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				for (Control c : group.getChildren())
					if (c instanceof FieldEditor)
						{c.setFocus(); return;}
			}
		});
		
		return group;
	}

	protected void addTextFieldEditor(Composite parent, ConfigKey e, String label) {
		FieldEditor editor = e.isGlobal() ? 
				new TextFieldEditor(parent, e, getInifileDocument(), this, label) :
				new ExpandableTextFieldEditor(parent, e, getInifileDocument(), this, label);
		addFieldEditor(editor);		
	}

	protected void addCheckboxFieldEditor(Composite parent, ConfigKey e, String label) {
		FieldEditor editor = e.isGlobal() ? 
				new CheckboxFieldEditor(parent, e, getInifileDocument(), this, label) :
				new ExpandableCheckboxFieldEditor(parent, e, getInifileDocument(), this, label);
		addFieldEditor(editor);		
	}

	protected void addFieldEditor(FieldEditor editor) {
		fieldEditors.add(editor);		
		editor.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
	}

	protected boolean occursOutsideGeneral(String key) {
		IInifileDocument doc = getInifileDocument();
		for (String section : doc.getSectionNames())
			if (doc.containsKey(section, key) && !section.equals(GENERAL))
				return true;
		return false;
	}
	
	@Override
	public String getPageCategory() {
		return category;
	}
	
	@Override
	public boolean setFocus() {
		return fieldEditors.isEmpty() ? false : fieldEditors.get(0).setFocus();
	}

	@Override
	public void reread() {
		super.reread();
		for (FieldEditor e : fieldEditors)
			e.reread();
	}
	
	@Override
	public List<ConfigKey> getSupportedKeys() {
		List<ConfigKey> result = new ArrayList<ConfigKey>();
		for (FieldEditor e : fieldEditors)
			result.add(e.getConfigKey());
		return result;
	}
	
}
