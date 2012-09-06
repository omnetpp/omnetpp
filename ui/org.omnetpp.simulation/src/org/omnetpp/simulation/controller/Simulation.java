package org.omnetpp.simulation.controller;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.lang.ref.WeakReference;
import java.net.SocketException;
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
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.json.JSONReader;
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
public class Simulation {
    static boolean debugHttp = true;
    static boolean debugCache = true;

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

    private ISimulationUICallback simulationUICallback;
    private ISimulationCallback simulationCallback;

    // simulation status (as returned by the GET "/sim/status" request)
    private String hostName;
    private int portNumber;
    private long processId;
    private SimState state = SimState.NONETWORK;
    private String configName;
    private int runNumber;
    private String networkName;
    private long lastEventNumber; // last event's event number
    private BigDecimal lastEventSimulationTime = BigDecimal.getZero(); // last event's simulation time
    //TODO last event module and message
    private long nextEventNumber;
    private BigDecimal nextEventSimulationTimeGuess = BigDecimal.getZero(); // hw-in-the-loop may inject new event before first one in the FES!
    private int nextEventModuleIdGuess;  //TODO why we use module ID? why not the module?
    private long nextEventMessageIdGuess; //TODO display on UI
    private String eventlogFile;

    // object cache
    private Map<String, cObject> rootObjects = new HashMap<String, cObject>(); // keys: "simulation", "network", etc.
    private Map<Long, WeakReference<cObject>> cachedObjects = new HashMap<Long, WeakReference<cObject>>();

    // module logs
    private LogBuffer logBuffer = new LogBuffer();

    /**
     * Constructor.
     */
    public Simulation(String hostName, int portNumber, Job launcherJob) {
        this.hostName = hostName;
        this.portNumber = portNumber;
        this.urlBase = "http://" + hostName + ":" + portNumber + "/";
        this.launcherJob = launcherJob;
        this.cancelJobOnDispose = launcherJob != null;

        // if simulation was launched via a local launcher job (see SimulationLauncherJob),
        // set up simulationProcessExited() to be called when the launcher job exits
        if (launcherJob != null) {
            Job.getJobManager().addJobChangeListener(jobChangeListener = new JobChangeAdapter() {
                @Override
                public void done(IJobChangeEvent event) {
                    if (event.getJob() == Simulation.this.launcherJob) {
                        Job.getJobManager().removeJobChangeListener(jobChangeListener);
                        Simulation.this.launcherJob = null;
                        Simulation.this.jobChangeListener = null;
                        simulationProcessExited();
                    }
                }
            });
        }
    }

