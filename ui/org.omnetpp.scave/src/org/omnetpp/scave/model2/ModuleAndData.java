/**
 * 
 */
package org.omnetpp.scave.model2;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;

import org.apache.commons.lang.ObjectUtils;
import org.omnetpp.common.util.StringUtils;

public class ModuleAndData implements Comparable<ModuleAndData>
{
	public String moduleName;
	public String dataName;
	
	public ModuleAndData() {
	}
	
	public ModuleAndData(String moduleName, String dataName) {
		this.moduleName = moduleName;
		this.dataName = dataName;
	}
	
	public boolean isValid() {
		return !StringUtils.isEmpty(moduleName) && !StringUtils.isEmpty(dataName);
	}
	
	public String getModuleName() {
		return moduleName;
	}

	public void setModuleName(String moduleName) {
		this.moduleName = moduleName;
	}

	public String getDataName() {
		return dataName;
	}

	public void setDataName(String dataName) {
		this.dataName = dataName;
	}

	public static ModuleAndData fromFilterPattern(String pattern) {
		FilterUtil filter = pattern != null ? new FilterUtil(pattern, true) : null;
		return filter == null || filter.isLossy() ?
				new ModuleAndData() :
				new ModuleAndData(filter.getField(MODULE),
								  filter.getField(NAME));
	}
	
	public String asListItem() {
		return moduleName + "/" + dataName;
	}
	
	public String asFilterPattern() {
		FilterUtil filter = new FilterUtil();
		filter.setField(MODULE, moduleName);
		filter.setField(NAME, dataName);
		return filter.getFilterPattern();
	}

	@Override
	public int hashCode() {
		int result = 1;
		if (moduleName != null)
			result = 31 * result + moduleName.hashCode();
		if (dataName != null)
			result = 31 * result + dataName.hashCode();
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null || getClass() != obj.getClass())
			return false;

		final ModuleAndData other = (ModuleAndData) obj;
		return ObjectUtils.equals(this.moduleName, other.moduleName) && ObjectUtils.equals(this.dataName, other.dataName);
	}

	public int compareTo(ModuleAndData other) {
		int result = StringUtils.dictionaryCompare(this.moduleName, other.moduleName);
		if (result == 0)
			result = StringUtils.dictionaryCompare(this.dataName, other.dataName);
		return result;
	}
}