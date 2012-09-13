package org.omnetpp.simulation.controller;

import java.io.IOException;
import java.net.SocketException;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.Simulation.AskParameter;
import org.omnetpp.simulation.controller.Simulation.ErrorInfo;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.Simulation.StatusResponse;
import org.omnetpp.simulation.liveanimation.LiveAnimationController;

/**
 * TODO
 *
 * @author Andras
 */
//TODO introduce BUSY state in Cmdenv? it would be active during setting up a network and calling finish
public class SimulationController implements ISimulationCallback {
    private Simulation simulation;
    private LiveAnimationController liveAnimationController;  //TODO should probably be done via listeners, and not by storing LiveAnimationController reference here!

//    private SimState state;  //TODO keep in sync with Simulation's state
    private boolean lastEventAnimationDone = false;
    private Simulation.RunMode currentRunMode = RunMode.NOTRUNNING;
    private BigDecimal runUntilSimTime;
    private long runUntilEventNumber;
    private boolean stopRequested;

    private ISimulationUICallback simulationUICallback;
    private ListenerList simulationListeners = new ListenerList(); // listeners we have to notify on changes


    public SimulationController(Simulation simulation) {
        this.simulation = simulation;
        simulation.setSimulationCallback(this);
    }

    public Simulation getSimulation() {
        return simulation;
    }

    /**
     * The state of the simulation. NOTE: this is not always the same as the
     * state of the simulation process! While running, the UI runs the
     * simulation in chunks of n events (or seconds) and queries the process
     * state after each chunk; naturally the process will report state==READY
     * between the chunks, while UI is obviously in state==RUNNING.
     */
    public SimState getUIState() {
        SimState state = simulation.getState();
        return (state == SimState.READY && currentRunMode != RunMode.NOTRUNNING) ? SimState.RUNNING : state;
    }

    public long getEventNumber() {
        return isLastEventAnimationDone() ? simulation.getNextEventNumber() : simulation.getLastEventNumber();
    }

    public BigDecimal getSimulationTime() {
        return isLastEventAnimationDone() ? simulation.getNextEventSimulationTimeGuess() : simulation.getLastEventSimulationTime();
    }

    //XXX we'd need:
    // module ptr, ID, fullPath, nedType  (ptr is not enough, because module itself may have been deleted already)
    // msg ptr, ID, name, className (ptr is not enough, because message itself may have been deleted already)
    //TODO group eventnum,simtime,module,msg into an EventInfo class?

    public int getEventModuleId() { 
        return isLastEventAnimationDone() ? simulation.getNextEventModuleIdGuess() : 0 /*XXX no getLastEventModuleId*/;
    }

    public long getEventMessageId() {
        return isLastEventAnimationDone() ? simulation.getNextEventMessageIdGuess() : 0 /*XXX no getLastEventMessageId*/;
    }

    public boolean isNetworkPresent() {
        return getUIState() != SimState.DISCONNECTED && getUIState() != SimState.NONETWORK;
    }

    public boolean isSimulationOK() {
        return getUIState() == SimState.READY || getUIState() == SimState.RUNNING;
    }

    public boolean isRunning() {
        return getUIState() == SimState.RUNNING;
    }

    public Simulation.RunMode getCurrentRunMode() {
        return currentRunMode;
    }

    public boolean isRunUntilActive() {
        return isRunning() && runUntilSimTime != null && runUntilEventNumber  > 0;
    }

    public boolean isLastEventAnimationDone() {
        return lastEventAnimationDone;
    }

    public void setLiveAnimationController(LiveAnimationController liveAnimationController) {
        this.liveAnimationController = liveAnimationController;
    }

    public LiveAnimationController getLiveAnimationController() {
        return liveAnimationController;
    }

    public void setSimulationUICallback(ISimulationUICallback simulationUICallback) {
        this.simulationUICallback = simulationUICallback;
    }

    public ISimulationUICallback getSimulationUICallback() {
        return simulationUICallback;
    }

    public void addSimulationStateListener(ISimulationStateListener listener) {
        simulationListeners.add(listener);
    }

    public void removeSimulationStateListener(ISimulationStateListener listener) {
        simulationListeners.remove(listener);
    }

    public List<ConfigDescription> getConfigDescriptions() throws IOException {
        return simulation.getConfigDescriptions();
    }

    public void setupRun(String configName, int runNumber) throws IOException {
        simulation.sendSetupRunCommand(configName, runNumber);
        refreshUntil(SimState.READY); //TODO in background thread, plus callback in the end?
    }

    public void setupNetwork(String networkName) throws IOException {
        simulation.sendSetupNetworkCommand(networkName);
        refreshUntil(SimState.READY); //TODO in background thread, plus callback in the end?
    }

