package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_ANIMATION_ENABLED;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_ANIMATION_MSGCLASSNAMES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_ANIMATION_MSGCOLORS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_ANIMATION_MSGNAMES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_ANIMATION_SPEED;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_ANIM_METHODCALLS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_AUTOFLUSH;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CMDENV_EXTRA_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CONFIGURATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_CPU_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DEBUG_ON_ERRORS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DEFAULT_RUN;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EVENTLOG_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EVENT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EVENT_BANNER_DETAILS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXPRESSMODE_AUTOUPDATE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXPRESS_MODE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_FINGERPRINT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_FNAME_APPEND_HOST;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_IMAGE_PATH;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_INI_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_LOAD_LIBS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_MESSAGE_TRACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_METHODCALLS_DELAY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_MODULE_MESSAGES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NEXT_EVENT_MARKERS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NUM_RNGS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUTSCALARMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUTVECTORMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_OUTPUT_FILE;
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
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PENGUIN_MODE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PERFORMANCE_DISPLAY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PERFORM_GC;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PLUGIN_PATH;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRINT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRINT_UNDISPOSED;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_REALTIMESCHEDULER_SCALING;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_RNG_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_RUNS_TO_EXECUTE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SCHEDULER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SENDDIRECT_ARROWS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SHOW_BUBBLES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SHOW_LAYOUTING;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SIMTIME_SCALE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SIM_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SLOWEXEC_DELAY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SNAPSHOTMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_SNAPSHOT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_STATUS_FREQUENCY;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TKENV_EXTRA_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_TOTAL_STACK_KB;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_UPDATE_FREQ_EXPRESS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_UPDATE_FREQ_FAST;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USE_MAINWINDOW;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USE_NEW_LAYOUTER;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_WARNINGS;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Makeshift form page, just throws in all field editors.
 * TODO design proper forms instead, and throw this out
 * 
 * @author Andras
 */
public class GenericConfigPage2 extends FormPage {
    public static final String CAT_GENERAL = "General";
    public static final String CAT_RANDOMNUMBERS = "Random Numbers";
    public static final String CAT_DEBUGGING = "Debugging";
    public static final String CAT_EXECUTION = "Execution";
    public static final String CAT_ADVANCED = "Advanced";
    public static final String CAT_CMDENV = "Cmdenv";
    public static final String CAT_TKENV = "Tkenv";
    public static final String CAT_PARSIM = "Parallel Simulation";
    public static final String CAT_OUTPUTFILES = "Output Files";
    //public static final String CAT_OUTPUTVECTORS = "Output Vectors";

    public static String[] getCategoryNames() {
    	return new String[] {
    			CAT_GENERAL, 
    			CAT_RANDOMNUMBERS,
    			CAT_DEBUGGING,
    			CAT_EXECUTION,
    			CAT_ADVANCED,
    			CAT_CMDENV,
    			CAT_TKENV,
    			CAT_PARSIM,
    			CAT_OUTPUTFILES,
    	};
    }
    
