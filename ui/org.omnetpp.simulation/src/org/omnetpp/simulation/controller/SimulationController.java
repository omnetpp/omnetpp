package org.omnetpp.simulation.controller;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.ConnectException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpMethod;
import org.apache.commons.httpclient.HttpMethodRetryHandler;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.auth.BasicScheme;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.params.HttpMethodParams;
import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.logging.impl.Jdk14Logger;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.animation.controller.AnimationPlaybackController;
import org.omnetpp.animation.controller.AnimationStateAdapter;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.json.JSONReader;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.LogBuffer.EventEntry;
import org.omnetpp.simulation.model.SimObject;

/**
 * Interacts with a simulation process over HTTP.
 *
 * @author Andras
 */
// TODO errors in the simulation should be turned into SimulationException thrown from these methods?
// TODO cmdenv: implement "runUntil" parameter "wallclocktimelimit", to be used with Express (or Fast too)
// TODO finish the "displayerror" and "askparameter" stuff
public class SimulationController {
    // root object IDs
    public static final String ROOTOBJ_SIMULATION = "simulation";
    public static final String ROOTOBJ_FES = "fes";
    public static final String ROOTOBJ_SYSTEMMODULE = "systemModule";
    public static final String ROOTOBJ_DEFAULTLIST = "defaultList";
    public static final String ROOTOBJ_COMPONENTTYPES = "componentTypes";
    public static final String ROOTOBJ_NEDFUNCTIONS = "nedFunctions";
    public static final String ROOTOBJ_CLASSES = "classes";
    public static final String ROOTOBJ_ENUMS = "enums";
    public static final String ROOTOBJ_CLASSDESCRIPTORS = "classDescriptors";
    public static final String ROOTOBJ_CONFIGOPTIONS = "configOptions";
    public static final String ROOTOBJ_RESULTFILTERS = "resultFilters";
    public static final String ROOTOBJ_RESULTRECORDERS = "resultRecorders";

    /**
     * See Cmdenv for state transitions
     */
    public enum SimState {
        DISCONNECTED, // no simulation process, e.g. it has terminated
        NONETWORK, READY, RUNNING, TERMINATED, ERROR, FINISHCALLED  // as defined in cmdenv.h
    };

    public enum RunMode {
        NOTRUNNING, STEP, NORMAL, FAST, EXPRESS
    }

    private String urlBase;

    private Job launcherJob; // we want to be notified when the launcher job exits (== simulation process exits); may be null
    private IJobChangeListener jobChangeListener;
    private boolean cancelJobOnDispose;  // whether to kill the simulation launcher job when the controller is disposed

    private ISimulationCallback simulationCallback;
    private ListenerList simulationListeners = new ListenerList(); // listeners we have to notify on changes
    private AnimationPlaybackController animationPlaybackController; //TODO get rid of this reference in this class if possible

    // simulation status (as returned by the GET "/sim/status" request)
    private String hostName;
    private int portNumber;
    private long processId;
    private SimState state = SimState.NONETWORK;
    private String configName;
    private int runNumber;
    private String networkName;
    private long eventNumber; // last event number
    private BigDecimal simulationTime; // last simulation time
    private long nextEventNumber;
    private BigDecimal nextSimulationTime;
    private int nextEventModuleId;  //TODO display on UI
    private long nextEventMessageId; //TODO display on UI
    private String eventlogFile;

    // run state
    private RunMode currentRunMode = RunMode.NOTRUNNING; // UI state (simulation might be in READY, since UI runs it in increments of n events)
    private BigDecimal runUntilSimTime = null;
    private long runUntilEventNumber = 0;
    private boolean stopRequested;

    // object cache
    private long refreshSerial;
    private Map<String, Long> rootObjectIds = new HashMap<String, Long>(); // keys: "simulation", "network", etc
    private Map<Long, SimObject> cachedObjects = new HashMap<Long, SimObject>();
    private static final long MAX_AGE = 1; // unaccessed objects are removed from the cached after this many refresh cycles

    // module logs
    private LogBuffer logBuffer = new LogBuffer();
    