    public void rebuildNetwork() throws IOException {
        simulation.sendRebuildNetworkCommand();
        refreshUntil(SimState.READY); //TODO in background thread, plus callback in the end?
    }

    public void step() throws IOException {
        Assert.isTrue(simulation.getState() == SimState.READY);
        if (currentRunMode == RunMode.NOTRUNNING) {
//            animationPlaybackController.jumpToEnd();

            currentRunMode = RunMode.STEP;
            notifyListeners(); // because currentRunMode has changed
            simulation.sendStepCommand();
            lastEventAnimationDone = false;
            refreshUntil(SimState.READY);

            // animate it
//            animationPlaybackController.play();
            EventEntry lastEvent = getSimulation().getLogBuffer().getLastEventEntry();
            liveAnimationController.startAnimatingLastEvent(lastEvent);
            // Note: currentRunMode=RunMode.NOTRUNNING and lastEventAnimationDone=false will be done in animationStopped()!
        }
        else {
            stop(); // if already running, just stop it
        }
    }

    public void run() throws IOException {
        run(RunMode.NORMAL);
    }

    public void fastRun() throws IOException {
        run(RunMode.FAST);
    }

    public void expressRun() throws IOException {
        run(RunMode.EXPRESS);
    }

    public void run(RunMode mode) throws IOException {
        runUntil(mode, null, 0);
    }

    public void runUntil(RunMode mode, BigDecimal simTime, long eventNumber) throws IOException {
        runUntilSimTime = simTime;
        runUntilEventNumber = eventNumber;

//        animationPlaybackController.jumpToEnd();

        if (currentRunMode == RunMode.NOTRUNNING) {
            Assert.isTrue(simulation.getState() == SimState.READY);
            stopRequested = false;
            currentRunMode = mode;
            notifyListeners(); // because currentRunMode has changed
            doRun();
        }
        else {
            // asyncExec() already scheduled, just change the runMode for it
            currentRunMode = mode;
        }
    }

    protected void doRun() throws IOException {
        // should we run at all?
        boolean run = true;
        if (simulation.getState() != SimState.READY && simulation.getState() != SimState.RUNNING)
            run = false;
        else if (stopRequested)
            run = false;
        if (runUntilEventNumber > 0 && simulation.getNextEventNumber() >= runUntilEventNumber)
            run = false;
        if (runUntilSimTime != null && simulation.getNextEventSimulationTimeGuess().greaterOrEqual(runUntilSimTime))
            run = false;
        if (!run) {
            currentRunMode = RunMode.NOTRUNNING;
            notifyListeners(); // because currentRunMode has changed
            return;
        }

        // determine how much we'll run in this chunk
        long eventDelta = -1;
        switch (currentRunMode) {
            case NORMAL: eventDelta = 1; break;
            case FAST: eventDelta = 10; break;
            case EXPRESS: eventDelta = 1000; break;  //TODO: express should rather use wall-clock seconds as limit!!!
            default: Assert.isTrue(false);
        }

        long untilEvent = runUntilEventNumber == 0 ? simulation.getLastEventNumber()+eventDelta : Math.min(runUntilEventNumber, simulation.getLastEventNumber()+eventDelta);

        // tell process to run...
        simulation.sendRunUntilCommand(currentRunMode, runUntilSimTime, untilEvent);
        if (currentRunMode == RunMode.NORMAL)
            lastEventAnimationDone = false;

        // ...and wait for it to complete
        refreshUntil(SimState.READY);  //XXX note: error dialog will *precede* animation of last event -- weird!

        // animate, or asyncExec next run chunk 
        if (currentRunMode == RunMode.NORMAL) {
            // animate it
            //animationPlaybackController.play();
            EventEntry lastEvent = getSimulation().getLogBuffer().getLastEventEntry();
            liveAnimationController.startAnimatingLastEvent(lastEvent);
            // Note: next doRun() will be called from animationStopped()!
        }
        else if (simulation.getState() == SimState.READY) {
            Display.getCurrent().asyncExec(new Runnable() {
                @Override
                public void run() {
                    try {
                        // animationPlaybackController.stop();  // in case it was running
                        // animationPlaybackController.jumpToEnd(); // show current state on animation canvas
                        doRun();
                    }
                    catch (IOException e) {
                        MessageDialog.openError(Display.getDefault().getActiveShell(), "Error", "An error occurred: " + e.getMessage());
                        SimulationPlugin.logError("Error running simulation", e);
                    }
                }
            });
        }
    }