	public GenericConfigPage2(Composite parent, String category, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		setLayout(new GridLayout(1,false));

		// populate with field editors
		IInifileDocument doc = getInifileDocument();
		if (category.equals(CAT_GENERAL)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_LOAD_LIBS, doc, "load-libs"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_USER_INTERFACE, doc, "user-interface"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_INI_WARNINGS, doc, "ini-warnings"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PRELOAD_NED_FILES, doc, "preload-ned-files"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_TOTAL_STACK_KB, doc, "total-stack-kb"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_FNAME_APPEND_HOST, doc, "fname-append-host"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_DEBUG_ON_ERRORS, doc, "debug-on-errors"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_SIMTIME_SCALE, doc, "simtime-scale"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_DESCRIPTION, doc, "description"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_NETWORK, doc, "network"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_WARNINGS, doc, "warnings"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_FINGERPRINT, doc, "fingerprint"));
		}
		else if (category.equals(CAT_RANDOMNUMBERS)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_NUM_RNGS, doc, "num-rngs"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_RNG_CLASS, doc, "rng-class"));
		}
		else if (category.equals(CAT_DEBUGGING)) {
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PERFORM_GC, doc, "perform-gc"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PRINT_UNDISPOSED, doc, "print-undisposed"));
		}
		else if (category.equals(CAT_EXECUTION)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_SIM_TIME_LIMIT, doc, "sim-time-limit"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_CPU_TIME_LIMIT, doc, "cpu-time-limit"));
		}
		else if (category.equals(CAT_ADVANCED)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_CONFIGURATION_CLASS, doc, "configuration-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_SCHEDULER_CLASS, doc, "scheduler-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUTVECTORMANAGER_CLASS, doc, "outputvectormanager-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUTSCALARMANAGER_CLASS, doc, "outputscalarmanager-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_SNAPSHOTMANAGER_CLASS, doc, "snapshotmanager-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_REALTIMESCHEDULER_SCALING, doc, "realtimescheduler-scaling"));
		}
		else if (category.equals(CAT_CMDENV)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_RUNS_TO_EXECUTE, doc, "runs-to-execute"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_CMDENV_EXTRA_STACK_KB, doc, "cmdenv-extra-stack-kb"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUT_FILE, doc, "output-file"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_EXPRESS_MODE, doc, "express-mode"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_AUTOFLUSH, doc, "autoflush"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_MODULE_MESSAGES, doc, "module-messages"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_EVENT_BANNERS, doc, "event-banners"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_EVENT_BANNER_DETAILS, doc, "event-banner-details"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_MESSAGE_TRACE, doc, "message-trace"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_STATUS_FREQUENCY, doc, "status-frequency"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PERFORMANCE_DISPLAY, doc, "performance-display"));
		}
		else if (category.equals(CAT_TKENV)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_TKENV_EXTRA_STACK_KB, doc, "tkenv-extra-stack-kb"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_DEFAULT_RUN, doc, "default-run"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_SLOWEXEC_DELAY, doc, "slowexec-delay"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_UPDATE_FREQ_FAST, doc, "update-freq-fast"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_UPDATE_FREQ_EXPRESS, doc, "update-freq-express"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_ANIMATION_ENABLED, doc, "animation-enabled"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_NEXT_EVENT_MARKERS, doc, "next-event-markers"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_SENDDIRECT_ARROWS, doc, "senddirect-arrows"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_ANIM_METHODCALLS, doc, "anim-methodcalls"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_METHODCALLS_DELAY, doc, "methodcalls-delay"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_ANIMATION_MSGNAMES, doc, "animation-msgnames"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_ANIMATION_MSGCLASSNAMES, doc, "animation-msgclassnames"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_ANIMATION_MSGCOLORS, doc, "animation-msgcolors"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PENGUIN_MODE, doc, "penguin-mode"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_SHOW_LAYOUTING, doc, "show-layouting"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_USE_NEW_LAYOUTER, doc, "use-new-layouter"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_SHOW_BUBBLES, doc, "show-bubbles"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_ANIMATION_SPEED, doc, "animation-speed"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PRINT_BANNERS, doc, "print-banners"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_USE_MAINWINDOW, doc, "use-mainwindow"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_EXPRESSMODE_AUTOUPDATE, doc, "expressmode-autoupdate"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_IMAGE_PATH, doc, "image-path"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PLUGIN_PATH, doc, "plugin-path"));
		}
		else if (category.equals(CAT_PARSIM)) {
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PARALLEL_SIMULATION, doc, "parallel-simulation"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_COMMUNICATIONS_CLASS, doc, "parsim-communications-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_SYNCHRONIZATION_CLASS, doc, "parsim-synchronization-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_FILECOMM_PREFIX, doc, "parsim-filecommunications-prefix"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_FILECOMM_READ_PREFIX, doc, "parsim-filecommunications-read-prefix"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PARSIM_FILECOMM_PRESERVE_READ, doc, "parsim-filecommunications-preserve-read"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, doc, "parsim-idealsimulationprotocol-tablesize"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, doc, "parsim-mpicommunications-mpibuffer"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_NAMEDPIPECOMM_PREFIX, doc, "parsim-namedpipecommunications-prefix"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, doc, "parsim-nullmessageprotocol-lookahead-class"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, doc, "parsim-nullmessageprotocol-laziness"));
			addField(new CheckboxFieldEditor(this, SWT.NONE, CFGID_PARSIM_DEBUG, doc, "parsim-debug"));
		}
		else if (category.equals(CAT_OUTPUTFILES)) {
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUT_VECTOR_FILE, doc, "output-vector-file"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUT_VECTOR_PRECISION, doc, "output-vector-precision"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUT_SCALAR_FILE, doc, "output-scalar-file"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUT_SCALAR_PRECISION, doc, "output-scalar-precision"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_SNAPSHOT_FILE, doc, "snapshot-file"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, doc, "output-vectors-memory-limit"));
			addField(new TextFieldEditor(this, SWT.NONE, CFGID_EVENTLOG_FILE, doc, "eventlog-file"));
		}
		
	}
	
	private void addField(FieldEditor editor) {
		editor.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
	}

	@Override
	public void reread() {
		for (Control c : getChildren())  //XXX temp code
			if (c instanceof FieldEditor)
				((FieldEditor) c).reread();
	}

	@Override
	public void commit() {
		for (Control c : getChildren())  //XXX temp code
			if (c instanceof FieldEditor)
				((FieldEditor) c).commit();
	}
}
