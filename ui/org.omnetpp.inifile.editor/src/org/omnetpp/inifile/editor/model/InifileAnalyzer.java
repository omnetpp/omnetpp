package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.HashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.ui.texteditor.MarkerUtilities;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.inifile.editor.model.InifileUtils.IModuleTreeVisitor;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * This is a layer above IInifileDocument, and contains
 * info about the relationship of inifile contents and 
 * NED. For example, which inifile parameter settings apply
 * to which NED module parameters.
 * 
 * @author Andras
 */
public class InifileAnalyzer {
	public static final String INIFILEANALYZERPROBLEM_MARKER_ID = InifileEditorPlugin.PLUGIN_ID + ".inifileanalyzerproblem";
	private IInifileDocument doc;

	/**
	 * Classifies inifile keys; see getKeyType().
	 */
	public enum KeyType {
		CONFIG, // contains no dot (like sim-time-limit=, etc) 					
		PARAM,  // contains dot, but no hyphen: parameter setting (like **.app.delay) 
		PER_OBJECT_CONFIG; // dotted, and contains hyphen (like **.use-default, rng mapping, vector configuration, etc)
	};

	/**
	 * Used internally: class of objects attached to IInifileDocument entries 
	 * (see getKeyData()/setKeyData())
	 */
	class KeyData {
		ArrayList<ParamResolution> paramResolutions = new ArrayList<ParamResolution>();
	};

	/**
	 * Used internally: class of objects attached to IInifileDocument sections 
	 * (see getSectionData()/setSectionData())
	 */
	class SectionData {
		ArrayList<ParamResolution> paramResolutions;
	}
	
	/**
	 * Constructor.
	 */
	public InifileAnalyzer(IInifileDocument doc) {
		this.doc = doc;
	}

	public IInifileDocument getDocument() {
		return doc;
	}

