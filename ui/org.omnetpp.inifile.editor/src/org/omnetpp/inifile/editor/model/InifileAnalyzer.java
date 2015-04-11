/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING_INTERVALS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.ned.model.NedElementConstants.NED_PARTYPE_BOOL;
import static org.omnetpp.ned.model.NedElementConstants.NED_PARTYPE_DOUBLE;
import static org.omnetpp.ned.model.NedElementConstants.NED_PARTYPE_INT;
import static org.omnetpp.ned.model.NedElementConstants.NED_PARTYPE_STRING;
import static org.omnetpp.ned.model.NedElementConstants.NED_PARTYPE_XML;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.collections.ResettableIterator;
import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.omnetpp.common.Debug;
import org.omnetpp.common.collections.ProductIterator;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.engine.StringSet;
import org.omnetpp.common.engine.StringTokenizer2;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.engineext.StringTokenizerException;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.form.AnalysisTimeoutDialog;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.inifile.editor.model.ParamResolutionStatus.Entry;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.ParamElement;

/**
 * This is a layer above IInifileDocument, and contains info about the
 * relationship of inifile contents and NED. For example, which inifile
 * parameter settings apply to which NED module parameters.
 *
 * Implementation note: there are several synchronized(doc) { } blocks in the
 * code. This is necessary because e.g. we need to prevent InifileDocument from
 * getting re-parsed while we are analyzing it. In particular, any two of the
 * following threads may collide: reconciler, content assist, update of the
 * Module Parameters view.
 *
 * The time consuming part of the analysis is performed as a Job.
 * The thread that called a method which requires analyzing the ini file
 * is blocked until the job is finished or a specified amount of time used up
 * (then throwing a timeout exception).
 *
 * The long running job works on a read-only copy of the ini file,
 * so we do not need to lock the resources during its execution.
 *
 * @author Andras
 */
//XXX Issue. Consider the following:
//  **.ppp[1].queueType = "Foo"
//  **.ppp[*].queueType = "DropTailQueue"
//  **.ppp[*].queue.frameCapacity = 10     ===> "unused entry", although DropTailQueue has that parameter
// because the queue type resolves to "Foo", and "DropTailQueue" is not considered.
// How to fix this? Maybe: resolveLikeType() returns a set of potential types
// (ie both "Foo" and "DropTailQueue", and NedTreeTraversal should recurse with **all** of them?
//
public final class InifileAnalyzer {
    // marker types
    public static final String INIFILEANALYZERPROBLEM_MARKER_ID = InifileEditorPlugin.PLUGIN_ID + ".inifileanalyzerproblem"; // errors in config options, etc (immediate)
    public static final String INIFILEANALYZER2PROBLEM_MARKER_ID = InifileEditorPlugin.PLUGIN_ID + ".inifileanalyzer2problem"; // param and property resolution related errors/warnings (by param resolution job)

    // properties (see addPropertyChangeListener())
    public static final String PROP_ANALYSIS_ENABLED = "AnalysisEnabled";

    // to speed up validating of values. Matches boolean, number+unit, string literal
    public static final Pattern SIMPLE_EXPRESSION_REGEX = Pattern.compile("true|false|(-?\\d+(\\.\\d+)?\\s*[a-zA-Z]*)|\"[^\"]*\"");

    // parameters of the analyzer
    final private IInifileDocument doc;
    private boolean paramResolutionEnabled = true; // enables the slow part of the analysis, can be disabled from the GUI

    // state of the analyzer
    private boolean changed = true;  // true if the document has changed since last validate()
    //private boolean paramResolutionDirty = true; // true if the document has changed while paramResolutionJob was running
    private boolean withinValidate = false; // debug only
    private IReadonlyInifileDocument analysisDocCopy; // copy of the ini file belongs to the last analysis
    private INedTypeResolver analysisNedResolverCopy; // copy of the ned resources belong to the last analysis
    private ParamResolutionJob paramResolutionJob;
    private Object paramResolutionLock; // for threads that are waiting for the param resolution job

    // infrastructure
    private InifileProblemMarkerSynchronizer markers; // only used during analyze()
    private ListenerList propertyChangeListeners = new ListenerList();
    private ListenerList analysisListeners = new ListenerList();
    // InifileDocument, InifileAnalyzer, and NedResources are all accessed from
    // background threads (must be synchronized), and the analyze procedure needs
    // NedResources -- so use NedResources as lock to prevent deadlocks
    private Object globalLock = NedResourcesPlugin.getNedResources();


    /**
     * Used internally: an iteration variable definition "${...}", stored as part of SectionData
     */
    public static class IterationVariable {
        public String varname; // printable variable name ("x"); null for an unnamed variable
        public String value;   // "1,2,5..10"; never empty
        public String parvar;  // "in parallel to" variable", as in the ${1,2,5..10 ! var} notation, or null
        public String section; // section where it was defined
        public String key;     // key where it was defined
    };

    /**
     * Used internally: class of objects attached to IInifileDocument entries
     * (see getKeyData()/setKeyData())
     */
    public static class KeyData {
        public List<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
    };

    /**
     * Used internally: class of objects attached to IInifileDocument sections
     * (see getSectionData()/setSectionData())
     */
    public static class SectionData {
        public List<ParamResolution> allParamResolutions = new ArrayList<ParamResolution>();
        public List<ParamResolution> unassignedParams = new ArrayList<ParamResolution>(); // subset of allParamResolutions
        public List<ParamResolution> implicitlyAssignedParams = new ArrayList<ParamResolution>(); // subset of allParamResolutions
        public List<PropertyResolution> propertyResolutions = new ArrayList<PropertyResolution>();
        public List<IterationVariable> iterations = new ArrayList<IterationVariable>();
        public Map<String,IterationVariable> namedIterations = new HashMap<String, IterationVariable>();
        public StringSet validVariableRefs = new StringSet(); // global config vars + named iteration variables in the section chain
    }


    /**
     * Constructor.
     */
    public InifileAnalyzer(IInifileDocument doc) {
        this.doc = doc;
        this.paramResolutionLock = new Object();
        this.paramResolutionJob = new ParamResolutionJob(doc);

        // hook on inifile changes (unhooking is not necessary, because everything
        // will be gc'd when the editor closes)
        doc.addInifileChangeListener(new IInifileChangeListener() {
            public void modelChanged() {
                InifileAnalyzer.this.modelChanged();
            }
        });

        paramResolutionJob.addJobChangeListener(new JobChangeAdapter() {
            public void done(IJobChangeEvent event) {
                paramResolutionJobDone(event.getResult());
            }
        });
    }

    private void modelChanged() {
        synchronized (globalLock) {
            changed = true;
        }
    }

    public boolean isAnalysisUpToDate() {
        synchronized (globalLock) {
            return analysisDocCopy != null && analysisNedResolverCopy != null &&
                    doc.isImmutableCopyUpToDate(analysisDocCopy) &&
                    NedResourcesPlugin.getNedResources().isImmutableCopyUpToDate(analysisNedResolverCopy);
        }
    }

