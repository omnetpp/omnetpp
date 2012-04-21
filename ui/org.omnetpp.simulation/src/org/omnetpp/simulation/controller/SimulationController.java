package org.omnetpp.simulation.controller;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.ConnectException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
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
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.json.JSONReader;
import org.omnetpp.simulation.SimulationPlugin;

/**
 * Interacts with a simulation process over HTTP.
 * 
 * @author Andras
 */
//TODO errors in the simulation should be turned into SimulationException thrown from these methods?
//TODO status should send processID or random identifier to determine we're talking to the right process over HTTP
public class SimulationController {
    /**
     * See Cmdenv for state transitions
     */
    public enum SimState {
        DISCONNECTED, // no simulation process, e.g. it has terminated
        NONETWORK,
        NEW,
        READY,
        RUNNING,
        TERMINATED,
        FINISHCALLED,
        ERROR,
        BUSY  // busy doing active wait
    };

    public enum RunMode {
        NOTRUNNING,
        STEP,
        NORMAL,
        FAST,
        EXPRESS
    }

    private String urlBase;
    
    private Job launcherJob;  // we want to be notified when the launcher job exits (== simulation process exits); may be null
    private IJobChangeListener jobChangeListener;

    private ListenerList simulationListeners = new ListenerList(); // listeners we have to notify on changes
    
    // simulation status (as returned by the GET "/sim/status" request)
    private SimState state = SimState.NONETWORK;
    private String configName;
    private int runNumber;
    private String networkName;
    private long eventNumber;
    private BigDecimal simulationTime;

    private boolean stopRequested;

    // cached objects
    private Map<String,Long> rootObjectIds = new HashMap<String, Long>(); // keys: "simulation", "network", etc
    private Map<Long,SimObject> cachedObjects = new HashMap<Long, SimObject>();



    public SimulationController(String hostName, int portNumber, Job launcherJob) {
        this.urlBase = "http://" +  hostName + ":" + portNumber + "/";
        
        // if simulation was launched via a local launcher job (see SimulationLauncherJob), 
        // set up simulationProcessTerminated() to be called when the launcher job exits
        this.launcherJob = launcherJob;
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
        if (launcherJob != null)
            Job.getJobManager().removeJobChangeListener(jobChangeListener);
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
                    ((ISimulationStateListener)listener).simulationStateChanged(SimulationController.this);
                }
                
