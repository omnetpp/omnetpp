package org.omnetpp.runtimeenv;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.experimental.simkernel.swig.EmptyConfig;
import org.omnetpp.experimental.simkernel.swig.MinimalEnv;
import org.omnetpp.experimental.simkernel.swig.cConfiguration;
import org.omnetpp.experimental.simkernel.swig.cModuleType;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.experimental.simkernel.swig.cStaticFlag;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "org.omnetpp.runtimeenv";

	// The shared instance
	private static Activator plugin;
	
	/**
	 * The constructor
	 */
	public Activator() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
		
        // set up an active simulation object
		cStaticFlag.set(true);
	    cConfiguration config = new EmptyConfig();
        MinimalEnv env = new MinimalEnv(0, null, config);
        config.disown();
        cSimulation simulation = new cSimulation("simulation", env);
        env.disown();
		cSimulation.setActiveSimulation(simulation);
		simulation.disown();
		System.out.println(simulation.getName());

		try {
		    cSimulation.loadNedSourceFolder("c:/home/omnetpp40/omnetpp/samples/aloha"); //XXX
		    cSimulation.doneLoadingNedFiles();
		    cModuleType networkType = cModuleType.find("Aloha_tmp");
		    if (networkType == null)
		        System.out.println("network not found");
		    else
		        simulation.setupNetwork(networkType);
		} catch (Exception e) {
		    e.printStackTrace();
		}
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
	    // delete the active simulation object
	    cSimulation simulation = cSimulation.getActiveSimulation();
	    cSimulation.setActiveSimulation(null);
	    if (simulation != null)
	        simulation.delete();
        cStaticFlag.set(false);
        
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static Activator getDefault() {
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return imageDescriptorFromPlugin(PLUGIN_ID, path);
	}
}
