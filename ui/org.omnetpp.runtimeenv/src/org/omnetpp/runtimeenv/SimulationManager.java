package org.omnetpp.runtimeenv;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.runtime.nativelibs.WrappedException;
import org.omnetpp.runtime.nativelibs.simkernel.Javaenv;
import org.omnetpp.runtime.nativelibs.simkernel.LogBuffer;
import org.omnetpp.runtime.nativelibs.simkernel.cConfigurationEx;
import org.omnetpp.runtime.nativelibs.simkernel.cEnvir;
import org.omnetpp.runtime.nativelibs.simkernel.cException;
import org.omnetpp.runtime.nativelibs.simkernel.cModule;
import org.omnetpp.runtime.nativelibs.simkernel.cModuleType;
import org.omnetpp.runtime.nativelibs.simkernel.cSimpleModule;
import org.omnetpp.runtime.nativelibs.simkernel.cSimulation;

/**
 *
 * @author Andras
 */
//XXX flusLastLine() everywhere
public class SimulationManager {
	protected ListenerList simulationListeners = new ListenerList();
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

    public enum RunMode {
    	NOTRUNNING,
    	STEP,
    	NORMAL,
    	FAST,
    	EXPRESS
	}


    public SimulationManager() {

        Display.getDefault().asyncExec(new Runnable() {
        	@Override
        	public void run() {
        		// wait until the workbench window appears
        		IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        		if (workbenchWindow == null) {
        			Display.getDefault().asyncExec(this); //FIXME this is a dirty hack
        			return;
        		}

        		// set up the default config/run
        		//newRun("Terminal", 0);
        		newRun("Net2", 0);
        		//newRun("PureAloha2", 0);

        		// inspect the network
        		cModule systemModule = cSimulation.getActiveSimulation().getSystemModule();
				if (systemModule != null)
        			Activator.openInspector2(systemModule, true);
        	}
        });
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

    public void addSimulationListener(ISimulationListener listener) {
        simulationListeners.add(listener);
    }

    public void removeSimulationListener(ISimulationListener listener) {
        simulationListeners.remove(listener);
    }

    static int counter = 0;
    protected void updateUI() {
        for (final Object listener : simulationListeners.getListeners()) {
            SafeRunner.run(new ISafeRunnable() {
                public void handleException(Throwable e) {
                    // exception logged in SafeRunner#run
                }
                public void run() throws Exception {
                    ((ISimulationListener)listener).changed();
                }
            });
        }

        if ((counter++ % 100) == 0) {
        	Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
        	long oldSize = env.swigTableSize();
        	env.swigPurge();
        	System.out.println("purging: Swig table size before/after purge: " + oldSize + " / " + env.swigTableSize());
        }
//		System.out.println("Swig table size: " + env.swigTableSize());
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

    public cConfigurationEx getConfig() {
        return Javaenv.cast(cSimulation.getActiveEnvir()).getConfigEx();
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
			getConfig().activateConfig("General", 0);
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
		updateUI();
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
	        getConfig().activateConfig(configName, runNumber);
	        env.readPerRunOptions();

//TODO
//            if (opt_network_name.empty())
//	        {
//	            confirm("No network specified in the configuration.");
//	            return;
//	        }

//	        cModuleType network = env.resolveNetwork(opt_network_name.c_str());  //XXX
	        cModuleType network = env.resolveNetwork(getConfig().getConfigValue("network")); //FIXME use opt_network_name as above!
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
		updateUI();
	    //XXX animating = false; // affects how network graphics is drawn!
	    //XXX updateNetworkRunDisplay();
	}

    protected void displayError(RuntimeException e) {
	    updateUI();

	    if (e instanceof WrappedException) {
    		cException ce = ((WrappedException)e).getException();
    		String message;
    		if (!ce.hasContext())
    	    	message = "Error: " + ce.what() + ".";
    		else if (ce.getModuleID()==-1)
    	        message = String.format("Error in component (%s) %s: %s.", ce.getContextClassName(), ce.getContextFullPath(), ce.what());
    	    else
    	        message = String.format("Error in module (%s) %s (id=%d): %s.", ce.getContextClassName(), ce.getContextFullPath(), ce.getModuleID(), ce.what());
        	MessageDialog.openError(getShell(), "Error", message);
    	}
    	else {
    		MessageDialog.openError(getShell(), "Error", e.getMessage());
    	}
	}

    public boolean isNetworkPresent() {
        if (cSimulation.getActiveSimulation().getSystemModule() == null) {
    		MessageDialog.openError(getShell(), "Error", "No network has been set up yet.");
    		return false;
        }
        return true;
    }

    public boolean isNetworkReady() { //XXX rename to assertNetworkReady()
        if (!isNetworkPresent())
        	return false;
        if (isSimulationOK())
            return true;
        if (!MessageDialog.openQuestion(getShell(), "Warning", "Cannot continue this simulation. Rebuild network?"))
        	return false;
        rebuildNetwork();
        return isSimulationOK();
    }

    public boolean isSimulationOK() {
    	return state==SimState.NEW || state==SimState.RUNNING || state==SimState.READY;
    }

    public boolean isRunning() {
    	return state==SimState.RUNNING || state==SimState.BUSY;
    }

    public boolean checkRunning() {  //XXX rename to assertNotRunning()
    	if (state==SimState.RUNNING) {
    		MessageDialog.openInformation(getShell(), "Simulation Running", "Sorry, you cannot do this while the simulation is running.");
    		return true;
    	}
    	if (state==SimState.BUSY) {
    		MessageDialog.openInformation(getShell(), "Simulation Busy", "The simulation is waiting for external synchronization -- press STOP to interrupt it.");
    		return true;
    	}
    	return false;
    }

    protected void setupUIForRunMode(RunMode mode, cModule module, boolean until) {
    	//XXX currently unused -- but see Tcl code
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
		// implements Simulate|Rebuild
		if (checkRunning())
			return;
		if (!isNetworkPresent())
			return;
		cSimulation simulation = cSimulation.getActiveSimulation();
		if (isConfigRun)
			newRun(getConfig().getActiveConfigName(), getConfig().getActiveRunNumber());
		else if (simulation.getNetworkType()!=null)
			newNetwork(simulation.getNetworkType().getFullName());
		else
			confirm("No network has been set up yet.");
		if (simulation.getSystemModule() != null)
			Activator.openInspector2(simulation.getSystemModule(), true);
	}

	private void confirm(String message) {
		MessageDialog.openConfirm(getShell(), "Confirm", message);
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
            while (!stopRequested) {
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
    	// make sure state is not RUNNING, NONET, or FINISHCALLED
    	if (checkRunning())
    		return;
    	if (!isNetworkPresent())
    		return;
    	if (state==SimState.FINISHCALLED) {
    		MessageDialog.openInformation(getShell(), "Finish Called", "finish() has been invoked already.");
    		return;
    	}

    	// let the user confirm the action, esp. if we are in the ERROR state
    	if (state==SimState.ERROR) {
    		if (!MessageDialog.openQuestion(getShell(), "Warning", "Simulation was stopped with error, calling finish() might produce unexpected results. Proceed anyway?"))
    			return;
    	}
    	else {
    		if (!MessageDialog.openQuestion(getShell(), "Question", "Do you want to conclude this simulation run and invoke finish() on all modules?"))
    			return;
    	}

    	//XXX busy "Invoking finish() on all modules..."
    	doCallFinish();
    	//XXX busy
    }

	private void doCallFinish() {
	    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
	    Assert.isTrue(state==SimState.NEW || state==SimState.READY || state==SimState.TERMINATED || state==SimState.ERROR);

	    //XXX logBuffer.addInfo("{** Calling finish() methods of modules\n}");

        Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());

	    // now really call finish()
	    try {
	        cSimulation.getActiveSimulation().callFinish();
	        //XXX TODO env.checkFingerprint();
	    }
	    catch (RuntimeException e) {
	        displayError(e);
	    }

	    // then endrun
	    try {
	        env.endRun();
	    }
	    catch (RuntimeException e) {
	        displayError(e);
	    }
	    state = SimState.FINISHCALLED;

	    updateUI();
	}

	private Shell getShell() {
		Shell activeShell = Display.getCurrent().getActiveShell();
		if (activeShell != null)
			return activeShell;
		return PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
	}
}
