package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.cdt.core.settings.model.ICBuildSetting;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.ui.newui.CDTPropertyManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.statushandlers.StatusManager;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.BuildSpecification;
import org.omnetpp.common.util.StringUtils;


/**
 * Utility functions
 *  
 * @author Andras
 */
public class ProjectConfigurationUtils {
    // "Fix it" href values 
    public static final String PROBLEMID_SOURCE_LOCATIONS_DIFFER = "sourceLocationsDiffer";
    
    /**
     * Analyzes project settings, and returns a report about potential problems. 
     * Returns null if no problem was found. If addFixItLink is true, it assumes 
     * the returned text will be displayed in a Link widget, and adds a "Fix it!" 
     * link with a problem ID as href. The link should end up calling the fixProblem()
     * method. 
     */
    public static String getDiagnosticMessage(IProject project, BuildSpecification buildSpec, boolean addFixItLink) {
        // Check CDT settings
        ICProjectDescription projectDescription = CDTPropertyManager.getProjectDescription(project);
        if (projectDescription == null)
            return "Cannot access CDT build information for this project. Is this a C/C++ project?";
        ICConfigurationDescription activeConfiguration = projectDescription.getActiveConfiguration();
        if (activeConfiguration == null)
            return "No active build configuration -- please create one.";
        boolean isOmnetppConfig = false;
        for (ICConfigurationDescription c = activeConfiguration; c != null; c = (ICConfigurationDescription)c.getParent())
            if (c.getId().startsWith("org.omnetpp.cdt.")) {
                isOmnetppConfig = true; break;}
        if (!isOmnetppConfig)
            return "The active build configuration \""+ activeConfiguration.getName() + "\" is " +
            		"not an OMNeT++ configuration. Please re-create the project it with the " +
            		"New OMNeT++ Project wizard, overwriting the existing project settings.";
        ICBuildSetting buildSetting = activeConfiguration.getBuildSetting();
        if (buildSetting == null)
            return "No CDT Project Builder. Activate one on the C/C++ Build / Tool Chain Editor page."; //???
        String builderId = buildSetting.getId();
        if (builderId==null || !builderId.startsWith("org.omnetpp.cdt."))
            return "C/C++ Builder \"" + buildSetting.getName()+ "\" set in the active build configuration " +
                   "is not suitable for OMNeT++. Please re-create the project with the " +
                   "New OMNeT++ Project wizard, overwriting the existing project settings.";
    
        // warn if referenced projects not present or not open
        try {
            List<String> badRefProjs = new ArrayList<String>();
            for (IProject ref : project.getReferencedProjects())
                if (!ref.isAccessible())
                    badRefProjs.add(ref.getFullPath().toString());
            if (!badRefProjs.isEmpty())
                return "The following referenced projects are missing or closed: " + StringUtils.join(badRefProjs, ", ");
        }
        catch (CoreException e) {
            Activator.logError(e);
            return "Cannot query list of referenced projects: " + e.getMessage();
        }
    
        // warn if there're differences in source entries across configurations.
        // first, collect which entries occur in which configurations
        Map<String,List<String>> sourceEntryMap = new HashMap<String, List<String>>(); // srcEntry -> config*
        for (ICConfigurationDescription cfg : projectDescription.getConfigurations()) {
            for (ICSourceEntry e : cfg.getSourceEntries()) {
                String entryText = StringUtils.defaultIfEmpty(CDataUtil.makeAbsolute(project, e).getFullPath().toString(), ".");
                if (e.getExclusionPatterns().length > 0)
                    entryText += " (excl: " + StringUtils.join(e.getExclusionPatterns(), ", ") + ")";
                if (!sourceEntryMap.containsKey(entryText))
                    sourceEntryMap.put(entryText, new ArrayList<String>());
                sourceEntryMap.get(entryText).add(cfg.getName());
            }
        }
        List<String> wrongSourceLocations = new ArrayList<String>();
        int numConfigs = projectDescription.getConfigurations().length;
        for (String e : sourceEntryMap.keySet())
            if (sourceEntryMap.get(e).size() != numConfigs)
                wrongSourceLocations.add(StringUtils.join(sourceEntryMap.get(e), ",") + ": " + e);
        Collections.sort(wrongSourceLocations);
        if (!wrongSourceLocations.isEmpty())
            return "Note: Source locations are set up differently across configurations: " + StringUtils.join(wrongSourceLocations, "; ") + 
            (addFixItLink ? ProjectConfigurationUtils.makeFixItLinkText(ProjectConfigurationUtils.PROBLEMID_SOURCE_LOCATIONS_DIFFER) : "");
    
        // warn if there's no makefile generated at all
        if (buildSpec.getMakeFolders().isEmpty())
            return "No makefile has been specified for this project.";
    
        // check if build directory exists, and there's a makefile in it
        String buildLocation = activeConfiguration.getBuildSetting().getBuilderCWD().toString();
        IContainer buildFolder = ProjectMakemakePropertyPage.resolveFolderLocation(buildLocation, project, activeConfiguration);
        if (buildFolder == null)
            return "Wrong build location: filesystem location \""+ buildLocation + "\" does not exist, or does not map to any folder in the workspace. Check the C/C++ Build page."; //FIXME also print what macro gets resolved to
        if (!buildSpec.getMakeFolders().contains(buildFolder))
            return "Root build folder " + buildFolder.getFullPath().toString() + " is set to \"No Make\"";
        for (IContainer folder : buildSpec.getMakeFolders())
            if (buildSpec.getFolderMakeType(folder) == BuildSpecification.CUSTOM)
                if (!folder.getFile(new Path("Makefile")).exists())  //XXX or Makefile.vc 
                    return "Custom Make folder " + buildFolder.getFullPath().toString() + " contains no Makefile";
    
        return null;
    }

