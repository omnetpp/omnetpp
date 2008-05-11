package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING_INTERVAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.dot_APPLY_DEFAULT;
import static org.omnetpp.ned.model.NEDElementConstants.NED_PARTYPE_BOOL;
import static org.omnetpp.ned.model.NEDElementConstants.NED_PARTYPE_DOUBLE;
import static org.omnetpp.ned.model.NEDElementConstants.NED_PARTYPE_INT;
import static org.omnetpp.ned.model.NEDElementConstants.NED_PARTYPE_STRING;
import static org.omnetpp.ned.model.NEDElementConstants.NED_PARTYPE_XML;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
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
 * @author Andras
 */
//XXX consider this:
//Net.host[0].p = ...
//Net.host[*].p = ...
//Then host[0] should NOT be warned about being unused!!!
//This affects all keys containing an index which is not "*", that is, where key.matches(".*\\[([^*]|(\\*[^]]+))\\].*")

public class InifileAnalyzer {
	public static final String INIFILEANALYZERPROBLEM_MARKER_ID = InifileEditorPlugin.PLUGIN_ID + ".inifileanalyzerproblem";
	private IInifileDocument doc;
	private boolean changed = true;
	private Set<String> sectionsCausingCycles;
	private ProblemMarkerSynchronizer markerSynchronizer; // only used during analyze()

	// InifileDocument, InifileAnalyzer, and NEDResources are all accessed from
	// background threads (must be synchronized), and the analyze procedure needs
	// NEDResources -- so use NEDResources as lock to prevent deadlocks
	private Object lock = NEDResourcesPlugin.getNEDResources();

	/**
	 * Classifies inifile keys; see getKeyType().
	 */
	public enum KeyType {
		CONFIG, // contains no dot (like sim-time-limit=, etc)
		PARAM,  // contains dot, but no hyphen: parameter setting (like **.app.delay)
		PER_OBJECT_CONFIG; // dotted, and contains hyphen (like **.apply-default, rng mapping, vector configuration, etc)
	};

    /**
     * Used internally: an iteration variable "${...}", stored as part of SectionData
     */
	static class IterationVariable {
        String varname; // printable variable name ("x"); null for an unnamed variable
        String value;   // "1,2,5..10"; never empty
        String parvar;  // "in parallel to" variable", as in the ${1,2,5..10 ! var} notation
        String section; // section where it was defined
        String key;     // key where it was defined
    };

    /**
	 * Used internally: class of objects attached to IInifileDocument entries
	 * (see getKeyData()/setKeyData())
	 */
	static class KeyData {
		List<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
	};

	/**
	 * Used internally: class of objects attached to IInifileDocument sections
	 * (see getSectionData()/setSectionData())
	 */
	static class SectionData {
		List<ParamResolution> unassignedParams = new ArrayList<ParamResolution>();
		List<ParamResolution> allParamResolutions = new ArrayList<ParamResolution>(); // incl. unassigned
		List<IterationVariable> iterations = new ArrayList<IterationVariable>();
		Map<String,IterationVariable> namedIterations = new HashMap<String, IterationVariable>();
	}


	/**
	 * Constructor.
	 */
	public InifileAnalyzer(IInifileDocument doc) {
		this.doc = doc;

		// hook on inifile changes (unhooking is not necessary, because everything
		// will be gc'd when the editor closes)
		doc.addInifileChangeListener(new IInifileChangeListener() {
			public void modelChanged() {
				InifileAnalyzer.this.modelChanged();
			}
		});
	}

	protected void modelChanged() {
		synchronized (lock) {
			changed = true;
		}
	}

	/**
	 * Returns the underlying inifile document that gets analyzed.
	 */
	public IInifileDocument getDocument() {
		return doc;
	}

	/**
	 * Analyzes the inifile if it changed since last analyzed. Side effects:
	 * error/warning markers may be placed on the IFile, and parameter
	 * resolutions (see ParamResolution) are recalculated.
	 */
	public void analyzeIfChanged() {
		synchronized (lock) {
			if (changed)
				analyze();
		}
	}