	/**
	 * Analyzes the inifile. Side effects: error/warning markers may be placed
	 * on the IFile, and parameter resolutions (see ParamResolution) are 
	 * recalculated.
	 */
	public void run() {
		long startTime = System.currentTimeMillis();
		INEDTypeResolver ned = NEDResourcesPlugin.getNEDResources();
		
		// remove existing markers
		try {
			IFile file = doc.getDocumentFile();
			file.deleteMarkers(INIFILEANALYZERPROBLEM_MARKER_ID, true, 0);
		} catch (CoreException e) {
			InifileEditorPlugin.logError(e);
		}
		
		// analyze file
		for (String section : doc.getSectionNames()) {
			for (String key : doc.getKeys(section)) {
				switch (getKeyType(key)) {
				case CONFIG:
					// must be some configuration, look up in the database
					validateConfig(section, key, ned);
					break;
				case PARAM:
					//XXX rename **.interval to **.record-interval, and warn for old name here
					doc.setData(section, key, new KeyData());
					break;
				case PER_OBJECT_CONFIG:
					doc.setData(section, key, new KeyData());
					break;
				}
			}
			
			doc.setData(section, new SectionData());
			calculateParamResolutions(section, ned);

			for (String key : getUnusedParameterKeys(section))
				addWarning(section, key, "Unused entry (does not match any parameters)");
		}
		System.out.println("Inifile analysed in "+(System.currentTimeMillis()-startTime)+"ms");
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

	protected void validateConfig(String section, String key, INEDTypeResolver ned) {
		ConfigurationEntry e = ConfigurationRegistry.getEntry(key);
		if (e == null) {
			addError(section, key, "Unknown configuration entry: "+key);
		}
		if (key.equals(ConfigurationRegistry.CFGID_EXPRESS_MODE.getName()) && section.equals("General")) {
			addError(section, key, "Key \""+key+"\" cannot occur in the [General] section");
		}
		else if (e.isGlobal() && !section.equals("General")) {
			addError(section, key, "Key \""+key+"\" can only be specified globally, in the [General] section");
		}
		
		//XXX check the syntax of the value too, etc...
		//XXX check validity of some settings, like network=, preload-ned-files=, etc
	}

	protected void calculateParamResolutions(String section, INEDTypeResolver ned) {
		String networkName = doc.getValue("General", "network"); //XXX or [Run X], if that's the selected one!
		if (networkName == null) {
			//XXX displayMessage("Network not specified (no [General]/network= setting)");
			return;
		}

		// calculate param resolutions
		ArrayList<ParamResolution> resList = collectParameters(networkName, networkName, ned);
		
		// store with every key the list of parameters it resolves
		for (ParamResolution res : resList) {
			if (res.key != null) {
				((KeyData)doc.getKeyData(res.section, res.key)).paramResolutions.add(res);
			}
		}

		// store with the section the list of all parameter resolutions (incl unassigned params)
		SectionData data = new SectionData();
		data.paramResolutions = resList;
		doc.setData(section, data);
	}

	protected ArrayList<ParamResolution> collectParameters(String moduleFullPath, String moduleTypeName, INEDTypeResolver ned) {
		String moduleName = moduleFullPath.replaceFirst("^.*\\.", "");  // stuff after the last dot
		final IInifileDocument finalDoc = doc;
		final ArrayList<ParamResolution> list = new ArrayList<ParamResolution>();
		InifileUtils.traverseModuleUsageHierarchy(moduleName, moduleFullPath, moduleTypeName, ned, doc, new IModuleTreeVisitor() {
			public void visit(String moduleName, String moduleFullPath, INEDTypeInfo moduleType) {
				collectParameters(list, moduleFullPath, moduleType, finalDoc);
			}
			public void visitUnresolved(String moduleName, String moduleFullPath, String moduleTypeName) {
			}
		});
		return list;
	}

	protected void collectParameters(ArrayList<ParamResolution> resultList, String moduleFullPath, INEDTypeInfo moduleType, IInifileDocument doc) {
		for (NEDElement node : moduleType.getParamValues().values()) {
			resultList.add(resolveParameter(moduleFullPath, (ParamNode)node, doc));
		}
	}

	/**
	 * Determines how a NED parameter gets assigned (inifile, NED file, etc). 
	 * The IInifile parameter may be null, which means that only parameter 
	 * assignments given in NED will be taken into account.
	 * 
	 * XXX should take section name. also: do fallback from [Run X] to [Parameters]!
	 * XXX probably not good (does not handle all cases): what if parameter is assigned in a submodule decl? 
	 * what if it's assigned using a /pattern/? this info cannot be expressed in the arg list! 
	 */
	public static ParamResolution resolveParameter(String moduleFullPath, ParamNode param, IInifileDocument doc) {
		// value in the NED file
		String nedValue = param.getValue(); //XXX what if parsed expressions?
		if (StringUtils.isEmpty(nedValue)) nedValue = null;
		boolean isDefault = param.getIsDefault(); //XXX should be used somehow

		// look up its value in the ini file
		String iniKey = null;
		String iniValue = null;
		String iniUseDefaultKey = null;
		boolean iniUseDefault = false;
		if (doc != null) {
			String paramFullPath = moduleFullPath + "." + param.getName();
			iniKey = InifileUtils.lookupParameter(paramFullPath, doc, "General"); //XXX run-specific sections too!
			iniValue = doc.getValue("General", iniKey);
			iniUseDefaultKey = InifileUtils.lookupParameter(paramFullPath+".use-default", doc, "General");
			String iniUseDefaultStr = doc.getValue("General", iniKey);
			iniUseDefault = "true".equals(iniUseDefaultStr);
		}

		//XXX this issue is much more complicated, as there may be multiple possibly matching 
		// inifile entries. For example, we have "net.node[*].power", and inifile contains
		// "*.node[0..4].power=...", "*.node[5..9].power=...", and "net.node[10..].power=...".
		// Current code would not match any (!!!), only "net.node[*].power=..." if it existed.
		// lookupParameter() should actually return multiple matches. 
		if (nedValue==null && iniValue==null) {
			return new ParamResolution(moduleFullPath, param, ParamResolutionType.UNASSIGNED, null, null);
		}
		else {
			ParamResolutionType type;
			if (nedValue!=null && iniValue==null) {
				type = ParamResolutionType.NED; //XXX default(x) or not??
			}
			else if (nedValue==null && iniValue!=null) {
				type = ParamResolutionType.INI;
			}
			else if (nedValue.equals(iniValue)) {
				type = ParamResolutionType.INI_NEDDEFAULT;
			}
			else {
				type = ParamResolutionType.INI;
			}
			//XXX return Type.DEFAULTED sometimes
			return new ParamResolution(moduleFullPath, param, type, "General", iniKey);
		}
	}

	/**
	 * Classify an inifile key, based on its syntax.
	 * XXX syntax rules used here must be enforced throughout the system
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
		KeyData data = (KeyData) doc.getKeyData(section,key);
		return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0]; 
	}

	public String[] getUnusedParameterKeys(String section) {
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
		SectionData data = (SectionData) doc.getSectionData(section);
		return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0]; 
	}

	/**
	 * Returns unassigned parameters for the given inifile section.
	 */
	public ParamResolution[] getUnassignedParams(String section) {
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
