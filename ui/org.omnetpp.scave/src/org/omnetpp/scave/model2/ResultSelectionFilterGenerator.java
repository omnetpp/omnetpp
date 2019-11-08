package org.omnetpp.scave.model2;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.model.ResultType;

public class ResultSelectionFilterGenerator {

    public static boolean debug = false;

    public static List<String> getIDListAsFilters(IDList ids, String[] runidFields, ResultFileManager manager) {
        Assert.isNotNull(runidFields);
        String[] filterFields = getFilterFieldsFor(runidFields);
        List<String> filters = new ArrayList<String>(ids.size());
        for (int i = 0; i < ids.size(); ++i) {
            long id = ids.get(i);
            String filter = new FilterUtil(manager.getItem(id), filterFields).getFilterPattern(); //TODO include: getTypeOf(item)
            filters.add(filter);
        }
        return filters;
    }

    public static String[] getFilterFieldsFor(String[] runidFields) {
        Assert.isNotNull(runidFields);
        int runidFieldCount = runidFields.length;
        String[] filterFields = new String[runidFieldCount+2];
        System.arraycopy(runidFields, 0, filterFields, 0, runidFieldCount);
        filterFields[runidFieldCount] = MODULE;
        filterFields[runidFieldCount + 1] = NAME;
        return filterFields;
    }
    // TODO better fitting name
    public static String getResultItemAttribute(ResultItem item, String attrName) {
        if (attrName.startsWith("runattr:"))
            return item.getRun().getAttribute(attrName.substring("runattr:".length()));

        switch (attrName) {
        case "module": return item.getModuleName();
        case "name": return item.getName();
        default: throw new RuntimeException("Unhandled result item attribute queried: " + attrName);
        }
    }


    static class AttributeValueCounts {
        int targetCount;
        int nonTargetCount;
    }

    static class ColumnValueCounts {
        Map<String, AttributeValueCounts> valueCounts;
    }

    // TODO reasonable name
    public static ColumnValueCounts computeColumnData(IDList target, IDList all, String attrName, ResultFileManager manager) {

        IDList nonTarget = all.getDifference(target);

        Map<String, AttributeValueCounts> valueCounts = new HashMap<String, AttributeValueCounts>();

        for (int i = 0; i < target.size(); ++i) {
            String value = getResultItemAttribute(manager.getItem(target.get(i)), attrName);
            if (!valueCounts.containsKey(value))
                valueCounts.put(value, new AttributeValueCounts());

            valueCounts.get(value).targetCount += 1;
        }

        for (int i = 0; i < nonTarget.size(); ++i) {
            String value = getResultItemAttribute(manager.getItem(nonTarget.get(i)), attrName);
            if (!valueCounts.containsKey(value))
                valueCounts.put(value, new AttributeValueCounts());

            valueCounts.get(value).nonTargetCount += 1;
        }

        System.out.println("counters for " + attrName);
        for (String v : valueCounts.keySet()) {
            AttributeValueCounts c = valueCounts.get(v);
            System.out.println("   " + v + " : target = " + c.targetCount + " , nonTarget = " + c.nonTargetCount);
        }

        ColumnValueCounts result = new ColumnValueCounts();
        result.valueCounts = valueCounts;
        return result;
    }

