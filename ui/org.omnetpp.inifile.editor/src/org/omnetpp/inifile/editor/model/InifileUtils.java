package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Various lookups in inifiles, making use of NED declarations as well.
 * This class cannot be instantiated, all functionality is provided via
 * static methods.
 * 
 * @author Andras
 */
//TODO somehow detect if an inifile line matches parameters of more than one module type; that might be an error.
// i.e. EtherMAC.backoffs and Ieee80211.backoffs.
public class InifileUtils {
	/**
	 * Looks up a configuration value in the given section or its fallback sections.
	 * Returns null if not found.
	 */
	public static String lookupConfig(String section, String key, IInifileDocument doc) {
		String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
		return lookupConfig(sectionChain, key, doc);
	}

	/**
	 * Looks up a configuration value. Returns null if not found.
	 */
	public static String lookupConfig(String[] sectionChain, String key, IInifileDocument doc) {
		for (String section : sectionChain)
			if (doc.containsKey(section, key))
				return doc.getValue(section, key);
		return null;
	}

	/**
	 * Given a parameter's fullPath, returns the key of the matching
	 * inifile entry, or null the parameter matches nothing. If hasDefault
	 * is set, ".apply-default" entries are also considered.
	 */
	public static SectionKey lookupParameter(String paramFullPath, boolean hasNedDefault, String[] sectionChain, IInifileDocument doc) {
		//
		//XXX this issue is much more complicated, as there may be multiple possibly matching 
		// inifile entries. For example, we have "net.node[*].power", and inifile contains
		// "*.node[0..4].power=...", "*.node[5..9].power=...", and "net.node[10..].power=...".
		// Current code would not match any (!!!), only "net.node[*].power=..." if it existed.
		// lookupParameter() should actually return multiple matches.
		//
		String paramApplyDefault = paramFullPath + ".apply-default";
		boolean considerApplyDefault = hasNedDefault;
		for (String section : sectionChain) {
			for (String key : doc.getKeys(section)) {
				if (new PatternMatcher(key, true, true, true).matches(paramFullPath))
					return new SectionKey(section, key);
				if (considerApplyDefault && new PatternMatcher(key, true, true, true).matches(paramApplyDefault))
					if (doc.getValue(section, key).equals("true"))
						return new SectionKey(section, key);
					else
						considerApplyDefault = false;
			}
		}
		return null;
	}

	/**
	 * Visitor for traverseModuleUsageHierarchy().
	 */
	public static interface IModuleTreeVisitor {
		void visitUnresolved(String moduleName, String moduleFullPath, String moduleTypeName);
		void enter(String moduleName, String moduleFullPath, INEDTypeInfo moduleType);
		void leave(String moduleName, String moduleFullPath, INEDTypeInfo moduleType);
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public static void traverseModuleUsageHierarchy(String moduleName, String moduleFullPath, String moduleTypeName, INEDTypeResolver nedResources, IInifileDocument doc, IModuleTreeVisitor visitor) {
		Stack<INEDTypeInfo> visitedTypes = new Stack<INEDTypeInfo>();
		doTraverseModuleUsageHierarchy(moduleName, moduleFullPath, moduleTypeName, nedResources, doc, visitor, visitedTypes);
	}

	private static void doTraverseModuleUsageHierarchy(String moduleName, String moduleFullPath, String moduleTypeName, INEDTypeResolver nedResources, IInifileDocument doc, IModuleTreeVisitor visitor, Stack<INEDTypeInfo> visitedTypes) {
		// dig out type info (NED declaration)
		if (StringUtils.isEmpty(moduleTypeName)) {
			visitor.visitUnresolved(moduleName, moduleFullPath, null);
			return;
		}
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType == null) {
			visitor.visitUnresolved(moduleName, moduleFullPath, moduleTypeName);
			return;
		}

		// cycle detection
		if (visitedTypes.contains(moduleType)) {
			return; //XXX signal cycle!
		}
		visitedTypes.push(moduleType);
		
		// enter module
		visitor.enter(moduleName, moduleFullPath, moduleType);

		// traverse submodules
		for (NEDElement node : moduleType.getSubmods().values()) {
			SubmoduleNode submodule = (SubmoduleNode) node;
			String submoduleName = getSubmoduleFullName(submodule);
			String submoduleType = getSubmoduleType(submodule);

			// recursive call
			doTraverseModuleUsageHierarchy(submoduleName, moduleFullPath+"."+submoduleName, submoduleType, nedResources, doc, visitor, visitedTypes);
		}

		// leave module
		visitor.leave(moduleName, moduleFullPath, moduleType);
		
		visitedTypes.pop();
	}

	/**
	 * Returns the submodule name. If vector, appends [*].
	 */
	public static String getSubmoduleFullName(SubmoduleNode submodule) {
		String submoduleName = submodule.getName();
		if (!StringUtils.isEmpty(submodule.getVectorSize())) //XXX what if parsed expressions are in use?
			submoduleName += "[*]"; //XXX
		return submoduleName;
	}

