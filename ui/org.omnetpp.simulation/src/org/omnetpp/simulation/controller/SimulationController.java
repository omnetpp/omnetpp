package org.omnetpp.simulation.controller;

import java.io.IOException;
import java.net.SocketException;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.ISimulationProcess;
import org.omnetpp.common.simulation.ISimulationProcessListener;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.Simulation.AskParameterRequest;
import org.omnetpp.simulation.controller.Simulation.AskYesNoRequest;
import org.omnetpp.simulation.controller.Simulation.GetsRequest;
import org.omnetpp.simulation.controller.Simulation.MsgDialogRequest;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.Simulation.StatusResponse;
import org.omnetpp.simulation.controller.Simulation.StoppingReason;
import org.omnetpp.simulation.controller.SimulationChangeEvent.Reason;
import org.omnetpp.simulation.liveanimation.LiveAnimationController;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;

/**
 * TODO
 *
 * @author Andras
 */
//TODO introduce BUSY state in Cmdenv? it would be active during setting up a network and calling finish
//TODO: "set up network" after 1st resume
//TODO resurrect dialog on going offline
//TODO process info dialog: add connState
//TODO rename SimState.NONE to UNKNOWN?
//TODO rewrite refreshUntil() to use DelayedJob
public class SimulationController implements ISimulationCallback, ISimulationProcessListener {
    /**
     * State of the connection to the simulation process
     */
    public enum ConnState {
        INIT,  // just created
        CONNECTING,  // pinging
        ONLINE,  // normal
        OFFLINE, // temporarily not talking to the process (although we believe it still exists and is running)
        SUSPENDED, // process suspended in the debugger
        RESUMABLE, // still off-line, but process has already been resumed in the debugger
        DISCONNECTED, // no simulation process, e.g. it has terminated
    }

    private ConnState connState;  // connection state
    private ISimulationProcess simulationProcess; // we want to be notified when the simulation process exits, is suspended or resumed
    private boolean cancelJobOnDispose;  // whether to kill the simulation launcher job when the controller is disposed
    private boolean connectDone = false;  // whether we have at least once successfully talked to the process via socket
    private DelayedJob resumableStateDelayedJob = null;
    private DelayedJob reconnectDelayedJob = null;
    private boolean isDisposed = false;
    private Simulation simulation;
    private LiveAnimationController liveAnimationController;  //TODO should probably be done via listeners, and not by storing LiveAnimationController reference here!

    private boolean lastEventAnimationDone = false;
    private RunMode currentRunMode = RunMode.NONE;
    private BigDecimal runUntilSimTime;
    private long runUntilEventNumber;
    private cModule runUntilModule;
    private cMessage runUntilMessage;
    private boolean stopRequested;

    private ISimulationUICallback simulationUICallback;
    private ListenerList simulationChangeListeners = new ListenerList();

    public SimulationController(String hostName, int portNumber, ISimulationProcess simulationProcess, boolean cancelJobOnDispose) {
        Assert.isNotNull(hostName);
        Assert.isNotNull(simulationProcess);
        this.simulationProcess = simulationProcess;
        this.cancelJobOnDispose = cancelJobOnDispose;
        this.connState = ConnState.INIT;

        this.simulation = new Simulation(hostName, portNumber);
        simulation.setSimulationCallback(this);
        simulation.setOnline(false);

        // get notified about process suspend/resume/termination
        simulationProcess.addListener(this);
    }

    public Simulation getSimulation() {
        return simulation;
    }

    public ConnState getConnectionState() {
        return connState;
    }

