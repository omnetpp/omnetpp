package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_AUTOFLUSH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_CONFIG_NAME;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EVENT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EVENT_BANNER_DETAILS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EV_OUTPUT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EXPRESS_MODE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EXTRA_STACK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_INTERACTIVE;
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
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_RECORDING_INTERVALS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXPERIMENT_LABEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FINGERPRINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FNAME_APPEND_HOST;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_INI_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_LOAD_LIBS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_MAX_BUFFERED_VALUES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MAX_MODULE_NESTING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MEASUREMENT_LABEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NED_PATH;
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
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_SYNCHRONIZATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARTITION_ID;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PERFORM_GC;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PRINT_UNDISPOSED;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REALTIMESCHEDULER_SCALING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING_INTERVAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RECORD_EVENTLOG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORD_EVENTNUMBERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MODULE_EVENTLOG_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SCALAR_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REPEAT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REPLICATION_LABEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RESULT_DIR;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RNG_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARAM_RECORD_AS_SCALAR;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SCHEDULER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SEED_SET;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIMTIME_SCALE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIM_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SNAPSHOTMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SNAPSHOT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_DEFAULT_CONFIG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_DEFAULT_RUN;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_EXTRA_STACK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_IMAGE_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_PLUGIN_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TOTAL_STACK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.ArrayList;
import java.util.Arrays;
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

    public static final String[] SIMTIME_SCALE_CHOICES = new String[] {
        "0", "-3", "-6", "-9", "-12", "-15", "-18",  //TODO add (s),(ms),(us),(ns),(ps),(fs),(as) once combo supports it
    };

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
            Group group1 = createGroup(form, "Network");
			addTextFieldEditor(group1, CFGID_NETWORK, "Network to simulate"); //FIXME use ComboBoxFieldEditor
			addSpacer(form);
			Group group2 = createGroup(form, "Stopping condition");
			addTextFieldEditor(group2, CFGID_SIM_TIME_LIMIT, "Simulation time limit");
            addTextFieldEditor(group2, CFGID_CPU_TIME_LIMIT, "CPU time limit");
			addSpacer(form);
            Group group3 = createGroup(form, "Other");
            addComboboxFieldEditor(group3, CFGID_SIMTIME_SCALE, "Simulation time precision");
            //TODO display extra info: "nanosecond resolution; range: +-100 days"
			addCheckboxFieldEditor(group3, CFGID_DEBUG_ON_ERRORS, "Debug on errors");
            addSpacer(form);
		}
		else if (category.equals(CAT_ADVANCED)) {
		    Group group0 = createGroup(form, "Regression");
		    addTextFieldEditor(group0, CFGID_FINGERPRINT, "Fingerprint to verify");
            addSpacer(form);
		    Group group1 = createGroup(form, "Setup");
            addTextFieldEditor(group1, CFGID_NED_PATH, "NED file path");
            addTextFieldEditor(group1, CFGID_USER_INTERFACE, "User interface");
            addTextFieldEditor(group1, CFGID_TOTAL_STACK, "Total activity stack");
            addTextFieldEditor(group1, CFGID_MAX_MODULE_NESTING, "Allowed maximum module nesting");
            addSpacer(form);
			Group group2 = createGroup(form, "Debugging");
			addCheckboxFieldEditor(group2, CFGID_PRINT_UNDISPOSED, "Dump names of undisposed objects");
			addCheckboxFieldEditor(group2, CFGID_PERFORM_GC, "Delete leaked objects on network cleanup");
			addSpacer(form);
			Group group3 = createGroup(form, "Warnings");
			addCheckboxFieldEditor(group3, CFGID_WARNINGS, "Warnings"); //XXX
			addCheckboxFieldEditor(group3, CFGID_INI_WARNINGS, "Ini warnings"); //XXX
			addSpacer(form);
            Group group4 = createGroup(form, "Output vector recording");
	        addTextFieldEditor(group4, CFGID_OUTPUT_VECTOR_PRECISION, "Precision");
	        addCheckboxFieldEditor(group4, CFGID_VECTOR_RECORD_EVENTNUMBERS, "Record event numbers");
	        addTextFieldEditor(group4, CFGID_VECTOR_MAX_BUFFERED_VALUES, "Buffered size for output vectors");
	        addTextFieldEditor(group4, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, "Total memory limit");
            addSpacer(form);
            Group group5 = createGroup(form, "Output scalar recording");
            addTextFieldEditor(group5, CFGID_OUTPUT_SCALAR_PRECISION, "Precision");
		}
		else if (category.equals(CAT_RANDOMNUMBERS)) {
			Group group1 = createGroup(form, "Random Number Generators");
			addTextFieldEditor(group1, CFGID_NUM_RNGS, "Number of RNGs");
			addTextFieldEditor(group1, CFGID_RNG_CLASS, "RNG class");
			addSpacer(form);
			Group group2 = createGroup(form, "Automatic Seeds");
			addTextFieldEditor(group2, CFGID_SEED_SET, "Seed set");
			//TODO the ones that contain "%":
			//Group group3 = createGroup(form, "Manual Seeds");
            //addTextTableFieldEditor(form, CFGID_RNG_n, "Module RNG mapping");
            //addTextTableFieldEditor(form, CFGID_SEED_n_LCG32, "Seed for LCG32 RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT, "Seed for Mersenne Twister RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT_Pn, "Per-partition Mersenne Twister seeds (for parsim)");
		}
		else if (category.equals(CAT_SCENARIO)) {
			Group group1 = createGroup(form, "Run labeling");
			addTextFieldEditor(group1, CFGID_EXPERIMENT_LABEL, "Experiment label");
			addTextFieldEditor(group1, CFGID_MEASUREMENT_LABEL, "Measurement label");
			addTextFieldEditor(group1, CFGID_REPLICATION_LABEL, "Replication label");
			addSpacer(form);
			Group group2 = createGroup(form, "Scenario generation");
			addTextFieldEditor(group2, CFGID_REPEAT, "Repeat count");
			addTextFieldEditor(group2, CFGID_CONSTRAINT, "Constraint");
            addSpacer(form);
		}
		else if (category.equals(CAT_OUTPUTFILES)) {
			addTextFieldEditor(form, CFGID_RESULT_DIR, "Result directory");
			addCheckboxFieldEditor(form, CFGID_FNAME_APPEND_HOST, "Append host name to filenames");
			addSpacer(form);
			Group group0 = createGroup(form, "Event log");
			addCheckboxFieldEditor(group0, CFGID_RECORD_EVENTLOG, "Enable recording");
			addTextFieldEditor(group0, CFGID_EVENTLOG_FILE, "Eventlog file");
			addTextFieldEditor(group0, CFGID_EVENTLOG_RECORDING_INTERVALS, "Recording intervals");
			addTextFieldEditor(group0, CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "Message details to record");
			addCheckboxFieldEditor(group0, CFGID_MODULE_EVENTLOG_RECORDING, "Record events");
			addSpacer(form);
            Group group1 = createGroup(form, "Output vector recording");
			addTextFieldEditor(group1, CFGID_OUTPUT_VECTOR_FILE, "Output vector file");
            addCheckboxFieldEditor(group1, CFGID_VECTOR_RECORDING, "Enable recording");
            addTextFieldEditor(group1, CFGID_VECTOR_RECORDING_INTERVAL, "Recording interval");
            addSpacer(form);
            Group group2 = createGroup(form, "Output scalar recording");
			addTextFieldEditor(group2, CFGID_OUTPUT_SCALAR_FILE, "Output scalar file");
			addCheckboxFieldEditor(group2, CFGID_OUTPUT_SCALAR_FILE_APPEND, "Append to existing file");
			addCheckboxFieldEditor(group2, CFGID_SCALAR_RECORDING, "Enable recording");
			addCheckboxFieldEditor(group2, CFGID_PARAM_RECORD_AS_SCALAR, "Parameters to save as scalars");
			addSpacer(form);
            Group group3 = createGroup(form, "Snapshots");
			addTextFieldEditor(group3, CFGID_SNAPSHOT_FILE, "Snapshot file");
			addSpacer(form);
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
            addSpacer(form);
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
			addCheckboxFieldEditor(group2, CFGID_CMDENV_EV_OUTPUT, "Enable text output for modules");
			addSpacer(form);
            Group group3 = createGroup(form, "Miscellaneus");
            addCheckboxFieldEditor(group3, CFGID_CMDENV_INTERACTIVE, "Interactive mode");
            addCheckboxFieldEditor(group3, CFGID_CMDENV_AUTOFLUSH, "Auto-flush output files");
			addTextFieldEditor(group3, CFGID_CMDENV_OUTPUT_FILE, "Redirect stdout to file");
			addTextFieldEditor(group3, CFGID_CMDENV_EXTRA_STACK, "Cmdenv extra stack");
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
			addTextFieldEditor(form, CFGID_TKENV_EXTRA_STACK, "Tkenv extra stack");
            addSpacer(form);
		}
		else if (category.equals(CAT_PARSIM)) {
			addCheckboxFieldEditor(form, CFGID_PARALLEL_SIMULATION, "Enable parallel simulation");
            Group group0 = createGroup(form, "Partitioning");
            addTextTableFieldEditor(group0, CFGID_PARTITION_ID, "Module partitioning");
            addSpacer(form);
			Group group1 = createGroup(form, "General");
			addTextFieldEditor(group1, CFGID_PARSIM_COMMUNICATIONS_CLASS, "Communications class");
			addTextFieldEditor(group1, CFGID_PARSIM_SYNCHRONIZATION_CLASS, "Synchronization class");
			addCheckboxFieldEditor(group1, CFGID_PARSIM_DEBUG, "Debug parallel simulation");
			addSpacer(form);
			Group group2 = createGroup(form, "Communications");
			addTextFieldEditor(group2, CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, "MPI communications: MPI buffer");
			addTextFieldEditor(group2, CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX, "Named Pipe communications: prefix");
			addTextFieldEditor(group2, CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX, "File-based communications: prefix");
			addTextFieldEditor(group2, CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX, "File-based communications: read prefix");
			addCheckboxFieldEditor(group2, CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ, "File-based communications: preserve read files");
			addSpacer(form);
			Group group3 = createGroup(form, "Protocol-specific settings");
			addTextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, "Null Message Protocol: lookahead class");
			addTextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, "Null Message Protocol: laziness");
			addTextFieldEditor(group3, CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, "Ideal Simulation Protocol: table size");
            addSpacer(form);
		}
		else {
			throw new IllegalArgumentException("no such category: "+category);
		}

		// initialize combo boxes with static content
        FieldEditor simtimeScaleEditor = getFieldEditorFor(CFGID_SIMTIME_SCALE);
        if (simtimeScaleEditor != null)
            simtimeScaleEditor.setComboContents(Arrays.asList(SIMTIME_SCALE_CHOICES));
	}