	/**
	 * Analyzes the inifile. Side effects: error/warning markers may be placed
	 * on the IFile, and parameter resolutions (see ParamResolution) are
	 * recalculated.
	 */
	public void analyze() {
		synchronized (lock) {
			long startTime = System.currentTimeMillis();
			INEDTypeResolver ned = NEDResourcesPlugin.getNEDResources();

			// collect errors/warnings in a ProblemMarkerSynchronizer
			markerSynchronizer = new ProblemMarkerSynchronizer(INIFILEANALYZERPROBLEM_MARKER_ID);
			markerSynchronizer.register(doc.getDocumentFile());
			for (IFile file : doc.getIncludedFiles())
				markerSynchronizer.register(file);

			//XXX catch all exceptions during analyzing, and set changed=false in finally{} ?

			// calculate parameter resolutions for each section
			calculateParamResolutions(ned);

			// collect iteration variables
			collectIterationVariables();

			// data structure is done
			changed = false;

			// check section names, detect cycles in the fallback chains
			validateSections();

			// validate config entries and parameter keys; this must be done AFTER changed=false
			for (String section : doc.getSectionNames()) {
				for (String key : doc.getKeys(section)) {
					switch (getKeyType(key)) {
					case CONFIG: validateConfig(section, key, ned); break;
					case PARAM:  validateParamKey(section, key, ned); break;
					case PER_OBJECT_CONFIG: validatePerObjectConfig(section, key, ned); break;
					}
				}
			}

			// make sure that an iteration variable isn't redefined in other sections
			for (String section : doc.getSectionNames()) {
				String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
				Map<String, IterationVariable> namedIterations = ((SectionData) doc.getSectionData(section)).namedIterations;
				for (String var : namedIterations.keySet())
					for (String ancestorSection : sectionChain)
						if (!section.equals(ancestorSection))
							if (((SectionData) doc.getSectionData(ancestorSection)).namedIterations.containsKey(var))
								addError(section, namedIterations.get(var).key, "Redeclaration of iteration variable $"+var+", originally defined in section ["+ancestorSection+"]");
			}

			// warn for unused param keys; this must be done AFTER changed=false
			for (String section : doc.getSectionNames())
				for (String key : getUnusedParameterKeys(section))
					addWarning(section, key, "Unused entry (does not match any parameters)");

			System.out.println("Inifile analyzed in "+(System.currentTimeMillis()-startTime)+"ms");

			// synchronize detected problems with the file's existing markers
			markerSynchronizer.runAsWorkspaceJob();
			markerSynchronizer = null;
		}
	}

	protected void addError(String section, String message) {
		LineInfo line = doc.getSectionLineDetails(section);
		addMarker(line.getFile(), INIFILEANALYZERPROBLEM_MARKER_ID, IMarker.SEVERITY_ERROR, message, line.getLineNumber());
	}

	protected void addError(String section, String key, String message) {
		LineInfo line = doc.getEntryLineDetails(section, key);
		addMarker(line.getFile(), INIFILEANALYZERPROBLEM_MARKER_ID, IMarker.SEVERITY_ERROR, message, line.getLineNumber());
	}

	protected void addWarning(String section, String message) {
		LineInfo line = doc.getSectionLineDetails(section);
		addMarker(line.getFile(), INIFILEANALYZERPROBLEM_MARKER_ID, IMarker.SEVERITY_WARNING, message, line.getLineNumber());
	}

	protected void addWarning(String section, String key, String message) {
		LineInfo line = doc.getEntryLineDetails(section, key);
		addMarker(line.getFile(), INIFILEANALYZERPROBLEM_MARKER_ID, IMarker.SEVERITY_WARNING, message, line.getLineNumber());
	}

	protected void addMarker(final IFile file, final String type, int severity, String message, int line) {
	    Map<String, Object> map = new HashMap<String, Object>();
		map.put(IMarker.SEVERITY, severity);
		map.put(IMarker.LINE_NUMBER, line);
		map.put(IMarker.MESSAGE, message);
		markerSynchronizer.addMarker(file, type, map);
	}

	/**
	 * Check section names, and detect cycles in the fallback sequences ("extends=")
	 */
	protected void validateSections() {
		sectionsCausingCycles = new HashSet<String>();

		// check section names and extends= everywhere
		if (doc.getValue(GENERAL, EXTENDS) != null)
			addError(GENERAL, EXTENDS, "The extends= key cannot occur in the [General] section");

		for (String section : doc.getSectionNames()) {
			if (!section.equals(GENERAL)) {
				if (!section.startsWith(CONFIG_))
					addError(section, "Invalid section name: must be [General] or [Config <name>]");
				else if (section.contains("  "))
					addError(section, "Invalid section name: contains too many spaces");
				else if (!section.substring(0,1).matches("[a-zA-Z_]"))
					addError(section, "Invalid section name: config name must begin a letter or underscore");
				else if (!section.matches("[^ ]+ [a-zA-Z0-9_@-]+"))
					addError(section, "Invalid section name: contains illegal character(s)");
				String extendsName = doc.getValue(section, EXTENDS);
				if (extendsName != null && !doc.containsSection(CONFIG_+extendsName))
					addError(section, EXTENDS, "No such section: [Config "+extendsName+"]");
			}
		}

		// check fallback chain for every section
		for (String section : doc.getSectionNames()) {
			// follow section fallback sequence, and detect cycles in it
			Set<String> sectionChain = new HashSet<String>();
			String currentSection = section;
			while (true) {
				sectionChain.add(currentSection);
				currentSection = InifileUtils.resolveBaseSection(doc, currentSection);
				if (currentSection==null)
					break; // [General] reached
				if (sectionChain.contains(currentSection)) {
					sectionsCausingCycles.add(currentSection);
					break; // error: cycle in the fallback chain
				}
			}
		}

		// add error markers
		for (String section : sectionsCausingCycles)
			addError(section, "Cycle in the fallback chain at section ["+section+"]");
	}