    public static String getApproximateFilter(IDList target, IDList all, ResultFileManager manager) {

        if (target.size() >= all.size() * 0.9)
            return "*";

        String[] fields = new String[] {"runattr:experiment", "runattr:measurement", "runattr:replication", "module", "name"};

        // note: it might be possible to optimize this by taking the difference IDLists as parameters
        IDList toBeExcluded = all.getDifference(target);
        IDList toBeIncluded = target.getDifference(all);

        Map<String, ColumnValueCounts> columnData = new HashMap<String, ColumnValueCounts>();

        for (String f : fields)
            columnData.put(f, computeColumnData(target, all, f, manager));

        String bestAttr = null;
        float bestScore = 0;

        for (String f : fields) {
            ColumnValueCounts cd = columnData.get(f);

            float columnScore = 0;
            int numVals = 0;

            for (String v : cd.valueCounts.keySet()) {
                AttributeValueCounts ad = cd.valueCounts.get(v);

                float tc = (float)ad.targetCount / (ad.nonTargetCount + ad.targetCount);
                float ntc = (float)ad.nonTargetCount / (ad.nonTargetCount + ad.targetCount);

                columnScore += Math.max(tc, ntc);
                numVals += 1;
            }

            columnScore /= numVals;

            System.out.println("column score for " + f + " is: " + columnScore);

            if (columnScore > bestScore) {
                bestAttr = f;
                bestScore = columnScore;
            }
        }

        System.out.println("best attr: " + bestAttr);

        Set<String> selectedValues = new HashSet<String>();

        ColumnValueCounts bcd = columnData.get(bestAttr);
        for (String k : bcd.valueCounts.keySet()) {
            AttributeValueCounts vcd = bcd.valueCounts.get(k);

            if (vcd.targetCount > vcd.nonTargetCount)
                selectedValues.add(k);
        }

        if (bestScore >= 0.8) {

            String newTerm = "";
            if (!selectedValues.isEmpty()) {
                if (selectedValues.size() == 1)
                    newTerm = bestAttr + " =~ " + selectedValues.iterator().next();
                else {
                    newTerm += "(";
                    String[] vals = selectedValues.toArray(new String[]{});
                    boolean first = true;
                    for (String v : vals) {
                        if (!first)
                            newTerm += " OR ";
                        newTerm += bestAttr + " =~ " + v;
                        first = false;
                    }
                    newTerm += ")";
                }

                return newTerm;
            }
        }


        String[] dumbFields = new String[] {"experiment", "measurement", "replication", "module", "name"};
        return getIDListAsDumbFilter(target, manager, dumbFields);
    }

    public static String getFilter(IDList target, IDList all, ResultFileManager manager) {

        String approxFilter = getApproximateFilter(target, all, manager);

        System.out.println("getFilter, trying to reduce " + all.size() + " IDs to " + target.size());
        System.out.println("approx filter is: " + approxFilter);

        IDList approxMatching = manager.filterIDList(all, approxFilter);

        System.out.println("approx matching count: " + approxMatching.size());

        IDList approxNotMatching = all.dup();
        approxNotMatching.subtract(approxMatching);

        IDList toBeExcluded = approxMatching.getDifference(target);
        IDList toBeIncluded = target.getDifference(approxMatching);

        String includeFilter = null;
        if (!toBeIncluded.isEmpty()) {
            System.out.println("computing include filter...");
            includeFilter = getFilter(toBeIncluded, approxNotMatching,  manager);
            System.out.println("include filter: " + includeFilter);
        }

        String excludeFilter = null;
        if (!toBeExcluded.isEmpty()) {
            System.out.println("computing exclude filter...");
            excludeFilter = getFilter(toBeExcluded, approxMatching, manager);
            System.out.println("exclude filter: " + excludeFilter);
        }

        boolean haveInclude = includeFilter != null && !includeFilter.equals("(\n    *\n)");
        boolean haveExclude = excludeFilter != null && !excludeFilter.equals("(\n    *\n)");

        if (approxFilter.equals("*")) {
            String result = "";

            if (haveInclude) {
                if (haveExclude)
                    result += " (\n" + StringUtils.indentLines(includeFilter, "    ") + "\n)";
                else
                    result += includeFilter;
            }

            if (haveExclude) {
                if (haveInclude)
                    result += "\n AND \n";

                result += "NOT (\n" + StringUtils.indentLines(excludeFilter, "    ") + "\n)";
            }

            return result;
        }
        else {
            if (!haveInclude && !haveExclude)
                return approxFilter;
            else {
                String result = "(\n" + StringUtils.indentLines(approxFilter, "    ") + "\n)";

                if (haveInclude)
                    result += " OR (\n" + StringUtils.indentLines(includeFilter, "    ") + "\n)";

                if (haveExclude)
                    result += " AND NOT (\n" + StringUtils.indentLines(excludeFilter, "    ") + "\n)";

                return result;
            }
        }
    }