    /**
     * Returns the underlying inifile document that gets analyzed.
     */
    public IInifileDocument getDocument() {
        return doc;
    }

    public synchronized boolean isParamResolutionEnabled() {
        return paramResolutionEnabled;
    }

    public synchronized void setParamResolutionEnabled(boolean paramResolutionEnabled) {
        if (this.paramResolutionEnabled != paramResolutionEnabled) {
            this.paramResolutionEnabled = paramResolutionEnabled;
            firePropertyChange(PROP_ANALYSIS_ENABLED, !paramResolutionEnabled, paramResolutionEnabled);
        }
    }

    public void addPropertyChangeListener(IPropertyChangeListener listener) {
        propertyChangeListeners.add(listener);
    }

    public void removePropertyChangeListener(IPropertyChangeListener listener) {
        propertyChangeListeners.remove(listener);
    }

    private void firePropertyChange(String propertyName, Object oldValue, Object newValue) {
        final Object[] list = propertyChangeListeners.getListeners();
        for (int i = 0; i < list.length; ++i) {
            ((IPropertyChangeListener) list[i]).propertyChange(new PropertyChangeEvent(this, propertyName, oldValue, newValue));
        }
    }

    public interface IAnalysisListener {
        void analysisCompleted(InifileAnalyzer analyzer);
    }

    public void addAnalysisListener(IAnalysisListener listener) {
        analysisListeners.add(listener);
    }

    public void removeAnalysisListener(IAnalysisListener listener) {
        analysisListeners.remove(listener);
    }

    private void fireAnalysisCompletedEvent() {
        final Object[] list = analysisListeners.getListeners();
        for (int i = 0; i < list.length; ++i) {
            ((IAnalysisListener) list[i]).analysisCompleted(this);
        }
    }

    /**
     * Analyzes the inifile if it changed since last analyzed. Side effects:
     * error/warning markers may be placed on the IFile, and parameter
     * resolutions (see ParamResolution) are recalculated.
     *
     * Throws exception if the parameter resolution job has not completed within timeout.
     */
    public synchronized void analyzeIfChanged(ITimeout timeout) throws ParamResolutionTimeoutException {
        validateIfChanged();
        if (paramResolutionEnabled && !isAnalysisUpToDate())
            executeParamResolution(timeout);
    }

    /**
     * Forces analysis of the ini file even it is not changed.
     * Throws exception if the parameter resolution job has not completed within timeout.
     */
    public synchronized void analyze(Timeout timeout) throws ParamResolutionTimeoutException {
        validate();
        if (paramResolutionEnabled)
            executeParamResolution(timeout);
    }

    /**
     * Starts inifile analysis if the inifile has change since the last analysis.
     * The analysis takes place in a background thread; you can use addAnalysisListener()
     * to become notified when the analysis completes.
     */
    public synchronized void startAnalysisIfChanged() {
        try {
            analyzeIfChanged(new Timeout(0));
        } catch (ParamResolutionTimeoutException e) {
            // ignore
        }
    }

    /**
     * Validate the contents of the inifile if changed since last validated.
     * Does not perform parameter resolution.
     */
    private void validateIfChanged() {
        synchronized (globalLock) {
            if (changed)
                validate();
        }
    }

    /**
     * Validates the contents of the inifile. Side effects: error/warning markers may be placed
     * on the IFile. This method does not calculate param resolutions.
     */
    private void validate() {
        synchronized (globalLock) {
            Assert.isTrue(!withinValidate, "validate() recursively called?");
            withinValidate = true;

            try {
                long startTime = System.currentTimeMillis();

                // collect errors/warnings in a ProblemMarkerSynchronizer
                markers = new InifileProblemMarkerSynchronizer(doc, INIFILEANALYZERPROBLEM_MARKER_ID);

                // collect iteration variables, needed for doValidate()
                collectIterationVariables();

                // data structure is done
                changed = false;

                // validate options, param keys, iteration variables, etc.
                doValidate();

                Debug.println("Inifile validated in "+(System.currentTimeMillis()-startTime)+"ms");

                // synchronize detected problems with the file's existing markers
                markers.synchronize();
                markers = null;
            }
            finally {
                withinValidate = false;
            }
        }
    }

    /**
     * Executes param resolution as a job and wait for its completion at most {@code timeout} milliseconds.
     * Throws an exception if the job did not finish in time.
     */
    private void executeParamResolution(ITimeout timeout) throws ParamResolutionTimeoutException {
        Debug.format("executeParamResolutionJob(%s)\n", timeout);

        final long start = System.currentTimeMillis();

        synchronized(paramResolutionLock) {
            // the job may already be scheduled or even running...
            if (paramResolutionJob.getState() == Job.NONE)
                paramResolutionJob.schedule();

            long timeLeft = timeout != null ? timeout.getRemainingTime() : 0;
            Debug.format("InifileAnalyzer: waiting for parameter resolution at most %dms.\n", timeLeft<0 ? 0 : timeLeft);

            while (true) {
                IStatus jobResult = paramResolutionJob.getResult();

                if (paramResolutionJob.getState() == Job.NONE && jobResult != null) {
                    if (jobResult.isOK()) {
                        // check result is still valid
                        if (isAnalysisUpToDate())
                        {
                            Debug.format("InifileAnalyzer: parameter resolution finished in %dms\n", System.currentTimeMillis() - start);
                            return;
                        }
                        else {
                            // doc changed, restart
                            paramResolutionJob.schedule();
                        }
                    }
                    else if (jobResult.getSeverity() == IStatus.CANCEL && jobResult.getCode() == ParamResolutionJob.USER_CANCELED) {
                        Debug.format("InifileAnalyzer: parameter resolution canceled by the user.\n", System.currentTimeMillis() - start);
                        throw new OperationCanceledException();
                    }
                    else {
                        // job stopped without completing the param resolution
                        return; // throw exception?
                    }
                }

                // check timeout
                timeLeft = timeout != null ? timeout.getRemainingTime() : 0;
                if (timeout != null && timeLeft <= 0) {
                    Debug.format("InifileAnalyzer: parameter resolution timeout after %dms\n", System.currentTimeMillis() - start);
                    throw new ParamResolutionTimeoutException();
                }

                try {
                    // wait for the job to finish
                    paramResolutionLock.wait(timeLeft);
                } catch (InterruptedException e) {
                    // loop and keep trying
                }
            }
        }
    }

    // this is called from the job's worker thread
    private void paramResolutionJobDone(IStatus status) {
        if (status.getSeverity() == IStatus.CANCEL && status.getCode() == ParamResolutionJob.DOC_CHANGED) {
            // resheduled from job, waiting for next notification
        }
        else {
            // job finished
            try {
                if (status.isOK() && status instanceof ParamResolutionStatus) {
                    ParamResolutionStatus okStatus = (ParamResolutionStatus)status;
                    INedResources nedResources = NedResourcesPlugin.getNedResources();
                    synchronized (globalLock) {
                        if (doc.isImmutableCopyUpToDate(okStatus.docCopy) && nedResources.isImmutableCopyUpToDate(okStatus.nedResolverCopy)) {
                            analysisDocCopy = okStatus.docCopy;
                            analysisNedResolverCopy = okStatus.nedResolverCopy;
                            annotateParamResolutions(okStatus.result);
                            validateAnalyzedDocument();

                            fireAnalysisCompletedEvent();
                        }
                    }
                }
            }
            finally {
                // wake up threads that are waiting for the result of the analysis in executeParamResolutionJob()
                synchronized (paramResolutionLock) {
                    paramResolutionLock.notifyAll();
                }
            }
        }
    }