    /**
     * Constructor.
     */
    public SimulationController(String hostName, int portNumber, Job launcherJob) {
        this.hostName = hostName;
        this.portNumber = portNumber;
        this.urlBase = "http://" + hostName + ":" + portNumber + "/";
        this.launcherJob = launcherJob;
        this.cancelJobOnDispose = launcherJob != null;

        // if simulation was launched via a local launcher job (see SimulationLauncherJob),
        // set up simulationProcessTerminated() to be called when the launcher job exits
        if (launcherJob != null) {
            Job.getJobManager().addJobChangeListener(jobChangeListener = new JobChangeAdapter() {
                @Override
                public void done(IJobChangeEvent event) {
                    if (event.getJob() == SimulationController.this.launcherJob) {
                        Job.getJobManager().removeJobChangeListener(jobChangeListener);
                        SimulationController.this.launcherJob = null;
                        SimulationController.this.jobChangeListener = null;
                        simulationProcessExited();
                    }
                }
            });
        }
    }

    protected void simulationProcessExited() {
        state = SimState.DISCONNECTED;
        notifyListeners();
    }

    public void dispose() {
        if (launcherJob != null) {
            Job.getJobManager().removeJobChangeListener(jobChangeListener);
            if (cancelJobOnDispose && launcherJob.getState() != Job.NONE)
                launcherJob.cancel();
        }
    }

    public boolean canCancelLaunch() {
        return launcherJob != null && launcherJob.getState() != Job.NONE;
    }

    /**
     * Terminates the simulation; may only be called if canCancelLaunch() returns true
     */
    public void cancelLaunch() {
        if (launcherJob != null)
            launcherJob.cancel();
    }

    public void setSimulationCallback(ISimulationCallback simulationCallback) {
        this.simulationCallback = simulationCallback;
    }

    public ISimulationCallback getSimulationCallback() {
        return simulationCallback;
    }
    
    public void setAnimationPlaybackController(AnimationPlaybackController animationPlaybackController) {
        this.animationPlaybackController = animationPlaybackController;
        this.animationPlaybackController.getAnimationController().addAnimationListener(new AnimationStateAdapter() {
            @Override
            public void animationStopped() {
                SimulationController.this.animationStopped();
            }
        });
    }

    public void addSimulationStateListener(ISimulationStateListener listener) {
        simulationListeners.add(listener);
    }

