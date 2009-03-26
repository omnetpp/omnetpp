package org.omnetpp.runtimeenv;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.experimental.simkernel.swig.EmptyConfig;
import org.omnetpp.experimental.simkernel.swig.ExecuteOnStartup;
import org.omnetpp.experimental.simkernel.swig.MinimalEnv;
import org.omnetpp.experimental.simkernel.swig.SimTime;
import org.omnetpp.experimental.simkernel.swig.cConfiguration;
import org.omnetpp.experimental.simkernel.swig.cModuleType;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.experimental.simkernel.swig.cStaticFlag;
import org.omnetpp.runtimeenv.editors.ModelCanvas;
import org.omnetpp.runtimeenv.editors.ModuleIDEditorInput;
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

		ImageFactory.setImageDirs(new String[]{"C:\\home\\omnetpp40\\omnetpp\\images"}); //FIXME just temporary
		
		// library initializations
		cStaticFlag.set(true);
	    ExecuteOnStartup.executeAll();
	    SimTime.setScaleExp(-12);
	    
	    // set up an active simulation object
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
		        throw new RuntimeException("network not found");
		    simulation.setupNetwork(networkType);
		    
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    if (workbenchWindow == null) {
                        Display.getDefault().asyncExec(this); //FIXME this is a dirty hack
                        return;
                    }

                    IWorkbenchPage page = workbenchWindow.getActivePage();
                    int moduleID = cSimulation.getActiveSimulation().getSystemModule().getId();
                    try {
                        page.openEditor(new ModuleIDEditorInput(moduleID), ModelCanvas.EDITOR_ID);
                    }
                    catch (PartInitException e) {
                        e.printStackTrace(); //XXX
                    }
                }
            });
		} 
		catch (Exception e) {
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
	
	/**
	 * Copied from IDE.openEditor().
	 */
	public static IEditorPart openEditor(IWorkbenchPage page, IEditorInput input, String editorId) throws PartInitException {
	    return page.openEditor(input, editorId);
	}

}
