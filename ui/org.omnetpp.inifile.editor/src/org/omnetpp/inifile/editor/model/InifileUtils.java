package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Various lookups in inifiles, making use of NED declarations as well.
 * @author Andras
 */
//TODO somehow detect if an inifile line matches parameters of more than one module type; that might be an error.
// i.e. EtherMAC.backoffs and Ieee80211.backoffs.
public class InifileUtils {
	/**
	 * Given a parameter's fullPath, returns the key of the matching
	 * inifile entry, or null the parameter matches nothing. If hasDefault
	 * is set, ".apply-default" entries are also considered.
	 */
	//XXX this issue is much more complicated, as there may be multiple possibly matching 
	// inifile entries. For example, we have "net.node[*].power", and inifile contains
	// "*.node[0..4].power=...", "*.node[5..9].power=...", and "net.node[10..].power=...".
	// Current code would not match any (!!!), only "net.node[*].power=..." if it existed.
	// lookupParameter() should actually return multiple matches.
	//
	public static SectionKey lookupParameter(String paramFullPath, boolean hasNedDefault, String[] sectionChain, IInifileDocument doc) {
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
		void visit(String moduleName, String moduleFullPath, INEDTypeInfo moduleType);
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public static void traverseModuleUsageHierarchy(String moduleName, String moduleFullPath, String moduleTypeName, INEDTypeResolver nedResources, IInifileDocument doc, IModuleTreeVisitor visitor) {
		//FIXME detect circles!!! currently it results in stack overflow
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

		// do useful work: add tree node corresponding to this module
		visitor.visit(moduleName, moduleFullPath, moduleType);

		// traverse submodules
		for (NEDElement node : moduleType.getSubmods().values()) {
			SubmoduleNode submodule = (SubmoduleNode) node;
			String submoduleName = getSubmoduleFullName(submodule);
			String submoduleType = getSubmoduleType(submodule);

			// recursive call
			traverseModuleUsageHierarchy(submoduleName, moduleFullPath+"."+submoduleName, submoduleType, nedResources, doc, visitor);
		}
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
		boolean boolValue;
		if (value.equals("yes") || value.equals("true") || value.equals("on") || value.equals("1"))
	    	boolValue = true;
	    else if (value.equals("no") || value.equals("false") || value.equals("off") || value.equals("0"))
	    	boolValue = false;
	    else
	    	boolValue = false; //XXX something invalid: exception?
		return boolValue;
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

	//XXX section/config/param/perobjectconfig tooltips have to be generated here as well!!!
}