	/**
	 * Validate a configuration entry (key+value) using ConfigRegistry.
	 */
	protected void validateConfig(String section, String key, INEDTypeResolver ned) {
		// check key and if it occurs in the right section
		ConfigKey e = ConfigRegistry.getEntry(key);
		if (e == null) {
			addError(section, key, "Unknown configuration entry: "+key);
			return;
		}
		else if (e.isGlobal() && !section.equals(GENERAL)) {
			addError(section, key, "Key \""+key+"\" can only be specified globally, in the [General] section");
		}
		else if (key.equals(CFGID_NETWORK.getKey()) && !section.equals(GENERAL)) {
			// it does not make sense to override "network=" in another section, warn for it
			String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
			for (String sec : sectionChain)
				if (!sec.equals(section) && doc.containsKey(sec, key))
					addWarning(section, key, "Network is already specified in section ["+sec+"], as \""+doc.getValue(sec, key)+"\"");
		}

		// check value
		String value = doc.getValue(section, key);

		// if it contains "${...}" variables, check that those variables exist. Any more
		// validation would be significantly more complex, and not done at the moment
		if (value.indexOf('$') != -1) {
			if (validateValueWithIterationVars(section, key))
				return;
		}

		// check if value has the right type
		String errorMessage = validateConfigValueByType(value, e);
		if (errorMessage != null) {
			addError(section, key, errorMessage);
			return;
		}

		if (e.getDataType()==ConfigKey.DataType.CFG_STRING && value.startsWith("\""))
			value =	Common.parseQuotedString(value); // cannot throw exception: value got validated above

		// check validity of some settings, like network=, preload-ned-files=, etc
		if (e==CFGID_EXTENDS) {
			// note: we do not validate "extends=" here -- that's all done in validateSections()!
		}
		else if (e==CFGID_NETWORK) {
			INEDTypeInfo network = resolveNetwork(ned, value);
			if (network == null) {
				addError(section, key, "No such NED type: "+value);
				return;
			}
			INedTypeElement node = network.getNEDElement();
            if (!(node instanceof IModuleTypeElement)) {
                addError(section, key, "Not a module type: "+value);
                return;
            }
			if (!((IModuleTypeElement)node).isNetwork()) {
				addError(section, key, "Module type '"+value+"' was not declared to be a network");
				return;
			}
		}
	}

	public INEDTypeInfo resolveNetwork(INEDTypeResolver ned, String value) {
		INEDTypeInfo network = null;
		IFile iniFile = doc.getDocumentFile();
		String inifilePackage = ned.getExpectedPackageFor(iniFile);
		IProject contextProject = iniFile.getProject();
		if (inifilePackage != null) {
			String networkName = inifilePackage + (inifilePackage.length()!=0 && value.length()!=0 ? "." : "")+value;
			network = ned.getToplevelNedType(networkName, contextProject);
		}
		if (network == null)
			network = ned.getToplevelNedType(value, contextProject);

		return network;
	}

	protected boolean validateValueWithIterationVars(String section, String key) {
		Pattern p = Pattern.compile(
				"\\$\\{" +   // opening dollar+brace
				"\\s*([a-zA-Z0-9@_-]+)" + // variable name
				"(\\s*=.*?)?" +  // equal sign (part after this is optional)
				"\\s*\\}");  // closing brace

		String value = doc.getValue(section, key);
		Matcher m = p.matcher(value);
		SectionData sectionData = (SectionData) doc.getSectionData(section);

		// find all occurrences of the pattern in the input string
		boolean foundAny = false;
		while (m.find()) {
			foundAny = true;
			String varname = m.group(1);
			if (!sectionData.namedIterations.containsKey(varname) && !Arrays.asList(ConfigRegistry.getConfigVariableNames()).contains(varname))
				addError(section, key, "${"+varname+"} is undefined");
		}
		return foundAny;
	}