    private void annotateParamResolutions(List<ParamResolutionStatus.Entry> resolutions) {
        // initialize SectionData and KeyData objects
        for (String section : doc.getSectionNames()) {
            if (doc.getSectionData(section) instanceof SectionData) {
                SectionData sectionData = (SectionData) doc.getSectionData(section);
                sectionData.allParamResolutions.clear();
                sectionData.implicitlyAssignedParams.clear();
                sectionData.propertyResolutions.clear();
                sectionData.unassignedParams.clear();
            }
            else
                doc.setSectionData(section, new SectionData());

            for (String key : doc.getKeys(section))
                if (KeyType.getKeyType(key)!=KeyType.CONFIG) {
                    if (doc.getKeyData(section, key) instanceof KeyData) {
                        KeyData keyData = (KeyData)doc.getKeyData(section, key);
                        keyData.paramResolutions.clear();
                    }
                    else
                        doc.setKeyData(section, key, new KeyData());
                }
        }

        // copy parameter and property resolutions for each section
        for (Entry entry : resolutions) {
            String activeSection = entry.section;
            List<ParamResolution> paramResolutions = entry.paramResolutions;
            List<PropertyResolution> propertyResolutions = entry.propertyResolutions;

            // store with the section the list of all parameter resolutions (including unassigned params)
            // store with every key the list of parameters it resolves
            SectionData sectionData = ((SectionData)doc.getSectionData(activeSection));
            sectionData.propertyResolutions = propertyResolutions;
            for (ParamResolution res : paramResolutions) {
                sectionData.allParamResolutions.add(res);

                switch (res.type) {
                case UNASSIGNED: sectionData.unassignedParams.add(res); break;
                case IMPLICITDEFAULT: sectionData.implicitlyAssignedParams.add(res); break;
                }

                if (res.key != null) {
                    KeyData keyData = (KeyData)doc.getKeyData(res.section, res.key);
                    if (keyData != null)
                        keyData.paramResolutions.add(res);
                }
            }
        }
    }

    /**
     * Validates all keys (config options, param keys, etc) and iteration variables
     * in the document. This is intended to be a quick operation, i.e. does not need
     * to be run in a background job.
     */
    private void doValidate() {
        INedTypeResolver ned = NedResourcesPlugin.getNedResources();

        // check section names, detect cycles in the fallback chains
        validateSections();

        // validate config entries and parameter keys; this must be done AFTER changed=false
        for (String section : doc.getSectionNames()) {
            for (String key : doc.getKeys(section)) {
                switch (KeyType.getKeyType(key)) {
                case CONFIG: validateConfig(section, key, ned); break;
                case PARAM:  validateParamKey(section, key, ned); break;
                case PER_OBJECT_CONFIG: validatePerObjectConfig(section, key, ned); break;
                }
            }
        }

        // make sure that an iteration variable isn't redefined in other sections
        for (String section : doc.getSectionNames()) {
            SectionData sectionData = (SectionData)doc.getSectionData(section);
            String[] sectionChain = doc.getSectionChain(section);
            Map<String, IterationVariable> namedIterations = sectionData.namedIterations;
            for (String var : namedIterations.keySet())
                for (String ancestorSection : sectionChain)
                    if (!section.equals(ancestorSection))
                        if (((SectionData) doc.getSectionData(ancestorSection)).namedIterations.containsKey(var))
                            markers.addError(section, namedIterations.get(var).key, "Redeclaration of iteration variable $"+var+", originally defined in section ["+ancestorSection+"]");
        }

        // the "${var = ... ! parvar}" syntax (variable plus parallel iteration) is not supported yet
        for (String section : doc.getSectionNames()) {
            List<IterationVariable> iterations = ((SectionData) doc.getSectionData(section)).iterations;
            for (IterationVariable var : iterations) {
                if (var.parvar != null && !isValidIterationVariable(section, var.parvar))  //XXX this again calculates sectionChain! (sectionChain should be cached in sectionData)
                    markers.addError(section, var.key, "Unknown iteration variable " + var.parvar);
                //TODO check whether a ${...!var} line has enough values (at least as many as the variable definition, ${var=...}
            }
        }

    }

    /**
     * Check section names, and detect cycles in the fallback sequences ("extends=")
     */
    protected void validateSections() {
        // check section names and extends= everywhere
        if (doc.getValue(GENERAL, EXTENDS) != null)
            markers.addError(GENERAL, EXTENDS, "The extends= key cannot occur in the [General] section");

        for (String section : doc.getSectionNames()) {
            if (!section.equals(GENERAL)) {
                if (!section.startsWith(CONFIG_))
                    markers.addError(section, "Invalid section name: must be [General] or [Config <name>]");
                else if (section.contains("  "))
                    markers.addError(section, "Invalid section name: contains too many spaces");
                else if (!String.valueOf(section.charAt(CONFIG_.length())).matches("[a-zA-Z_]"))
                    markers.addError(section, "Invalid section name: config name must begin a letter or underscore");
                else if (!section.matches("[^ ]+ [a-zA-Z0-9_@-]+"))
                    markers.addError(section, "Invalid section name: contains illegal character(s)");
                String extendsList = doc.getValue(section, EXTENDS);
                if (!StringUtils.isEmpty(extendsList)) {
                    for (String configName : InifileUtils.parseExtendsList(extendsList))
                        if (!doc.containsSection(CONFIG_+configName))
                            markers.addError(section, EXTENDS, "No such section: [Config "+configName+"]");
                }
            }
        }
    }

