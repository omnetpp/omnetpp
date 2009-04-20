package org.omnetpp.runtimeenv;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.experimental.simkernel.DummyEnvirCallback;
import org.omnetpp.experimental.simkernel.WrappedException;
import org.omnetpp.experimental.simkernel.swig.Javaenv;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.cEnvir;
import org.omnetpp.experimental.simkernel.swig.cException;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModuleType;
import org.omnetpp.experimental.simkernel.swig.cSimpleModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.editors.BlankCanvasEditorInput;
import org.omnetpp.runtimeenv.editors.ModelCanvas;

/**
 * 
 * @author Andras
 */
public class SimulationManager {
    protected ListenerList listeners = new ListenerList();
    protected boolean stopRequested = false;

    /**
     * State transitions:
     *   NONET -> NEW -> NONET;
     *   NONET -> NEW -> (READY<->RUNNING) -> NONET;
     *   NONET -> NEW -> (READY<->RUNNING) -> TERMINATED -> FINISHCALLED -> NONET;
     *   NONET -> NEW -> (READY<->RUNNING) -> ERROR -> NONET.
     * Plus, at any time it may be temporarily BUSY inside in an idle() call.
     */
    public enum SimState {
        NONET, 
        NEW,
        READY,
        RUNNING,
        TERMINATED,
        FINISHCALLED,
        ERROR,
        BUSY  // busy doing active wait
    };
    protected SimState state = SimState.NONET;
	protected boolean isConfigRun; //XXX unused currently