	/**
	 * Returns the submodule type name. If it uses "like", returns the "like" name.
	 */
	public static String getSubmoduleType(SubmoduleNode submodule) {
		//XXX should try to evaluate "like" expression and use result as type (if possible)
		String submoduleType = submodule.getType();
		if (StringUtils.isEmpty(submoduleType))
			submoduleType = submodule.getLikeType();
		return submoduleType;
	}
	
	/**
	 * Follows the section "extends" chain back to the [General] section, and
	 * returns the list of section names (including the given section and
	 * [General] as well).
	 * 
	 * Errors (such as nonexistent section, circle in the fallback chain, etc)
	 * are handled in a forgiving way, and a reasonably complete section chain
	 * is returned without throwing an exception -- so this method may be safely
	 * called during any calculation.
	 */
	public static String[] resolveSectionChain(IInifileDocument doc, String section) {
		ArrayList<String> sectionChain = new ArrayList<String>();
		if (!section.equals(GENERAL)) {
			String currentSection = section;
		    while (true) {
		    	if (!doc.containsSection(currentSection))
		            break; // error: nonexisting section
		        if (sectionChain.contains(currentSection))
		            break; // error: circle in the fallback chain
		        sectionChain.add(currentSection);
		        String extendsName = doc.getValue(currentSection, CFGID_EXTENDS.getKey());
		        if (extendsName==null)
		        	break; // done
		        currentSection = CONFIG_+extendsName;
		    }
		}
	    if (doc.containsSection(GENERAL))
	        sectionChain.add(GENERAL);
	    return sectionChain.toArray(new String[] {});
	}

	/**
	 * Whether the section chain contains the given section. Useful for detecting 
	 * circles in the "extends" hierarchy.
	 */
	public static boolean sectionChainContains(IInifileDocument doc, String chainStartSection, String section) {
		String[] sectionChain = resolveSectionChain(doc, chainStartSection);
		return ArrayUtils.indexOf(sectionChain, section) >= 0;
	}
	
	/**
	 * Useful as a basis for an IInputValidator, e.g. for a cell editor or InputDialog.
	 */
	public static String validateParameterKey(IInifileDocument doc, String section, String key) {
		if (key==null || key.equals(""))
			return "Key cannot be empty";
		if (!key.contains("."))
			return "Parameter keys must contain at least one dot";
		if (doc.containsKey(section, key))
			return "Key "+key+" already exists in section ["+section+"]";
		return null;
	}

	/**
	 * Parse a boolean config value. Anything not recognized also counts as false.
	 */
	public static boolean parseAsBool(String value) {
		if (value.equals("yes") || value.equals("true") || value.equals("on") || value.equals("1"))
	    	return true;
	    else if (value.equals("no") || value.equals("false") || value.equals("off") || value.equals("0"))
	    	return false;
	    else
	    	return false; // unrecongnized
	}
	
	/**
	 * Insert a section at the right place in the file.
	 * [General] at top, and other sections ordered alphabetically.
	 */
	public static void addSection(IInifileDocument doc, String newSection) {
		if (doc.containsSection(newSection)) 
			return;
		String[] sections = doc.getSectionNames();
		if (newSection.equals(GENERAL)) {
			doc.addSection(newSection, sections.length==0 ? null : sections[0]);
			return;
		}
		for (String section : sections) {
			if (section.compareToIgnoreCase(newSection) > 0 && !section.equals(GENERAL)) {
				doc.addSection(newSection, section);
				return;
			}				
		}
		doc.addSection(newSection, null);
	}

	/**
	 * Insert a key at the right place in the file. Config keys at top
	 * (extends= first, description= next, network= after, and the
	 * rest follows in alphabetical order), then parameters.  
	 * If even the section is not present, it is added first.
	 * The entry MUST NOT exist yet.
	 */
	public static void addEntry(IInifileDocument doc, String section, String newKey, String value, String comment) {
		if (!doc.containsSection(section))
			addSection(doc, section);
		Assert.isTrue(!doc.containsKey(section, newKey));
		String keys[] = doc.getKeys(section);
		for (String key : keys) {
			if (precedesKey(newKey, key)) {
				doc.addEntry(section, newKey, value, comment, key);
				return;
			}				
		}
		doc.addEntry(section, newKey, value, comment, null);
	}

	private static boolean precedesKey(String key1, String key2) {
		if (key1.equals(CFGID_EXTENDS.getKey())) return true;
		if (key2.equals(CFGID_EXTENDS.getKey())) return false;
		if (key1.equals(CFGID_DESCRIPTION.getKey())) return true;
		if (key2.equals(CFGID_DESCRIPTION.getKey())) return false;
		if (key1.equals(CFGID_NETWORK.getKey())) return true;
		if (key2.equals(CFGID_NETWORK.getKey())) return false;
		if (key1.contains(".")) return false;
		if (key2.contains(".")) return true;
		return key1.compareToIgnoreCase(key2) < 0;
	}
	