    /**
     * Validate a configuration entry (key+value) using ConfigRegistry.
     */
    protected void validateConfig(String section, String key, INedTypeResolver ned) {
        // check key and if it occurs in the right section
        ConfigOption e = ConfigRegistry.getOption(key);
        if (e == null) {
            if (!key.matches("[a-zA-Z0-9-_]+"))
                markers.addError(section, key, "Syntax error in configuration option: "+key);
            else
                markers.addError(section, key, "Unknown configuration option: "+key);
            return;
        }
        else if (e.isGlobal() && !section.equals(GENERAL)) {
            markers.addError(section, key, "Option \""+key+"\" can only be specified globally, in the [General] section");
        }
        else if (key.equals(CFGID_NETWORK.getName()) && !section.equals(GENERAL)) {
            // it does not make sense to override "network=" in another section, warn for it
            String[] sectionChain = doc.getSectionChain(section);
            for (String sec : sectionChain)
                if (!sec.equals(section) && doc.containsKey(sec, key))
                    markers.addWarning(section, key, "Network is already specified in section ["+sec+"], as \""+doc.getValue(sec, key)+"\"");
        }

        // check value
        String value = doc.getValue(section, key);

        // if it contains "${...}" variables, check that those variables exist. Any more
        // validation would be significantly more complex, and not done at the moment
        if (value.indexOf('$') != -1) {
            if (validateValueWithIterationVars(section, key, value))
                return;
        }

        // check if value has the right type
        String errorMessage = validateConfigValueByType(value, e);
        if (errorMessage != null) {
            markers.addError(section, key, errorMessage);
            return;
        }

        if (e.getDataType()==ConfigOption.DataType.CFG_STRING && value.startsWith("\""))
            value = Common.parseQuotedString(value); // cannot throw exception: value got validated above

        // check validity of some settings, like network=, preload-ned-files=, etc
        if (e==CFGID_EXTENDS) {
            // note: we do not validate "extends=" here -- that's all done in validateSections()!
        }
        else if (e==CFGID_NETWORK) {
            INedTypeInfo network = resolveNetwork(ned, value);
            if (network == null) {
                markers.addError(section, key, "No such NED type: "+value);
                return;
            }
            INedTypeElement node = network.getNedElement();
            if (!(node instanceof IModuleTypeElement)) {
                markers.addError(section, key, "Not a module type: "+value);
                return;
            }
            if (!((IModuleTypeElement)node).isNetwork()) {
                markers.addError(section, key, "Module type '"+value+"' was not declared to be a network");
                return;
            }
        }
    }

    public INedTypeInfo resolveNetwork(INedTypeResolver ned, String value) {
        return ParamCollector.resolveNetwork(doc, ned, value);
    }

    public static final String IDENTIFIER = "[a-zA-Z_][a-zA-Z0-9@_-]*";
    public final static Pattern
        DOLLAR_BRACES_PATTERN = Pattern.compile("\\$\\{\\s*(.*?)\\s*\\}"),      // "${...}"
        VARIABLE_DEFINITION_PATTERN = Pattern.compile(IDENTIFIER + "\\s*=\\s*(.*?)\\s*(!\\s*(" + IDENTIFIER + "))?"), // "<name> = <something> [ ! <var> ]"
        VARIABLE_REFERENCE_PATTERN = Pattern.compile("(" + IDENTIFIER + ")"),     // name only
        VALUES_PATTERN = Pattern.compile("\\s*(.*?)\\s*(!\\s*" + IDENTIFIER + "+)?"),    // something that optionally ends with "! <var>"
        START_END_STEP_VALUE_PATTERN = Pattern.compile("(.*?)\\s*\\.\\.\\s*(.*?)\\s*step\\s*(.*)"),  // "<from> .. <to> step <incr>"
        START_END_VALUE_PATTERN = Pattern.compile("(.*?)\\s*\\.\\.\\s*(.*?)"),  // "<from> .. <to>"
        ANY_VALUE_PATTERN = Pattern.compile("(.*)"),
        DOLLAR_VARNAME_PATTERN = Pattern.compile("\\$(" + IDENTIFIER + ")");

    protected boolean validateValueWithIterationVars(String section, String key, String value) {
        Matcher iterationVarMatcher = DOLLAR_BRACES_PATTERN.matcher(value);

        // check referenced iteration variables
        boolean foundAny = false;
        boolean validateValues = true;
        while (iterationVarMatcher.find()) {
            String iteration = iterationVarMatcher.group(1); // the stuff inside ${...}
            foundAny = true;

            Matcher variableReferenceMatcher = VARIABLE_REFERENCE_PATTERN.matcher(iteration);
            if (variableReferenceMatcher.matches()) {
                // not an iteration, just a reference to another iteration variable in the ${foo} syntax
                String varName = variableReferenceMatcher.group(1);
                if (!isValidIterationVariable(section, varName))
                    markers.addError(section, key, "${"+varName+"} is undefined");
                validateValues = false; // because references are not followed
            }
            else {
                // check referenced variables ($var syntax) within the iteration
                Matcher varRefMatcher = DOLLAR_VARNAME_PATTERN.matcher(iteration);
                while (varRefMatcher.find()) {
                    validateValues = false; // because references are not followed
                    String varName = varRefMatcher.group(1);
                    if (!isValidIterationVariable(section, varName))
                        markers.addError(section, key, "${"+varName+"} is undefined");
                }
            }
        }

        // validate the first 100 values that come from iterating the constants in the variable definitions
        if (foundAny && validateValues) {
            try {
                IterationVariablesIterator values = new IterationVariablesIterator(value);  // iterates over all ${} occurrences
                int count = 0;
                while (values.hasNext() && count < 100) {
                    String v = values.next();
                    //System.out.format("Validating: %s%n", v);
                    validateParamKey(section, key, v);
                    count++;
                }
            }
            catch (StringTokenizerException e) {
                markers.addError(section, key, "Syntax error: " + e.getMessage());
            }
        }

        return foundAny;
    }

    protected boolean isValidIterationVariable(String section, String varName) {
        SectionData sectionData = (SectionData)doc.getSectionData(section);
        return sectionData.validVariableRefs.has_key("$"+varName);
    }

    private void validateAnalyzedDocument() {
        InifileProblemMarkerSynchronizer markers = new InifileProblemMarkerSynchronizer(doc, INIFILEANALYZER2PROBLEM_MARKER_ID);

        for (String section : doc.getSectionNames()) {
            for (String key : doc.getKeys(section)) {
                if (KeyType.getKeyType(key) == KeyType.PARAM)
                    validateParamKey(section, key, markers);
            }
        }

        markers.synchronize();
    }