    public void stop() throws IOException {
        // make sure doRun()'s asyncExec() code won't do anything (unfortunately Display does not offer a ways to cancel asyncExec code)
        stopRequested = true;

        // cancel animation (TODO if running!)
        liveAnimationController.cancelAnimation();

        // stop the underlying simulation
        if (simulation.getState() == SimState.RUNNING) {
            try {
                simulation.sendStopCommand();
            } finally {
                refreshUntil(SimState.READY);
            }
        }

        // update the UI
        currentRunMode = RunMode.NOTRUNNING;
        lastEventAnimationDone = true;
        runUntilSimTime = null;
        runUntilEventNumber = 0;

        notifyListeners(); // because currentRunMode has changed
    }

    public void callFinish() throws IOException {
        // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR but it comes handy in practice...
        SimState state = simulation.getState();
        Assert.isTrue(state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR);
        simulation.sendCallFinishCommand();
        refreshUntil(SimState.FINISHCALLED); //TODO in background thread, plus callback in the end?
    }

    /**
     * Repeatedly issue refreshStatus() until the state reported by simulation process becomes 
     * the expected state, or a quasi terminal state (TERMINATED, ERROR or DISCONNECTED).
     * The caller should check which one occurred.
     */
    public void refreshUntil(SimState expectedState) throws IOException {
        //TODO this method is called after sending commands for potentially long-running operations 
        // in the simulation process, e.g. setting up a network, calling finish, or processing and event.
        // Then HTTP (or this loop) will keep up the UI thread and make the IDE nonresponsive.
        // So, this whole thing should be probably done in a background thread, and report completion 
        // via a callback...
        //TODO or, at least bring up a cancellable progress dialog after a few seconds

        for (int retries = 0; true; retries++) {
            refreshStatus();  // note: this will trigger at least one but potentially several statusRefreshed() callbacks
            SimState state = simulation.getState();
            if (state == expectedState || state == SimState.TERMINATED || state == SimState.ERROR || state == SimState.DISCONNECTED)
                return;
            if (retries > 1)
                try { Thread.sleep(retries <= 5 ? 100 : 500); } catch (InterruptedException e) {}
        }
    }

    public void refreshStatus() throws IOException {
        Assert.isTrue(simulationUICallback != null); // callbacks must be set
        long startTime = System.currentTimeMillis();
        boolean again;
        do {
            //TODO implement PAUSE/RESUME commands in Cmdenv!! this method calls getObjectINfo even while
            // simulation is running!!!! or while network is being set up! this could be solved with PAUSE/RESUME

            StatusResponse request = simulation.refreshStatus();
            simulation.refreshObjectCache(); //XXX not good! not atomic with previous one, so doing this during simulation will bring interesting results

            // allow the UI to be updated before we ask parameters or pop up and error dialog
            notifyListeners();

            // carry out action requested by the simulation
            again = false;
            if (request instanceof AskParameter) {
                // parameter value prompt
                AskParameter info = (AskParameter)request;
                String value = simulationUICallback.askParameter(info.paramName, info.ownerFullPath, info.paramType, info.prompt, info.defaultValue, info.unit, info.choices);
                simulation.sendParameterValue(value);
                again = true;
            }
            if (request instanceof ErrorInfo) {
                // display error message
                ErrorInfo info = (ErrorInfo)request;
                simulationUICallback.displayError(info.message);
                again = true;
            }
        } while (again);
        Debug.println("Simulation.refreshStatus(): " + (System.currentTimeMillis() - startTime) + "ms\n");
    }

    @Override
    public void socketError(SocketException e) {
        notifyListeners();
    }

    @Override
    public void simulationProcessExited() {
        // this comes is a background thread, but listeners are typically UI related
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            @Override
            public void run() {
                notifyListeners();
            }
        });
    }

    // XXX called from outside
    public void animationStopped() {
        lastEventAnimationDone = true;
        if (currentRunMode == RunMode.STEP) {
            currentRunMode = RunMode.NOTRUNNING;
            notifyListeners();
        }
        else if (currentRunMode == RunMode.NORMAL) {
            try {
                notifyListeners();
                doRun();  //FIXME not if stopRequested???
            }
            catch (Exception e) {
                MessageDialog.openError(Display.getDefault().getActiveShell(), "Error", "An error occurred: " + e.getMessage());
                SimulationPlugin.logError("Error running simulation", e);
            }
        }
    }

    protected void notifyListeners() {
        for (final Object listener : simulationListeners.getListeners()) {
            SafeRunner.run(new ISafeRunnable() {
                @Override
                public void run() throws Exception {
                    ((ISimulationStateListener) listener).simulationStateChanged(SimulationController.this);
                }

                @Override
                public void handleException(Throwable e) {
                    SimulationPlugin.logError(e);
                }
            });
        }
    }

    public void dispose() {
        simulation.dispose();
        //TODO cancel timers, etc.
    }

}
