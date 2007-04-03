package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Various lookups in inifiles, making use of NED declarations as well.
 * @author Andras
 */
//TODO somehow detect if an inifile line matches parameters of more than one module type; that might be an error.
// i.e. EtherMAC.backoffs and Ieee80211.backoffs.
public class InifileUtils {
	// to
	public enum Type {
		ERROR, // unknown module type etc 
		UNASSIGNED_PAR, // unassigned parameter
		NED_PAR, // parameter assigned in NED
		INI_PAR, // parameter assigned in inifile
		DEFAULTED_PAR, // parameter set to the default value (**.use-default=true)
		INI_PAR_REDUNDANT, // inifile sets param to its NED default value
		OTHER  // none of the above
	}
	
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
	 * Value class for returning data from collectParameters()
	 */
	public static class ParameterData {
		public Type type;
		public NEDElement element;
		public String moduleFullPath;
		public String parameterName;
		public String value;
		public String remark;

		/* for convenience */
		public ParameterData(Type type, NEDElement element, String moduleFullPath, String parameterName, String value, String remark) {
			this.type = type;
			this.element = element;
			this.moduleFullPath = moduleFullPath;
			this.parameterName = parameterName;
			this.value = value;
			this.remark = remark;
		}
	}
	
	public static ParameterData[] collectParameters(String moduleFullPath, String moduleTypeName, INEDTypeResolver nedResources, IInifileDocument doc, boolean unassignedOnly, final boolean collectErrors) {
		String moduleName = moduleFullPath.replaceFirst("^.*\\.", "");  // stuff after the last dot
		final IInifileDocument finalDoc = doc;
		final boolean finalUnassignedOnly = unassignedOnly;
		final ArrayList<ParameterData> list = new ArrayList<ParameterData>();
		traverseModuleUsageHierarchy(moduleName, moduleFullPath, moduleTypeName, nedResources, doc, new IModuleTreeVisitor() {
			public void visit(String moduleName, String moduleFullPath, INEDTypeInfo moduleType) {
				collectParameters(list, moduleFullPath, moduleType, finalDoc, finalUnassignedOnly);
			}
			public void visitUnresolved(String moduleName, String moduleFullPath, String moduleTypeName) {
				if (collectErrors)
					addErrorEntry(list, moduleFullPath, moduleTypeName);
			}
		});
		return list.toArray(new ParameterData[0]);
	}

	private static void collectParameters(ArrayList<ParameterData> resultList, String moduleFullPath, INEDTypeInfo moduleType, IInifileDocument doc, boolean unassignedOnly) {

		for (NEDElement node : moduleType.getParamValues().values()) {
			ParamNode param = (ParamNode) node;

			// value in the NED file
			String nedValue = param.getValue(); //XXX what if parsed expressions?
			if (StringUtils.isEmpty(nedValue)) nedValue = null;
			boolean isDefault = param.getIsDefault(); //XXX should be used somehow

			// look up its value in the ini file
			String iniValue = null;
			if (doc != null) {
				String paramFullPath = moduleFullPath + "." + param.getName();
				String iniKey = InifileUtils.lookupParameter(paramFullPath, doc, "Parameters"); //XXX run-specific sections too!
				iniValue = doc.getValue("Parameters", iniKey);
				//XXX observe "**.use-default=true" style settings as well!!!
			}

			//XXX this issue is much more complicated, as there may be multiple possibly matching 
			// inifile entries. For example, we have "net.node[*].power", and inifile contains
			// "*.node[0..4].power=...", "*.node[5..9].power=...", and "net.node[10..].power=...".
			// Current code would not match any (!!!), only "net.node[*].power=..." if it existed.
			// lookupParameter() should actually return multiple matches. 
			if (nedValue==null && iniValue==null) {
				resultList.add(new ParameterData(Type.UNASSIGNED_PAR, param, moduleFullPath, param.getName(), "", "unassigned"));
			}
			else if (!unassignedOnly) {
				Type type;
				String value;
				String remark;
				if (nedValue!=null && iniValue==null) {
					type = Type.NED_PAR;
					value = nedValue; 
					remark = "NED";  //XXX default(x) or not??
				}
				else if (nedValue==null && iniValue!=null) {
					type = Type.INI_PAR;
					value = iniValue;
					remark = "ini file";
				}
				else if (nedValue.equals(iniValue)) {
					type = Type.INI_PAR_REDUNDANT;
					value = nedValue;
					remark = "same value both in NED and ini file";
				}
				else {
					type = Type.INI_PAR;
					value = iniValue;
					remark = "ini file (overrides NED value "+nedValue+")"; //XXX if NED is default()!
				}
				//XXX return Type.DEFAULTED sometimes
				resultList.add(new ParameterData(type, param, moduleFullPath, param.getName(), value, remark));
			}
		}
	}

	private static void addErrorEntry(final ArrayList<ParameterData> list, String moduleFullPath, String moduleTypeName) {
		list.add(new ParameterData(Type.ERROR, null, moduleFullPath, "", "", "no such module type: "+moduleTypeName));
	}
}
