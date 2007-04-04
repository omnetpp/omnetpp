package org.omnetpp.inifile.editor.model;

import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * Stores the result of a parameter resolution.
 */
public class ParamResolution {
	public enum ParamResolutionType {
		UNASSIGNED, // unassigned parameter
		NED, // parameter assigned in NED
		NED_DEFAULT, // parameter set to the default value (**.use-default=true)
		INI, // parameter assigned in inifile, with no default in NED file
		INI_OVERRIDE, // inifile setting overrides NED default
		INI_NEDDEFAULT, // inifile sets param to its NED default value
	}
	
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

	/* Generated; needed for GenericTreeUtil.treeEquals() */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		final ParamResolution other = (ParamResolution) obj;
		if (key == null) {
			if (other.key != null)
				return false;
		} else if (!key.equals(other.key))
			return false;
		if (moduleFullPath == null) {
			if (other.moduleFullPath != null)
				return false;
		} else if (!moduleFullPath.equals(other.moduleFullPath))
			return false;
		if (paramNode == null) {
			if (other.paramNode != null)
				return false;
		} else if (!paramNode.equals(other.paramNode))
			return false;
		if (section == null) {
			if (other.section != null)
				return false;
		} else if (!section.equals(other.section))
			return false;
		if (type == null) {
			if (other.type != null)
				return false;
		} else if (!type.equals(other.type))
			return false;
		return true;
	}
}