    public SimulationManager() {
        ImageFactory.initialize(new String[]{"C:\\home\\omnetpp40\\omnetpp\\images"}); //FIXME just temporary; maybe into Application?

        Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
        env.setJCallback(null, new DummyEnvirCallback());
        
        try {
        	//env.getConfigEx().activateConfig("Terminal",0); //XXX
        	env.getConfigEx().activateConfig("Net2",0); //XXX
        	env.readPerRunOptions();
        	
            String networkName = env.getConfigEx().getConfigValue("network");
			cModuleType networkType = cModuleType.find(networkName);
            if (networkType == null)
                throw new RuntimeException("network not found");

            cSimulation simulation = cSimulation.getActiveSimulation();
            simulation.setupNetwork(networkType);
            env.startRun();
            
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    if (workbenchWindow == null) {
                        Display.getDefault().asyncExec(this); //FIXME this is a dirty hack
                        return;
                    }

                    IWorkbenchPage page = workbenchWindow.getActivePage();
                	cSimulation simulation = cSimulation.getActiveSimulation();
                    cModule module = simulation.getSystemModule();
                    try {
                        page.openEditor(new BlankCanvasEditorInput(""), ModelCanvas.EDITOR_ID);
                        Activator.openInspector2(module, false);
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
    
    public void dispose() {
    	//FIXME do all such stuff in Application?
        // delete the active simulation object
//    	Simkernel.shutdownUserInterface();
/*
    	cSimulation simulation = cSimulation.getActiveSimulation();
        cSimulation.setActiveSimulation(null);
        if (simulation != null)
            simulation.delete();
*/            
//        cStaticFlag.set(false);
    }
    
    public void addChangeListener(ISimulationListener listener) {
        listeners.add(listener);
    }

    public void removeChangeListener(ISimulationListener listener) {
        listeners.remove(listener);
    }

    protected void updateUI() {
        for (final Object listener : listeners.getListeners()) {
            SafeRunner.run(new ISafeRunnable() {
                public void handleException(Throwable e) {
                    // exception logged in SafeRunner#run
                }
                public void run() throws Exception {
                    ((ISimulationListener)listener).changed();
                }
            });
        }
    }

    public LogBuffer getLogBuffer() {
        cEnvir env = cSimulation.getActiveEnvir();
        return Javaenv.cast(env).getLogBuffer();
    }
    
    public SimState getState() {
    	return state;
    }
    
    public String getLocalPackage() {
    	return "";  //FIXME
    }
    
	public void newNetwork(String networkName) {
		try {
			cSimulation simulation = cSimulation.getActiveSimulation();
	        Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
			
			// finish & cleanup previous run if we haven't done so yet
			if (state != SimState.NONET)
			{
				env.endRun();
				simulation.deleteNetwork();
				state = SimState.NONET;
			}

			cModuleType network = env.resolveNetwork(networkName);
			Assert.isNotNull(network);

			// set up new network with config General.
			isConfigRun = false;
			env.getConfigEx().activateConfig("General", 0);
			env.readPerRunOptions();
			//TODO opt_network_name = network->getName();  // override config setting
			//TODO answers.clear();
			simulation.setupNetwork(network);
			env.startRun();

			state = SimState.NEW;
		}
		catch (RuntimeException e) {
			displayError(e);
			state = SimState.ERROR;
		}

		// update GUI
		//XXX animating = false; // affects how network graphics is drawn!
		//XXX updateNetworkRunDisplay();
    }

	public void newRun(String configName, int runNumber) {
	    try {
			cSimulation simulation = cSimulation.getActiveSimulation();
	        Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());

	        // finish & cleanup previous run if we haven't done so yet
	        if (state!=SimState.NONET)
	        {
	            env.endRun();
	            simulation.deleteNetwork();
	            state = SimState.NONET;
	        }

	        // set up new network
	        isConfigRun = true;
	        env.getConfigEx().activateConfig(configName, runNumber);
	        env.readPerRunOptions();

//TODO
//            if (opt_network_name.empty())
//	        {
//	            confirm("No network specified in the configuration.");
//	            return;
//	        }

//	        cModuleType network = env.resolveNetwork(opt_network_name.c_str());  //XXX
	        cModuleType network = env.resolveNetwork(env.getConfigEx().getConfigValue("network")); //FIXME use opt_network_name as above!
	        Assert.isNotNull(network);

	        //XXX answers.clear();
	        simulation.setupNetwork(network);
	        env.startRun();

	        state = SimState.NEW;
	    }
		catch (RuntimeException e) {
			displayError(e);
			state = SimState.ERROR;
		}

	    // update GUI
	    //XXX animating = false; // affects how network graphics is drawn!
	    //XXX updateNetworkRunDisplay();
	}

    protected void displayError(RuntimeException e) {
    	if (e instanceof WrappedException) {
    		cException ce = ((WrappedException)e).getException();
    		String message;
    		if (!ce.hasContext())
    	    	message = "Error: " + ce.what() + ".";
    		else if (ce.getModuleID()==-1)
    	        message = String.format("Error in component (%s) %s: %s.", ce.getContextClassName(), ce.getContextFullPath(), ce.what());
    	    else
    	        message = String.format("Error in module (%s) %s (id=%d): %s.", ce.getContextClassName(), ce.getContextFullPath(), ce.getModuleID(), ce.what());
        	MessageDialog.openError(null, "Error", message); //XXX parent shell
    	}
    	else {
    		MessageDialog.openError(null, "Error", e.getMessage()); //XXX parent shell
    	}
	}

	public void step() {
	    Assert.isTrue(state==SimState.NEW || state==SimState.READY);

//	    clearNextModuleDisplay();
//	    clearPerformanceDisplay();
//	    updateSimtimeDisplay();

//	    animating = true;

//	    rununtil_msg = NULL; // deactivate corresponding checks in eventCancelled()/objectDeleted()

/*TODO
	    simstate = SIM_RUNNING;
	    startClock();
	    simulation.getScheduler()->executionResumed();
	    try
	    {
	        cSimpleModule *mod = simulation.selectNextModule();
	        if (mod)  // selectNextModule() not interrupted
	        {
	            if (opt_print_banners)
	               printEventBanner(simulation.msgQueue.peekFirst(), mod);
	            simulation.doOneEvent(mod);
	            performAnimations();
	        }
	        updateSimtimeDisplay();
	        updateNextModuleDisplay();
	        updateInspectors();
	        simstate = SIM_READY;
	        outvectormgr->flush();
	        outscalarmgr->flush();
	        if (eventlogmgr)
	            eventlogmgr->flush();
	    }
	    catch (cTerminationException& e)
	    {
	        simstate = SIM_TERMINATED;
	        stoppedWithTerminationException(e);
	        displayMessage(e);
	    }
	    catch (std::exception& e)
	    {
	        simstate = SIM_ERROR;
	        stoppedWithException(e);
	        displayError(e);
	    }
	    stopClock();
	    stopsimulation_flag = false;

	    if (simstate==SIM_TERMINATED)
	    {
	        // call wrapper around simulation.callFinish() and simulation.endRun()
	        //
	        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
	        // called yet, because we want to allow the user to force finish() from
	        // the GUI -- and finish() has to precede endRun(). endRun() will be called
	        // just before a new network gets set up, or on Tkenv shutdown.
	        //
	        finishSimulation();
	    }
*/
		
        cSimulation sim = cSimulation.getActiveSimulation();
        try {
            cSimpleModule mod = sim.selectNextModule();
            if (mod == null)
                System.out.println("no more events"); //XXX
            else {
                sim.doOneEvent(mod);

                updateUI();
            }
        }
//XXX
//        catch (cTerminationException& e) {
//            printf("Finished: %s\n", e.what());
//        }
        catch (RuntimeException e) {
            e.printStackTrace();
        }
	}
	public void rebuildNetwork() {
    }

    public void slowRun() {
        // TODO Auto-generated method stub
    }

    public void run() {
        cSimulation sim = cSimulation.getActiveSimulation();
        stopRequested = false;
        long tStart = System.currentTimeMillis();
        long numEvents = 0;
        try {
            while (sim.getSimTime() < 100000 && !stopRequested) {
                cSimpleModule mod = sim.selectNextModule();
                if (mod == null)
                    break;  //XXX
                sim.doOneEvent(mod);

                updateUI();
                while (Display.getCurrent().readAndDispatch());
                
                //XXX preliminary speed metering, for debugging purposes
                numEvents++;
                if ((numEvents&16) == 0 && System.currentTimeMillis()-tStart > 1000) {
                    long t = System.currentTimeMillis();
                    long dt = t - tStart;
                    System.out.println("events/sec: " + (numEvents * 1000.0 / dt));
                    numEvents = 0;
                    tStart = t;
                }
            }
        }
//XXX
//        catch (cTerminationException& e) {
//            printf("Finished: %s\n", e.what());
//        }
        catch (RuntimeException e) {
            e.printStackTrace();
        }
    }

    public void fastRun() {
        // TODO Auto-generated method stub
    }

    public void expressRun() {
        // TODO Auto-generated method stub
    }

    public void runUntil() {
        // TODO Auto-generated method stub
    }

    public void stop() {
        stopRequested = true;
    }

    public void callFinish() {
        // TODO Auto-generated method stub
    }
}
