package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
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
	 * inifile entry in the given section, or null if it's not 
	 * in the given inifile section. 
	 */
	public static String lookupParameter(String paramFullPath, IInifileDocument doc, String section) {
		String[] keys = doc.getKeys(section);
		if (keys != null)
			for (String key : keys)
				if (new PatternMatcher(key, true, true, true).matches(paramFullPath))
					return key;
		return null;
	}

	/**
	 * Visitor for traverseModuleUsageHierarchy().
	 */
	public static interface IModuleTreeVisitor {
		void visitUnresolved(String moduleName, String moduleFullPath, String moduleTypeName);
		void visit(String moduleName, String moduleFullPath, INEDTypeInfo moduleType);
	}

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
	 * Follows the section "extends" chain back to the [General] section, and returns
	 * the list of section names (including the given section and [General] as well).
	 */
	public static String[] resolveSectionChain(IInifileDocument doc, String section) {
		ArrayList<String> sectionChain = new ArrayList<String>();
		if (!section.equals("General")) {
			String currentSection = section;
		    while (true) {
		        if (sectionChain.contains(currentSection))
		            throw new IllegalStateException("circularity detected in section fallback chain at section ["+currentSection+"]");
		        sectionChain.add(currentSection);
		        String extendsName = doc.getValue(currentSection, "extends");
		        if (extendsName==null)
		        	break;
		        currentSection = "Config "+extendsName;
		    }
		}
	    if (doc.containsSection(GENERAL))
	        sectionChain.add(GENERAL);
	    return sectionChain.toArray(new String[] {});
	}

	public static boolean sectionChainContains(IInifileDocument doc, String chainStartSection, String section) {
		String[] sectionChain = resolveSectionChain(doc, chainStartSection);
		return ArrayUtils.indexOf(sectionChain, section) >= 0;
	}
	
	public static String validateParameterKey(IInifileDocument doc, String section, String key) {
		if (key==null || key.equals(""))
			return "Key cannot be empty";
		if (!key.contains("."))
			return "Parameter keys must contain at least one dot";
		if (doc.containsKey(section, key))
			return "Key "+key+" already exists in section ["+section+"]";
		return null;
	}
	
}
