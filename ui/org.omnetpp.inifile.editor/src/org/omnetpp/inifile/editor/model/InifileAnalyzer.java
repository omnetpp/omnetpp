package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;
import static org.omnetpp.ned.model.NEDElementUtil.NED_PARTYPE_BOOL;
import static org.omnetpp.ned.model.NEDElementUtil.NED_PARTYPE_DOUBLE;
import static org.omnetpp.ned.model.NEDElementUtil.NED_PARTYPE_INT;
import static org.omnetpp.ned.model.NEDElementUtil.NED_PARTYPE_STRING;
import static org.omnetpp.ned.model.NEDElementUtil.NED_PARTYPE_XML;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.Stack;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.ui.texteditor.MarkerUtilities;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

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
	private boolean containsSectionCircles; 

	/**
	 * Classifies inifile keys; see getKeyType().
	 */
	public enum KeyType {
		CONFIG, // contains no dot (like sim-time-limit=, etc) 					
		PARAM,  // contains dot, but no hyphen: parameter setting (like **.app.delay) 
		PER_OBJECT_CONFIG; // dotted, and contains hyphen (like **.apply-default, rng mapping, vector configuration, etc)
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
		synchronized (doc) {
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
		synchronized (doc) {
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
		synchronized (doc) {
			long startTime = System.currentTimeMillis();
			INEDTypeResolver ned = NEDResourcesPlugin.getNEDResources();

			// remove existing markers
			try {
				IFile file = doc.getDocumentFile();
				file.deleteMarkers(INIFILEANALYZERPROBLEM_MARKER_ID, true, 0);
			} catch (CoreException e) {
				InifileEditorPlugin.logError(e);
			}

			//XXX catch all exceptions during analyzing, and set changed=false in finally{} ? 

			// calculate parameter resolutions for each section
			calculateParamResolutions(ned);

			// data structure is done
			changed = false;

			// check section names, detect circles in the fallback chains
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

			// warn for unused param keys; this must be done AFTER changed=false
			for (String section : doc.getSectionNames())
				for (String key : getUnusedParameterKeys(section))
					addWarning(section, key, "Unused entry (does not match any parameters)");

			System.out.println("Inifile analyzed in "+(System.currentTimeMillis()-startTime)+"ms");
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

	@SuppressWarnings("unchecked")
	private static void addMarker(final IFile file, final String type, int severity, String message, int line) {
		try {
			HashMap map = new HashMap();
			MarkerUtilities.setMessage(map, message);
			MarkerUtilities.setLineNumber(map, line);
			map.put(IMarker.SEVERITY, severity);
			MarkerUtilities.createMarker(file, map, type);
		} catch (CoreException e) {
			InifileEditorPlugin.logError(e);
		}
	}

	/**
	 * Check section names, and detect circles in the fallback sequences ("extends=")
	 */
	protected void validateSections() {
		containsSectionCircles = false;
		Set<String> bogusSections = new HashSet<String>();

		// check fallback chain for every section, except [General]
		for (String section : doc.getSectionNames()) {
			// check section name
			if (!section.equals(GENERAL) && !section.startsWith(CONFIG_))
				addError(section, "Wrong section name: must be [General] or [Config <name>]");

			// follow section fallback sequence, and detect circle in it
			if (!section.equals(GENERAL)) {
				Set<String> sectionChain = new HashSet<String>();
				String currentSection = section;
				while (true) {
					if (!doc.containsSection(currentSection))
						break; // error: nonexisting section
					if (sectionChain.contains(currentSection)) {
						bogusSections.add(currentSection);
						break; // error: circle in the fallback chain
					}
					sectionChain.add(currentSection);
					String extendsName = doc.getValue(currentSection, CFGID_EXTENDS.getKey());
					if (extendsName==null)
						break; // done
					currentSection = CONFIG_+extendsName;
				}
			}
		}

		// add error markers
		containsSectionCircles = !bogusSections.isEmpty();
		for (String section : bogusSections)
			addError(section, "circle in the fallback chain at section ["+section+"]");
	}

	/**
	 * Validate a configuration entry (key+value) using ConfigurationRegistry.
	 */
	protected void validateConfig(String section, String key, INEDTypeResolver ned) {
		// check key and if it occurs in the right section
		ConfigurationEntry e = ConfigurationRegistry.getEntry(key);
		if (e == null) {
			addError(section, key, "Unknown configuration entry: "+key);
			return;
		}
		else if (key.equals(CFGID_EXTENDS.getKey()) && section.equals(GENERAL)) {
			addError(section, key, "Key \""+key+"\" cannot occur in the [General] section");
		}
		else if (e.isGlobal() && !section.equals(GENERAL)) {
			addError(section, key, "Key \""+key+"\" can only be specified globally, in the [General] section");
		}

		// check value: if it is the right type
		String value = doc.getValue(section, key);
		String errorMessage = validateConfigValueByType(value, e);
		if (errorMessage != null) {
			addError(section, key, errorMessage);
			return;
		}

		if (e.getDataType()==ConfigurationEntry.DataType.CFG_STRING && value.startsWith("\""))
			value =	Common.parseQuotedString(value); // cannot throw exception: value got validated above

		// check validity of some settings, like network=, preload-ned-files=, etc
		if (e==CFGID_EXTENDS) {
			if (!doc.containsSection(CONFIG_+value))
				addError(section, key, "No such section: [Config "+value+"]");
		}
		else if (e==CFGID_NETWORK) {
			INEDTypeInfo network = ned.getComponent(value);
			if (network == null) {
				addError(section, key, "No such NED network: "+value);
				return;
			}
			NEDElement node = network.getNEDElement();
			if (!(node instanceof CompoundModuleNode) || ((CompoundModuleNode)node).getIsNetwork()==false) {
				addError(section, key, "Type '"+value+"' was not declared in NED with the keyword 'network'");
				return;
			}
		}
	}

	/**
	 * Validate a configuration entry's value.
	 */
	protected static String validateConfigValueByType(String value, ConfigurationEntry e) {
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
				String typeName = resList[0].paramDeclNode.getAttribute(ParamNode.ATT_TYPE);
				addError(section, key, "Wrong data type: "+typeName+" expected");
			}
		}
	}

	protected void validatePerObjectConfig(String section, String key, INEDTypeResolver ned) {
		//TODO look up in ConfigurationRegistry, etc.
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
			List<ParamResolution> resList = collectParameters(activeSection, ned);

			// store with the section the list of all parameter resolutions (incl unassigned params)
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

	protected List<ParamResolution> collectParameters(final String activeSection, INEDTypeResolver ned) {
		final String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		if (networkName == null)
			networkName = CFGID_NETWORK.getDefaultValue().toString(); 
		if (networkName == null)
			return new ArrayList<ParamResolution>();

		final IInifileDocument finalDoc = doc;
		NEDResources res = NEDResourcesPlugin.getNEDResources();
		final ArrayList<ParamResolution> list = new ArrayList<ParamResolution>();

		NEDTreeIterator treeIterator = new NEDTreeIterator(res, new IModuleTreeVisitor() {
			Stack<SubmoduleNode> pathModules = new Stack<SubmoduleNode>(); 
			Stack<String> fullPathStack = new Stack<String>();

			public void enter(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
				pathModules.push(submodule);
				fullPathStack.push(submodule==null ? submoduleType.getName() : InifileUtils.getSubmoduleFullName(submodule));
				String submoduleFullPath = StringUtils.join(fullPathStack.toArray(), "."); //XXX optimize here if slow
				SubmoduleNode[] pathModulesArray = pathModules.toArray(new SubmoduleNode[]{});
				resolveModuleParameters(list, submoduleFullPath, pathModulesArray, submoduleType, sectionChain, finalDoc);
			}
			public void leave() {
				fullPathStack.pop();
				pathModules.pop();
			}

			public void unresolvedType(SubmoduleNode submodule, String submoduleTypeName) {}
			
			public void recursiveType(SubmoduleNode submodule, INEDTypeInfo submoduleType) {}

			public String resolveLikeType(SubmoduleNode submodule) {
				// Note: we cannot use InifileUtils.resolveLikeParam(), as that calls
				// resolveLikeParam() relies on the data structure we are currently building

				// get like parameter name
				String likeParamName = submodule.getLikeParam();
				if (!likeParamName.matches("[A-Za-z0-9_]+"))
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
				return value;
			}
		});

		treeIterator.traverse(networkName);
		return list;
	}

	protected void resolveModuleParameters(ArrayList<ParamResolution> resultList, String moduleFullPath, SubmoduleNode[] pathModules, INEDTypeInfo moduleType, String[] sectionChain, IInifileDocument doc) {
		for (String paramName : moduleType.getParams().keySet()) {
			ParamNode paramDeclNode = (ParamNode)moduleType.getParams().get(paramName);
			SubmoduleNodeEx submodule = (SubmoduleNodeEx) pathModules[pathModules.length-1];
			ParamNode paramValueNode = submodule==null ?
					(ParamNode)moduleType.getParamValues().get(paramName) :
					(ParamNode)submodule.getParamValues().get(paramName);
			resultList.add(resolveParameter(moduleFullPath, pathModules, paramDeclNode, paramValueNode, sectionChain, doc));
		}
	}

	/**
	 * Determines how a NED parameter gets assigned (inifile, NED file, etc). 
	 * The sectionChain and doc parameters may be null, which means that only parameter 
	 * assignments given in NED will be taken into account.
	 * 
	 * XXX probably not good (does not handle all cases): what if parameter is assigned in a submodule decl? 
	 * what if it's assigned using a /pattern/? this info cannot be expressed in the arg list! 
	 */
	protected static ParamResolution resolveParameter(String moduleFullPath, SubmoduleNode[] pathModules, ParamNode paramDeclNode, ParamNode paramValueNode, String[] sectionChain, IInifileDocument doc) {
		// value in the NED file
		String nedValue = paramValueNode==null ? null : paramValueNode.getValue(); //XXX what if parsed expressions?
		if (StringUtils.isEmpty(nedValue)) nedValue = null;
		boolean isNedDefault = paramValueNode==null ? false : paramValueNode.getIsDefault();

		// look up its value in the ini file
		String activeSection = doc==null ? null : sectionChain[0];
		String iniSection = null;
		String iniKey = null;
		String iniValue = null;
		boolean iniApplyDefault = false;
		if (doc!=null && (nedValue==null || isNedDefault)) {
			String paramFullPath = moduleFullPath + "." + paramDeclNode.getName();
			SectionKey sectionKey = InifileUtils.lookupParameter(paramFullPath, isNedDefault, sectionChain, doc);
			if (sectionKey!=null) {
				iniSection = sectionKey.section;
				iniKey = sectionKey.key;
				if (isNedDefault && iniKey.endsWith(".apply-default")) {
					Assert.isTrue("true".equals(doc.getValue(iniSection, iniKey)));
					iniApplyDefault = true;
				} 
				else {
					iniValue = doc.getValue(iniSection, iniKey);
				}
			}
		}

		// so, find out how the parameter's going to be assigned...
		ParamResolutionType type;
		if (nedValue==null) {
			if (iniValue!=null)
				type = ParamResolutionType.INI;
			else
				type = ParamResolutionType.UNASSIGNED;
		}		
		else {
			if (!isNedDefault)
				type = ParamResolutionType.NED; // value assigned in NED (unchangeable from ini files)
			else if (iniApplyDefault)
				type = ParamResolutionType.NED_DEFAULT; // **.apply-default=true
			else if (iniValue==null)
				type = ParamResolutionType.UNASSIGNED;
			else if (nedValue.equals(iniValue))
				type = ParamResolutionType.INI_NEDDEFAULT;
			else 
				type = ParamResolutionType.INI_OVERRIDE;
		}
		return new ParamResolution(moduleFullPath, pathModules, paramDeclNode, paramValueNode, type, activeSection, iniSection, iniKey);
	}

	public boolean containsSectionCircles() {
		return containsSectionCircles;
	}

	/**
	 * Classify an inifile key, based on its syntax.
	 * XXX syntax rules used here must be enforced throughout the system
	 */
	//XXX into InifileUtils? (KeyType too)
	public static KeyType getKeyType(String key) {
		if (!key.contains(".")) 
			return KeyType.CONFIG;  // contains no dot
		else if (!key.contains("-"))
			return KeyType.PARAM; // contains dot, but no hyphen
		else
			return KeyType.PER_OBJECT_CONFIG; // contains both dot and hyphen
	}

	public boolean isUnusedParameterKey(String section, String key) {
		synchronized (doc) {
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
		synchronized (doc) {
			analyzeIfChanged();
			KeyData data = (KeyData) doc.getKeyData(section,key);
			return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0];
		}
	}

	public String[] getUnusedParameterKeys(String section) {
		synchronized (doc) {
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
		synchronized (doc) {
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
		synchronized (doc) {
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
		synchronized (doc) {
			analyzeIfChanged();
			SectionData sectionData = (SectionData) doc.getSectionData(section);
			return sectionData.allParamResolutions.toArray(new ParamResolution[]{});
		}
	}

	/**
	 * Returns unassigned parameters for the given inifile section.
	 */
	public ParamResolution[] getUnassignedParams(String section) {
		synchronized (doc) {
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
			case UNASSIGNED: remark = "unassigned"; break;
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

}