    private void validateParamKey(String section, String key, InifileProblemMarkerSynchronizer markers) {
        // check if unused
        KeyData data = (KeyData) doc.getKeyData(section,key);
        boolean isUnused = data!=null && data.paramResolutions!=null && data.paramResolutions.isEmpty();
        if (isUnused)
            markers.addWarning(section, key, "Unused entry (does not match any parameters)");

        String value = doc.getValue(section, key).trim();
        if (value.equals("") || InifileAnalyzer.DOLLAR_BRACES_PATTERN.matcher(value).find() ||
                value.equals(ConfigRegistry.DEFAULT) || value.equals(ConfigRegistry.ASK) ||
                (!InifileAnalyzer.SIMPLE_EXPRESSION_REGEX.matcher(value).matches() && !NedTreeUtil.isExpressionValid(value))) {
            // already validated in the analyzer
            return;
        }

        // check parameter data types are consistent with each other
        @SuppressWarnings("unchecked")
        List<ParamResolution> resList = (data!=null && data.paramResolutions!=null) ? data.paramResolutions : Collections.EMPTY_LIST;
        int paramType = -1;
        for (ParamResolution res : resList) {
            if (paramType == -1)
                paramType = res.paramDeclaration.getType();
            else if (paramType != res.paramDeclaration.getType()) {
                markers.addError(section, key, "Entry matches parameters of different data types");
                return;
            }
        }

        // check units are consistent with each other
        String paramUnit = null;
        for (ParamResolution res : resList) {
            PropertyElementEx unitProperty = res.paramDeclaration.getLocalProperties().get("unit");
            String unit = unitProperty==null ? "" : StringUtils.nullToEmpty(unitProperty.getSimpleValue());
            if (paramUnit == null)
                paramUnit = unit;
            else if (!paramUnit.equals(unit)) {
                markers.addError(section, key, "Entry matches parameters with different units: " +
                                 (paramUnit.equals("") ? "none" : paramUnit) + ", " + (unit.equals("") ? "none" : unit));
                return;
            }
        }

        // check value is consistent with the data type
        if (paramType != -1) {
            // determine value's data type
            int valueType = -1;
            String valueUnit = null;
            if (value.equals("true") || value.equals("false"))
                valueType = NED_PARTYPE_BOOL;
            else if (value.startsWith("\""))
                valueType = NED_PARTYPE_STRING;
            else if (value.startsWith("xmldoc(") || value.startsWith("xml("))
                valueType = NED_PARTYPE_XML;
            else {
                try {
                    valueUnit = UnitConversion.parseQuantityForUnit(value); // throws exception if not a quantity
                    Assert.isNotNull(valueUnit);
                } catch (RuntimeException e) {}

                if (valueUnit != null)
                    valueType = NED_PARTYPE_DOUBLE;
            }

            // provided we could figure out the value's data type, check it's the same as parameter's data type
            int tmpParamType = paramType==NED_PARTYPE_INT ? NED_PARTYPE_DOUBLE : paramType; // replace "int" with "double"
            if (valueType != -1 && valueType != tmpParamType) {
                String typeName = resList.get(0).paramDeclaration.getAttribute(ParamElement.ATT_TYPE);
                markers.addError(section, key, "Wrong data type: "+typeName+" expected");
            }

            // if value is numeric, check units
            if (valueUnit!=null) {
                try {
                    UnitConversion.parseQuantity(value, paramUnit); // throws exception on incompatible units
                }
                catch (RuntimeException e) {
                    markers.addError(section, key, e.getMessage());
                }
            }

            // mark line if value is the same as the NED default
            Assert.isTrue(resList.size() > 0);
            boolean allAreIniNedDefault = true;
            for (ParamResolution res : resList)
                if (res.type != ParamResolutionType.INI_NEDDEFAULT)
                    allAreIniNedDefault = false;
            if (allAreIniNedDefault)
                markers.addInfo(section, key, "Value is same as the NED default");
        }
    }

    static Pair<Integer,Integer> findNextIterationVariable(String value, int startPos) {
        int index = value.indexOf("${", startPos);
        if (index >= 0) {
            StringTokenizer2 tokenizer = new StringTokenizer2(value.substring(index), "}", "{}()[]", "\"");
            String token = tokenizer.nextToken(); // does not contain ending '}' !
            int start = index;
            int end = start + token.length() + 1; // note: do not use tokenizer.getTokenLength(), as it fails for multi-byte chars (returns byte count instead of character count)
            return Pair.pair(start, end);
        }
        else
            return null;
    }

    static List<String> splitValueByIterationVariables(String value) {
        List<String> tokens = new ArrayList<String>();
        int pos = 0;
        Pair<Integer,Integer> loc = null;
        while((loc = findNextIterationVariable(value, pos)) != null) {
            int start = loc.first;
            int end = loc.second;
            tokens.add(value.substring(pos, start));
            tokens.add(value.substring(start, end));
            pos = end;
        }
        tokens.add(value.substring(pos));
        return tokens;
    }

    /**
     * Iterates on the values, that comes from the substitutions of iteration variables
     * with the constants found in their definition.
     *
     * Example: "x${i=1..5}y${2 .. 8 step 3}" gives ["x1y2", "x5y2", "x1y8", "x2y8", "x1y3", "x2y3"].
     *
     * Note: variable references are not followed, therefore the iterator will be empty
     *       if the parameter value contained referenced variables.
     */
    static class IterationVariablesIterator implements ResettableIterator
    {
        String value;
        List<Object> format;
        ResettableIterator iterator;
        StringBuilder sb;

        public IterationVariablesIterator(String value) {
            this.value = value;
            this.format = new ArrayList<Object>();
            this.sb = new StringBuilder(100);

            List<String> tokens = splitValueByIterationVariables(value);
            List<ResettableIterator> valueIterators = new ArrayList<ResettableIterator>();
            int i = 0;
            for (String token : tokens) {
                if (i % 2 == 0)
                    format.add(token);
                else {
                    format.add(valueIterators.size());
                    valueIterators.add(new IterationVariableIterator(token));
                }
                i++;
            }

            iterator = new ProductIterator(valueIterators.toArray(new ResettableIterator[valueIterators.size()]));
        }

        public void reset() {
            iterator.reset();
        }

        public boolean hasNext() {
            return iterator.hasNext();
        }

        public String next() {
            Object[] values = (Object[])iterator.next();
            if (values == null)
                return null;

            sb.setLength(0);
            for (Object obj : format) {
                if (obj instanceof String)
                    sb.append(obj);
                else if (obj instanceof Integer) {
                    int index = (Integer)obj;
                    sb.append(values[index]);
                }
            }
            return sb.toString();
        }

        public void remove() {
            iterator.remove();
        }
    }

    /**
     * Iterates on the constants in one iteration variable.
     *
     * Example: ${x=1,3..10 step 2} gives [1,3,10,2].
     *
     */
    static class IterationVariableIterator implements ResettableIterator
    {
        String values;
        StringTokenizer2 tokenizer;
        Matcher matcher;
        int groupIndex;

        public IterationVariableIterator(String iteration) {
            Matcher m = DOLLAR_BRACES_PATTERN.matcher(iteration);
            Assert.isTrue(m.matches()); // must be called with strings that start with "${" and end with "}"

            String content = m.group(1);
            if ((m = VARIABLE_DEFINITION_PATTERN.matcher(content)).matches())  // ${a=...}
                values = m.group(1);
            else if ((m=VARIABLE_REFERENCE_PATTERN.matcher(content)).matches()) // ${a}
                // TODO follow the reference?
                values = "";
            else if ((m = VALUES_PATTERN.matcher(content)).matches()) // ${...}
                values = m.group(1);
            else
                Assert.isTrue(false); // cannot happen (VALUES_PATTERN eats anything)
            reset();
        }

        public void reset() {
            tokenizer = new StringTokenizer2(values, ",", "()[]{}", "\"");
            matcher = null;
            groupIndex = 0;
        }

        public boolean hasNext() {
            return tokenizer.hasMoreTokens() || matcher != null && groupIndex <= matcher.groupCount();
        }

        public Object next() {
            if (matcher == null || groupIndex > matcher.groupCount()) {
                if (!tokenizer.hasMoreTokens())
                    return null;
                String token = tokenizer.nextToken().trim();
                match(token);
            }
            if (matcher != null && groupIndex <= matcher.groupCount()) {
                String next = matcher.group(groupIndex);
                groupIndex++;
                return next;
            }
            return null;
        }