    protected void setConnectionState(ConnState state) {
        if (connState != state) {
            final ConnState oldState = connState;
            connState = state;

            boolean online = (connState == ConnState.ONLINE);
            if (online != simulation.isOnline())
                simulation.setOnline(online);

            // state changes often come in a background thread, but listeners are typically UI related
            DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                @Override
                public void run() {
                    if (!isDisposed())
                        fireSimulationStateChanged(new SimulationChangeEvent(Reason.CONNSTATE_CHANGE, oldState, connState));
                }
            });
        }
    }

    /**
     * The state of the simulation. NOTE: this is not always the same as the
     * state of the simulation process! While running, the UI runs the
     * simulation in chunks of n events (or seconds) and queries the process
     * state after each chunk; naturally the process will report state==READY
     * between the chunks, while UI is obviously in state==RUNNING.
     */
    public SimState getUIState() {
        SimState state = simulation.getSimState();
        return (state == SimState.READY && currentRunMode != RunMode.NONE) ? SimState.RUNNING : state;
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

    public boolean isDisposed() {
        return isDisposed;
    }

    public boolean hasSimulationProcess() {
        return getConnectionState() != ConnState.DISCONNECTED;
    }

    public boolean isOnline() {
        return getConnectionState() == ConnState.ONLINE;
    }

    public boolean isNetworkPresent() {
        return isOnline() && getUIState() != SimState.NONE && getUIState() != SimState.NONETWORK;
    }

    public boolean isSimulationOK() {
        return getUIState() == SimState.READY || getUIState() == SimState.RUNNING;
    }

    public boolean isReady() {
        return getUIState() == SimState.READY;
    }

    public boolean isRunning() {
        return getUIState() == SimState.RUNNING;
    }

    public Simulation.RunMode getCurrentRunMode() {
        return currentRunMode;
    }

    public boolean isRunUntilActive() {
        return isRunning() && (runUntilSimTime != null || runUntilEventNumber  > 0 || runUntilModule != null || runUntilMessage != null);
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

    public void addSimulationChangeListener(ISimulationChangeListener listener) {
        simulationChangeListeners.add(listener);
    }

    public void removeSimulationChangeListener(ISimulationChangeListener listener) {
        simulationChangeListeners.remove(listener);
    }

    public List<ConfigDescription> getConfigDescriptions() throws CommunicationException {
        return simulation.getConfigDescriptions();
    }

    public void setCancelJobOnDispose(boolean cancelJobOnDispose) {
        this.cancelJobOnDispose = cancelJobOnDispose;
    }

    public boolean getCancelJobOnDispose() {
        return cancelJobOnDispose;
    }

    public boolean canCancelLaunch() {
        return simulationProcess.canCancel();
    }

    /**
     * Terminates the simulation; may only be called if canCancelLaunch() returns true
     */
    public void cancelLaunch() {
        simulationProcess.cancel();
    }

    public void connect() {
        // wait until the simulation process starts to respond to HTTP requests.
        // Meanwhile, all actions should be disabled.
        Assert.isTrue(getConnectionState() == ConnState.INIT || getConnectionState() == ConnState.RESUMABLE);
        Assert.isTrue(!connectDone);
        setConnectionState(ConnState.CONNECTING);
        tryConnect();
    }

    protected void tryConnect() {
        int origTimeout = simulation.getTimeoutMillis();
        simulation.setTimeoutMillis(1000); // 1s
        try {
            Debug.println("Ping...");
            simulation.ping();  // throws exception on failure

            // ping successful:
            Debug.println("OK");
            connectDone = true;
            setConnectionState(ConnState.ONLINE);
        }
        catch (IOException e) {
            Debug.println(e.getClass().getSimpleName() + ": " + e.getMessage());
            Display.getCurrent().timerExec(1000, new Runnable() {
                @Override
                public void run() {
                    if (!isDisposed() && !simulationProcess.isSuspended())
                        tryConnect();
                }
            });
        }
        finally {
            simulation.setTimeoutMillis(origTimeout);
        }
    }

    public void setupRun(String configName, int runNumber) throws CommunicationException {
        if (StringUtils.isNotEmpty(simulation.getNetworkName()))
            fireSimulationStateChanged(Reason.BEFORE_NETWORK_DELETION);
        simulation.sendSetupRunCommand(configName, runNumber);
        refreshUntil(SimState.READY); //TODO in background thread, plus callback in the end?
        fireSimulationStateChanged(Reason.NETWORK_SET_UP);
    }

    public void setupNetwork(String networkName) throws CommunicationException {
        if (StringUtils.isNotEmpty(simulation.getNetworkName()))
            fireSimulationStateChanged(Reason.BEFORE_NETWORK_DELETION);
        simulation.sendSetupNetworkCommand(networkName);
        refreshUntil(SimState.READY); //TODO in background thread, plus callback in the end?
        fireSimulationStateChanged(Reason.NETWORK_SET_UP);
    }

    public void rebuildNetwork() throws CommunicationException {
        if (StringUtils.isNotEmpty(simulation.getNetworkName()))
            fireSimulationStateChanged(Reason.BEFORE_NETWORK_DELETION);
        simulation.sendRebuildNetworkCommand();
        refreshUntil(SimState.READY); //TODO in background thread, plus callback in the end?
        fireSimulationStateChanged(Reason.NETWORK_SET_UP);
    }

    public void step() throws CommunicationException {
        Assert.isTrue(simulation.getSimState() == SimState.READY);
        if (currentRunMode == RunMode.NONE) {
//            animationPlaybackController.jumpToEnd();

            setRunMode(RunMode.STEP);
            simulation.sendStepCommand();
            lastEventAnimationDone = false;
            refreshUntil(SimState.READY);
            fireSimulationStateChanged(Reason.EVENTS_PROCESSED);

            // animate it
            doNowOrAfterAnimation(true, resetRunState);
        }
        else {
            stop(); // if Step is hit while simulation is running, just stop it so that user can single-step further
        }
    }

    public void run(RunMode mode) throws CommunicationException {
        runUntil(mode, null, 0, null, null);
    }

    public void runLocal(RunMode mode, cModule module) throws CommunicationException {
        runUntil(mode, null, 0, module, null);
    }

    public void runUntilMessage(RunMode mode, cMessage message) throws CommunicationException {
        runUntil(mode, null, 0, null, message);
    }

    public void switchToRunMode(RunMode mode) {
        if (mode != currentRunMode) {
            Assert.isTrue(mode != RunMode.NONE);
            Assert.isTrue(currentRunMode != RunMode.NONE, "must be running");
            setRunMode(mode);
        }
    }

    public void runUntil(RunMode mode, BigDecimal simTime, long eventNumber, cModule module, cMessage message) throws CommunicationException {
        if (currentRunMode != RunMode.NONE)
            stop();  //XXX maybe doStop(), to spare the extra notifyListeners() call and its consequences

        runUntilSimTime = simTime;
        runUntilEventNumber = eventNumber;
        runUntilModule = module;
        runUntilMessage = message;

//        animationPlaybackController.jumpToEnd();

        if (currentRunMode == RunMode.NONE) {
            Assert.isTrue(simulation.getSimState() == SimState.READY);
            stopRequested = false;
            setRunMode(mode);
            doRun();
        }
        else {
            // asyncExec() already scheduled, just change the runMode for it
            setRunMode(mode);
        }
    }

    private Runnable resetRunState = new Runnable() {
        @Override
        public void run() {
            resetRunState();
        }
    };

    private Runnable asyncExec_doRun = new Runnable() {
        @Override
        public void run() {
            Display.getCurrent().asyncExec(new Runnable() {
                @Override
                public void run() {
                    try {
                        if (!isDisposed())
                            doRun();
                    }
                    catch (CommunicationException e) {
                        // nothing -- error dialog and logging is already taken care of in the lower layers
                    }
                    catch (Exception e) {
                        MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
                        SimulationPlugin.logError(e);
                    }
                }
            });
        }
    };

    protected void doRun() throws CommunicationException {
        // should we run at all? (stopRequested is set while we are in asyncExec)
        if (stopRequested || (simulation.getSimState() != SimState.READY && simulation.getSimState() != SimState.RUNNING)) {
            resetRunState();
            return;
        }

        boolean success = false; // for detecting exceptions
        try {
            // determine how much we'll run in this chunk
            long eventDelta = 0;
            long realTimeMillis = 0;
            switch (currentRunMode) {
                case NORMAL: eventDelta = 1; break;
                case FAST: eventDelta = 10; break;
                case EXPRESS: realTimeMillis = 1000; break;
                default: Assert.isTrue(false);
            }

            long untilEvent = (runUntilEventNumber == 0 && eventDelta == 0) ? 0 :
                runUntilEventNumber == 0 ? simulation.getLastEventNumber()+eventDelta :
                    eventDelta == 0 ? runUntilEventNumber :
                        Math.min(runUntilEventNumber, simulation.getLastEventNumber()+eventDelta);

            // tell process to run...
            simulation.sendRunUntilCommand(currentRunMode, realTimeMillis, runUntilSimTime, untilEvent, runUntilModule, runUntilMessage);
            if (currentRunMode == RunMode.NORMAL)
                lastEventAnimationDone = false;

            // ...and wait for it to complete
            refreshUntil(SimState.READY);  //XXX note: error dialog will *precede* animation of last event -- weird!
            fireSimulationStateChanged(Reason.EVENTS_PROCESSED);

            boolean animate = (currentRunMode == RunMode.NORMAL);

            if (simulation.getSimState() != SimState.READY) {
                // likely an error condition -- animate last part if needed, and then we're done
                doNowOrAfterAnimation(animate, resetRunState);
            }
            else {
                // run chunk finished without error
                boolean shouldContinue = simulation.getStoppingReason() == StoppingReason.REALTIMECHUNK ||
                        (simulation.getStoppingReason() == StoppingReason.UNTILEVENT && (runUntilEventNumber==0 || simulation.getNextEventNumber() < runUntilEventNumber));
                if (!shouldContinue) {
                    // until limit reached or simulation terminated -- animate last part if needed, and then we're done
                    doNowOrAfterAnimation(animate, resetRunState);
                }
                else {
                    // animate, or asyncExec next run chunk
                    doNowOrAfterAnimation(animate, asyncExec_doRun);
                }
            }
            success = true;
        }
        finally {
            // if there was an exception, we have to reset our state, tell the process to stop the simulation, etc.
            if (!success)
                stop();
        }
    }

    protected void doNowOrAfterAnimation(boolean animate, Runnable runnable) {
        if (animate) {
            EventEntry lastEvent = getSimulation().getLogBuffer().getLastEventEntry();
            liveAnimationController.startAnimatingLastEvent(lastEvent, runnable);
        }
        else {
            runnable.run();
        }
    }

    public void stop() throws CommunicationException {
        // make sure doRun()'s asyncExec() code won't do anything (unfortunately Display does not offer a ways to cancel asyncExec code)
        stopRequested = true;

        // cancel animation
        if (liveAnimationController.isAnimating())
            liveAnimationController.cancelAnimation();

        // stop the underlying simulation
        if (simulation.getSimState() == SimState.RUNNING) {
            try {
                simulation.sendStopCommand();
            } finally {
                refreshUntil(SimState.READY);
            }
        }

        // update the UI
        if (currentRunMode != RunMode.NONE)
            resetRunState();
    }

    protected void resetRunState() {
        lastEventAnimationDone = true;
        runUntilSimTime = null;
        runUntilEventNumber = 0;
        runUntilMessage = null;
        runUntilModule = null;
        setRunMode(RunMode.NONE);
    }

    protected void setRunMode(RunMode runMode) {
        if (currentRunMode != runMode) {
            RunMode oldRunMode = currentRunMode;
            currentRunMode = runMode;
            fireSimulationStateChanged(new SimulationChangeEvent(Reason.RUNMODE_CHANGE, oldRunMode, currentRunMode));
        }
    }

    public void callFinish() throws CommunicationException {
        // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR but it comes handy in practice...
        SimState state = simulation.getSimState();
        Assert.isTrue(state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR);
        simulation.sendCallFinishCommand();
        refreshUntil(SimState.FINISHCALLED); //TODO in background thread, plus callback in the end?
    }

    public void refreshStatus() throws CommunicationException {
        refreshUntil(null);
    }

    /**
     * Repeatedly issue refreshStatus() until the state reported by simulation process becomes
     * the expected state, or a quasi terminal state (TERMINATED, ERROR or DISCONNECTED).
     * The caller should check which one occurred.
     */
    public void refreshUntil(SimState expectedState) throws CommunicationException {
        //TODO this method is called after sending commands for potentially long-running operations
        // in the simulation process, e.g. setting up a network, calling finish, or processing and event.
        // Then HTTP (or this loop) will keep up the UI thread and make the IDE nonresponsive.
        // So, this whole thing should be probably done in a background thread, and report completion
        // via a callback...
        //TODO or, at least bring up a cancellable progress dialog after a few seconds

        Assert.isTrue(simulationUICallback != null); // callbacks must be set
        long startTime = System.currentTimeMillis();
        int retries = 0;
        do {
            boolean again;
            do {
                //TODO implement PAUSE/RESUME commands in Cmdenv!! this method calls getObjectINfo even while
                // simulation is running!!!! or while network is being set up! this could be solved with PAUSE/RESUME

                // refresh status
                SimState oldState = simulation.getSimState();
                StatusResponse response = simulation.refreshStatus();
                if (oldState != simulation.getSimState())
                    fireSimulationStateChanged(new SimulationChangeEvent(Reason.SIMSTATE_CHANGE, oldState, simulation.getSimState()));
                fireSimulationStateChanged(Reason.STATUS_REFRESH);

                again = false;

                if (response != null) {
                    // allow the UI to be updated before we pop up an parameter prompt or error dialog
                    simulation.refreshObjectCache();
                    fireSimulationStateChanged(Reason.OBJECTCACHE_REFRESH);

                    // carry out action requested by the simulation
                    if (response instanceof AskParameterRequest) {
                        // parameter value prompt
                        AskParameterRequest info = (AskParameterRequest)response;
                        String value = simulationUICallback.askParameter(info.paramName, info.ownerFullPath, info.paramType, info.prompt, info.defaultValue, info.unit, info.choices);
                        simulation.sendReply(value);
                        again = true;
                    }
                    else if (response instanceof GetsRequest) {
                        // parameter value prompt
                        GetsRequest info = (GetsRequest)response;
                        String value = simulationUICallback.gets(info.prompt, info.defaultValue);
                        simulation.sendReply(value);
                        again = true;
                    }
                    else if (response instanceof AskYesNoRequest) {
                        // parameter value prompt
                        AskYesNoRequest info = (AskYesNoRequest)response;
                        boolean value = simulationUICallback.askYesNo(info.message);
                        simulation.sendReply(value ? "y" : "n");
                        again = true;
                    }
                    else if (response instanceof MsgDialogRequest) {
                        // parameter value prompt
                        MsgDialogRequest info = (MsgDialogRequest)response;
                        simulationUICallback.messageDialog(info.message);
                        simulation.sendReply("");
                        again = true;
                    }
                    else {
                        Assert.isTrue(false, "unknown StatusResponse");
                    }
                    retries = 0;
                }
            } while (again);

            SimState state = simulation.getSimState();
            if (!isOnline() || expectedState == null || state == expectedState || state == SimState.FINISHCALLED || state == SimState.TERMINATED || state == SimState.ERROR)
                break;

            if (retries > 1)
                try { Thread.sleep(retries <= 5 ? 100 : 500); } catch (InterruptedException e) {}

            retries++;

        } while (true);

        // UI update
        simulation.refreshObjectCache(); // note: if state is still "running" (shouldn't be), the simulation may have progressed since the last simulation.refreshStatus() call, so results might be inconsistent
        fireSimulationStateChanged(Reason.OBJECTCACHE_REFRESH);

        Debug.println("SimulationController.refreshUntil(): " + (System.currentTimeMillis() - startTime) + "ms\n");
    }

    /**
     * Put the simulation front-end into the "transient communication failure" mode.
     */
    public void goOffline() {
        Assert.isTrue(connState == ConnState.ONLINE);
        setConnectionState(ConnState.OFFLINE);

//            //FIXME dialog -- move this into the UI part (editor)
//            Display.getDefault().asyncExec(new Runnable() {
//                @Override
//                public void run() {
//                    //FIXME obey "don't show again" !!!!
//                    final String KEY_SUPPRESSDIALOGONCOMMUNICATIONERROR = "suppressDialogOnCommunicationError";
//                    MessageDialogWithToggle.openError(
//                            PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(),
//                            "Communication Error",
//                            "An error occurred while talking to the simulation process.\n" +
//                            "\n" +
//                            "To prevent a cascade of further such errors, all communication " +
//                            "with the simulation process has been suspended until you hit Refresh. " +
//                            "In this reduced mode, you may still use the UI, open inspectors, browse objects " +
//                            "or the module log, etc., but be aware that any information you see on the screen " +
//                            "may be obsolete or invalid. Refresh will attempt to re-establish communication " +
//                            "with the simulation process and return to normal operation.",
//                            "Don't show this message again",
//                            false,
//                            SimulationPlugin.getDefault().getPreferenceStore(),
//                            KEY_SUPPRESSDIALOGONCOMMUNICATIONERROR
//                            );
//                }
//            });
    }

    public void goOnline() {
        Assert.isTrue(connState == ConnState.OFFLINE || connState == ConnState.RESUMABLE);
        if (!connectDone) {
            connect(); // if we've never had a successful ping, we may need to wait until process starts up and opens socket
        }
        else {
            setConnectionState(ConnState.ONLINE);
            try {
                refreshStatus();
            } catch (CommunicationException e) { }
        }
    }

    @Override
    public void communicationInterrupted() {
        SimulationPlugin.logError("Interrupted", null);
    }

    @Override
    public void transientCommunicationFailure(Exception e) {
        // go to failure mode until user hits Refresh
        SimulationPlugin.logError("Transient communication error, going offline", e);
        goOffline();
    }

    @Override
    public void fatalCommunicationError(SocketException e) {
        SimulationPlugin.logError("Fatal communication error, going to state DISCONNECTED", e);
        setConnectionState(ConnState.DISCONNECTED);
    }

    @Override
    public void simulationProcessSuspended() {
        Debug.println("Process suspended");
        if (!isDisposed()) {
            simulation.abortOngoingHttpRequest();
            setConnectionState(ConnState.SUSPENDED);
            if (resumableStateDelayedJob != null)
                resumableStateDelayedJob.cancel();
            if (reconnectDelayedJob != null)
                reconnectDelayedJob.cancel();
        }
    }

    @Override
    public void simulationProcessResumed() {
        Debug.println("Process resumed");
        if (!isDisposed()) {
            // enter RESUMABLE after a delay. We must wait with actually trying to contact 
            // the simulation process again, because this Resume may be immediately followed 
            // by a Suspend; this occurs e.g. when the user single-steps through the code 
            // in the debugger.
            if (resumableStateDelayedJob == null)
                createDelayedJobs();
            resumableStateDelayedJob.restartTimer();
        }
    }

    @Override
    public void simulationProcessTerminated() {
        setConnectionState(ConnState.DISCONNECTED);
    }

    protected void createDelayedJobs() {
        // sets state=RESUMABLE after 0.5s
        resumableStateDelayedJob = new DelayedJob(500) {
            @Override
            public void run() {
                if (!isDisposed()) {
                    setConnectionState(ConnState.RESUMABLE);
                    reconnectDelayedJob.restartTimer();
                }
            }
        };

        // goes online after 2s delay
        reconnectDelayedJob = new DelayedJob(2000) {
            @Override
            public void run() {
                if (!isDisposed() && getConnectionState() == ConnState.RESUMABLE)
                    goOnline();
            }
        };
    }

    public void dispose() {
        //TODO try sendQuitCommand() first
        if (cancelJobOnDispose && simulationProcess.canCancel())
            simulationProcess.cancel();
        simulationProcess.removeListener(this);
        simulationChangeListeners = null;
        isDisposed = true;
        //TODO cancel timers, etc.
    }

    protected void fireSimulationStateChanged(SimulationChangeEvent.Reason reason) {
        fireSimulationStateChanged(new SimulationChangeEvent(reason));
    }

    protected void fireSimulationStateChanged(SimulationChangeEvent event) {
        Assert.isTrue(Display.getCurrent() != null); // mandate that we are in the UI thread, as listeners are typically UI related
        event.simulationController = this;
        for (final Object listener : simulationChangeListeners.getListeners()) {
            try {
                ((ISimulationChangeListener) listener).simulationStateChanged(event);
            } catch (Exception e) {
                SimulationPlugin.logError("Error invoking simulation listener " + listener, e);
            }
        }
    }
}

