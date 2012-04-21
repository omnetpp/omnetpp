package org.omnetpp.simulation.actions;

import java.util.List;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.dialogs.ISelectionStatusValidator;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.controller.ConfigDescription;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * The Setup Inifile Config action.
 *  
 * @author Andras
 */
public class SetupIniConfigAction extends AbstractSimulationAction {
    private static Image IMAGE_CONFIG_EMPTY = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_CONFIG_EMPTY);
    private static Image IMAGE_CONFIG_SINGLE = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_CONFIG_SINGLE);
    private static Image IMAGE_CONFIG_REPEAT = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_CONFIG_REPEAT);
    private static Image IMAGE_RUN = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_CONFIGRUN);

    public SetupIniConfigAction() {
        setText("Set Up...");
        setToolTipText("Set Up...");
        setImageDescriptor(SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_NEWRUN));
    }

    /**
     * Dialog for selecting a config name and a run number.
     */
    public static class RunSelectionDialog extends ElementTreeSelectionDialog {
        static class ConfigAndRun {
            String configName;
            int runNumber;
            public ConfigAndRun(String configName, int runNumber) {
                this.configName = configName;
                this.runNumber = runNumber;
            }
            @Override
            public int hashCode() {
                return configName.hashCode()*31 + runNumber;
            }
            @Override
            public boolean equals(Object obj) {
                if (this == obj) return true;
                if (obj == null) return false;
                if (getClass() != obj.getClass()) return false;
                ConfigAndRun other = (ConfigAndRun) obj;
                return other.configName.equals(configName) && other.runNumber==runNumber;
            }
        }

        static class ConfigLabelProvider extends LabelProvider {
            @Override
            public String getText(Object element) {
                if (element instanceof ConfigDescription) {
                    ConfigDescription cfg = (ConfigDescription)element;
                    String configName = cfg.name;
                    int n = cfg.numRuns;
                    String numRuns = n==0 ? " (0 run)" : n==1 ? "" : " (" + n + " runs)";
                    String desc = cfg.description;
                    if (StringUtils.isEmpty(desc))
                        return configName + numRuns;
                    else
                        return configName + numRuns + " -- " + desc;
                }
                else if (element instanceof ConfigAndRun) {
                    ConfigAndRun run = (ConfigAndRun)element;
                    String vars = "TODO itervars"; //XXX cfg.unrollConfig(run.configName, false).get(run.runNumber);
                    return "Run #" + run.runNumber + " (" + vars + ")";
                }
                return element.toString();
            }
            
            @Override
            public Image getImage(Object element) {
                if (element instanceof ConfigDescription) {
                    ConfigDescription cfg = (ConfigDescription)element;
                    return cfg.numRuns==0 ? IMAGE_CONFIG_EMPTY : cfg.numRuns==1 ? IMAGE_CONFIG_SINGLE : IMAGE_CONFIG_REPEAT;
                }
                else if (element instanceof ConfigAndRun) {
                    return IMAGE_RUN;
                }
                return null;
            }
        };

        static class ConfigContentProvider implements ITreeContentProvider {
            @SuppressWarnings("rawtypes")
            public Object[] getChildren(Object element) {
                if (element instanceof List)
                    return ((List)element).toArray();
                else if (element instanceof ConfigDescription) {
                    ConfigDescription cfg = (ConfigDescription)element;
                    int numRuns = cfg.numRuns;
                    if (numRuns > 1) {
                        ConfigAndRun runs[] = new ConfigAndRun[numRuns];
                        for (int i = 0; i < numRuns; i++)
                            runs[i] = new ConfigAndRun(cfg.name, i);
                        return runs;
                    }
                }
                return new Object[0];
            }

            public Object getParent(Object element) {
                return null;
            }

            public boolean hasChildren(Object element) {
                return getChildren(element).length > 0;
            }

            @SuppressWarnings("rawtypes")
            public Object[] getElements(Object inputElement) {
                // input is expected to be a List<ConfigDescription>
                return ((List)inputElement).toArray();
            }

            public void dispose() {
            }

            public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
            }
        };

        public RunSelectionDialog(Shell parent, List<ConfigDescription> configDescriptions) {
            super(parent, new ConfigLabelProvider(), new ConfigContentProvider());

            // set default texts
            setTitle("Select Configuration");
            setMessage("Select simulation configuration and run number:");
            setAllowMultiple(false);
            setInput(configDescriptions);

            setValidator(new ISelectionStatusValidator() {
                public IStatus validate(Object[] selection) {
                    if (selection.length==0)
                        return new Status(IStatus.ERROR, SimulationPlugin.PLUGIN_ID, IStatus.ERROR, "Nothing is selected.", null);
                    if (getRunNumber()==-1)
                        return new Status(IStatus.ERROR, SimulationPlugin.PLUGIN_ID, IStatus.ERROR, "Select a run from the configuration.", null);
                    return new Status(IStatus.OK, SimulationPlugin.PLUGIN_ID, IStatus.OK, "", null); // Status.OK_STATUS would display as "OK"
                }
            });
        }

        public String getConfigName() {
            Object result = getFirstResult();
            if (result instanceof ConfigDescription)
                return ((ConfigDescription)result).name;
            if (result instanceof ConfigAndRun)
                return ((ConfigAndRun)result).configName;
            return null;
        }

        public int getRunNumber() {
            Object result = getFirstResult();
            if (result instanceof ConfigDescription) {
                ConfigDescription cfg = (ConfigDescription)result;
                return cfg.numRuns==1 ? 0 : -1;  // if config contains multiple runs, answer -1 for "unknown"
            }
            if (result instanceof ConfigAndRun)
                return ((ConfigAndRun)result).runNumber;
            return -1;
        }
    }

    @Override
    public void run() {
        try {
            SimulationController controller = getActiveSimulationController();
            if (!ensureNotRunning(controller))
                return;

            List<ConfigDescription> configDescriptions = controller.getConfigDescriptions(); //XXX in background thread!
            RunSelectionDialog dialog = new RunSelectionDialog(Display.getCurrent().getActiveShell(), configDescriptions);

            if (dialog.open() == ListDialog.OK) {
                String configName = dialog.getConfigName();
                int runNumber = dialog.getRunNumber();
                //XXX check something was selected
                //XXX next stuff should be using a progress monitor...
                controller.newRun(configName, runNumber);
                //XXX inspect system module...
            }
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }
}