        public void remove() {
            throw new UnsupportedOperationException();
        }

        private boolean match(String token) {
            if (matcher == null)
                matcher = START_END_STEP_VALUE_PATTERN.matcher(token);
            else {
                matcher.reset(token);
                matcher.usePattern(START_END_STEP_VALUE_PATTERN);
            }
            groupIndex = 1;
            if (matcher.matches())
                return true;
            matcher.usePattern(START_END_VALUE_PATTERN);
            if (matcher.matches())
                return true;
            matcher.usePattern(ANY_VALUE_PATTERN);
            if (matcher.matches())
                return true;
            return false;
        }
    }

    /**
     * Validate a configuration entry's value.
     */
    protected static String validateConfigValueByType(String value, ConfigOption e) {
        switch (e.getDataType()) {
        case CFG_BOOL:
            if (!value.equals("true") && !value.equals("false"))
                return "Value should be a boolean constant: true or false";
            break;
        case CFG_INT:
            try {
                Integer.parseInt(value);
            } catch (NumberFormatException ex) {
                return "Value should be an integer constant";
            }
            break;
        case CFG_DOUBLE:
            if (e.getUnit()==null) {
                try {
                    Double.parseDouble(value);
                } catch (NumberFormatException ex) {
                    return "Value should be a numeric constant (a double)";
                }
            }
            else {
                try {
                    UnitConversion.parseQuantity(value, e.getUnit());
                } catch (RuntimeException ex) {
                    return StringUtils.capitalize(ex.getMessage());
                }
            }
            break;
        case CFG_STRING:
            try {
                if (value.startsWith("\""))
                    Common.parseQuotedString(value);  //XXX wrong: what if it's an expression like "foo"+"bar" ?
            } catch (RuntimeException ex) {
                return "Error in string constant: "+ex.getMessage();
            }
            break;
        case CFG_FILENAME:
        case CFG_FILENAMES:
        case CFG_PATH:
            //XXX
            break;
        }
        return null;
    }

    protected void validateParamKey(String section, String key, INedTypeResolver ned) {
        String value = doc.getValue(section, key).trim();
        validateParamKey(section, key, value);
    }

    protected void validateParamKey(String section, String key, String value) {
        // value cannot be empty
        if (value.equals("")) {
            markers.addError(section, key, "Value cannot be empty");
            return;
        }

        // if value contains "${...}" variables, check that those variables exist. Any more
        // validation would be significantly more complex, and not done at the moment
        if (DOLLAR_BRACES_PATTERN.matcher(value).find()) {
            if (validateValueWithIterationVars(section, key, value))
                return;
        }

        if (value.equals(ConfigRegistry.DEFAULT) || value.equals(ConfigRegistry.ASK)) {
            // nothing to check, actually
            return;
        }

        // check syntax. note: regex is faster in most cases than parsing
        if (!SIMPLE_EXPRESSION_REGEX.matcher(value).matches()) {
            if (!NedTreeUtil.isExpressionValid(value))
                markers.addError(section, key, "Syntax error in expression: " + value);
        }
    }

    protected void validatePerObjectConfig(String section, String key, INedTypeResolver ned) {
        Assert.isTrue(key.lastIndexOf('.') > 0);
        String configName = key.substring(key.lastIndexOf('.')+1);
        ConfigOption e = ConfigRegistry.getPerObjectOption(configName);
        if (e == null) {
            if (configName.equals("type-name"))
                markers.addError(section, key, "Configuration option \"type-name\" has been renamed to \"typename\", please update the ini file");
            else if (!configName.matches("[a-zA-Z0-9-_]+"))
                markers.addError(section, key, "Syntax error in per-object configuration option: "+configName);
            else
                markers.addError(section, key, "Unknown per-object configuration option: "+configName);
            return;
        }
        else if (e.isGlobal() && !section.equals(GENERAL)) {
            markers.addError(section, key, "Per-object configuration option \""+configName+"\" can only be specified globally, in the [General] section");
        }

        // check value
        String value = doc.getValue(section, key);

        // if it contains "${...}" variables, check that those variables exist. Any more
        // validation would be significantly more complex, and not done at the moment
        if (value.indexOf('$') != -1) {
            if (validateValueWithIterationVars(section, key, value))
                return;
        }

        // check if value has the right type
        String errorMessage = validateConfigValueByType(value, e);
        if (errorMessage != null) {
            markers.addError(section, key, errorMessage);
            return;
        }

        if (e.getDataType()==ConfigOption.DataType.CFG_STRING && value.startsWith("\""))
            value = Common.parseQuotedString(value); // cannot throw exception: value got validated above

        // check validity of some settings, like record-interval=, etc
        if (e==CFGID_VECTOR_RECORDING_INTERVALS) {
            // validate syntax
            StringTokenizer tokenizer = new StringTokenizer(value, ",");
            while (tokenizer.hasMoreTokens()) {
                String interval = tokenizer.nextToken();
                if (!interval.contains(".."))
                    markers.addError(section, key, "Syntax error in output vector intervals");
                else {
                    try {
                        String from = StringUtils.substringBefore(interval, "..").trim();
                        String to = StringUtils.substringAfter(interval, "..").trim();
                        if (!from.equals("") && !from.contains("${"))
                            Double.parseDouble(from);  // check format
                        if (!to.equals("") && !to.contains("${"))
                            Double.parseDouble(to);  // check format
                    }
                    catch (NumberFormatException ex) {
                        markers.addError(section, key, "Syntax error in output vector interval");
                    }
                }
            }
        }
    }

    protected void collectIterationVariables() {
        for (String section : doc.getSectionNames()) {
            SectionData sectionData = (SectionData) doc.getSectionData(section);
            if (sectionData == null)
                doc.setSectionData(section, sectionData = new SectionData());
            else {
                sectionData.iterations.clear();
                sectionData.namedIterations.clear();
                sectionData.validVariableRefs.clear();
            }

            for (String key : doc.getKeys(section))
                if (doc.getValue(section, key).indexOf('$') != -1)
                    parseIterationVariables(section, key);
        }

        // compute valid variable references
        for (String section : doc.getSectionNames()) {
            SectionData sectionData = (SectionData) doc.getSectionData(section);
            for (String var : ConfigRegistry.getConfigVariableNames())
                sectionData.validVariableRefs.insert("$"+var);

            String[] sectionChain = doc.getSectionChain(section);
            for (String ancestorOrSelfSection : sectionChain) {
                SectionData ancestorData = (SectionData) doc.getSectionData(ancestorOrSelfSection);
                for (String var : ancestorData.namedIterations.keySet())
                    sectionData.validVariableRefs.insert("$"+var);
            }
        }
    }

