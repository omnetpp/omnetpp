package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileUtils.IModuleTreeVisitor;
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
	private IInifileDocument doc;

	/**
	 * Classifies inifile keys; see getKeyType().
	 */
	public enum KeyType {
		CONFIG, // contains no dot (like sim-time-limit=, etc) 					
		PARAM,  // contains dot, but no hyphen: parameter setting (like **.app.delay) 
		PER_OBJECT_CONFIG; // dotted, and contains hyphen (like **.use-default, rng mapping, vector configuration, etc)
	};

	public enum ParamResolutionType {
		UNASSIGNED, // unassigned parameter
		NED, // parameter assigned in NED
		NED_DEFAULT, // parameter set to the default value (**.use-default=true)
		INI, // parameter assigned in inifile, with no default in NED file
		INI_OVERRIDE, // inifile setting overrides NED default
		INI_NEDDEFAULT, // inifile sets param to its NED default value
	} 
	
	/**
	 * Stores the result of a parameter resolution.
	 */
	public static class ParamResolution {
		// moduleFullPath and paramNode identify the NED parameter 
		public String moduleFullPath;
		public ParamNode paramNode;
		// how the parameter value gets resolved: from NED, from inifile, unassigned, etc
		public ParamResolutionType type;
		// section+key identify the value assignment in the inifile; 
		// they are null if parameter is assigned from NED
		public String section;
		public String key;
		
		// for convenience
		public ParamResolution(String moduleFullPath, ParamNode paramNode, ParamResolutionType type, String section, String key) {
			this.moduleFullPath = moduleFullPath;
			this.paramNode = paramNode;
			this.type = type;
			this.section = section;
			this.key = key;
		}
	}
	
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
		
		for (String section : doc.getSectionNames()) {
			boolean isParamSection = isParamSection(section);
			for (String key : doc.getKeys(section)) {
				switch (getKeyType(key)) {
				case CONFIG:
					// must be some configuration, look up in the database
					validateConfig(section, key, ned);
					break;
				case PARAM:
					//XXX rename **.interval to **.record-interval, and warn for old name here
					if (isParamSection)
						doc.setData(section, key, new KeyData());
					else
						System.out.println("ERROR: parameter settings must be in section [Parameters] or in a run-specific section [Run X]");
					break;
				case PER_OBJECT_CONFIG:
					if (isParamSection)
						doc.setData(section, key, new KeyData());
					else
						System.out.println("ERROR: this configuration must be in section [Parameters] or in a run-specific section [Run X]"); //XXX really?
					break;
				}
			}
			
			if (isParamSection) {
				doc.setData(section, new SectionData());
				calculateParamResolutions(section, ned);
			}
		}
		System.out.println("Inifile analysed in "+(System.currentTimeMillis()-startTime)+"ms");
	}
	
	protected void validateConfig(String section, String key, INEDTypeResolver ned) {
		ConfigurationEntry e = ConfigurationRegistry.getEntry(key);
		boolean isPerRunSection = section.startsWith("Run ");
		if (e == null) {
			System.out.println("ERROR: unknown configuration entry: "+key);
		}
		else if (e.isGlobal() && !section.equals(e.getSection())) {
			System.out.println("ERROR: key "+key+" occurs in wrong section, must be in ["+e.getSection()+"]");
		}
		else if (!e.isGlobal() && !section.equals(e.getSection()) && !isPerRunSection) {
			System.out.println("ERROR: key "+key+" occurs in wrong section, must be in ["+e.getSection()+"], or in a run-specific section [Run X]");
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
		String iniValue = null;
		String iniKey = null;
		if (doc != null) {
			String paramFullPath = moduleFullPath + "." + param.getName();
			iniKey = InifileUtils.lookupParameter(paramFullPath, doc, "Parameters"); //XXX run-specific sections too!
			iniValue = doc.getValue("Parameters", iniKey);
			//XXX observe "**.use-default=true" style settings as well!!!
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
			return new ParamResolution(moduleFullPath, param, type, "Parameters", iniKey);
		}
	}

	/**
	 * Returns whether a section of the given name may contain parameter assignments. 
	 */
	public static boolean isParamSection(String section) {
		return section.equals("Parameters") || section.startsWith("Run ");
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
	
	public boolean isUsed(String section, String key) {
		KeyData data = (KeyData) doc.getKeyData(section,key);
		return data!=null && data.paramResolutions!=null && !data.paramResolutions.isEmpty(); 
	}

	/**
	 * Returns the parameter resolutions for the given key. If the returned array is
	 * empty, this key is not used to resolve any module parameters.
	 */
	public ParamResolution[] getParamResolutionsForKey(String section, String key) {
		KeyData data = (KeyData) doc.getKeyData(section,key);
		return (data!=null && data.paramResolutions!=null) ? data.paramResolutions.toArray(new ParamResolution[]{}) : new ParamResolution[0]; 
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
	public ParamResolution[] getUnresolvedParams(String section) {
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