	/**
	 * Validate a configuration entry's value.
	 */
	protected static String validateConfigValueByType(String value, ConfigKey e) {
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
			//XXX
			break;
		}
		return null;
	}

	protected void validateParamKey(String section, String key, INEDTypeResolver ned) {
		// value cannot be empty
		String value = doc.getValue(section, key).trim();
		if (value.equals("")) {
			addError(section, key, "Value cannot be empty");
			return;
		}

		// check parameter types are consistent with each other
		ParamResolution[] resList = getParamResolutionsForKey(section, key);
		int dataType = -1;
		for (ParamResolution res : resList) {
			if (dataType == -1)
				dataType = res.paramDeclNode.getType();
			else if (dataType != res.paramDeclNode.getType()) {
				addError(section, key, "Entry matches parameters of different data types");
				return;
			}
		}

		// if value contains "${...}" variables, check that those variables exist. Any more
		// validation would be significantly more complex, and not done at the moment
		if (value.indexOf('$') != -1) {
			if (validateValueWithIterationVars(section, key))
				return;
		}

		// check value is consistent with the data type
		if (dataType != -1) {
			int valueType = -1;
			if (value.equals("true") || value.equals("false"))
				valueType = NED_PARTYPE_BOOL;
			else if (value.startsWith("\""))
				valueType = NED_PARTYPE_STRING;
			else if (value.matches("[-+0-9.eE]+"))
				valueType = NED_PARTYPE_DOUBLE;
			else if (value.startsWith("xmldoc"))
				valueType = NED_PARTYPE_XML;

			if (dataType == NED_PARTYPE_INT)
				dataType = NED_PARTYPE_DOUBLE;

			if (valueType!=-1 && valueType!=dataType) {
				String typeName = resList[0].paramDeclNode.getAttribute(ParamElement.ATT_TYPE);
				addError(section, key, "Wrong data type: "+typeName+" expected");
			}
		}
	}

	protected void validatePerObjectConfig(String section, String key, INEDTypeResolver ned) {
		Assert.isTrue(key.lastIndexOf('.') > 0);
		String configName = key.substring(key.lastIndexOf('.')+1);
		ConfigKey e = ConfigRegistry.getPerObjectEntry(configName);
		if (e == null) {
			addError(section, key, "Unknown per-object configuration: "+configName);
			return;
		}
		else if (e.isGlobal() && !section.equals(GENERAL)) {
			addError(section, key, "Per-object configuration \""+configName+"\" can only be specified globally, in the [General] section");
		}

		// check value
		String value = doc.getValue(section, key);

		// if it contains "${...}" variables, check that those variables exist. Any more
		// validation would be significantly more complex, and not done at the moment
		if (value.indexOf('$') != -1) {
			if (validateValueWithIterationVars(section, key))
				return;
		}

		// check if value has the right type
		String errorMessage = validateConfigValueByType(value, e);
		if (errorMessage != null) {
			addError(section, key, errorMessage);
			return;
		}

		if (e.getDataType()==ConfigKey.DataType.CFG_STRING && value.startsWith("\""))
			value =	Common.parseQuotedString(value); // cannot throw exception: value got validated above

		// check validity of some settings, like record-interval=, etc
		if (e==CFGID_VECTOR_RECORDING_INTERVAL) {
			// validate syntax
			StringTokenizer tokenizer = new StringTokenizer(value, ",");
			while (tokenizer.hasMoreTokens()) {
				String interval = tokenizer.nextToken();
				if (!interval.contains(".."))
					addError(section, key, "Syntax error in output vector interval");
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
						addError(section, key, "Syntax error in output vector interval");
					}
				}
			}
		}
	}

	protected void collectIterationVariables() {
		for (String section : doc.getSectionNames()) {
			SectionData sectionData = (SectionData) doc.getSectionData(section);
			sectionData.namedIterations.clear();
			for (String key : doc.getKeys(section))
				if (doc.getValue(section, key).indexOf('$') != -1)
					parseIterationVariables(section, key);
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
		Matcher m = p.matcher(value);
		SectionData sectionData = (SectionData) doc.getSectionData(section);

		// find all occurrences of the pattern in the input string
		while (m.find()) {
			IterationVariable v = new IterationVariable();
			v.varname = m.group(2);
			v.value = m.group(3);
			v.parvar = m.group(5);
			v.section = section;
			v.key = key;
			System.out.println("found: $"+v.varname+" = ``"+v.value+"'' ! "+v.parvar);
			if (Arrays.asList(ConfigRegistry.getConfigVariableNames()).contains(v.varname))
				addError(section, key, "${"+v.varname+"} is a predefined variable and cannot be changed");
			else if (sectionData.namedIterations.containsKey(v.varname))
				// Note: checking that it doesn't redefine a variable in a base section can only be done
				// elsewhere, after all sections have been processed
				addError(section, key, "Redefinition of iteration variable ${"+v.varname+"}");
			else {
				sectionData.iterations.add(v);
				if (v.varname != null)
					sectionData.namedIterations.put(v.varname, v);
			}
		}
	}

	/**
	 * Calculate how parameters get assigned when the given section is the active one.
	 */
	protected void calculateParamResolutions(INEDTypeResolver ned) {
		// initialize SectionData and KeyData objects
		for (String section : doc.getSectionNames()) {
			doc.setSectionData(section, new SectionData());
			for (String key : doc.getKeys(section))
				if (getKeyType(key)!=KeyType.CONFIG)
					doc.setKeyData(section, key, new KeyData());
		}

		// calculate parameter resolutions for each section
		for (String activeSection : doc.getSectionNames()) {
			// calculate param resolutions
			List<ParamResolution> resList = collectParameters(activeSection);

			// store with the section the list of all parameter resolutions (including unassigned params)
			// store with every key the list of parameters it resolves
			for (ParamResolution res : resList) {
				SectionData sectionData = ((SectionData)doc.getSectionData(activeSection));
				sectionData.allParamResolutions.add(res);
				if (res.type == ParamResolutionType.UNASSIGNED)
					sectionData.unassignedParams.add(res);

				if (res.key != null) {
					((KeyData)doc.getKeyData(res.section, res.key)).paramResolutions.add(res);
				}
			}
		}
	}

	protected List<ParamResolution> collectParameters(final String activeSection) {
	    // resolve section chain and network
		INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
		final String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		if (networkName == null)
			networkName = CFGID_NETWORK.getDefaultValue();
		if (networkName == null)
			return new ArrayList<ParamResolution>();
		INEDTypeInfo network = resolveNetwork(res, networkName);
		if (network == null )
			return new ArrayList<ParamResolution>();

		// traverse the network and collect resolutions meanwhile
		ArrayList<ParamResolution> list = new ArrayList<ParamResolution>();
		IProject contextProject = doc.getDocumentFile().getProject();
		NEDTreeTraversal treeTraversal = new NEDTreeTraversal(res, createParamCollectingNedTreeVisitor(list, res, sectionChain, doc));
		treeTraversal.traverse(network.getFullyQualifiedName(), contextProject);
		return list;
	}

	/**
	 * Collects parameters of a module type (recursively), *without* an inifile present.
	 */
	public static List<ParamResolution> collectParameters(INEDTypeInfo moduleType) {
		ArrayList<ParamResolution> list = new ArrayList<ParamResolution>();
		INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
		NEDTreeTraversal treeTraversal = new NEDTreeTraversal(res, createParamCollectingNedTreeVisitor(list, res, null, null));
		treeTraversal.traverse(moduleType);
		return list;
	}

	/**
	 * Collects parameters of a submodule subtree, *without* an inifile present.
	 */
	public static List<ParamResolution> collectParameters(SubmoduleElementEx submodule) {
		List<ParamResolution> list = new ArrayList<ParamResolution>();
		INEDTypeResolver res = NEDResourcesPlugin.getNEDResources();
		NEDTreeTraversal treeTraversal = new NEDTreeTraversal(res, createParamCollectingNedTreeVisitor(list, res, null, null));
		treeTraversal.traverse(submodule);
		return list;
	}

	protected static IModuleTreeVisitor createParamCollectingNedTreeVisitor(final List<ParamResolution> list, INEDTypeResolver res, final String[] sectionChain, final IInifileDocument doc) {
		return new IModuleTreeVisitor() {
			Stack<SubmoduleElementEx> pathModules = new Stack<SubmoduleElementEx>();
			Stack<String> fullPathStack = new Stack<String>();

			public void enter(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
				pathModules.push(submodule);
				fullPathStack.push(submodule==null ? submoduleType.getName() : InifileUtils.getSubmoduleFullName(submodule));
				String submoduleFullPath = StringUtils.join(fullPathStack.toArray(), "."); //XXX optimize here if slow
				SubmoduleElementEx[] pathModulesArray = pathModules.toArray(new SubmoduleElementEx[]{});
				resolveModuleParameters(list, submoduleFullPath, pathModulesArray, submoduleType, sectionChain, doc);
			}
			public void leave() {
				fullPathStack.pop();
				pathModules.pop();
			}

			public void unresolvedType(SubmoduleElementEx submodule, String submoduleTypeName) {}

			public void recursiveType(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {}

			public String resolveLikeType(SubmoduleElementEx submodule) {
				// Note: we cannot use InifileUtils.resolveLikeParam(), as that calls
				// resolveLikeParam() which relies on the data structure we are currently building

				// get like parameter name
				String likeParamName = submodule.getLikeParam();
				if (!likeParamName.matches("[A-Za-z_][A-Za-z0-9_]*"))
					return null;  // sorry, we are only prepared to resolve parent module parameters (but not expressions)

				// look up parameter value (note: we cannot use resolveLikeParam() here yet)
				String moduleFullPath = StringUtils.join(fullPathStack.toArray(), ".");
				ParamResolution res = null;
				for (ParamResolution r : list)
					if (r.paramDeclNode.getName().equals(likeParamName) && r.moduleFullPath.equals(moduleFullPath))
						{res = r; break;}
				if (res == null)
					return null; // likely no such parameter
				String value = getParamValue(res, doc);
				if (value == null)
					return null; // likely unassigned
				try {
					value = Common.parseQuotedString(value);
				} catch (RuntimeException e) {
					return null; // something is wrong: value is not a string constant?
				}
				// note: value is likely a simple (unqualified) name, it'll be resolved
				// to fully qualified name in the caller (NEDTreeTraversal)
				return value;
			}
		};
	}

	protected static void resolveModuleParameters(List<ParamResolution> resultList, String moduleFullPath, SubmoduleElementEx[] pathModules, INEDTypeInfo moduleType, String[] sectionChain, IInifileDocument doc) {
		SubmoduleElementEx submodule = (SubmoduleElementEx) pathModules[pathModules.length-1];
		
		// loop through all parameters of the module
		for (String paramName : moduleType.getParamDeclarations().keySet()) {
		    // find declaration and value ParamElements
			ParamElement paramDeclNode = moduleType.getParamDeclarations().get(paramName);
			ParamElement paramValueNode = submodule==null ?
					moduleType.getParamAssignments().get(paramName) :
					submodule.getParamAssignments().get(paramName);
			if (paramValueNode != null && StringUtils.isEmpty(paramValueNode.getValue()))
				paramValueNode = null;
			
			// then figure out how the parameter gets its value (find matching ini entries etc)
			resolveParameter(resultList, moduleFullPath, pathModules, paramDeclNode, paramValueNode, sectionChain, doc);
		}
	}

	/**
	 * Determines how a NED parameter gets assigned (inifile, NED file, etc).
	 * The sectionChain and doc parameters may be null, which means that only parameter
	 * assignments given in NED will be taken into account.
	 *
     * This method adds one or more ParamResolution objects to resultList. For example,
     * if the inifile contains lines like:
     *     Network.node[0].address = value1
     *     Network.node[1].address = value2
     *     Network.node[*].address = valueX
     * then this method will add three ParamResolutions. 
     *  
	 * XXX what if parameter is assigned in a submodule decl?
	 * XXX what if it's assigned using a /pattern/? this info cannot be expressed in the arg list!
	 */
	protected static void resolveParameter(List<ParamResolution> resultList, String moduleFullPath, SubmoduleElementEx[] pathModules, ParamElement paramDeclNode, ParamElement paramValueNode, String[] sectionChain, IInifileDocument doc) {
	    // value in the NED file
	    String nedValue = paramValueNode==null ? null : paramValueNode.getValue();
	    if (StringUtils.isEmpty(nedValue)) nedValue = null;
	    boolean isNedDefault = paramValueNode==null ? false : paramValueNode.getIsDefault();

	    String activeSection = doc==null ? null : sectionChain[0];

	    if (doc==null || (nedValue!=null && !isNedDefault)) {
	        // value hardcoded in NED (unchangeable from ini files), or there's no inifile
	        ParamResolutionType type = nedValue!=null ? ParamResolutionType.NED : ParamResolutionType.UNASSIGNED;
	        resultList.add(new ParamResolution(moduleFullPath, pathModules, paramDeclNode, paramValueNode, type, activeSection, null, null));
	        return;
	    }

	    // look up its value in the inifile
	    String paramFullPath = moduleFullPath + "." + paramDeclNode.getName();
	    List<SectionKey> sectionKeys = InifileUtils.lookupParameter(paramFullPath, isNedDefault, sectionChain, doc);

	    if (sectionKeys.isEmpty()) {
	        // inifile contains nothing useful
	        ParamResolutionType type = ParamResolutionType.UNASSIGNED;
	        resultList.add(new ParamResolution(moduleFullPath, pathModules, paramDeclNode, paramValueNode, type, activeSection, null, null));
	        return;
	    }

	    // add a ParamResolution for each matching line in the inifile
	    for (SectionKey sectionKey : sectionKeys) {
	        String iniSection = sectionKey.section;
	        String iniKey = sectionKey.key;
	        String iniValue = null;
	        boolean iniApplyDefault = iniKey.endsWith(dot_APPLY_DEFAULT);
	        if (iniApplyDefault)
	            Assert.isTrue(nedValue!=null && isNedDefault && "true".equals(doc.getValue(iniSection, iniKey))); // assert lookupParameter() sanity
	        if (!iniApplyDefault)
	            iniValue = doc.getValue(iniSection, iniKey);

	        // so, find out how the parameter's going to be assigned...
	        ParamResolutionType type;
            if (iniApplyDefault)
                type = ParamResolutionType.NED_DEFAULT; // **.apply-default=true
            else if (nedValue==null)
	            type = ParamResolutionType.INI;
	        else if (nedValue.equals(iniValue))
	            type = ParamResolutionType.INI_NEDDEFAULT;
	        else
	            type = ParamResolutionType.INI_OVERRIDE;
	        resultList.add(new ParamResolution(moduleFullPath, pathModules, paramDeclNode, paramValueNode, type, activeSection, iniSection, iniKey));
	    }
	}

	/**
	 * Resolve parameters of a module type or submodule, based solely on NED information, 
	 * without inifile. This is useful for views when a NED editor is active. 
	 */
	public static ParamResolution[] resolveModuleParameters(String moduleFullPath, SubmoduleElementEx submodule, INEDTypeInfo moduleType) {
		ArrayList<ParamResolution> resultList = new ArrayList<ParamResolution>();
		for (String paramName : moduleType.getParamDeclarations().keySet()) {
			ParamElement paramDeclNode = moduleType.getParamDeclarations().get(paramName);
			ParamElement paramValueNode = submodule==null ?
					moduleType.getParamAssignments().get(paramName) :
					submodule.getParamAssignments().get(paramName);
			if (paramValueNode != null && StringUtils.isEmpty(paramValueNode.getValue()))
				paramValueNode = null;
			resolveParameter(resultList, moduleFullPath, null, paramDeclNode, paramValueNode, null, null);
		}
		return resultList.toArray(new ParamResolution[]{});
	}

	public boolean containsSectionCycles() {
		analyzeIfChanged();
		return !sectionsCausingCycles.isEmpty();
	}

	public boolean isCausingCycle(String section) {
		analyzeIfChanged();
		return sectionsCausingCycles.contains(section);
	}

	/**
	 * Classify an inifile key, based on its syntax.
	 */
	public static KeyType getKeyType(String key) {
		if (!key.contains("."))
			return KeyType.CONFIG;  // contains no dot
		else if (!key.contains("-"))
			return KeyType.PARAM; // contains dot, but no hyphen
		else
			return KeyType.PER_OBJECT_CONFIG; // contains both dot and hyphen
	}

	public boolean isUnusedParameterKey(String section, String key) {
		synchronized (lock) {
			analyzeIfChanged();
			if (getKeyType(key)!=KeyType.PARAM)
				return false;
			KeyData data = (KeyData) doc.getKeyData(section,key);
			return data!=null && data.paramResolutions!=null && data.paramResolutions.isEmpty();
		}
	}

	/**
	 * Returns the parameter resolutions for the given key. If the returned array is
	 * empty, this key is not used to resolve any module parameters.
	 */
	public ParamResolution[] getParamResolutionsForKey(String section, String key) {
		synchronized (lock) {
			analyzeIfChanged();
			KeyData data = (KeyData) doc.getKeyData(section,key);
			return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0];
		}
	}

	public String[] getUnusedParameterKeys(String section) {
		synchronized (lock) {
			analyzeIfChanged();
			ArrayList<String> list = new ArrayList<String>();
			for (String key : doc.getKeys(section))
				if (isUnusedParameterKey(section, key))
					list.add(key);
			return list.toArray(new String[list.size()]);
		}
	}

	/**
	 * Returns parameter resolutions from the given section that correspond to the
	 * parameters of the given module.
	 */
	public ParamResolution[] getParamResolutionsForModule(String moduleFullPath, String section) {
		synchronized (lock) {
			analyzeIfChanged();
			SectionData data = (SectionData) doc.getSectionData(section);
			List<ParamResolution> pars = data==null ? null : data.allParamResolutions;
			if (pars == null || pars.isEmpty())
				return new ParamResolution[0];

			// Note: linear search -- can be made more efficient with some lookup table if needed
			ArrayList<ParamResolution> result = new ArrayList<ParamResolution>();
			for (ParamResolution par : pars)
				if (par.moduleFullPath.equals(moduleFullPath))
					result.add(par);
			return result.toArray(new ParamResolution[]{});
		}
	}

	/**
	 * Returns the resolution of the given module parameter from the given section,
	 * or null if not found.
	 */
	public ParamResolution getResolutionForModuleParam(String moduleFullPath, String paramName, String section) {
		synchronized (lock) {
			analyzeIfChanged();
			SectionData data = (SectionData) doc.getSectionData(section);
			List<ParamResolution> pars = data==null ? null : data.allParamResolutions;
			if (pars == null || pars.isEmpty())
				return null;

			// Note: linear search -- can be made more efficient with some lookup table if needed
			for (ParamResolution par : pars)
				if (par.paramDeclNode.getName().equals(paramName) && par.moduleFullPath.equals(moduleFullPath))
					return par;
			return null;
		}
	}

	/**
	 * Returns all parameter resolutions for the given inifile section; this includes
	 * unassigned parameters as well.
	 */
	public ParamResolution[] getParamResolutions(String section) {
		synchronized (lock) {
			analyzeIfChanged();
			SectionData sectionData = (SectionData) doc.getSectionData(section);
			return sectionData.allParamResolutions.toArray(new ParamResolution[]{});
		}
	}

	/**
	 * Returns unassigned parameters for the given inifile section.
	 */
	public ParamResolution[] getUnassignedParams(String section) {
		synchronized (lock) {
			analyzeIfChanged();
			SectionData sectionData = (SectionData) doc.getSectionData(section);
			return sectionData.unassignedParams.toArray(new ParamResolution[]{});
		}
	}

	public static String getParamValue(ParamResolution res, IInifileDocument doc) {
		switch (res.type) {
			case UNASSIGNED:
				return null;
			case NED: case NED_DEFAULT:
				return res.paramValueNode.getValue();
			case INI: case INI_OVERRIDE: case INI_NEDDEFAULT:
				return doc.getValue(res.section, res.key);
			default: throw new IllegalArgumentException("invalid param resolution type: "+res.type);
		}
	}

	public static String getParamRemark(ParamResolution res, IInifileDocument doc) {
		String remark;
		switch (res.type) {
			case UNASSIGNED: remark = "unassigned";
				if (res.paramValueNode != null)
					remark += " (NED default: "+res.paramValueNode.getValue()+")";
				else
					; //remark += " (no NED default)";
				break;
			case NED: remark = "NED"; break;
			case NED_DEFAULT: remark = "NED default applied"; break;
			case INI: remark = "ini"; break;
			case INI_OVERRIDE: remark = "ini, overrides NED default: "+res.paramValueNode.getValue(); break;
			case INI_NEDDEFAULT: remark = "ini, sets same value as NED default"; break;
			default: throw new IllegalStateException("invalid param resolution type: "+res.type);
		}
		if (res.key!=null)
			remark += "; see ["+res.section+"] / " + res.key + "=" + doc.getValue(res.section, res.key);
		return remark;
	}

	/**
	 * Returns names of declared iteration variables ("${variable=...}") from
	 * the given section and all its fallback sections. Note: unnamed iterations
	 * are not in the list.
	 */
	public String[] getIterationVariableNames(String activeSection) {
		synchronized (lock) {
			analyzeIfChanged();
			List<String> result = new ArrayList<String>();
			String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
			for (String section : sectionChain) {
				SectionData sectionData = (SectionData) doc.getSectionData(section);
				result.addAll(sectionData.namedIterations.keySet());
			}
			String[] array = result.toArray(new String[]{});
			Arrays.sort(array);
			return array;
		}
	}

	/**
	 * Returns true if the given section or any of its fallback sections
	 * contain an iteration, like "${1,2,5}" or "${x=1,2,5}".
	 */
	public boolean containsIteration(String activeSection) {
		synchronized (lock) {
			analyzeIfChanged();
			String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
			for (String section : sectionChain) {
				SectionData sectionData = (SectionData) doc.getSectionData(section);
				if (!sectionData.iterations.isEmpty())
					return true;
			}
			return false;
		}
	}

	/**
	 * Returns the value string (e.g. "1,2,6..10") for an iteration variable
	 * from the given section and its fallback sections.
	 */
	public String getIterationVariableValueString(String activeSection, String variable) {
		synchronized (lock) {
			analyzeIfChanged();
			String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
			for (String section : sectionChain) {
				SectionData sectionData = (SectionData) doc.getSectionData(section);
				if (sectionData.namedIterations.containsKey(variable))
					return sectionData.namedIterations.get(variable).value;
			}
			return null;
		}
	}
}

