package org.omnetpp.experimental.simkernel;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.experimental.simkernel.swig.Simkernel;
import org.omnetpp.experimental.simkernel.swig.cEnvir;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.experimental.simkernel.swig.cStaticFlag;
import org.osgi.framework.BundleContext;

/**
 * The main plugin class to be used in the desktop.
 */
public class SimkernelPlugin extends AbstractUIPlugin {

	// The shared instance.
	private static SimkernelPlugin plugin;

	/**
	 * The constructor.
	 */
	public SimkernelPlugin() {
		plugin = this;
	}

	/**
	 * This method is called upon plug-in activation
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		
		cEnvir ev = Simkernel.getEV();
		cStaticFlag a = new cStaticFlag(); // needed by simkernel!!!
		
		System.out.println("STARTING SIMULATION");
		//Simkernel.changeToDir("D:\\home\\omnetpp40\\sandbox\\omnetpp\\samples\\p-to-p");
		IResource workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
		Simkernel.changeToDir(workspaceRoot.getLocation().toOSString()+"\\rxtx");

		//cSimulation simulation = Simkernel.getSimulation();
		Simkernel.evSetupDummyCall(); //XXX ev.setup(argc, argv);
		ev.run();
		ev.shutdown();
		
	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
		plugin = null;
	}

	/**
	 * Returns the shared instance.
	 *
	 * @return the shared instance.
	 */
	public static SimkernelPlugin getDefault() {
		return plugin;
	}
}
