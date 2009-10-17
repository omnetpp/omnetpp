package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.dialogs.ISelectionStatusValidator;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.runtime.nativelibs.simkernel.GUIEnv;
import org.omnetpp.runtime.nativelibs.simkernel.cConfigurationEx;
import org.omnetpp.runtime.nativelibs.simkernel.cModule;
import org.omnetpp.runtime.nativelibs.simkernel.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.SimulationManager;


/**
 * Handler for the "Setup Inifile Configuration" action
 * @author Andras
 */
public class SetupIniConfigHandler extends AbstractHandler {
	private static Image IMAGE_CONFIG_EMPTY = Activator.getCachedImage("icons/obj16/config_empty.png");
	private static Image IMAGE_CONFIG_SINGLE = Activator.getCachedImage("icons/obj16/config_single.png");
	private static Image IMAGE_CONFIG_REPEAT = Activator.getCachedImage("icons/obj16/config_repeat.png");
	private static Image IMAGE_RUN = Activator.getCachedImage("icons/obj16/configrun.png");

	public SetupIniConfigHandler() {
	}

	/**
	 * Dialog for selecting a config name and a run number.
	 */
	public static class RunSelectionDialog extends ElementTreeSelectionDialog {
		static class ConfigRun {
			String configName;
			int runNumber;
			public ConfigRun(String configName, int runNumber) {
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
				ConfigRun other = (ConfigRun) obj;
				return other.configName.equals(configName) && other.runNumber==runNumber;
			}
		}

		static class ConfigLabelProvider extends LabelProvider {
			@Override
			public String getText(Object element) {
				cConfigurationEx cfg = GUIEnv.cast(cSimulation.getActiveEnvir()).getConfigEx();
				if (element instanceof String) {
					String configName = (String) element;
					int n = cfg.getNumRunsInConfig(configName);
					String numRuns = n==0 ? " (0 run)" : n==1 ? "" : " (" + n + " runs)";
					String desc = cfg.getConfigDescription(configName);
					if (StringUtils.isEmpty(desc))
						return configName + numRuns;
					else
						return configName + numRuns + " -- " + desc;
				}
				else if (element instanceof ConfigRun) {
					ConfigRun run = (ConfigRun)element;
					String vars = cfg.unrollConfig(run.configName, false).get(run.runNumber);
					return "Run #" + run.runNumber + " (" + vars + ")";
				}
				return element.toString();
			}
	
			@Override
			public Image getImage(Object element) {
				cConfigurationEx cfg = GUIEnv.cast(cSimulation.getActiveEnvir()).getConfigEx();
				if (element instanceof String) {
					String configName = (String) element;
					int n = cfg.getNumRunsInConfig(configName);
					return n==0 ? IMAGE_CONFIG_EMPTY : n==1 ? IMAGE_CONFIG_SINGLE : IMAGE_CONFIG_REPEAT;
				}
				else if (element instanceof ConfigRun) {
					return IMAGE_RUN;
				}
				return null;
			}
		};

		static class ConfigContentProvider implements ITreeContentProvider {
			//@Override
			public Object[] getChildren(Object element) {
				if (element instanceof String) {
					cConfigurationEx cfg = GUIEnv.cast(cSimulation.getActiveEnvir()).getConfigEx();
					String configName = (String) element;
					int n = cfg.getNumRunsInConfig(configName);
					if (n > 1) {
						ConfigRun runs[] = new ConfigRun[n];
						for (int i=0; i<n; i++)
							runs[i] = new ConfigRun(configName, i);
						return runs;
					}
				}
				return new Object[0];
			}

			//@Override
			public Object getParent(Object element) {
				return null;
			}

			//@Override
			public boolean hasChildren(Object element) {
				return getChildren(element).length > 0;
			}

			//@Override
			public Object[] getElements(Object inputElement) {
				GUIEnv env = GUIEnv.cast(cSimulation.getActiveEnvir());
				String[] configNames = env.getConfigEx().getConfigNames().toArray();
				return configNames;
			}

			//@Override
			public void dispose() {
			}

			//@Override
			public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
			}
		};

		public RunSelectionDialog(Shell parent) {
			super(parent, new ConfigLabelProvider(), new ConfigContentProvider());

			// set default texts
			setTitle("Select Configuration");
			setMessage("Select configuration and run:");
			setAllowMultiple(false);
			setInput("");

			setValidator(new ISelectionStatusValidator() {
				//@Override
				public IStatus validate(Object[] selection) {
					if (selection.length==0)
						return new Status(IStatus.ERROR, Activator.PLUGIN_ID, IStatus.ERROR, "Nothing is selected.", null);
					if (getRunNumber()==-1)
						return new Status(IStatus.ERROR, Activator.PLUGIN_ID, IStatus.ERROR, "Select a run from the configuration.", null);
					return new Status(IStatus.OK, Activator.PLUGIN_ID, IStatus.OK, "", null); // Status.OK_STATUS would display as "OK"
				}
			});
		}

		public String getConfigName() {
			Object result = getFirstResult();
			if (result instanceof String)
				return (String)result;
			if (result instanceof ConfigRun)
				return ((ConfigRun)result).configName;
			return null;
		}

		public int getRunNumber() {
			Object result = getFirstResult();
			if (result instanceof String) {
				// config node selected
				cConfigurationEx cfg = GUIEnv.cast(cSimulation.getActiveEnvir()).getConfigEx();
				String configName = (String)result;
				int n = cfg.getNumRunsInConfig(configName);
				return n==1 ? 0 : -1;  // if config contains multiple runs, answer -1 for "unknown"
			}
			if (result instanceof ConfigRun)
				return ((ConfigRun)result).runNumber;
			return -1;
		}
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
		SimulationManager simulationManager = Activator.getSimulationManager();
		if (simulationManager.checkRunning())
			return null;

		RunSelectionDialog dialog = new RunSelectionDialog(null); //XXX parent
		//XXX set old values
		// set configname [opp_getactiveconfigname]
		// set runnumber  [opp_getactiverunnumber]

		if (dialog.open() == ListDialog.OK) {
			String configName = dialog.getConfigName();
			int runNumber = dialog.getRunNumber();
			//XXX check something was selected
			//XXX next stuff should be using a progress monitor...
			simulationManager.newRun(configName, runNumber);
			cModule systemModule = cSimulation.getActiveSimulation().getSystemModule();
			if (systemModule != null)
				Activator.openInspector2(systemModule, true);
		}
		return null;
	}
}
