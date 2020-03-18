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
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.FilterField.Kind;

/**
 * Given an IDList, produces hints for different filter fields such as
 * module name, statistic name, run attributes etc.
 *
 * @author tomi
 */
//TODO this class is largely superfluous, remove!
public class FilterHints {

    private static final String[] EMPTY = new String[0];

    private Map<FilterField,String[]> hints = new HashMap<FilterField, String[]>();

    public FilterHints() {
    }

    public FilterHints(final ResultFileManager manager, final ResultType type) {
        ResultFileManager.runWithReadLock(manager, () -> {
            fillAllFields(manager, ScaveModelUtil.getAllIDs(manager, type));
        });
    }

    public FilterHints(final ResultFileManager manager, final IDList idlist) {
        ResultFileManager.runWithReadLock(manager, () -> {
            fillAllFields(manager, idlist);
        });
    }

    public void addHints(final FilterField field, final ResultFileManager manager, final IDList idlist)
    {
        ResultFileManager.runWithReadLock(manager, () -> {
            fillOneField(manager, idlist, field);
        });
    }

    private void fillAllFields(ResultFileManager manager, IDList idlist) {
        manager.checkReadLock();
        ResultFileList fileList = manager.getUniqueFiles(idlist);
        RunList runList = manager.getUniqueRuns(idlist);

        setHints(FILE, manager.getFilePathFilterHints(fileList).toArray());
        setHints(RUN, manager.getRunNameFilterHints(runList).toArray());
        setHints(MODULE, manager.getModuleFilterHints(idlist).toArray());
        setHints(NAME, manager.getNameFilterHints(idlist).toArray());
        for (String attrName : manager.getUniqueAttributeNames(idlist).keys().toArray())
            setHints(Kind.ItemField, attrName, manager.getResultItemAttributeFilterHints(idlist, attrName).toArray());
        for (String attrName : manager.getUniqueRunAttributeNames(runList).keys().toArray())
            setHints(Kind.RunAttribute, attrName, manager.getRunAttributeFilterHints(runList, attrName).toArray());
        for (String varName : manager.getUniqueIterationVariableNames(runList).keys().toArray())
            setHints(Kind.IterationVariable, varName, manager.getIterationVariableFilterHints(runList, varName).toArray());
        for (String key : manager.getUniqueParamAssignmentKeys(runList).keys().toArray())
            setHints(Kind.ParamAssignment, key, manager.getParamAssignmentFilterHints(runList, key).toArray());
    }

    public static String[] computeHints(ResultFileManager manager, IDList idlist, FilterField field, String prefix) {
        if (field.equals(RUN)) {
            RunList runList = manager.getUniqueRuns(idlist);
            return filterAndConvert(manager.getRunNameFilterHints(runList), prefix);
        }
        else if (field.equals(FILE)) {
            ResultFileList fileList = manager.getUniqueFiles(idlist);
            return filterAndConvert(manager.getFilePathFilterHints(fileList), prefix);
        }
        else if (field.equals(MODULE)) {
            return filterAndConvert(manager.getModuleFilterHints(idlist), prefix);
        }
        else if (field.equals(NAME)) {
            return filterAndConvert(manager.getNameFilterHints(idlist), prefix);
        }
        else if (field.getKind() == Kind.ItemField) {
            return filterAndConvert(manager.getResultItemAttributeFilterHints(idlist, field.getName()), prefix);
        }
        else if (field.getKind() == Kind.RunAttribute) {
            RunList runList = manager.getUniqueRuns(idlist);
            return filterAndConvert(manager.getRunAttributeFilterHints(runList, field.getName()), prefix);
        }
        else if (field.getKind() == Kind.IterationVariable) {
            RunList runList = manager.getUniqueRuns(idlist);
            return filterAndConvert(manager.getIterationVariableFilterHints(runList, field.getName()), prefix);
        }
        else if (field.getKind() == Kind.ParamAssignment) {
            RunList runList = manager.getUniqueRuns(idlist);
            return filterAndConvert(manager.getParamAssignmentFilterHints(runList, field.getName()), prefix);
        }
        else {
            throw new IllegalArgumentException();
        }
    }

    private static String[] filterAndConvert(StringVector v, String prefix) {
        if (prefix == null)
            return v.toArray();

        PatternMatcher prefixAsPattern = new PatternMatcher(prefix, true, false /*=substring*/, false /*=ignorecase*/);
        List<String> tmp = new ArrayList<>();
        long n = v.size();
        for (int i = 0; i < n; i++) {
            String s = v.get(i);
            if (prefixAsPattern.matches(s))
                tmp.add(s);
        }
        return tmp.toArray(new String[]{});
    }

    private void fillOneField(ResultFileManager manager, IDList idlist, FilterField field) {
        String[] fieldHints = computeHints(manager, idlist, field, null);
        manager.checkReadLock();
        if (field.equals(RUN) || field.equals(FILE) || field.equals(MODULE) ||field.equals(NAME))
            setHints(field, fieldHints);
        else if (field.getKind() == Kind.ItemField || field.getKind() == Kind.RunAttribute || field.getKind() == Kind.IterationVariable || field.getKind() == Kind.ParamAssignment)
            setHints(field.getKind(), field.getName(), fieldHints);
        else
            throw new IllegalArgumentException();
    }

    public FilterField[] getFields() {
        Set<FilterField> keys = hints.keySet();
        FilterField[] fields = keys.toArray(new FilterField[keys.size()]);
        Arrays.sort(fields);
        return fields;
    }

    public String[] getHints(FilterField field) {
        if (hints.containsKey(field))
            return hints.get(field);
        else
            return EMPTY;
    }

    private void setHints(FilterField field, String[] value) {
        this.hints.put(field, value);
    }

    private void setHints(Kind kind, String name, String[] value) {
        this.hints.put(new FilterField(kind, name), value);
    }
}
