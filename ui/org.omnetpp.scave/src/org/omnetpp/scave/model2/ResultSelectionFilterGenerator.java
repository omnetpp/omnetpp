package org.omnetpp.scave.model2;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;

/**
 * Given a set of selected results (an IDList of selected items, out of an IDList
 * of "all" items), generates a filter expression that results in the first IDList,
 * when used to filter the second one. Tries to keep the expression short and reasonable.
 */
public class ResultSelectionFilterGenerator {

    protected static boolean debug = false; // TODO initialize from OMNETPP_DEBUG envvar, like elsewhere

    // TODO better fitting name
    protected static String getResultItemAttribute(ResultItem item, String attrName) {
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
    protected static ColumnValueCounts computeColumnData(IDList target, IDList all, String attrName, ResultFileManager manager) {

        IDList nonTarget = all.subtract(target);

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

        if (debug)
            Debug.println("counters for " + attrName);
        for (String v : valueCounts.keySet()) {
            AttributeValueCounts c = valueCounts.get(v);
            if (debug)
                Debug.println("   " + v + " : target = " + c.targetCount + " , nonTarget = " + c.nonTargetCount);
        }

        ColumnValueCounts result = new ColumnValueCounts();
        result.valueCounts = valueCounts;
        return result;
    }

    protected static String getApproximateFilter(IDList target, IDList all, ResultFileManager manager, IProgressMonitor monitor) throws InterruptedException {

        if (target.size() >= all.size() * 0.9)
            return "*";

        String[] fields = new String[] {"runattr:experiment", "runattr:measurement", "runattr:replication", "module", "name"};

        Map<String, ColumnValueCounts> columnData = new HashMap<String, ColumnValueCounts>();

        for (String f : fields) {
            columnData.put(f, computeColumnData(target, all, f, manager));
            checkMonitor(monitor);
        }

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

            if (debug)
                Debug.println("column score for " + f + " is: " + columnScore);

            if (columnScore > bestScore) {
                bestAttr = f;
                bestScore = columnScore;
            }
        }

        if (debug)
            Debug.println("best attr: " + bestAttr);

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
                    newTerm = bestAttr + " =~ " + FilterUtil.quoteStringIfNeeded(selectedValues.iterator().next());
                else {
                    newTerm += "(";
                    String[] vals = selectedValues.toArray(new String[]{});
                    boolean first = true;
                    for (String v : vals) {
                        if (!first)
                            newTerm += " OR ";
                        newTerm += bestAttr + " =~ " + FilterUtil.quoteStringIfNeeded(v);
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

    public static String getFilter(IDList target, IDList all, ResultFileManager manager, IProgressMonitor monitor) throws InterruptedException {
        return ResultFileManager.callWithReadLock(manager, () -> {
            if (monitor != null)
                monitor.beginTask("Generating filter", 1);

            if (!target.isSubsetOf(all))
                throw new IllegalArgumentException("IDs to be selected must be a subset of all IDs");

            return doGetFilter(target, all, manager, monitor);
        });
    }

    protected static String doGetFilter(IDList target, IDList all, ResultFileManager manager, IProgressMonitor monitor) throws InterruptedException {

        checkMonitor(monitor);

        String approxFilter = getApproximateFilter(target, all, manager, monitor);

        checkMonitor(monitor);

        if (debug) {
            Debug.println("getFilter, trying to reduce " + all.size() + " IDs to " + target.size());
            Debug.println("approx filter is: " + approxFilter);
        }


        IDList approxMatching = Debug.timed("approxfilter", 1, () -> manager.filterIDList(all, approxFilter));

        checkMonitor(monitor);

        if (debug)
            Debug.println("approx matching count: " + approxMatching.size());

        IDList approxNotMatching = all.subtract(approxMatching);

        IDList toBeExcluded = approxMatching.subtract(target);
        IDList toBeIncluded = target.subtract(approxMatching);

        String includeFilter = null;
        if (!toBeIncluded.isEmpty()) {
            if (debug)
                Debug.println("computing include filter...");
            includeFilter = doGetFilter(toBeIncluded, approxNotMatching,  manager, monitor);
            if (debug)
                Debug.println("include filter: " + includeFilter);
        }

        String excludeFilter = null;
        if (!toBeExcluded.isEmpty()) {
            if (debug)
                Debug.println("computing exclude filter...");
            excludeFilter = doGetFilter(toBeExcluded, approxMatching, manager, monitor);
            if (debug)
                Debug.println("exclude filter: " + excludeFilter);
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

    public static String getIDListAsFilterExpression(IDList allIDs, IDList ids, String[] runidFields, String viewFilter, ResultFileManager manager, IProgressMonitor monitor) throws InterruptedException {
        return ResultFileManager.callWithReadLock(manager,
                () -> doGetIDListAsFilterExpression(allIDs, ids, runidFields, viewFilter, manager, monitor)
        );
    }

    protected static String doGetIDListAsFilterExpression(IDList allIDs, IDList ids, String[] runidFields, String viewFilter, ResultFileManager manager, IProgressMonitor monitor) throws InterruptedException {
        IDList allItemsOfType = allIDs;
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

        if (!ids.isSubsetOf(allIDs))
            throw new IllegalArgumentException("IDs to be selected must be a subset of all IDs");

        if (viewFilter.equals("*"))
            return getFilter(ids, itemsMatchingViewFilter, manager, monitor);
        else
            return viewFilter + " AND (" + getFilter(ids, itemsMatchingViewFilter, manager, monitor) + ")";

//TODO integrate or delete:
//
//        String refinedFilter = refineFilterForSelection(itemsMatchingViewFilter, ids, manager);
//        if (debug
//            Debug.println("refined filter: " + refinedFilter);
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

    private static String getIDListAsRunGroupedFilter(IDList ids, ResultFileManager manager, String[] filterFields) { //TODO this method is never used.

        RunList runs = manager.getUniqueRuns(ids);
        StringBuilder sb = new StringBuilder();
        boolean first = true;

        for (Run r :runs.toArray()) {
            if (!first)
                sb.append("\n OR \n");
            sb.append("( run =~ " + FilterUtil.quoteStringIfNeeded(r.getRunName()) + " AND (\n");

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

    private static void checkMonitor(IProgressMonitor monitor) throws InterruptedException {
        if (monitor != null) {
            monitor.worked(1);
            if (monitor.isCanceled())
                throw new InterruptedException();
        }
    }


}