	/**
	 * Renames the given section. Also changes the extends= keys in other sections
	 * that refer to it.
	 */
	public static void renameSection(IInifileDocument doc, String oldSectionName, String newSectionName) {
		doc.renameSection(oldSectionName, newSectionName);

		String oldName = oldSectionName.replaceFirst("^Config +", "");
		String newName = newSectionName.replaceFirst("^Config +", "");
		for (String section : doc.getSectionNames())
			if (oldName.equals(doc.getValue(section, CFGID_EXTENDS.getKey())))
				doc.setValue(section, CFGID_EXTENDS.getKey(), newName);
	}

	/**
	 * Generates tooltip for an inifile section.
	 * @param section  null is accepted
	 */
	public static String getSectionTooltip(String section, IInifileDocument doc, InifileAnalyzer analyzer) {
		if (section == null || !doc.containsSection(section))
			return null;
		
		// name and description
		String text = "Section ["+section+"]";
		String description = doc.getValue(section, "description");
		if (description!=null) 
			text += " -- " + description;
		text += "\n";
		
		// section chain 
		String[] sectionChain = resolveSectionChain(doc, section);
		if (sectionChain.length >= 2)
			text += "   fallback order: " + StringUtils.join(sectionChain, " > ") + "\n"; //XXX decide terminology: "Lookup order" or "Section fallback chain" ? also: "," or ">" ? 
		
		// unassigned parameters
		if (analyzer != null) {
			ParamResolution[] resList = analyzer.getUnassignedParams(section);
			if (resList.length==0) { 
				text += "\nThis section seems to contain no unassigned NED parameters";
			}
			else {
				text += "\nThis section does not seem to assign the following NED parameters: \n";
				for (ParamResolution res : resList)
					text += "  - " + res.moduleFullPath + "." +res.paramNode.getName() + "\n";
			}
		}
		return text.trim();
	}

	/**
	 * Generate tooltip for an inifile entry.
	 * @param section  null is accepted
	 * @param key      null is accepted
	 */
	//XXX should tolerate analyzer==null
	public static String getEntryTooltip(String section, String key, IInifileDocument doc, InifileAnalyzer analyzer) {
		if (section == null || key == null || !doc.containsKey(section, key))
			return null;

		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.CONFIG) {
			// config key: display description
			ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
			if (entry == null)
				return null;
			return getConfigTooltip(entry, doc);
		}
		else if (keyType == KeyType.PARAM) {
			// parameter assignment: display which parameters it matches
			//XXX show more info: module type name ("TCP"), parameter type name ("double"), its docu maybe
			//XXX in [General], multiple lines are displayed for the same key! add explanation: "...while examining parameters for [Config Foo]"
			ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
			if (resList.length==0) 
				return "Entry \"" + key + "\" does not match any module parameters ";
			String text = "Entry \"" + key + "\" applies to the following module parameters: \n";
			for (ParamResolution res : resList)
				text += "  - " + res.moduleFullPath + "." +res.paramNode.getName() + "\n"; //XXX do we have module type, param type, maybe param doc etc?
			return text;
		}
		else if (keyType == KeyType.PER_OBJECT_CONFIG) {
			return null; // TODO for .apply-default, display parameters to which it applies
		}
		else {
			return null; // should not happen (invalid key type)
		}
	}

	/**
	 * Generates tooltip for a config entry.
	 */
	public static String getConfigTooltip(ConfigurationEntry entry, IInifileDocument doc) {
		String text = "[General]"+(entry.isGlobal() ? "" : " or [Config X]")+" / "+entry.getKey();
		text += " = <" + entry.getDataType().name().replaceFirst("CFG_", ""); 
		if (!"".equals(entry.getDefaultValue()))
			text += ", default: " + entry.getDefaultValue();
		text += "> \n\n";
		text += entry.getDescription() + "\n";

		if (doc != null) {
			List<String> sectionList = new ArrayList<String>();
			for (String sec : doc.getSectionNames())
				if (doc.containsKey(sec, entry.getKey()))
					sectionList.add(sec);
			if (sectionList.size()==0)
				text += "\nCurrently not set in any sections.\n";
			else
				text += "\nSet in the following sections: "+StringUtils.join(sectionList.toArray(), ", ")+"\n";
		}
		
		return StringUtils.breakLines(text, 80);
	}
	
	/**
	 * Returns the problem markers for a given inifile entry.
	 */
	//XXX use this in field editors etc.
	public static IMarker[] getProblemMarkersFor(String section, String key, IInifileDocument doc) {
		try {
			LineInfo line = doc.getEntryLineDetails(section, key);
			if (line==null)
				return new IMarker[0];
			IFile file = line.getFile();
			IMarker[] markers = file.findMarkers(IMarker.PROBLEM, true, 0);
			ArrayList<IMarker> result = new ArrayList<IMarker>();
			for (IMarker marker : markers) {
				int lineNumber = marker.getAttribute(IMarker.LINE_NUMBER, -1);
				if (lineNumber == line.getLineNumber())
					result.add(marker);
			}
			return result.toArray(new IMarker[]{});
		} 
		catch (CoreException e) {
			InifileEditorPlugin.logError(e);
			return new IMarker[0];
		}
	}
}
