/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 *
 */
package org.omnetpp.scave.model2;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;

public class IsoLineData implements Comparable<IsoLineData>
{
    // either moduleName and dataName or attributeName
    private String moduleName;
    private String dataName;
    private String attributeName;
    private String value;

    public IsoLineData() {
    }

    public IsoLineData(String moduleName, String dataName) {
        this.moduleName = moduleName;
        this.dataName = dataName;
    }

    public IsoLineData(String attributeName) {
        this.attributeName = attributeName;
    }

    public boolean isValid() {
        return (!StringUtils.isEmpty(moduleName) && !StringUtils.isEmpty(dataName)) ||
                !StringUtils.isEmpty(attributeName);
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

    public String getAttributeName() {
        return attributeName;
    }

    public void setAttributeName(String attributeName) {
        this.attributeName = attributeName;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public static IsoLineData fromFilterPattern(String pattern) {
        if (pattern != null) {
            FilterUtil filter = new FilterUtil(pattern, true);
            if (filter != null && !filter.isLossy()) {
                if (!StringUtils.isEmpty(filter.getField(MODULE)) && !StringUtils.isEmpty(filter.getField(NAME))) {
                    return new IsoLineData(filter.getField(MODULE),
                                             filter.getField(NAME));
                }
                else {
                    String[] filterFieldNames = filter.getFieldNames().toArray(new String[]{});
                    Assert.isTrue(filterFieldNames.length == 1);
                    return new IsoLineData(filterFieldNames[0]);
                }
            }
        }
        return null;
    }

    public String asListItem() {
        if (moduleName != null && dataName != null)
            return moduleName + "/" + dataName;
        else
            return attributeName;
    }

    public String asFilterPattern() {
        FilterUtil filter = new FilterUtil();
        if (moduleName != null && dataName != null) {
            filter.setField(MODULE, moduleName);
            filter.setField(NAME, dataName);
        }
        else if (attributeName != null)
            filter.setField(attributeName, "*");
        return filter.getFilterPattern();
    }

    @Override
    public int hashCode() {
        int result = 1;
        if (moduleName != null)
            result = 31 * result + moduleName.hashCode();
        if (dataName != null)
            result = 31 * result + dataName.hashCode();
        if (attributeName != null)
            result = 31 * result + attributeName.hashCode();
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;

        final IsoLineData other = (IsoLineData) obj;
        return ObjectUtils.equals(this.moduleName, other.moduleName) && ObjectUtils.equals(this.dataName, other.dataName)
                && ObjectUtils.equals(this.attributeName, other.attributeName);
    }

    public int compareTo(IsoLineData other) {
        if (this.moduleName != null && other.moduleName == null)
            return -1;
        if (this.moduleName == null && other.moduleName != null)
            return 1;

        int result = StringUtils.dictionaryCompare(this.moduleName, other.moduleName);
        if (result == 0)
            result = StringUtils.dictionaryCompare(this.dataName, other.dataName);
        return result;
    }
}