//	private void addMessage(Composite parent, Image image, String text) {
//		Composite warnPanel = new Composite(parent, SWT.NONE);
//		warnPanel.setLayout(new GridLayout(2,false));
//		Label warnImageLabel = new Label(warnPanel, SWT.NONE);
//		warnImageLabel.setImage(image); //XXX
//		Label warnLabel = new Label(warnPanel, SWT.NONE);
//		warnLabel.setText(text);
//	}

	private Label addSpacer(Composite parent) {
		return new Label(parent, SWT.NONE);
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

    protected FieldEditor addTextFieldEditor(Composite parent, ConfigKey e, String label) {
        FieldEditor editor = e.isGlobal() ?
                new TextFieldEditor(parent, e, getInifileDocument(), this, label) :
                new ExpandableTextFieldEditor(parent, e, getInifileDocument(), this, label);
        addFieldEditor(editor);
        return editor;
    }

    protected FieldEditor addComboboxFieldEditor(Composite parent, ConfigKey e, String label) {
		FieldEditor editor = e.isGlobal() ?
				new ComboFieldEditor(parent, e, getInifileDocument(), this, label) :
				new ExpandableTextFieldEditor(parent, e, getInifileDocument(), this, label); //FIXME make it combo too
		addFieldEditor(editor);
        return editor;
	}

	protected FieldEditor addCheckboxFieldEditor(Composite parent, ConfigKey e, String label) {
		FieldEditor editor = e.isGlobal() ?
				new CheckboxFieldEditor(parent, e, getInifileDocument(), this, label) :
				new ExpandableCheckboxFieldEditor(parent, e, getInifileDocument(), this, label);
		addFieldEditor(editor);
		return editor;
	}

    protected FieldEditor addTextTableFieldEditor(Composite parent, ConfigKey e, String label) {
        FieldEditor editor = new TextTableFieldEditor(parent, e, getInifileDocument(), this, label);
        addFieldEditor(editor);
        return editor;
    }

    protected FieldEditor addCheckboxTableFieldEditor(Composite parent, ConfigKey e, String label) {
        FieldEditor editor = new CheckboxTableFieldEditor(parent, e, getInifileDocument(), this, label);
        addFieldEditor(editor);
        return editor;
    }

	protected void addFieldEditor(FieldEditor editor) {
		fieldEditors.add(editor);
		editor.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
	}

	protected FieldEditor getFieldEditorFor(ConfigKey configKey) {
        for (FieldEditor e : fieldEditors)
            if (e.getConfigKey() == configKey)
                return e;
        return null;
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