    protected void parseIterationVariables(String section, String key) {
        Pattern p = Pattern.compile(
                "\\$\\{" +   // opening dollar+brace
                "(\\s*([a-zA-Z0-9@_-]+)" + // variable name (opt)
                "\\s*=)?" +  // equals (opt)
                "\\s*(.*?)" +  // value string
                "\\s*(!\\s*([a-zA-Z0-9@_-]+))?" + // optional trailing "! variable"
                "\\s*\\}");  // closing brace

        String value = doc.getValue(section, key);
        SectionData sectionData = (SectionData) doc.getSectionData(section);

        // find all occurrences of the pattern in the input string
        int pos = 0;
        Pair<Integer,Integer> loc = null;
        try {
            while ((loc = findNextIterationVariable(value, pos)) != null) {
                int start = loc.first;
                int end = loc.second;

                String iteration = value.substring(start, end);
                pos = end;

                Matcher m = p.matcher(iteration);
                boolean matches = m.matches();
                Assert.isTrue(matches);

                IterationVariable v = new IterationVariable();
                v.varname = m.group(2);
                v.value = m.group(3);
                v.parvar = m.group(5);
                v.section = section;
                v.key = key;
                //Debug.println("itervar found: $"+v.varname+" = ``"+v.value+"'' ! "+v.parvar);
                if (Arrays.asList(ConfigRegistry.getConfigVariableNames()).contains(v.varname))
                    markers.addError(section, key, "${"+v.varname+"} is a predefined variable and cannot be changed");
                else if (sectionData.namedIterations.containsKey(v.varname))
                    // Note: checking that it doesn't redefine a variable in a base section can only be done
                    // elsewhere, after all sections have been processed
                    markers.addError(section, key, "Redefinition of iteration variable ${"+v.varname+"}");
                else {
                    sectionData.iterations.add(v);
                    if (v.varname != null)
                        sectionData.namedIterations.put(v.varname, v);
                }
            }
        } catch (StringTokenizerException e) {
            markers.addError(section, key, "Syntax error: " + e.getMessage());
        }
    }

/*
    // TODO: move?
    // testParamAssignments("C:\\Workspace\\Repository\\omnetpp\\test\\param\\param.out", list);
    public void testParamAssignments(String fileName, ArrayList<ParamResolution> list) {
        try {
            int index = 0;
            Properties properties = new Properties();
            properties.load(new FileInputStream(fileName));

            for (Object key : CollectionUtils.toSorted((Set)properties.keySet(), new DictionaryComparator())) {
                String paramName = (String)key;
                String runtimeParamValue = properties.getProperty(paramName);
                boolean iniDefault = false;

                for (ParamResolution paramResolution : list) {
                    String paramPattern;

                    if (paramResolution.key != null)
                        paramPattern = paramResolution.key;
                    else {
                        String fullPath = paramResolution.fullPath;
                        String paramAssignment = paramResolution.paramAssignment != null ? paramResolution.paramAssignment.getName() : paramResolution.paramDeclaration.getName();

                        if (paramAssignment.indexOf('.') != -1)
                            fullPath = fullPath.substring(0, fullPath.lastIndexOf('.'));

                        paramPattern = fullPath + "." + paramAssignment;
                    }

                    PatternMatcher m = new PatternMatcher(paramPattern, true, true, true);

                    if (m.matches(paramName)) {
                        String ideParamValue = null;

                        switch (paramResolution.type) {
                            case INI_ASK:
                                if (iniDefault)
                                    continue;
                            case UNASSIGNED:
                                ideParamValue = "\"" + index + "\"";
                                index++;
                                break;
                            case INI_DEFAULT:
                                iniDefault = true;
                                continue;
                            case NED:
                            case IMPLICITDEFAULT:
                                ideParamValue = paramResolution.paramAssignment.getValue();
                                break;
                            case INI:
                            case INI_OVERRIDE:
                            case INI_NEDDEFAULT:
                                if (iniDefault)
                                    continue;
                                ideParamValue = doc.getValue(paramResolution.section, paramResolution.key);
                                break;
                            default:
                                throw new RuntimeException();
                        }

                        if (!runtimeParamValue.equals(ideParamValue))
                            Debug.println("*** Mismatch *** for name: " + paramName + ", runtime value: " + runtimeParamValue + ", ide value: " + ideParamValue + ", pattern: " + paramPattern);
                        else
                            Debug.println("Match for name: " + paramName + ", value: " + runtimeParamValue);

                        break;
                    }
                }
            }
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
*/

    public interface Runnable<T> {
        T run() throws ParamResolutionDisabledException;
    }

    private synchronized <T> T withAnalyzedDocument(ITimeout timeout, Runnable<T> body) throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        if (!paramResolutionEnabled)
            throw new ParamResolutionDisabledException();

