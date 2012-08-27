package org.omnetpp.simulation.controller;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.ConnectException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Arrays;
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
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.json.JSONReader;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.liveanimation.LiveAnimationController;
import org.omnetpp.simulation.model.cComponent;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPar;

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

    private static final int MONGOOSE_MAX_REQUEST_URI_SIZE = 256*1024-1000; // see MAX_REQUEST_SIZE in mongoose.h

    private enum ContentToLoadEnum { OBJECT, FIELDS };

    private String urlBase;

    private Job launcherJob; // we want to be notified when the launcher job exits (== simulation process exits); may be null
    private IJobChangeListener jobChangeListener;
    private boolean cancelJobOnDispose;  // whether to kill the simulation launcher job when the controller is disposed

    private ISimulationCallback simulationCallback;
    private ListenerList simulationListeners = new ListenerList(); // listeners we have to notify on changes
//    private AnimationPlaybackController animationPlaybackController; //TODO get rid of this reference in this class if possible
    private LiveAnimationController liveAnimationController;

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
    private Map<String, cObject> rootObjects = new HashMap<String, cObject>(); // keys: "simulation", "network", etc.
    private Map<Long, cObject> cachedObjects = new HashMap<Long, cObject>();
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

//    public void setAnimationPlaybackController(AnimationPlaybackController animationPlaybackController) {
//        this.animationPlaybackController = animationPlaybackController;
//        this.animationPlaybackController.getAnimationController().addAnimationListener(new AnimationAdapter() {
//            @Override
//            public void animationStopped() {
//                SimulationController.this.animationStopped();
//            }
//        });
//    }

    public void setLiveAnimationController(LiveAnimationController liveAnimationController) {
        this.liveAnimationController = liveAnimationController;
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
        long startTime = System.currentTimeMillis();
        boolean again;
        do {
            Assert.isNotNull(simulationCallback);  // simulationCallback must be set

            //TODO merge "status" and "getRootObjectIds" into one! also "getObjectInfo" and "getObjectChildren"

            // refresh basic simulation state
            Object responseJSON = getPageContentAsJSON(urlBase + "sim/status");
            //System.out.println(responseJSON.toString());
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
                String rootRef = (String) ((Map) json).get(key);
                cObject rootObj = getObjectByJSONRef(rootRef);
                rootObjects.put((String) key, rootObj);
            }

            // load the log
            List logEntries = (List) responseMap.get("log");
            EventEntry lastEventEntry = null;
            List<Object> logItems = new ArrayList<Object>();
            for (Object e : logEntries) {
                Map logEntry = (Map)e;
                String type = (String)logEntry.get("@");
                if (type.equals("E")) {
                    if (!logItems.isEmpty()) {
                        if (lastEventEntry == null)
                            logBuffer.addEventEntry(lastEventEntry = new EventEntry());
                        lastEventEntry.logItems = logItems.toArray(new Object[]{});
                        logItems.clear();
                    }

                    lastEventEntry = new EventEntry();
                    lastEventEntry.eventNumber = ((Number)logEntry.get("#")).longValue();
                    lastEventEntry.simulationTime = BigDecimal.parse((String)logEntry.get("t"));
                    lastEventEntry.moduleId = ((Number)logEntry.get("moduleId")).intValue();
                    lastEventEntry.moduleFullPath = (String) logEntry.get("moduleFullPath");
                    lastEventEntry.moduleNedType = (String) logEntry.get("moduleNedType");
                    lastEventEntry.messageName = (String)logEntry.get("messageName");
                    lastEventEntry.messageClassName = (String)logEntry.get("messageClassName");
                    logBuffer.addEventEntry(lastEventEntry);
                }
                else if (type.equals("L")) {
                    String line = (String)logEntry.get("txt");
                    logItems.add(line);
                }
                else if (type.equals("MB")) {
                    Anim.ComponentMethodBeginEntry item = new Anim.ComponentMethodBeginEntry();
                    item.srcModuleId = ((Number)logEntry.get("sm")).intValue();
                    item.destModuleId = ((Number)logEntry.get("tm")).intValue();
                    item.txt = (String) (String)logEntry.get("m");
                    logItems.add(item);
                }
                else if (type.equals("ME")) {
                    Anim.ComponentMethodEndEntry item = new Anim.ComponentMethodEndEntry();
                    logItems.add(item);
                }
                else if (type.equals("BS")) {  //TODO no need for the simulation to send these entries (BS..ES) in Fast mode! (and of course not in Express mode)
                    Anim.BeginSendEntry item = new Anim.BeginSendEntry();
                    item.msg = (cMessage) getObjectByJSONRef((String)logEntry.get("msg"));
                    logItems.add(item);
                }
                else if (type.equals("SH")) {
                    Anim.MessageSendHopEntry item = new Anim.MessageSendHopEntry();
                    item.srcGate = (cGate) getObjectByJSONRef((String)logEntry.get("srcGate"));
                    item.propagationDelay = defaultBigDecimalIfNull((String)logEntry.get("propagationDelay"), null);
                    item.transmissionDelay = defaultBigDecimalIfNull((String)logEntry.get("transmissionDelay"), null);
                    logItems.add(item);
                }
                else if (type.equals("SD")) {
                    Anim.MessageSendDirectEntry item = new Anim.MessageSendDirectEntry();
                    item.srcModule = (cModule) getObjectByJSONRef((String)logEntry.get("srcModule"));
                    item.destGate = (cGate) getObjectByJSONRef((String)logEntry.get("destGate"));
                    item.propagationDelay = defaultBigDecimalIfNull((String)logEntry.get("propagationDelay"), null);
                    item.transmissionDelay = defaultBigDecimalIfNull((String)logEntry.get("transmissionDelay"), null);
                    logItems.add(item);
                }
                else if (type.equals("ES")) {
                    Anim.EndSendEntry item = new Anim.EndSendEntry();
                    logItems.add(item);
                }
                else {
                    throw new RuntimeException("type: '" + type + "'");
                }
            }
            if (!logItems.isEmpty()) {
                if (lastEventEntry == null)
                    logBuffer.addEventEntry(lastEventEntry = new EventEntry());
                lastEventEntry.logItems = logItems.toArray(new Object[]{});
                logItems.clear();
            }

            logBuffer.fireChangeNotification();  // tell everyone interested about the change

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

            //XXX this 'garbage removal' stuff is simply WRONG now -- objects can still remain in memory via references from other objects
            List<Long> garbage = new ArrayList<Long>();
            for (Long id : cachedObjects.keySet()) {
                if (cachedObjects.get(id).lastAccessSerial - refreshSerial > MAX_AGE)
                    garbage.add(id);
            }
            cachedObjects.keySet().removeAll(garbage);

            //TODO add the objects referenced by the new logBuffer entries too
            List<cObject> filledObjects = new ArrayList<cObject>();
            for (cObject obj : cachedObjects.values()) {
                if (obj.isFilledIn() && !(obj instanceof cComponent || obj instanceof cGate || obj instanceof cPar)) //XXX hack: do not repeatedly reload modules!!!
                    filledObjects.add(obj);
            }
            doLoadObjects(filledObjects, ContentToLoadEnum.OBJECT);

            // refresh the detail fields of loaded objects too (where filled in)
            List<cObject> objectsWithFieldsLoaded = new ArrayList<cObject>();
            for (cObject obj : cachedObjects.values()) {
                if (obj.isFieldsFilledIn())
                    objectsWithFieldsLoaded.add(obj);
            }
            doLoadObjects(objectsWithFieldsLoaded, ContentToLoadEnum.FIELDS);

            //System.out.println("refreshStatus notifyListeners() follows:");
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
        System.out.println("\n*** SimulationController.refreshStatus(): " + (System.currentTimeMillis() - startTime) + "ms\n");
    }

    public boolean hasRootObjects() {
        return !rootObjects.isEmpty();
    }

    /**
     * Use ROOTOBJ_xxx constants as keys.
     */
    public cObject getRootObject(String key) {
        Assert.isTrue(!rootObjects.isEmpty(), "refresh() needs to be called before getRootObjectId() can be invoked");
        return rootObjects.get(key);
    }

    public void loadObject(cObject object) throws IOException {
        Set<cObject> objects = new HashSet<cObject>();
        objects.add(object);
        doLoadObjects(objects, ContentToLoadEnum.OBJECT);
    }

    public void loadObjectFields(cObject object) throws IOException {
        Set<cObject> objects = new HashSet<cObject>();
        objects.add(object);
        doLoadObjects(objects, ContentToLoadEnum.FIELDS);
    }

    public void loadObjects(cObject[] objects) throws IOException {
        doLoadObjects(Arrays.asList(objects), ContentToLoadEnum.OBJECT);
    }

    public void loadObjects(List<? extends cObject>objects) throws IOException {
        doLoadObjects(objects, ContentToLoadEnum.OBJECT);
    }

    //TODO we need a loadUnfilledObjectsAndTheirFields() too!!!  btw, why not 1 method and 2 flags: "unfilled objects only", "load fields too" ????
    public void loadUnfilledObjects(cObject[] objects) throws IOException {
        loadUnfilledObjects(Arrays.asList(objects));
    }

    public void loadUnfilledObjects(Collection<? extends cObject> objects) throws IOException {
        // load objects that are not yet filled in
        List<cObject> missing = new ArrayList<cObject>();
        for (cObject obj : objects) {
            if (!obj.isFilledIn())
                missing.add(obj);
        }
        doLoadObjects(missing, ContentToLoadEnum.OBJECT);
    }

    @SuppressWarnings("rawtypes")
    protected void doLoadObjects(Collection<? extends cObject> objects, ContentToLoadEnum what) throws IOException {
        if (objects.isEmpty())
            return;
        StringBuilder buf = new StringBuilder();
        for (cObject obj: objects)
            buf.append(obj.getObjectId()).append(',');
        String idsArg = buf.substring(0, buf.length()-1);  // trim trailing comma
        Object json = getPageContentAsJSON(urlBase + "/sim/getObjectInfo?what=" + (what==ContentToLoadEnum.OBJECT ? "ic" : "d") + "&ids=" + idsArg);

        // process response; objects not in the response no longer exist, purge them from the cache
        List<Long> garbage = new ArrayList<Long>();
        for (cObject obj: objects) {
            Map jsonObjectInfo = (Map) ((Map) json).get(String.valueOf(obj.getObjectId()));
            if (jsonObjectInfo == null) {
                garbage.add(obj.getObjectId()); // calling remove() here would cause ConcurrentModificationException
                obj.markAsDisposed();
            }
            else {
                if (what==ContentToLoadEnum.OBJECT)
                    obj.fillFromJSON(jsonObjectInfo);
                else
                    obj.fillFieldsFromJSON(jsonObjectInfo);
            }
        }
        cachedObjects.keySet().removeAll(garbage);
    }

    public cObject getObjectByJSONRef(String idAndType) {
        if (idAndType.equals("0"))
            return null;
        int colonPos = idAndType.indexOf(':');
        if (colonPos == -1)
            throw new RuntimeException("argument should be in the form \"<id>:<classname>\": " + idAndType);
        long id = Long.valueOf(idAndType.substring(0, colonPos));
        cObject obj = cachedObjects.get(id);
        if (obj != null) {
            return obj;
        }
        else {
            String className = idAndType.substring(colonPos+1);
            obj = createBlankObject(id, className);
            cachedObjects.put(id, obj);
            return obj;
        }
    }

    protected cObject createBlankObject(long id, String knownBaseClass) {
        try {
            String name = "org.omnetpp.simulation.model." + knownBaseClass;
            Class<?> clazz = Class.forName(name);
            return (cObject)clazz.getConstructors()[0].newInstance(this, id);
        }
        catch (Exception e) {
            throw new RuntimeException("internal error: support for known C++ base class " + knownBaseClass, e);
        }
//
//        if (knownBaseClass.equals("cObject")) //TODO more classes; could use reflection
//            return new cObject(this, id);
//        else if (knownBaseClass.equals("cPacket"))
//            return new cPacket(this, id);
//        else if (knownBaseClass.equals("cMessage"))
//            return new cMessage(this, id);
//        else if (knownBaseClass.equals("cModule"))
//            return new cModule(this, id);
//        else if (knownBaseClass.equals("cGate"))
//            return new cGate(this, id);
//        else if (knownBaseClass.equals("cQueue"))
//            return new cQueue(this, id);
//        else if (knownBaseClass.equals("cSimulation"))
//            return new cSimulation(this, id);
//        else if (knownBaseClass.equals("cRegistrationList"))
//            return new cRegistrationList(this, id);
//        else if (knownBaseClass.equals("cMessageHeap"))
//            return new cRegistrationList(this, id);
//        else //TODO: all others from cmdenv.cc
//            throw new RuntimeException("unsupported class " + knownBaseClass);
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
//            animationPlaybackController.jumpToEnd();

            currentRunMode = RunMode.STEP;
            notifyListeners(); // because currentRunMode has changed
            getPageContent(urlBase + "sim/step");
            refreshStatus();

            // animate it
//            animationPlaybackController.play();
            liveAnimationController.startAnimatingLastEvent();
            // Note: currentRunMode = RunMode.NOTRUNNING will be done in animationStopped()!
        }
        else {
            stop(); // if already running, just stop it
        }
    }

    // XXX called from outside
    public void animationStopped() {
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

//        animationPlaybackController.jumpToEnd();

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
//                        animationPlaybackController.stop();  // in case it was running
//                        animationPlaybackController.jumpToEnd(); // show current state on animation canvas
                        doRun();
                    }
                    else if (currentRunMode == RunMode.NORMAL) {
                        // animate it
//                        animationPlaybackController.play();
                        liveAnimationController.startAnimatingLastEvent();
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
        //long startTime = System.currentTimeMillis();
        String response = getPageContent(url);
        if (response == null)
            return new HttpException("Received empty document in response to GET " + url);
        //System.out.println("  - HTTP GET took " + (System.currentTimeMillis() - startTime) + "ms");

        // parse
        //startTime = System.currentTimeMillis();
        Object jsonTree = new JSONReader().read(response);
        //System.out.println("  got: " + jsonTree.toString());
        //System.out.println("  - JSON parsing took " + (System.currentTimeMillis() - startTime) + "ms");
        return jsonTree;
    }

    protected String getPageContent(String url) throws IOException {
        System.out.println("GET " + url);
        if (url.length() > MONGOOSE_MAX_REQUEST_URI_SIZE)
            throw new RuntimeException("Request URL length " + url.length() + "exceeds Mongoose limit " + MONGOOSE_MAX_REQUEST_URI_SIZE);

        if (getState() == SimState.DISCONNECTED)
            throw new IllegalStateException("Simulation process already terminated"); //TODO not good, as we catch IOException always

        // turn off log messages from Apache HttpClient if we can...
        Log log = LogFactory.getLog("org.apache.commons.httpclient");
        if (log instanceof Jdk14Logger)
            ((Jdk14Logger) log).getLogger().setLevel(Level.OFF);

        HttpClient client = new HttpClient();
        int timeoutMillis = 30 * 1000;
        client.getParams().setSoTimeout(timeoutMillis);
        client.getParams().setConnectionManagerTimeout(timeoutMillis);
        client.getHttpConnectionManager().getParams().setSoTimeout(timeoutMillis);
        client.getHttpConnectionManager().getParams().setConnectionTimeout(timeoutMillis);

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
            if (responseBody.length() > 1024)
                System.out.println("  response: ~" + ((responseBody.length()+511)/1024) + "KiB");
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

    private BigDecimal defaultBigDecimalIfNull(String txt, BigDecimal defaultValue) {
        return txt == null ? defaultValue : BigDecimal.parse(txt);
    }

    private int defaultIntegerIfNull(Number i, int defaultValue) {
        return i == null ? defaultValue : i.intValue();
    }

    private long defaultLongIfNull(Number l, long defaultValue) {
        return l == null ? defaultValue : l.longValue();
    }

}