    // filtering for the given result type will not be part of the returned expression!!!
    public static String getIDListAsFilterExpression(IDList ids, String[] runidFields, ResultType resultType, String viewFilter, ResultFileManager manager) {

        IDList allItemsOfType = manager.getAllItems(false, false).filterByTypes(resultType.getValue());
        IDList itemsMatchingViewFilter = manager.filterIDList(allItemsOfType, viewFilter);

        boolean allSelected = ids.equals(itemsMatchingViewFilter);

        if (debug) {
            Debug.println("allitems: " + itemsMatchingViewFilter.toString());
            Debug.println("selected: " + ids.toString());
            Debug.println("selected: " + ids.size() + " matching: " + itemsMatchingViewFilter.size());
            Debug.println("allselected: " + allSelected);
        }

        if (allSelected) {
            if (debug)
                Debug.println("returning the view filter: " + viewFilter);
            return StringUtils.defaultIfEmpty(viewFilter, "*");
        }


        if (viewFilter.equals("*"))
            return getFilter(ids, itemsMatchingViewFilter, manager);
        else
            return viewFilter + " AND (" + getFilter(ids, itemsMatchingViewFilter, manager) + ")";


//
//
//        String refinedFilter = refineFilterForSelection(itemsMatchingViewFilter, ids, manager);
//        System.out.println("refined filter: " + refinedFilter);
//
//        if (refinedFilter != null)
//            return refinedFilter;
//
//
//        IDList invisibleSelected = ids.dup();
//        invisibleSelected.subtract(itemsMatchingViewFilter);
//        Assert.isTrue(invisibleSelected.isEmpty());
//
//        IDList unselected = itemsMatchingViewFilter.dup();
//        unselected.subtract(ids);
//        if (debug)
//            Debug.println("number of unselected: " + unselected.size());
//
//
//        Assert.isNotNull(runidFields);
//        String[] filterFields = getFilterFieldsFor(runidFields);
//
//        String result;
//        if (ids.size() > 10 && unselected.size() < (ids.size() / 3)) {
//            result = "(" + viewFilter + ")\nAND NOT (\n" + StringUtils.indentLines(getIDListAsDumbFilter(unselected, manager, filterFields), "    ") + ")";
//        }
//        else {
//            RunList runs = manager.getUniqueRuns(ids);
//            if (runs.size() < ids.size() / 2)
//                result = getIDListAsRunGroupedFilter(ids, manager, filterFields);
//            else
//                result = getIDListAsDumbFilter(ids, manager, filterFields);
//        }
//
//        // debug check:
//        if (debug)
//            Debug.println("filter expression: " + result);
//        Assert.isTrue(manager.filterIDList(allItemsOfType, result).equals(ids), "Filter created from IDList does not reproduce the given IDs");
//
//        return result;
    }

    private static String getIDListAsRunGroupedFilter(IDList ids, ResultFileManager manager, String[] filterFields) {

        RunList runs = manager.getUniqueRuns(ids);
        StringBuilder sb = new StringBuilder();
        boolean first = true;

        for (Run r :runs.toArray()) {
            if (!first)
                sb.append("\n OR \n");
            sb.append("( run =~ \"" + r.getRunName() + "\" AND (\n");

            IDList idsInRun = manager.filterIDList(ids, r, null, null);

            for (int i = 0; i < idsInRun.size(); ++i) {
                long id = ids.get(i);
                ResultItem item = manager.getItem(id);
                String filter = new FilterUtil(item, filterFields).getFilterPattern();
                sb.append("    " + filter + " \n");
            }
            sb.append(") )");

            first = false;
        }
        return sb.toString();
    }

    private static String getIDListAsDumbFilter(IDList ids, ResultFileManager manager, String[] filterFields) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < ids.size(); ++i) {
            long id = ids.get(i);
            ResultItem item = manager.getItem(id);
            String filter = new FilterUtil(item, filterFields).getFilterPattern();
            if (i > 0)
                sb.append(" OR \n");
            sb.append(filter);
        }
        return sb.toString();
    }

}
