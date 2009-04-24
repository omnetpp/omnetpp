package org.omnetpp.runtimeenv;

import org.eclipse.equinox.app.IApplication;
import org.eclipse.equinox.app.IApplicationContext;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.experimental.simkernel.swig.Javaenv;
import org.omnetpp.experimental.simkernel.swig.Simkernel;
import org.omnetpp.experimental.simkernel.swig.StringVector;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.experimental.simkernel.swig.cStaticFlag;

/**
 * This class controls all aspects of the application's execution
 * @author Andras
 */
public class Application implements IApplication {
	private Object result;

	/* (non-Javadoc)
	 * @see org.eclipse.equinox.app.IApplication#start(org.eclipse.equinox.app.IApplicationContext)
	 */
	public Object start(IApplicationContext context) {
		System.out.println("Entering Application.start()");
        ImageFactory.initialize(new String[]{"C:\\home\\omnetpp40\\omnetpp\\images"}); //XXX
        //Simkernel.changeToDir("C:/home/omnetpp40/omnetpp/test/anim/dynamic"); //XXX
        Simkernel.changeToDir("C:/home/omnetpp40/omnetpp/samples/queuenet"); //XXX
        cStaticFlag.set(true); //FIXME also clear it later

		// assemble the command line
		//XXX test code...
		StringVector args = new StringVector();
		args.add("programname");
//		args.add("-c");   --XXX not yet observed by SimulationManager...
//		args.add("Terminal");

		// invoke setupUserInterface() in the C++ code. This will call back our doStart() method
		// which runs the application and stores the exit code in the "result" field.
		System.out.println("Calling setupUserInterface() C++ function in envir lib...");
		Javaenv.setJavaApplication(null, this); // call back "this" object
		Simkernel.setupUserInterface(args);  
		return result;
	}

	protected void doStart() { // this will be called back from C++
		System.out.println("Application.doStart() called back from C++ Javaapp::run(), starting workbench...");
		Activator.simulationManager = new SimulationManager(); // needs to be after everything else ws set up in C++

		Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
        env.setJCallback(null, new EnvirCallback());

        result = reallyDoStart();	
		System.out.println("Workbench exited");
	}
	
	protected Object reallyDoStart() {
		Display display = PlatformUI.createDisplay();
		try {
			int returnCode = PlatformUI.createAndRunWorkbench(display, new ApplicationWorkbenchAdvisor());
			if (returnCode == PlatformUI.RETURN_RESTART) {
				return IApplication.EXIT_RESTART;
			}
			return IApplication.EXIT_OK;
		} finally {
			display.dispose();
		}
	}

	/* (non-Javadoc)
	 * @see org.eclipse.equinox.app.IApplication#stop()
	 */
	public void stop() {
		final IWorkbench workbench = PlatformUI.getWorkbench();
		if (workbench == null)
			return;
		final Display display = workbench.getDisplay();
		display.syncExec(new Runnable() {
			public void run() {
				if (!display.isDisposed())
					workbench.close();
			}
		});
	}
}
