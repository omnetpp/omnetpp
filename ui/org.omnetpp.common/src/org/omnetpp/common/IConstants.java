package org.omnetpp.common;

/**
 * View IDs, builder IDs and other UI constants.
 * 
 * @author Andras
 */
public interface IConstants {
	public static final String NEWS_VIEW_ID = "org.omnetpp.main.NewsView";
	public static final String MODULEPARAMETERS_VIEW_ID = "org.omnetpp.inifile.ModuleParameters";
	public static final String MODULEHIERARCHY_VIEW_ID = "org.omnetpp.inifile.ModuleHierarchy";
    public static final String NEDINHERITANCE_VIEW_ID = "org.omnetpp.inifile.NedInheritance";
	public static final String DATASET_VIEW_ID = "org.omnetpp.scave.DatasetView";
	public static final String VECTORBROWSER_VIEW_ID = "org.omnetpp.scave.VectorBrowserView";
	public static final String SEQUENCECHART_VIEW_ID = "org.omnetpp.sequencechart.editors.SequenceChartView";
	public static final String EVENTLOG_VIEW_ID = "org.omnetpp.eventlogtable.editors.EventLogTableView";
	
    public static final String VECTORFILEINDEXER_BUILDER_ID = "org.omnetpp.scave.builder.vectorfileindexer";
    public static final String MAKEFILEBUILDER_BUILDER_ID = "org.omnetpp.cdt.MakefileBuilder";

    // ID missing from the eclipse API
    public static final String PROJECT_EXPOLRER_ID = "org.eclipse.ui.navigator.ProjectExplorer";
    /**
     * ID of this project nature
     */
    public static final String OMNETPP_NATURE_ID = "org.omnetpp.main.omnetppnature";
    // whether this version is a COMMERCIAL build or not
    public static final boolean IS_COMMERCIAL = false;
}
