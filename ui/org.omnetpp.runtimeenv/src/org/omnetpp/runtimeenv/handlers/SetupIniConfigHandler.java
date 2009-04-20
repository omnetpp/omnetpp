package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.experimental.simkernel.swig.Javaenv;
import org.omnetpp.experimental.simkernel.swig.cConfigurationEx;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.SimulationManager;
import org.omnetpp.runtimeenv.SimulationManager.SimState;


/**
 * Handler for the "Setup Inifile Configuration" action
 * @author Andras
 */
public class SetupIniConfigHandler extends AbstractHandler {

	public SetupIniConfigHandler() {
	}

	//XXX same as in SetupNetwork -- factor out somewhere
	protected boolean checkRunning() {
		SimState state = Activator.getSimulationManager().getState();
		if (state==SimState.RUNNING) {
			MessageDialog.openInformation(null, "Simulation Running", "Sorry, you cannot do this while the simulation is running.");
			return true;
		}
		if (state==SimState.BUSY) {
			MessageDialog.openInformation(null, "Simulation Busy", "The simulation is waiting for external synchronization -- press STOP to interrupt it.");
			return true;
		}
		return false;
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
				cConfigurationEx cfg = Javaenv.cast(cSimulation.getActiveEnvir()).getConfigEx();
				if (element instanceof String) {
					String configName = (String) element;
					String desc = cfg.getConfigDescription(configName);
					if (StringUtils.isEmpty(desc))
						return configName;
					else 
						return configName + " -- " + desc;
				}
				else if (element instanceof ConfigRun) {
					ConfigRun run = (ConfigRun)element;
					return "Run " + run.runNumber;  //TODO display also the iteration variables!!!!
				}
				return element.toString();
			}
		};
		
		static class ConfigContentProvider implements ITreeContentProvider {
			@Override
			public Object[] getChildren(Object element) {
				if (element instanceof String) {
					cConfigurationEx cfg = Javaenv.cast(cSimulation.getActiveEnvir()).getConfigEx();
					String configName = (String) element;
					int n = cfg.getNumRunsInConfig(configName);
					ConfigRun runs[] = new ConfigRun[n];
					for (int i=0; i<n; i++)
						runs[i] = new ConfigRun(configName, i);
					return runs; 
				}
				return new Object[0];
			}

			@Override
			public Object getParent(Object element) {
				return null;
			}

			@Override
			public boolean hasChildren(Object element) {
				return getChildren(element).length > 0;
			}

			@Override
			public Object[] getElements(Object inputElement) {
				Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
				String[] configNames = env.getConfigEx().getConfigNames().toArray();
				return configNames;
			}

			@Override
			public void dispose() {
			}

			@Override
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
			if (result instanceof String)
				return -1; // config node selected
			if (result instanceof ConfigRun)
				return ((ConfigRun)result).runNumber;
			return -1;
		}
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
		if (checkRunning())
			return null;
		
		RunSelectionDialog dialog = new RunSelectionDialog(null); //XXX parent

		//XXX set old values
		// set configname [opp_getactiveconfigname]
		// set runnumber  [opp_getactiverunnumber]

		if (dialog.open() == ListDialog.OK && dialog.getFirstResult() != null) {
			String configName = dialog.getConfigName();
			int runNumber = dialog.getRunNumber();
			//XXX next stuff should be using a progress monitor...
			SimulationManager simulationManager = Activator.getSimulationManager();
			simulationManager.newRun(configName, runNumber);
			cModule systemModule = cSimulation.getActiveSimulation().getSystemModule();
			if (systemModule != null)
				Activator.openInspector2(systemModule, true);
		}
		return null;
	}
}