                @Override
                public void handleException(Throwable e) {
                    SimulationPlugin.logError(e);
                }
            });
        }
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

    public long getEventNumber() {
        return eventNumber;
    }

    public BigDecimal getSimulationTime() {
        return simulationTime;
    }

    protected String encode(String s) {
        try {
            return URLEncoder.encode(s, "US-ASCII");
        }
        catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }
    
    @SuppressWarnings("rawtypes")
    public void refreshStatus() {
        Object responseJSON = getPageContentAsJSON(urlBase + "sim/status");

        Map responseMap = (Map)responseJSON;

        state = SimState.valueOf(((String)responseMap.get("state")).toUpperCase());
        simulationTime = BigDecimal.parse(StringUtils.defaultIfEmpty((String)responseMap.get("simtime"), "0"));
        eventNumber = defaultIfNull((Long)responseMap.get("eventnumber"), 0);
        configName = (String)responseMap.get("config");
        runNumber = (int)defaultIfNull((Long)responseMap.get("run"), -1);
        networkName = (String)responseMap.get("network");
        
        refreshCachedObjects(); //XXX not sure it belongs here
        
        notifyListeners();
    }

    private long defaultIfNull(Long l, long defaultValue) {
        return l==null ? defaultValue : l;
    }

    @SuppressWarnings("rawtypes")
    public List<ConfigDescription> getConfigDescriptions() {
        Object json = getPageContentAsJSON(urlBase + "/sim/enumerateConfigs");
        List<ConfigDescription> result = new ArrayList<ConfigDescription>();
        
        for (Object jitem : (List)json) {
            Map jmap = (Map)jitem;
            ConfigDescription config = new ConfigDescription();
            config.name = (String)jmap.get("name"); 
            config.description = (String)jmap.get("description"); 
            config.numRuns = (int)(long)(Long)jmap.get("numRuns");
            List jextends = (List)jmap.get("extends");
            String[] tmp = new String[jextends.size()];
            for (int i = 0; i < tmp.length; i++)
                tmp[i] = (String)jextends.get(i);
            config.extendsList = tmp;
            result.add(config);
        }
        return result;
    }
    
	public void newRun(String configName, int runNumber) {
        getPageContent(urlBase + "sim/setupRun?config=" + encode(configName) + "&run=" + runNumber);
        refreshStatus();
	}

	public void newNetwork(String networkName) {
	    getPageContent(urlBase + "sim/setupNetwork?network=" + encode(networkName));
        refreshStatus();
	}

    public boolean isNetworkPresent() {
        return state != SimState.DISCONNECTED && state != SimState.NONETWORK;
    }

    public boolean isSimulationOK() {
    	return state == SimState.NEW || state == SimState.RUNNING || state == SimState.READY;
    }

    public boolean isRunning() {
    	return state == SimState.RUNNING || state == SimState.BUSY;
    }

	public void step() {
	    Assert.isTrue(state == SimState.NEW || state == SimState.READY);
        getPageContent(urlBase + "sim/step");
        refreshStatus();
	}

	public void rebuildNetwork() {
        getPageContent(urlBase + "sim/rebuild");
        refreshStatus();
	}

	public void run() {
	    Assert.isTrue(state == SimState.NEW || state == SimState.READY);
	    stopRequested = false;
	    doRun();
	}
	    
	public void doRun() {
	    //NOTE: within doRun(), status is still READY not RUNNING!!! because we do single steps

	    if (stopRequested) {
	        stopRequested = false;
	        return;
	    }

	    getPageContent(urlBase + "sim/step");
	    refreshStatus();

	    Display.getCurrent().asyncExec(new Runnable() {
	        @Override
	        public void run() {
	            doRun();
	        }
	    });
	}

    public void fastRun() {
        run(RunMode.FAST);
    }

    public void expressRun() {
        run(RunMode.EXPRESS);
    }

    public void run(RunMode mode) {
        getPageContent(urlBase + "sim/run?mode=" + mode.name().toLowerCase());
        refreshStatus();
    }

    public void runUntil(RunMode mode, BigDecimal simTime, long eventNumber) {
        String args = "mode=" + mode.name().toLowerCase();
        if (!simTime.equals(BigDecimal.getZero()))
            args += "&utime=" + simTime.toString();
        if (eventNumber > 0)
            args += "&uevent=" + eventNumber;
        getPageContent(urlBase + "sim/run?" + args);
        refreshStatus();
    }

    public void stop() {
        stopRequested = true;
        if (state==SimState.RUNNING) {
            getPageContent(urlBase + "sim/stop");
            refreshStatus();
        }
    }

    public void callFinish() {
	    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
	    Assert.isTrue(state == SimState.NEW || state == SimState.READY || state == SimState.TERMINATED || state == SimState.ERROR);
	    getPageContent(urlBase + "sim/callFinish");
	    refreshStatus();
	}

    protected String getPageContent(String url) {
        if (getState() == SimState.DISCONNECTED)
            throw new IllegalStateException("Simulation process already terminated");

        // turn off log messages from Apache HttpClient if we can...
        Log log = LogFactory.getLog("org.apache.commons.httpclient");
        if (log instanceof Jdk14Logger)
            ((Jdk14Logger)log).getLogger().setLevel(Level.OFF);

        HttpClient client = new HttpClient();
        client.getParams().setSoTimeout(60*1000);

        // do not retry
        HttpMethodRetryHandler noRetryhandler = new HttpMethodRetryHandler() {
            public boolean retryMethod(final HttpMethod method,final IOException exception, int executionCount) {
                return false;
            }
        };

        GetMethod method = new GetMethod(url);
        method.getProxyAuthState().setAuthScheme(new BasicScheme());
        method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, noRetryhandler);
        method.setDoAuthentication(true);

        try {
            int status = client.executeMethod(method);
            String responseBody = method.getResponseBodyAsString();
            return status == HttpStatus.SC_OK ? responseBody : null;
        } catch (HttpException e) {
            e.printStackTrace();
            return null;
        } catch (ConnectException e) {
            e.printStackTrace(); //TODO this is a "Connection Refused" -- likely fatal!!!
            return null;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        } finally {
            method.releaseConnection();
        }
    }

    protected Object getPageContentAsJSON(String url) {
        String response = getPageContent(url);
        if (response == null)
            return null; //TODO throw error?
            
        // parse
        //TODO how to handle exceptions?
        Object jsonTree = new JSONReader().read(response);
        System.out.println("GET " + url + " -->\n" + jsonTree.toString());
        return jsonTree;
    }    

    @SuppressWarnings("rawtypes")
    protected void refreshRootObjectIds() {
        // refresh root object IDs
        Object json = getPageContentAsJSON(urlBase + "/sim/getRootObjectIds");
        for (Object key : ((Map)json).keySet()) {
            long objectId = (Long)((Map)json).get(key);
            rootObjectIds.put((String)key, objectId);
        }
        
        loadObjects(rootObjectIds.values()); //XXX maybe not here...
    }

    public long getRootObjectId(String key) { //TODO use enum for key!
        if (rootObjectIds.isEmpty())
            refreshRootObjectIds();
        return rootObjectIds.get(key);
    }
    
    protected void refreshCachedObjects() {
        refreshCachedObjects(cachedObjects.keySet());
    }
    
    public SimObject getCachedObject(long id) {
        return cachedObjects.get(id);
    }

    @SuppressWarnings("rawtypes")
    protected void refreshCachedObjects(Collection<Long> ids) {
        // refresh cached objects
        String idsArg = StringUtils.join(ids, ",");
        Object json = getPageContentAsJSON(urlBase + "/sim/getObjectInfo?ids=" + idsArg);

        List<Long> garbage = new ArrayList<Long>();
        for (long id : ids) {
            Map objectInfo = (Map)((Map)json).get(String.valueOf(id));
            if (objectInfo == null) {
                garbage.add(id); // calling remove() here would cause ConcurrentModificationException
            }
            else {
                SimObject object = cachedObjects.get(id);
                if (object == null)
                    cachedObjects.put(id, object = new SimObject(id));
                Assert.isTrue(object.id==id);
                object.className = (String)objectInfo.get("className");
                object.name = (String)objectInfo.get("name");
                object.fullName = (String)objectInfo.get("fullName");
                object.fullPath = (String)objectInfo.get("fullPath");
                object.icon = (String)objectInfo.get("icon");
                object.info = (String)objectInfo.get("info");
                object.ownerId = (Long)objectInfo.get("owner");
            }
        }
        for (Long id : garbage)
            cachedObjects.remove(id);

        json = getPageContentAsJSON(urlBase + "/sim/getObjectChildren?ids=" + idsArg);
        
        for (long id : ids) {
            SimObject object = cachedObjects.get(id);
            List childObjectIds = (List)((Map)json).get(String.valueOf(id));
            if (childObjectIds == null || childObjectIds.isEmpty()) {
                if (object != null)
                    object.childObjectIds = ArrayUtils.EMPTY_LONG_ARRAY;
            }
            else {
                Assert.isTrue(object.id==id);
                long[] tmp = new long[childObjectIds.size()];
                for (int i=0; i<tmp.length; i++)
                    tmp[i] = (Long)childObjectIds.get(i);
                object.childObjectIds = tmp;
            }
        }
    }

    public void loadObjects(Collection<Long> ids) {
        //FIXME this should be done in a background job, and fire a "changed" event when done 
        refreshCachedObjects(ids);
    }

}
