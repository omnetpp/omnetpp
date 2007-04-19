package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.Stack;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.ui.texteditor.MarkerUtilities;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResources;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * This is a layer above IInifileDocument, and contains info about the
 * relationship of inifile contents and NED. For example, which inifile
 * parameter settings apply to which NED module parameters.
 * 
 * @author Andras
 */
//XXX consider this:
//    Net.host[0].p = ...
//    Net.host[*].p = ...
// Then host[0] should NOT be warned about being unused!!! 
// This affects all keys containing an index which is not "*", that is, where key.matches(".*\\[([^*]|(\\*[^]]+))\\].*")
//
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
		ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
	};

	/**
	 * Used internally: class of objects attached to IInifileDocument sections 
	 * (see getSectionData()/setSectionData())
	 */
	static class SectionData {
		ArrayList<ParamResolution> paramResolutions;
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

	protected synchronized void modelChanged() {
		changed = true;
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
	public synchronized void analyzeIfChanged() {
		if (changed)
			analyze();
	}

	/**
	 * Analyzes the inifile. Side effects: error/warning markers may be placed
	 * on the IFile, and parameter resolutions (see ParamResolution) are 
	 * recalculated.
	 */
	public synchronized void analyze() {
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

		// analyze file
		for (String section : doc.getSectionNames()) {
			doc.setSectionData(section, new SectionData());
			if (!section.equals(GENERAL) && !section.startsWith(CONFIG_))
				addError(section, "Wrong section name: must be [General] or [Config <name>]");
			
			for (String key : doc.getKeys(section)) {
				switch (getKeyType(key)) {
				case CONFIG:
					// must be some configuration, look up in the database
					validateConfig(section, key, ned);
					break;
				case PARAM:
					//XXX rename **.interval (and **.enabled) to **.record-interval, and warn for old name here
					doc.setKeyData(section, key, new KeyData());
					break;
				case PER_OBJECT_CONFIG:
					doc.setKeyData(section, key, new KeyData());
					break;
				}
			}
		}

		// detect circles in the fallback chain, and report them
		detectSectionCircles();
		
		// calculate parameter resolutions for each section
		for (String section : doc.getSectionNames())
			calculateParamResolutions(section, ned);

		System.out.println("Inifile analysed in "+(System.currentTimeMillis()-startTime)+"ms");
		changed = false;

		// warn for unused param keys; this must be done AFTER changed=false
		for (String section : doc.getSectionNames())
			for (String key : getUnusedParameterKeys(section))
				addWarning(section, key, "Unused entry (does not match any parameters)");
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
	 * Validate a configuration entry.
	 */
	//XXX into InifileUtils?
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
		String errorMessage = validateConfigValueByType(value, e.getDataType());
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
	//XXX into InifileUtils?
	protected static String validateConfigValueByType(String value, ConfigurationEntry.DataType type) {
		switch (type) {
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
			try {
				Double.parseDouble(value);
			} catch (NumberFormatException ex) {
				return "Value should be a numeric constant (a double)";
			}
			break;
		case CFG_STRING:
			try {
				if (value.startsWith("\""))
					Common.parseQuotedString(value);
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

	protected void detectSectionCircles() {
		containsSectionCircles = false;
		Set<String> bogusSections = new HashSet<String>();
		
		// check fallback chain for every section, except [General]
		for (String section : doc.getSectionNames()) {
			if (!section.equals(GENERAL)) {
				// follow section fallback sequence, and detect circle in it
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
	 * Calculate how parameters get assigned when the given section is the active one.
	 */
	protected void calculateParamResolutions(String activeSection, INEDTypeResolver ned) {
		String[] sectionChain = InifileUtils.resolveSectionChain(doc, activeSection);
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		if (networkName == null)
			networkName = CFGID_NETWORK.getDefaultValue().toString(); 
		if (networkName == null)
			return;

		// calculate param resolutions
		ArrayList<ParamResolution> resList = collectParameters(networkName, networkName, sectionChain, ned);

		// store with every key the list of parameters it resolves
		for (ParamResolution res : resList) {
			if (res.key != null) {
				((KeyData)doc.getKeyData(res.section, res.key)).paramResolutions.add(res);
			}
		}

		// store with the section the list of all parameter resolutions (incl unassigned params)
		SectionData data = new SectionData();
		data.paramResolutions = resList;
		doc.setSectionData(activeSection, data);
	}

	protected ArrayList<ParamResolution> collectParameters(String moduleFullPath, String moduleTypeName, final String[] sectionChain, INEDTypeResolver ned) {
		//XXX moduleFullPath never used
		String moduleName = moduleFullPath.replaceFirst("^.*\\.", "");  // stuff after the last dot
		final IInifileDocument finalDoc = doc;
		NEDResources res = NEDResourcesPlugin.getNEDResources();
		final ArrayList<ParamResolution> list = new ArrayList<ParamResolution>();

		NEDTreeIterator treeIterator = new NEDTreeIterator(res, new IModuleTreeVisitor() {
			Stack<String> fullPath = new Stack<String>();
			public void enter(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
				fullPath.push(submodule==null ? submoduleType.getName() : InifileUtils.getSubmoduleFullName(submodule));
				String submoduleFullPath = StringUtils.join(fullPath.toArray(), "."); //XXX optimize here if slow
				collectParameters(list, submoduleFullPath, submoduleType, sectionChain, finalDoc);
			}
			public void leave() {
				fullPath.pop();
			}
			public void unresolvedType(SubmoduleNode submodule, String submoduleTypeName) {}
			public void recursiveType(SubmoduleNode submodule, INEDTypeInfo submoduleType) {}
			public String resolveLikeType(SubmoduleNode submodule) {return null;}
		});
		
		treeIterator.traverse(moduleTypeName);
		return list;
	}

	protected void collectParameters(ArrayList<ParamResolution> resultList, String moduleFullPath, INEDTypeInfo moduleType, String[] sectionChain, IInifileDocument doc) {
		//FIXME wrong: submodule param assignments get ignored
		for (NEDElement paramValueNode : moduleType.getParamValues().values()) {
			String paramName = ((ParamNode)paramValueNode).getName();
			ParamNode paramDeclNode = (ParamNode)moduleType.getParams().get(paramName);
			resultList.add(resolveParameter(moduleFullPath, paramDeclNode, (ParamNode)paramValueNode, sectionChain, doc));
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
	protected static ParamResolution resolveParameter(String moduleFullPath, ParamNode paramDeclNode, ParamNode paramValueNode, String[] sectionChain, IInifileDocument doc) {
		// value in the NED file
		String nedValue = paramValueNode.getValue(); //XXX what if parsed expressions?
		if (StringUtils.isEmpty(nedValue)) nedValue = null;
		boolean isNedDefault = paramValueNode.getIsDefault();

		// look up its value in the ini file
		String iniSection = null;
		String iniKey = null;
		String iniValue = null;
		boolean iniApplyDefault = false;
		if (doc!=null && (nedValue==null || isNedDefault)) {
			String paramFullPath = moduleFullPath + "." + paramValueNode.getName();
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
		return new ParamResolution(moduleFullPath, paramDeclNode, paramValueNode, type, iniSection, iniKey);
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
		analyzeIfChanged();
		if (getKeyType(key)!=KeyType.PARAM) 
			return false;
		KeyData data = (KeyData) doc.getKeyData(section,key);
		return data!=null && data.paramResolutions!=null && data.paramResolutions.isEmpty(); 
	}

	/**
	 * Returns the parameter resolutions for the given key. If the returned array is
	 * empty, this key is not used to resolve any module parameters.
	 */
	public ParamResolution[] getParamResolutionsForKey(String section, String key) {
		analyzeIfChanged();
		KeyData data = (KeyData) doc.getKeyData(section,key);
		return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0]; 
	}

	public String[] getUnusedParameterKeys(String section) {
		analyzeIfChanged();
		ArrayList<String> list = new ArrayList<String>();
		for (String key : doc.getKeys(section)) 
			if (isUnusedParameterKey(section, key))
				list.add(key);
		return list.toArray(new String[list.size()]);
	}

	/**
	 * Returns parameter resolutions from the given section that correspond to the 
	 * parameters of the given module.  
	 */
	public ParamResolution[] getParamResolutionsForModule(String moduleFullPath, String section) {
		analyzeIfChanged();
		SectionData data = (SectionData) doc.getSectionData(section);
		ArrayList<ParamResolution> pars = data==null ? null : data.paramResolutions;
		if (pars == null || pars.isEmpty())
			return new ParamResolution[0]; 

		// Note: linear search -- can be made more efficient with some lookup table if needed
		ArrayList<ParamResolution> result = new ArrayList<ParamResolution>();
		for (ParamResolution par : pars) 
			if (par.moduleFullPath.equals(moduleFullPath))
				result.add(par);
		return result.toArray(new ParamResolution[]{});
	}

	/**
	 * Returns all parameter resolutions for the given inifile section; this includes
	 * unassigned parameters as well.  
	 */
	public ParamResolution[] getParamResolutions(String section) {
		analyzeIfChanged();
		SectionData data = (SectionData) doc.getSectionData(section);
		return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0]; 
	}

	/**
	 * Returns unassigned parameters for the given inifile section.
	 */
	public ParamResolution[] getUnassignedParams(String section) {
		analyzeIfChanged();
		SectionData data = (SectionData) doc.getSectionData(section);
		ArrayList<ParamResolution> pars = data==null ? null : data.paramResolutions;
		if (pars == null || pars.isEmpty())
			return new ParamResolution[0]; 

		// Note: linear search -- can be made more efficient with some lookup table if needed
		ArrayList<ParamResolution> result = new ArrayList<ParamResolution>();
		for (ParamResolution par : pars) 
			if (par.type == ParamResolutionType.UNASSIGNED)
				result.add(par);
		return result.toArray(new ParamResolution[]{});
	}

}