    protected void simulationProcessExited() {
        state = SimState.DISCONNECTED;
        simulationCallback.simulationProcessExited();
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

    public void setSimulationUICallback(ISimulationUICallback simulationUICallback) {
        this.simulationUICallback = simulationUICallback;
    }

    public ISimulationUICallback getSimulationUICallback() {
        return simulationUICallback;
    }

    public void setSimulationCallback(ISimulationCallback simulationCallback) {
        this.simulationCallback = simulationCallback;
    }

    public ISimulationCallback getSimulationCallback() {
        return simulationCallback;
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
        return state;
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

    public long getLastEventNumber() {
        return lastEventNumber;
    }

    public BigDecimal getLastEventSimulationTime() {
        return lastEventSimulationTime;
    }

    public long getNextEventNumber() {
        return nextEventNumber;
    }

    public BigDecimal getNextEventSimulationTimeGuess() {
        return nextEventSimulationTimeGuess;
    }

    public int getNextEventModuleIdGuess() {
        return nextEventModuleIdGuess;
    }

    public long getNextEventMessageIdGuess() {
        return nextEventMessageIdGuess;
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

    public LogBuffer getLogBuffer() {
        return logBuffer;
    }

    @SuppressWarnings("rawtypes")
    public void refreshStatus() throws IOException {
        Assert.isTrue(simulationUICallback != null && simulationCallback != null); // callbacks must be set
        long startTime = System.currentTimeMillis();
        boolean again;
        do {

            //TODO merge "status" and "getRootObjectIds" into one! also "getObjectInfo" and "getObjectChildren"

            // refresh basic simulation state
            Object responseJSON = getPageContentAsJSON(urlBase + "sim/status");
            Map responseMap = (Map) responseJSON;
            hostName = (String) responseMap.get("hostname");
            processId = ((Number) responseMap.get("processid")).longValue();
            configName = (String) responseMap.get("config");
            runNumber = (int) defaultLongIfNull((Number) responseMap.get("run"), -1);
            networkName = (String) responseMap.get("network");
            state = SimState.valueOf(((String) responseMap.get("state")).toUpperCase());
            eventlogFile = (String) responseMap.get("eventlogfile");

            lastEventNumber = defaultLongIfNull((Number) responseMap.get("lastEventNumber"), 0);  //TODO rename JSON fields, also in cmdenv.cc!!!
            lastEventSimulationTime = BigDecimal.parse(StringUtils.defaultIfEmpty((String) responseMap.get("lastEventSimtime"), "0"));
            nextEventNumber = defaultLongIfNull((Number) responseMap.get("nextEventNumber"), 0);
            nextEventSimulationTimeGuess = BigDecimal.parse(StringUtils.defaultIfEmpty((String) responseMap.get("nextEventSimtimeGuess"), "0"));
            nextEventModuleIdGuess = defaultIntegerIfNull((Number) responseMap.get("nextEventModuleIdGuess"), 0);
            nextEventMessageIdGuess = defaultLongIfNull((Number) responseMap.get("nextEventMessageIdGuess"), 0);

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

            //
            // Maintain object cache:
            // - purge objects from cache that are unreferenced in Java, or have been deleted from C++;
            //   also unload (~clear) objects that have not been accessed lately, to reduce HTTP load 
            //   and allow other objects they reference to be garbage collected
            // - refresh contents of already-filled objects
            // - refresh the detail fields of loaded objects too (where filled in)
            //
            List<Long> garbage = new ArrayList<Long>();
            List<cObject> objectsToReload = new ArrayList<cObject>();
            List<cObject> objectsToReloadFields = new ArrayList<cObject>();
            int numFilled = 0, numUnloads = 0; 
            for (Long id : cachedObjects.keySet()) {
                cObject obj = cachedObjects.get(id).get();
                if (obj == null) {
                    garbage.add(id);
                }
                else {
                    Assert.isTrue(!obj.isDisposed(), "deleted objects should not be in the cache");
                    if (obj.isFilledIn()) {
                        numFilled++;
                        if (obj.getLastAccessEventNumber() < lastEventNumber-1) { //FIXME -1 is not good if simulation advanced >1 events since last refresh!
                            obj.unload();
                            numUnloads++;
                        }
                        else if (!(obj instanceof cComponent || obj instanceof cGate || obj instanceof cPar)) //XXX hack: do not repeatedly reload modules!!!
                            objectsToReload.add(obj);
                    }

                    if (obj.isFieldsFilledIn()) {
                        objectsToReloadFields.add(obj);
                    }
                }
            }
            cachedObjects.keySet().removeAll(garbage);
            doLoadObjects(objectsToReload, ContentToLoadEnum.OBJECT);
            doLoadObjects(objectsToReloadFields, ContentToLoadEnum.FIELDS);

            if (debugCache) 
                Debug.println("Object cache after refresh: size " + cachedObjects.size() + " (" + (numFilled-numUnloads) + " filled); " +
                        "refresh purged " + garbage.size() + ", unloaded " + numUnloads + ", reloaded " + objectsToReload.size() + ", fields-reloaded " + objectsToReloadFields.size());

            // allow the UI to be updated before we ask parameters or pop up and error dialog
            simulationCallback.statusRefreshed();

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
                String value = simulationUICallback.askParameter(paramName, ownerFullPath, paramType, prompt, defaultValue, unit, choices);
                getPageContent(urlBase + "sim/askParameterResp?v=" + urlEncode(value)); //TODO how to post "cancel"?

                // we need to refresh again
                again = true;
            }
            if (errorInfo != null) {
                // display error message
                String message = (String) errorInfo.get("message");
                simulationUICallback.displayError(message);

                // we need to refresh again
                again = true;
            }
        } while (again);
        if (debugHttp) Debug.println("Simulation.refreshStatus(): " + (System.currentTimeMillis() - startTime) + "ms\n");
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
        WeakReference<cObject> ref = cachedObjects.get(id);
        cObject obj = ref == null ? null : ref.get();
        if (obj != null) {
            return obj;
        }
        else {
            String className = idAndType.substring(colonPos+1);
            obj = createBlankObject(id, className);
            cachedObjects.put(id, new WeakReference<cObject>(obj));
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

    public void sendSetupRunCommand(String configName, int runNumber) throws IOException {
        getPageContent(urlBase + "sim/setupRun?config=" + urlEncode(configName) + "&run=" + runNumber);
    }

    public void sendSetupNetworkCommand(String networkName) throws IOException {
        getPageContent(urlBase + "sim/setupNetwork?network=" + urlEncode(networkName));
    }

    public void sendRebuildNetworkCommand() throws IOException {
        getPageContent(urlBase + "sim/rebuild");
    }

    public void sendStepCommand() throws IOException {
        getPageContent(urlBase + "sim/step");
    }

    public void sendRunCommand(RunMode mode) throws IOException {
        sendRunUntilCommand(mode, null, 0);
    }

    public void sendRunUntilCommand(RunMode mode, BigDecimal untilSimTime, long untilEventNumber) throws IOException {
        String untilArgs = "";
        if (untilEventNumber > 0)
            untilArgs += "&uevent=" + untilEventNumber;
        if (untilSimTime != null)
            untilArgs += "&utime=" + untilSimTime.toString();
        getPageContent(urlBase + "sim/run?mode=" + mode.name().toLowerCase() + untilArgs);
    }

    public void sendStopCommand() throws IOException {
        getPageContent(urlBase + "sim/stop");
    }

    public void sendCallFinishCommand() throws IOException {
        // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR but it comes handy in practice...
        Assert.isTrue(state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR);
        getPageContent(urlBase + "sim/callFinish");
    }

    protected String urlEncode(String s) {
        try {
            return URLEncoder.encode(s, "US-ASCII");
        }
        catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }

    protected Object getPageContentAsJSON(String url) throws IOException {
        //long startTime = System.currentTimeMillis();
        String response = getPageContent(url);
        if (response == null)
            return new HttpException("Received empty document in response to GET " + url);
        //if (debug) Debug.println("  - HTTP GET took " + (System.currentTimeMillis() - startTime) + "ms");

        // parse
        //startTime = System.currentTimeMillis();
        Object jsonTree = new JSONReader().read(response);
        //if (debug) Debug.println("  got: " + jsonTree.toString());
        //if (debug) Debug.println("  - JSON parsing took " + (System.currentTimeMillis() - startTime) + "ms");
        return jsonTree;
    }

    protected String getPageContent(String url) throws IOException {
        if (debugHttp) Debug.println("GET " + url);
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
                if (debugHttp) Debug.println("  response: ~" + ((responseBody.length()+511)/1024) + "KiB");
            return responseBody;
        }
        catch (SocketException e) {
            // this means connection refused -- very probably fatal
            state = SimState.DISCONNECTED;
            simulationCallback.socketError(e);
            throw e;
        }
        finally {
            method.releaseConnection();
        }
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
