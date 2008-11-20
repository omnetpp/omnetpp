package org.omnetpp.common;

/**
 * View IDs, builder IDs and other UI constants.
 * 
 * @author Andras
 */
public interface IConstants {
    // whether this version is a COMMERCIAL build or not
    public static final boolean IS_COMMERCIAL = true;

    // nature, builders
    public static final String OMNETPP_NATURE_ID = "org.omnetpp.main.omnetppnature";
    public static final String VECTORFILEINDEXER_BUILDER_ID = "org.omnetpp.scave.builder.vectorfileindexer";
    public static final String MAKEFILEBUILDER_BUILDER_ID = "org.omnetpp.cdt.MakefileBuilder";

    // views
    public static final String NEW_VERSION_VIEW_ID = "org.omnetpp.main.NewVersionView";
	public static final String MODULEPARAMETERS_VIEW_ID = "org.omnetpp.inifile.ModuleParameters";
	public static final String MODULEHIERARCHY_VIEW_ID = "org.omnetpp.inifile.ModuleHierarchy";
    public static final String NEDINHERITANCE_VIEW_ID = "org.omnetpp.inifile.NedInheritance";
	public static final String DATASET_VIEW_ID = "org.omnetpp.scave.DatasetView";
	public static final String VECTORBROWSER_VIEW_ID = "org.omnetpp.scave.VectorBrowserView";
	public static final String SEQUENCECHART_VIEW_ID = "org.omnetpp.sequencechart.editors.SequenceChartView";
	public static final String EVENTLOG_VIEW_ID = "org.omnetpp.eventlogtable.editors.EventLogTableView";

	public static final String PROJECT_EXPLORER_ID = "org.eclipse.ui.navigator.ProjectExplorer"; // ID missing from the Eclipse API
	
    // wizards
    public static final String NEW_SIMPLE_MODULE_WIZARD_ID = "org.omnetpp.ned.editor.wizard.new.simplemodule";
    public static final String NEW_COMPOUND_MODULE_WIZARD_ID = "org.omnetpp.ned.editor.wizard.new.compoundmodule";
    public static final String NEW_NETWORK_WIZARD_ID = "org.omnetpp.ned.editor.wizard.new.network";

    // preference IDs
	public static final String PREF_OMNETPP_ROOT = "omnetppRoot";
	public static final String PREF_OMNETPP_IMAGE_PATH = "omnetppImagePath";
	public static final String PREF_DOXYGEN_EXECUTABLE = "doxygenExecutable";
	public static final String PREF_GRAPHVIZ_DOT_EXECUTABLE = "graphvizDotExecutable";
}