        while (true) {
            analyzeIfChanged(timeout);
            synchronized (globalLock) {
                if (isAnalysisUpToDate())
                    return body.run();
            }
        }
    }

    public interface Runnable2<T> { T run(); }

    private <T> T withValidatedDocument(Runnable2<T> body) {
        synchronized (globalLock) {
            validateIfChanged();
            return body.run();
        }
    }

    public ITimeout getAdjustableTimeout(final long initialTimeout) {
        Assert.isTrue(initialTimeout > 0);
        return new ITimeout() {
            long currentTimeout = initialTimeout;
            long endTime = System.currentTimeMillis() + initialTimeout;

            public long getRemainingTime() {
                long time = endTime - System.currentTimeMillis();

                if (time <= 0) {
                    Dialog dialog = new AnalysisTimeoutDialog(null);
                    switch (dialog.open()) {
                    case AnalysisTimeoutDialog.WAIT:
                        currentTimeout *= 2;
                        endTime = System.currentTimeMillis() + currentTimeout;
                        return currentTimeout;
                    case AnalysisTimeoutDialog.DISABLE_AND_CANCEL:
                        setParamResolutionEnabled(false);
                        break;
                    }
                }

                return time;
            }
        };
    }

    /**
     * Returns the parameter resolutions for the given key. If the returned array is
     * empty, this key is not used to resolve any module parameters.
     */
    public ParamResolution[] getParamResolutionsForKey(final String section, final String key, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<ParamResolution[]>() {
            public ParamResolution[] run() {
                KeyData data = (KeyData) doc.getKeyData(section,key);
                return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0];
            }
        });
    }

    /**
     * Returns parameter resolutions from the given section that correspond to the
     * parameters of the given module.
     */
    public ParamResolution[] getParamResolutionsForModule(final Vector<ISubmoduleOrConnection> elementPath, final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<ParamResolution[]>() {
            public ParamResolution[] run() {
            SectionData data = (SectionData) doc.getSectionData(section);
            List<ParamResolution> pars = data==null ? null : data.allParamResolutions;
            if (pars == null || pars.isEmpty())
                return new ParamResolution[0];

            // Note: linear search -- can be made more efficient with some lookup table if needed
            ArrayList<ParamResolution> result = new ArrayList<ParamResolution>();
            for (ParamResolution par : pars)
                if (elementPathEquals(elementPath, par.elementPath))
                    result.add(par);
            return result.toArray(new ParamResolution[]{});
            }
        });
    }

    /**
     * Returns the resolution of the given module parameter from the given section,
     * or null if not found.
     */
    public ParamResolution getParamResolutionForModuleParam(final String fullPath, final String paramName, final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<ParamResolution>() {
            public ParamResolution run() {
            SectionData data = (SectionData) doc.getSectionData(section);
            List<ParamResolution> pars = data==null ? null : data.allParamResolutions;
            if (pars == null || pars.isEmpty())
                return null;

            // Note: linear search -- can be made more efficient with some lookup table if needed
            for (ParamResolution par : pars)
                if (par.paramDeclaration.getName().equals(paramName) && par.fullPath.equals(fullPath))
                    return par;
            return null;
            }
        });
    }

    /**
     * Returns all parameter resolutions for the given inifile section; this includes
     * unassigned parameters as well.
     */
    public ParamResolution[] getParamResolutions(final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<ParamResolution[]>() {
            public ParamResolution[] run() {
            SectionData sectionData = (SectionData) doc.getSectionData(section);
            return sectionData.allParamResolutions.toArray(new ParamResolution[]{});
            }
        });
    }

    /**
     * Returns unassigned parameters for the given inifile section.
     * (This is a filtered subset of the objects returned by getParamResolutions().)
     */
    public ParamResolution[] getUnassignedParams(final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<ParamResolution[]>() {
            public ParamResolution[] run() {
            SectionData sectionData = (SectionData) doc.getSectionData(section);
            return sectionData.unassignedParams.toArray(new ParamResolution[]{});
            }
        });
    }

    /**
     * Returns implicitly assigned parameters for the given inifile section.
     * (This is a filtered subset of the objects returned by getParamResolutions().)
     */
    public ParamResolution[] getImplicitlyAssignedParams(final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<ParamResolution[]>() {
            public ParamResolution[] run() {
            SectionData sectionData = (SectionData) doc.getSectionData(section);
            return sectionData.implicitlyAssignedParams.toArray(new ParamResolution[]{});
            }
        });
    }

    public PropertyResolution[] getPropertyResolutions(final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<PropertyResolution[]>() {
            public PropertyResolution[] run() {
                SectionData sectionData = (SectionData) doc.getSectionData(section);
                return sectionData.propertyResolutions.toArray(new PropertyResolution[]{});
            }
        });
    }

    public PropertyResolution[] getPropertyResolutionsForModule(final String propertyName, final Vector<ISubmoduleOrConnection> elementPath, final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<PropertyResolution[]>() {
            public PropertyResolution[] run() {
            SectionData data = (SectionData)doc.getSectionData(section);
            List<PropertyResolution> propertyResolutions = data == null ? null : data.propertyResolutions;
            if (propertyResolutions == null || propertyResolutions.isEmpty())
                return new PropertyResolution[0];

            // Note: linear search -- can be made more efficient with some lookup table if needed
            ArrayList<PropertyResolution> result = new ArrayList<PropertyResolution>();
            for (PropertyResolution propertyResolution : propertyResolutions)
                if (propertyName.equals(propertyResolution.propertyDeclaration.getName()) &&
                        elementPathEquals(elementPath, propertyResolution.elementPath))
                    result.add(propertyResolution);
            return result.toArray(new PropertyResolution[]{});
            }
        });
    }

    public PropertyResolution[] getPropertyResolutions(final String propertyName, final String fullPathPattern, final String section, ITimeout timeout)
            throws ParamResolutionDisabledException, ParamResolutionTimeoutException {
        return withAnalyzedDocument(timeout, new Runnable<PropertyResolution[]>() {
            public PropertyResolution[] run() {
                SectionData data = (SectionData)doc.getSectionData(section);
                List<PropertyResolution> propertyResolutions = data == null ? null : data.propertyResolutions;
                if (propertyResolutions == null || propertyResolutions.isEmpty())
                    return new PropertyResolution[0];

                PatternMatcher matcher = new PatternMatcher(fullPathPattern, true, true, true);

                // Note: linear search -- can be made more efficient with some lookup table if needed
                ArrayList<PropertyResolution> result = new ArrayList<PropertyResolution>();
                for (PropertyResolution propertyResolution : propertyResolutions)
                    if (propertyName.equals(propertyResolution.propertyDeclaration.getName()) && matcher.matches(propertyResolution.fullPath))
                        result.add(propertyResolution);
                return result.toArray(new PropertyResolution[]{});
            }
        });
    }


    private static boolean elementPathEquals(Vector<ISubmoduleOrConnection> original, ISubmoduleOrConnection[] copy) {
        if (original.size() != copy.length)
            return false;
        int i = 0;
        for (ISubmoduleOrConnection first : original) {
            ISubmoduleOrConnection second = copy[i++];
            if (!(ObjectUtils.equals(first, second) ||
                  (second != null && second.getOriginal() != null && ObjectUtils.equals(first, second.getOriginal()))))
                  return false;
        }
        return true;
    }

    /**
     * Returns names of declared iteration variables ("${variable=...}") from
     * the given section and all its fallback sections. Note: unnamed iterations
     * are not in the list.
     */
    public String[] getIterationVariableNames(final String activeSection) {
        return withValidatedDocument(new Runnable2<String[]>() {
            public String[] run() {
                List<String> result = new ArrayList<String>();
                for (String section : doc.getSectionChain(activeSection)) {
                    SectionData sectionData = (SectionData) doc.getSectionData(section);
                    result.addAll(sectionData.namedIterations.keySet());
                }
                String[] array = result.toArray(new String[]{});
                Arrays.sort(array);
                return array;
            }
        });
    }

    /**
     * Returns true if the given section or any of its fallback sections
     * contain an iteration, like "${1,2,5}" or "${x=1,2,5}".
     */
    public boolean containsIteration(final String activeSection) {
        return (Boolean)withValidatedDocument(new Runnable2<Boolean>() {
            public Boolean run() {
                for (String section : doc.getSectionChain(activeSection)) {
                    SectionData sectionData = (SectionData) doc.getSectionData(section);
                    if (sectionData == null)
                        // XXX Note: sectionData is NOT supposed to be null here. However, it sometimes is;
                        // this occurs with large models and ini files (e.g.INETMANET), and seems to be timing dependent;
                        // and also with greater probability when typing introduces a syntax error in the file.
                        // Workaround: check for null manually here.
                        // Cause: InifileReconcileStrategy calls doc.parse(), which clears the section data,
                        //        but does notify IInifileChangeListers, so `this.changed` remains false (so the validation  up to date)
                        Debug.println("WARNING: no sectionData for section " + section + " in InifileAnalyzer.containsIteration()!");
                    if (sectionData != null && !sectionData.iterations.isEmpty())
                        return true;
                }
                return false;
            }
        });
    }

    /**
     * Returns the value string (e.g. "1,2,6..10") for an iteration variable
     * from the given section and its fallback sections.
     */
    public String getIterationVariableValueString(final String activeSection, final String variable) {
        return withValidatedDocument(new Runnable2<String>() {
            public String run() {
                for (String section : doc.getSectionChain(activeSection)) {
                    SectionData sectionData = (SectionData) doc.getSectionData(section);
                    if (sectionData.namedIterations.containsKey(variable))
                        return sectionData.namedIterations.get(variable).value;
                }
                return null;
            }
        });
    }
}