    protected static String makeFixItLinkText(String problemId) {
        return " <a href=\"" + problemId + "\">(Fix it)</a>";
    }

    
    /**
     * Fix a problem detected by getDiagnosticMessage()
     */
    public static void fixProblem(IProject project, BuildSpecification buildSpec, String problemId) {
        Shell parentShell = Display.getDefault().getActiveShell();
        
        if (problemId.equals(ProjectConfigurationUtils.PROBLEMID_SOURCE_LOCATIONS_DIFFER)) {
            // fix the "Source locations differ across configurations" problem: copy the active configuration's 
            // source entry settings to all other configurations
            ICProjectDescription projectDescription = CDTPropertyManager.getProjectDescription(project);
            final ICConfigurationDescription activeConfiguration = projectDescription!=null ? projectDescription.getActiveConfiguration() : null;

            ILabelProvider labelProvider = new LabelProvider() {
                @Override
                public String getText(Object element) {
                    ICConfigurationDescription config = (ICConfigurationDescription)element;
                    return config.getName() + (config == activeConfiguration ? "  [active]" : "");
                }
            };
            
            ICConfigurationDescription selectedConfiguration = (ICConfigurationDescription)
            MessageDialogWithCombo.openAndSelect(parentShell, "Fix Project Setup", 
                    "This will copy the project's source location and excluded folder settings from one configuration to all others.", 
                    "Copy from:", projectDescription.getConfigurations(), labelProvider, activeConfiguration
                    );

            if (selectedConfiguration != null) {
                for (ICConfigurationDescription cfg : projectDescription.getConfigurations()) {
                    if (cfg != selectedConfiguration) {
                        try {
                            // copy the array from selectedConfiguration; the entries themselves seem to be immutable, 
                            // so we don't need to duplicate them
                            cfg.setSourceEntries(selectedConfiguration.getSourceEntries().clone());
                        }
                        catch (WriteAccessException e) {
                            MessageDialog.openError(parentShell, "Error", "No write access to configuration " + cfg.getName());
                        }
                        catch (CoreException e) {
                            StatusManager.getManager().handle(e, Activator.PLUGIN_ID);
                        }
                    }
                }
    
            }
        }
        else {
            Activator.logError("Unknown problemID: " + problemId, new RuntimeException());
        }
    }

}
