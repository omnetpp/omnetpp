/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import static org.omnetpp.scave.model2.FilterField.FILE;
import static org.omnetpp.scave.model2.FilterField.MODULE;
import static org.omnetpp.scave.model2.FilterField.NAME;
import static org.omnetpp.scave.model2.FilterField.RUN;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model2.FilterField.Kind;

/**
 * Given an IDList, produces hints for different filter fields such as
 * module name, statistic name, run attributes etc.
 *
 * @author tomi
 */
public class FilterHintsCache {
    //TODO cache hints for more than one IDList
    private ResultFileManager lastManager = null;
    private IDList lastIdList = null;
    private Map<FilterField,String[]> cachedHints = new HashMap<FilterField, String[]>();

    public FilterHintsCache() {
    }

    public String[] getHints(ResultFileManager manager, IDList idlist, FilterField field, String prefix) {
        return filter(getHints(manager, idlist, field), prefix);
    }

    public String[] getHints(ResultFileManager manager, IDList idlist, FilterField field) {
        if (manager != lastManager || !idlist.equals(lastIdList)) {
            cachedHints.clear();
            lastManager = manager;
            lastIdList = idlist;
        }

        if (!cachedHints.containsKey(field))
            cachedHints.put(field,  computeHints(manager, idlist, field));

        return cachedHints.get(field);
    }

    public static String[] computeHints(ResultFileManager manager, IDList idlist, FilterField field) {
        if (field.equals(RUN)) {
            RunList runList = manager.getUniqueRuns(idlist);
            return manager.getRunNameFilterHints(runList).toArray();
        }
        else if (field.equals(FILE)) {
            ResultFileList fileList = manager.getUniqueFiles(idlist);
            return manager.getFilePathFilterHints(fileList).toArray();
        }
        else if (field.equals(MODULE)) {
            return manager.getModuleFilterHints(idlist).toArray();
        }
        else if (field.equals(NAME)) {
            return manager.getResultNameFilterHints(idlist).toArray();
        }
        else if (field.getKind() == Kind.ItemField) {
            return manager.getResultItemAttributeFilterHints(idlist, field.getName()).toArray();
        }
        else if (field.getKind() == Kind.RunAttribute) {
            RunList runList = manager.getUniqueRuns(idlist);
            return manager.getRunAttributeFilterHints(runList, field.getName()).toArray();
        }
        else if (field.getKind() == Kind.IterationVariable) {
            RunList runList = manager.getUniqueRuns(idlist);
            return manager.getIterationVariableFilterHints(runList, field.getName()).toArray();
        }
        else if (field.getKind() == Kind.ParamAssignment) {
            RunList runList = manager.getUniqueRuns(idlist);
            return manager.getParamAssignmentFilterHints(runList, field.getName()).toArray();
        }
        else {
            throw new IllegalArgumentException();
        }
    }

    public static String[] filter(String[] v, String prefix) {
        if (prefix == null || prefix.isEmpty())
            return v;

        PatternMatcher prefixAsPattern = new PatternMatcher(prefix, true, false /*=substring*/, false /*=ignorecase*/);
        List<String> tmp = new ArrayList<>();
        for (String s : v)
            if (prefixAsPattern.matches(s))
                tmp.add(s);
        return tmp.toArray(new String[]{});
    }
}
