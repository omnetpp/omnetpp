/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.simkernel;

import org.eclipse.core.runtime.Plugin;
import org.osgi.framework.BundleContext;

/**
 * The main plugin class to be used in the desktop.
 */
public class SimkernelPlugin extends Plugin {

	// The shared instance.
	private static SimkernelPlugin plugin;
	
//	cStaticFlag staticFlag; 

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
		
//		staticFlag = new cStaticFlag(); // needed by sim kernel
//		
//		System.out.println("STARTING SIMULATION");
//		IResource workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
//		Simkernel.changeToDir(workspaceRoot.getLocation().toOSString()+"\\rxtx");
//
//		Simkernel.evSetupDummyCall(); // ev.setup(argc, argv);
//
//		//Javaenv jenv = Simkernel.getJavaenv();
//		//jenv.setJCallback(null, new DummyEnvirCallback());
//
//		//cEnvir ev = Simkernel.getEV();
//		//cSimulation simulation = Simkernel.getSimulation();
//
//		//jenv.newRun(1);
//		//for (int i=0; i<10; i++)
//		//	jenv.doOneStep();
	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);

//		cEnvir ev = Simkernel.getEV();
//		ev.shutdown();
//		staticFlag.delete();
//		staticFlag = null;
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