    public void removeSimulationStateListener(ISimulationStateListener listener) {
        simulationListeners.remove(listener);
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

    /**
     * Host where the simulation that we are talking to runs.
     */
    public String getHostName() {
        return hostName;
    }

    public int getPortNumber() {
        return portNumber;
    }

    public String getUrlBase() {
        return urlBase;
    }
    
    /**
     * Process ID of the simulation we are talking to.
     */
    public long getProcessId() {
        return processId;
    }

    public SimState getState() {
        if (state == SimState.READY && currentRunMode != RunMode.NOTRUNNING)
            return SimState.RUNNING; // a well-meant lie
        else
            return state;
    }

    public RunMode getCurrentRunMode() {
        return currentRunMode;
    }

    public boolean isRunUntilActive() {
        // whether a "Run Until" is active
        return currentRunMode != RunMode.NOTRUNNING && (runUntilSimTime != null || runUntilEventNumber != 0);
    }

    public String getConfigName() {
        return configName;
    }

    public int getRunNumber() {
        return runNumber;
    }

    public String getNetworkName() {
        return networkName;
    }

    public int getNextEventModuleId() {
        return nextEventModuleId;
    }

    public long getNextEventMessageId() {
        return nextEventMessageId;
    }

    public long getEventNumber() {
        return eventNumber;
    }

    public BigDecimal getSimulationTime() {
        return simulationTime;
    }

    public long getNextEventNumber() {
        return nextEventNumber;
    }

    public BigDecimal getNextSimulationTime() {
        return nextSimulationTime;
    }

    public String getEventlogFile() {
        return eventlogFile;
    }

    public void setCancelJobOnDispose(boolean cancelJobOnDispose) {
        this.cancelJobOnDispose = cancelJobOnDispose;
    }
    
    public boolean getCancelJobOnDispose() {
        return cancelJobOnDispose;
    }
    
    protected String encode(String s) {
        try {
            return URLEncoder.encode(s, "US-ASCII");
        }
        catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }

    public LogBuffer getLogBuffer() {
        return logBuffer;
    }
    
    @SuppressWarnings("rawtypes")
    public void refreshStatus() throws IOException {
        boolean again;
        do {
            Assert.isNotNull(simulationCallback);  // simulationCallback must be set

            //TODO merge "status" and "getRootObjectIds" into one! also "getObjectInfo" and "getObjectChildren"

            // refresh basic simulation state
            Object responseJSON = getPageContentAsJSON(urlBase + "sim/status");
            System.out.println(responseJSON.toString());
            Map responseMap = (Map) responseJSON;
            hostName = (String) responseMap.get("hostname");
            processId = ((Number) responseMap.get("processid")).longValue();
            state = SimState.valueOf(((String) responseMap.get("state")).toUpperCase());
            eventNumber = defaultLongIfNull((Number) responseMap.get("eventNumber"), 0);
            simulationTime = BigDecimal.parse(StringUtils.defaultIfEmpty((String) responseMap.get("simTime"), "0"));
            nextEventNumber = defaultLongIfNull((Number) responseMap.get("nextEventNumber"), 0);
            nextSimulationTime = BigDecimal.parse(StringUtils.defaultIfEmpty((String) responseMap.get("nextEventSimTime"), "0"));
            configName = (String) responseMap.get("config");
            runNumber = (int) defaultLongIfNull((Number) responseMap.get("run"), -1);
            networkName = (String) responseMap.get("network");
            nextEventModuleId = defaultIntegerIfNull((Number) responseMap.get("nextEventModuleId"), 0);
            nextEventMessageId = defaultLongIfNull((Number) responseMap.get("nextEventMessageId"), 0);
            eventlogFile = (String) responseMap.get("eventlogfile");

            // remember action requested by the simulation
            Map errorInfo = (Map) responseMap.get("errorInfo");
            Map askParameter = (Map) responseMap.get("askParameter");

            // refresh root object IDs
            Object json = getPageContentAsJSON(urlBase + "/sim/getRootObjectIds");
            for (Object key : ((Map) json).keySet()) {
                long objectId = ((Number) ((Map) json).get(key)).longValue();
                rootObjectIds.put((String) key, objectId);
            }

            // load the log
            List logEntries = (List) responseMap.get("log");
            EventEntry lastEventEntry = null;
            List<String> logLines = new ArrayList<String>();
            for (Object e : logEntries) {
                Map logEntry = (Map)e;
                String type = (String)logEntry.get("@");
                if (type.equals("E")) {
                    if (!logLines.isEmpty()) {
                        if (lastEventEntry == null)
                            logBuffer.addEventEntry(lastEventEntry = new EventEntry());
                        lastEventEntry.logLines = logLines.toArray(new String[]{});
                        logLines.clear();
                    }

                    lastEventEntry = new EventEntry();
                    lastEventEntry.eventNumber = ((Number)logEntry.get("#")).longValue();
                    lastEventEntry.simulationTime = BigDecimal.parse((String)logEntry.get("t"));
                    lastEventEntry.moduleId = ((Number)logEntry.get("m")).intValue();
                    //TODO lastEventEntry.moduleFullPath = 
                    //TODO lastEventEntry.moduleNedType = 
                    lastEventEntry.messageName = (String)logEntry.get("msgn");
                    lastEventEntry.messageClassName = (String)logEntry.get("msgt");
                    logBuffer.addEventEntry(lastEventEntry);
                }
                else if (type.equals("L")) {
                    String line = (String)logEntry.get("txt");
                    logLines.add(line);
                }
                else if (type.equals("MB")) {
                    //TODO
                }
                else if (type.equals("ME")) {
                    //TODO
                }
                else {
                    throw new RuntimeException("type: '" + type + "'");
                }
            }
            
            /*
             * Refresh cached objects. Strategy: Maintain a refreshSerial, and for
             * each object also maintain the refreshSerial when they were last
             * accessed. In each refresh() cycle, reload the cached objects that
             * have been accessed recently (say, sometime during the last 3 refresh
             * cycles); evoke the others from the cache as they are unlikely to be
             * accessed in the immediate future. When non-cached objects are
             * requested, load+cache them immediately.
             */
            refreshSerial++;

            List<Long> garbage = new ArrayList<Long>();
            for (Long id : cachedObjects.keySet()) {
                if (cachedObjects.get(id).lastAccessSerial - refreshSerial > MAX_AGE)
                    garbage.add(id);
            }
            cachedObjects.keySet().removeAll(garbage);

            doLoadObjects(cachedObjects.keySet());

            System.out.println("refreshStatus notifyListeners() follows:");
            notifyListeners();

            // carry out action requested by the simulation
            again = false;
            if (askParameter != null) {
                String paramName = (String) askParameter.get("paramName");
                String ownerFullPath = (String) askParameter.get("ownerFullPath");
                String paramType = (String) askParameter.get("paramType");
                String prompt = (String) askParameter.get("prompt");
                String defaultValue = (String) askParameter.get("defaultValue");
                String unit = (String) askParameter.get("unit");
                String[] choices = null; //TODO

                // parameter value prompt
                String value = simulationCallback.askParameter(paramName, ownerFullPath, paramType, prompt, defaultValue, unit, choices);
                getPageContent(urlBase + "sim/askParameterResp?v=" + encode(value)); //TODO how to post "cancel"?

                // we need to refresh again
                again = true;
            }
            if (errorInfo != null) {
                // display error message
                String message = (String) errorInfo.get("message");
                simulationCallback.displayError(message);

                // we need to refresh again
                again = true;
            }
        } while (again);
    }

    public boolean hasRootObjects() {
        return !rootObjectIds.isEmpty();
    }

    /**
     * Use ROOTOBJ_xxx constants as keys.
     */
    public long getRootObjectId(String key) {
        Assert.isTrue(!rootObjectIds.isEmpty(), "refresh() needs to be called before getRootObjectId() can be invoked");
        return rootObjectIds.get(key);
    }

    public boolean isCachedObject(long id) {
        return cachedObjects.containsKey(id);
    }

    /**
     * Loads the object if necessary. When you need several objects, it is more
     * efficient to call loadObjects() beforehand, as it will only issue a
     * single HTTP request into the simulation process.
     */
    public SimObject getCachedObject(long id) throws IOException {
        SimObject obj = cachedObjects.get(id);
        if (obj != null)
            obj.lastAccessSerial = refreshSerial;
        else {
            // load it
            Set<Long> ids = new HashSet<Long>();
            ids.add(id);
            doLoadObjects(ids);
            obj = cachedObjects.get(id);
            //TODO: result is null for invalid or stale object IDs -- remember those too so we won't ask the simulation again and again
        }
        return obj;
    }

    public void loadObjects(Collection<Long> ids) throws IOException {
        // load objects not yet in the cache
        if (!cachedObjects.keySet().containsAll(ids)) {
            Set<Long> missing = new HashSet<Long>(ids);
            missing.removeAll(cachedObjects.keySet());
            doLoadObjects(missing);
        }
    }

    @SuppressWarnings("rawtypes")
    protected void doLoadObjects(Collection<Long> ids) throws IOException {
        String idsArg = StringUtils.join(ids, ",");
        Object json = getPageContentAsJSON(urlBase + "/sim/getObjectInfo?ids=" + idsArg);

        // process response; objects not in the response no longer exist, purge their IDs from the cache
        List<Long> garbage = new ArrayList<Long>();
        for (long id : ids) {
            Map objectInfo = (Map) ((Map) json).get(String.valueOf(id));
            if (objectInfo == null) {
                garbage.add(id); // calling remove() here would cause ConcurrentModificationException
            }
            else {
                fillObjectFromJSON(id, objectInfo);
            }
        }
        cachedObjects.keySet().removeAll(garbage);

        // query and store object children
        json = getPageContentAsJSON(urlBase + "/sim/getObjectChildren?ids=" + idsArg);

        for (long id : ids) {
            SimObject object = cachedObjects.get(id);
            List childObjectIds = (List) ((Map) json).get(String.valueOf(id));
            if (childObjectIds == null || childObjectIds.isEmpty()) {
                if (object != null)
                    object.childObjectIds = ArrayUtils.EMPTY_LONG_ARRAY;
            }
            else {
                Assert.isTrue(object.id == id);
                long[] tmp = new long[childObjectIds.size()];
                for (int i = 0; i < tmp.length; i++)
                    tmp[i] = ((Number) childObjectIds.get(i)).longValue();
                object.childObjectIds = tmp;
            }
        }
    }

    protected void fillObjectFromJSON(long id, Map objectInfo) {
        SimObject object = cachedObjects.get(id);
        if (object == null)
            cachedObjects.put(id, object = new SimObject(id));
        Assert.isTrue(object.id == id);

        String knownBaseClassStr = (String) objectInfo.get("knownBaseClass");
        SimObject.KnownBaseClass knownBaseClass = SimObject.KnownBaseClass.valueOf(knownBaseClassStr);
        //TODO use json tree
        
        object.className = (String) objectInfo.get("className");
        object.name = (String) objectInfo.get("name");
        object.fullName = (String) objectInfo.get("fullName");
        object.fullPath = (String) objectInfo.get("fullPath");
        object.icon = (String) objectInfo.get("icon");
        object.info = (String) objectInfo.get("info");
        object.ownerId = ((Number) objectInfo.get("owner")).longValue();
    }

    @SuppressWarnings("rawtypes")
    public List<ConfigDescription> getConfigDescriptions() throws IOException {
        Object json = getPageContentAsJSON(urlBase + "/sim/enumerateConfigs");
        List<ConfigDescription> result = new ArrayList<ConfigDescription>();

        for (Object jitem : (List) json) {
            Map jmap = (Map) jitem;
            ConfigDescription config = new ConfigDescription();
            config.name = (String) jmap.get("name");
            config.description = (String) jmap.get("description");
            config.numRuns = ((Number) jmap.get("numRuns")).intValue();
            List jextends = (List) jmap.get("extends");
            String[] tmp = new String[jextends.size()];
            for (int i = 0; i < tmp.length; i++)
                tmp[i] = (String) jextends.get(i);
            config.extendsList = tmp;
            result.add(config);
        }
        return result;
    }

    public void newRun(String configName, int runNumber) throws IOException {
        getPageContent(urlBase + "sim/setupRun?config=" + encode(configName) + "&run=" + runNumber);
        refreshStatus();
    }

    public void newNetwork(String networkName) throws IOException {
        getPageContent(urlBase + "sim/setupNetwork?network=" + encode(networkName));
        refreshStatus();
    }

    public boolean isNetworkPresent() {
        return state != SimState.DISCONNECTED && state != SimState.NONETWORK;
    }

    public boolean isSimulationOK() {
        return state == SimState.READY || state == SimState.RUNNING;
    }

    public boolean isRunning() {
        return state == SimState.RUNNING || (state == SimState.READY && currentRunMode != RunMode.NOTRUNNING);
    }

    public void rebuildNetwork() throws IOException {
        getPageContent(urlBase + "sim/rebuild");
        refreshStatus();
    }

    public void step() throws IOException {
        Assert.isTrue(state == SimState.READY);
        if (currentRunMode == RunMode.NOTRUNNING) {
            animationPlaybackController.jumpToEnd();
            
            currentRunMode = RunMode.STEP;
            notifyListeners(); // because currentRunMode has changed
            getPageContent(urlBase + "sim/step");
            refreshStatus();
            
            // animate it
            animationPlaybackController.play();
            // Note: currentRunMode = RunMode.NOTRUNNING will be done in animationStopped()!
        }
        else {
            stop(); // if already running, just stop it
        }
    }

    protected void animationStopped() {
        if (currentRunMode == RunMode.STEP) {
            currentRunMode = RunMode.NOTRUNNING;
            notifyListeners();
        }
        else if (currentRunMode == RunMode.NORMAL) {
            try {
                doRun();
            }
            catch (Exception e) {
                MessageDialog.openError(Display.getDefault().getActiveShell(), "Error", "An error occurred: " + e.getMessage());
                SimulationPlugin.logError("Error running simulation", e);
            }
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
        runUntilSimTime = simTime;  //TODO if time/event already passed, don't do anything
        runUntilEventNumber = eventNumber;

        animationPlaybackController.jumpToEnd();
        
        if (currentRunMode == RunMode.NOTRUNNING) {
            Assert.isTrue(state == SimState.READY);
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
        if (stopRequested) {
            stopRequested = false;
            currentRunMode = RunMode.NOTRUNNING;
            notifyListeners(); // because currentRunMode has changed
            return;
        }

        //TODO exit if "until" limit has been reached

        long eventDelta = -1;
        switch (currentRunMode) {
            case NORMAL: eventDelta = 1; break;
            case FAST: eventDelta = 10; break;
            case EXPRESS: eventDelta = 1000; break;  //TODO: express should rather use wall-clock seconds as limit!!!
            default: Assert.isTrue(false);
        }

        long untilEvent = runUntilEventNumber == 0 ? eventNumber+eventDelta : Math.min(runUntilEventNumber, eventNumber+eventDelta);
        String untilArgs = "&uevent=" + untilEvent;
        if (runUntilSimTime != null)
            untilArgs += "&utime=" + runUntilSimTime.toString();

        getPageContent(urlBase + "sim/run?mode=" + currentRunMode.name().toLowerCase() + untilArgs);
        refreshStatus(); // note: state will be READY here again

        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    if (currentRunMode == RunMode.FAST || currentRunMode == RunMode.EXPRESS) {
                        animationPlaybackController.stop();  // in case it was running
                        animationPlaybackController.jumpToEnd(); // show current state on animation canvas
                        doRun();
                    }
                    else if (currentRunMode == RunMode.NORMAL) {
                        // animate it
                        animationPlaybackController.play();
                        // Note: next doRun() will be called from animationStopped()!
                    }
                }
                catch (IOException e) {
                    MessageDialog.openError(Display.getDefault().getActiveShell(), "Error", "An error occurred: " + e.getMessage());
                    SimulationPlugin.logError("Error running simulation", e);
                }
            }
        });
    }

    public void stop() throws IOException {
        stopRequested = true;
        if (state == SimState.RUNNING) {
            try {
                getPageContent(urlBase + "sim/stop");
            } finally {
                refreshStatus();
            }
        }
    }

    public void callFinish() throws IOException {
        // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR but it comes handy in practice...
        Assert.isTrue(state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR);
        getPageContent(urlBase + "sim/callFinish");
        refreshStatus();
    }

    protected Object getPageContentAsJSON(String url) throws IOException {
        String response = getPageContent(url);
        if (response == null)
            return new HttpException("Received empty document in response to GET " + url);

        // parse
        Object jsonTree = new JSONReader().read(response);
//        System.out.println("  got: " + jsonTree.toString());
        return jsonTree;
    }

    protected String getPageContent(String url) throws IOException {
        System.out.println("GET " + url);

        if (getState() == SimState.DISCONNECTED)
            throw new IllegalStateException("Simulation process already terminated"); //TODO not good, as we catch IOException always

        // turn off log messages from Apache HttpClient if we can...
        Log log = LogFactory.getLog("org.apache.commons.httpclient");
        if (log instanceof Jdk14Logger)
            ((Jdk14Logger) log).getLogger().setLevel(Level.OFF);

        HttpClient client = new HttpClient();
        client.getParams().setSoTimeout(10 * 1000);
        client.getParams().setConnectionManagerTimeout(10 * 1000);
        client.getHttpConnectionManager().getParams().setSoTimeout(10 * 1000);
        client.getHttpConnectionManager().getParams().setConnectionTimeout(10 * 1000);

        // do not retry
        HttpMethodRetryHandler noRetryhandler = new HttpMethodRetryHandler() {
            public boolean retryMethod(final HttpMethod method, final IOException exception, int executionCount) {
                return false;
            }
        };

        GetMethod method = new GetMethod(url);
        method.getProxyAuthState().setAuthScheme(new BasicScheme());
        method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, noRetryhandler);
        method.setDoAuthentication(true);

        try {
            int status = client.executeMethod(method);
            if (status != HttpStatus.SC_OK)
                throw new HttpException("Received non-\"200 OK\" HTTP status: " + status);
            String responseBody = method.getResponseBodyAsString();
            return responseBody;
        }
        catch (ConnectException e) {
            connectionRefused(e);
            throw e;
        }
        finally {
            method.releaseConnection();
        }
    }

    protected void connectionRefused(ConnectException e) {
        state = SimState.DISCONNECTED;
        notifyListeners();
    }

    private int defaultIntegerIfNull(Number i, int defaultValue) {
        return i== null ? defaultValue : i.intValue();
    }

    private long defaultLongIfNull(Number l, long defaultValue) {
        return l == null ? defaultValue : l.longValue();
    }

}
