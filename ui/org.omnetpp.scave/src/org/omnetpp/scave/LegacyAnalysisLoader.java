/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.XmlUtils;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * Loads a legacy Analysis file - created with OMNeT++ IDE older than 6.0.
 *
 * Turns each Chart "Dataset operation" in the old format into an independent
 * Chart object, using one of a fixed set of chart templates.
 *
 * It tries to reconstruct many aspects of the old Chart, including proper
 * data (result item) filtering, vector operations, data arrangement (grouping,
 * iso lines, etc.), and visual properties.
 *
 * However, it is far from complete, so it might fail in many non-trivial cases.
 *
 * It also discards Chart Sheets, as they are not supported by the current
 * version of the Analysis Tool.
 */
public class LegacyAnalysisLoader {

    /** Thje used Chart Templates are looked up from this registry */
    private ChartTemplateRegistry chartTemplateRegistry;
    /** Conversion errors are appended to this list */
    private ArrayList<String> errors;

    public static final String BARCHART_ID = "barchart_native";
    public static final String LINECHART_ID = "linechart_native";
    public static final String HISTOGRAMCHART_ID = "histogramchart_native";
    public static final String SCATTERCHART_ID = "scatterchart_legacy";

    /**
     * Represents an "add" or "discard" operation of the legacy Analysis,
     * for selecting results into (or dropping out of) a legacy Dataset.
     * Used to construct a corresponding filter expression for the new Charts.
     */
    private static class DataOp {
        String op;
        String filter;
        String type;

        DataOp(String op, String filter, String type) {
            this.op = op;
            this.filter = filter;
            this.type = type;
        }
    }

    /**
     * Represents an "apply" or "compute" vector operation of the legacy Analysis.
     * Used to construct a corresponding vector_operations property for the new Charts.
     */
    private static class DataVecOp {
        String type; // apply or compute
        String operation; // winavg, movingavg, mean, etc...
        Map<String, String> params; // often empty, casting to double might be necessary

        public static String mapOperationName(String operation) {
            return operation.replace("-", "_");
        }

        DataVecOp(String type, String operation, Map<String, String> params) {
            this.type = type;
            this.operation = operation;
            this.params = params;
        }

        /**
         * Turns the represented vector operation into a String, in a format
         * that is used in the vector_operations Chart property.
         */
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append(type);
            sb.append(":");
            sb.append(mapOperationName(operation));

            for (String k: params.keySet()) {
                sb.append(",");
                sb.append(k);
                sb.append("=");
                sb.append(params.get(k));
            }
            return sb.toString();
        }
    }

    /**
     * Represents an old-style result filter expression ("module(Aloha.*)" syntax)
     * plus a result item type (scalar, vector, etc.), used to select which
     * result items should be a part of a following chart operation. Is used for
     * reconstructing the corresponding filter expression for the new Charts.
     */
    private static class DataFilter {
        String type;
        String filter;

        DataFilter(String type, String filter) {
            this.type = type;
            this.filter = filter;
        }
    }

    /**
     * Constructs a LegacyAnalysisLoader that will look up the used chart
     * templates in chartTemplateRegistry, and will append the conversion errors
     * into the given errors list.
     */
    public LegacyAnalysisLoader(ChartTemplateRegistry chartTemplateRegistry, ArrayList<String> errors) {
        this.chartTemplateRegistry = chartTemplateRegistry;
        this.errors = errors;
    }

    /**
     * Combines a collected list of DataOps and DataFilters to construct a single,
     * complete, new style (module =~ *Aloha*) filter expression for new Charts.
     */
    private String makeFilterString(ArrayList<DataOp> ops, ArrayList<DataFilter> filters) {
        String opsExpr = "";
        for (DataOp o : ops) {
            // TODO: take data type into account
            if (opsExpr.isEmpty()) {
                if ("add".equals(o.op))
                    opsExpr = o.filter;
                else if ("discard".equals(o.op))
                    opsExpr = "NOT (" + o.filter + ")";
            }
            else {
                if ("add".equals(o.op))
                    opsExpr = "(" + opsExpr + ") OR (" + o.filter + ")";
                else if ("discard".equals(o.op))
                    opsExpr = "(" + opsExpr + ") AND NOT (" + o.filter + ")";
            }
        }

        String filtersExpr = "";
        for (DataFilter f : filters)
            if (filtersExpr.isEmpty())
                filtersExpr = f.filter;
            else // TODO: take filter "data" type into account
                filtersExpr = "(" + filtersExpr + ") OR (" + f.filter + ")";

        if (ops.isEmpty() && filters.isEmpty())
            return "";
        else if (opsExpr.isEmpty())
            return filtersExpr;
        else if (filtersExpr.isEmpty())
            return opsExpr;
        else
            return "(" + opsExpr + ")  AND  (" + filtersExpr + ")";
    }

    /**
     * Turns a legacy Chart of the given type, from the given legacy Dataset item
     * ("operation") node, into a new Chart, taking into account all the preceding
     * Data selection and vector operation operations, given in ops and vecOps.
     */
    private Chart makeLegacyChart(ArrayList<DataOp> ops, ArrayList<DataVecOp> vecOps, Node chartNode, String chartType) {
        Chart chart = null;

        if ("scave:BarChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(chartTemplateRegistry, BARCHART_ID);
        else if ("scave:HistogramChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(chartTemplateRegistry, HISTOGRAMCHART_ID);
        else if ("scave:ScatterChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(chartTemplateRegistry, SCATTERCHART_ID);
        else if ("scave:LineChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(chartTemplateRegistry, LINECHART_ID);
        else
            throw new RuntimeException("unknown chart type: " + chartType);

        chart.setName(chartNode.getAttributes().getNamedItem("name").getNodeValue());

        ArrayList<DataFilter> filters = new ArrayList<DataFilter>();

        NodeList props = chartNode.getChildNodes();

        for (int k = 0; k < props.getLength(); ++k) {
            Node propNode = props.item(k);
            if ("properties".equals(propNode.getNodeName())) {
                String name = propNode.getAttributes().getNamedItem("name").getNodeValue();
                String value = propNode.getAttributes().getNamedItem("value").getNodeValue();
                Property prop = new Property(name, value);
                chart.addProperty(prop);
            } else if ("filters".equals(propNode.getNodeName())) {
                String filterExpression = propNode.getAttributes().getNamedItem("filterPattern").getNodeValue();
                filterExpression = FilterUtil.translateLegacyFilterExpression(filterExpression);
                filters.add(new DataFilter(propNode.getAttributes().getNamedItem("xsi:type").getNodeValue(), filterExpression));
            }
        }


        if ("scave:BarChart".equals(chartType)) {
            chart.setPropertyValue("Plot.Title", chart.getName());
            chart.setPropertyValue("filter", makeFilterString(ops, filters));

            ArrayList<String> bars = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "barFields");
            chart.setPropertyValue("series", bars.isEmpty() ? "name,measurement" : StringUtils.join(bars, ","));

            ArrayList<String> groups = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "groupByFields");
            chart.setPropertyValue("groups", groups.isEmpty() ? "module,experiment" : StringUtils.join(groups, ","));

            // NOTE: averagedFields elements are not used, because they belong "to the rest"
        }
        else if ("scave:HistogramChart".equals(chartType)) {
            chart.setPropertyValue("Plot.Title", chart.getName());
            chart.setPropertyValue("filter", makeFilterString(ops, filters));
        }
        else if ("scave:ScatterChart".equals(chartType)) {
            String xDataPattern = chartNode.getAttributes().getNamedItem("xDataPattern").getNodeValue();
            xDataPattern = FilterUtil.translateLegacyFilterExpression(xDataPattern);
            Node avgRepls = chartNode.getAttributes().getNamedItem("averageReplications");
            String averageReplications = avgRepls != null ? avgRepls.getNodeValue() : "true";
            ArrayList<String> isoDataPatterns = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "isoDataPattern");
            for (int i = 0; i < isoDataPatterns.size(); ++i)
                isoDataPatterns.set(i, FilterUtil.translateLegacyFilterExpression(isoDataPatterns.get(i)));

            chart.setPropertyValue("Plot.Title", chart.getName());
            chart.setPropertyValue("filter", makeFilterString(ops, filters));
            chart.setPropertyValue("x_pattern", xDataPattern);
            chart.setPropertyValue("iso_patterns", StringUtils.join(isoDataPatterns, ";"));
            chart.setPropertyValue("average_replications", averageReplications);
        }
        else if ("scave:LineChart".equals(chartType)) {
            chart.setPropertyValue("Plot.Title", chart.getName());
            chart.setPropertyValue("filter", makeFilterString(ops, filters));

            ArrayList<String> vecOpStrings = new ArrayList<String>();
            for (DataVecOp vo : vecOps)
                vecOpStrings.add(vo.toString());

            chart.setPropertyValue("vector_operations", StringUtils.join(vecOps, "\n"));
        }
        else
            throw new RuntimeException("unknown chart type: " + chartType);

        return chart;
    }

    /**
     * Helper function to easily collect the vector operation parameters
     * (window length, moving average factor, etc.) from a legacy item node.
     */
    private Map<String, String> extractVectorOpParams(Node operationNode) {
        Map<String, String> result = new HashMap<String, String>();

        NodeList paramNodes = operationNode.getChildNodes();

        for (int k = 0; k < paramNodes.getLength(); k++) {
            Node paramNode = paramNodes.item(k);
            if ("params".equals(paramNode.getNodeName())) {
                NamedNodeMap attrs = paramNode.getAttributes();
                result.put(attrs.getNamedItem("name").getNodeValue(), attrs.getNamedItem("value").getNodeValue());
            }
        }

        return result;
    }

    /**
     * Loads all items from the given root DOM node, adding the converted
     * contents into the given Analysis.
     */
    private void loadItems(Node parentNode, Analysis analysis) {
        loadItems(parentNode, analysis, new ArrayList<DataOp>(), new ArrayList<DataVecOp>());
    }

    /**
     * Loads all items from the given child DOM node, adding the converted
     * contents into the given Analysis. Only differs from the root-level
     * overload in that this also takes a list of preceding data selection and
     * vector operation items as lists. Used recursively for legacy Group items.
     */
    private void loadItems(Node parentNode, Analysis analysis, ArrayList<DataOp> startingOps, ArrayList<DataVecOp> startingVecOps) {
        NodeList itemNodes = parentNode.getChildNodes();

        @SuppressWarnings("unchecked")
        ArrayList<DataOp> ops = (ArrayList<DataOp>)startingOps.clone();
        @SuppressWarnings("unchecked")
        ArrayList<DataVecOp> vecOps = (ArrayList<DataVecOp>)startingVecOps.clone();

        for (int k = 0; k < itemNodes.getLength(); k++) {
            Node itemNode = itemNodes.item(k);
            if ("items".equals(itemNode.getNodeName())) {
                Node itemTypeNode = itemNode.getAttributes().getNamedItem("xsi:type");

                String itemType = itemTypeNode.getNodeValue();

                // this is null for charts
                Node opTypeNode = itemNode.getAttributes().getNamedItem("type");

                String opType = "SCALAR";
                if (opTypeNode != null)
                    opType = opTypeNode.getNodeValue();

                if ("scave:Add".equals(itemType) || "scave:Discard".equals(itemType)) {
                    Node filterNode = itemNode.getAttributes().getNamedItem("filterPattern");
                    String filterExpr = FilterUtil.translateLegacyFilterExpression(filterNode.getNodeValue());
                    ops.add(new DataOp("scave:Add".equals(itemType) ? "add" : "discard", filterExpr, opType));
                }
                else if ("scave:Apply".equals(itemType) || "scave:Compute".equals(itemType)) {
                    Node operationNode = itemNode.getAttributes().getNamedItem("operation");
                    vecOps.add(new DataVecOp("scave:Apply".equals(itemType) ? "apply" : "compute", operationNode.getNodeValue(), extractVectorOpParams(itemNode)));
                }
                else if ("scave:BarChart".equals(itemType) || "scave:HistogramChart".equals(itemType) || "scave:ScatterChart".equals(itemType) || "scave:LineChart".equals(itemType))
                    analysis.getCharts().addChart(makeLegacyChart(ops, vecOps, itemNode, itemType));
                else if ("scave:Group".equals(itemType)) {
                    loadItems(itemNode, analysis, ops, vecOps);
                }
                else {
                    // TODO: handle better
                    Debug.println("UNIMPLEMENTED ITEM TYPE: " + itemType);
                    errors.add("Unimplemented item type: " + itemType);
                }
            } else if ("#text".equals(itemNode.getNodeName())) {
                if (!itemNode.getNodeValue().trim().isEmpty())
                    throw new RuntimeException("unexpected text content: " + itemNode.getNodeValue());
            } else {
                throw new RuntimeException("invalid child node: " + itemNode.getNodeName());
            }
        }
    }

    /**
     * Loads the contents (input patterns and charts) from a legacy root DOM
     * node, and returns a converted Analysis model. Conversion errors are
     * reported into the array passed to the LegacyAnalysisLoader constructor.
     */
    public Analysis loadLegacyAnalysis(Node rootNode) {

        Analysis analysis = new Analysis();
        NodeList topLevelNodes = rootNode.getChildNodes();

        for (int i = 0; i < topLevelNodes.getLength(); ++i) {
            Node node = topLevelNodes.item(i);

            if ("inputs".equals(node.getNodeName())) {
                NodeList inputNodes = node.getChildNodes();
                for (int j = 0; j < inputNodes.getLength(); ++j) {
                    Node inputNode = inputNodes.item(j);
                    if ("inputs".equals(inputNode.getNodeName())) {

                        Node nameNode = inputNode.getAttributes().getNamedItem("name");

                        InputFile input = new InputFile(nameNode.getNodeValue());

                        analysis.getInputs().addInput(input);
                    } else if ("#text".equals(inputNode.getNodeName())) {
                        if (!inputNode.getNodeValue().trim().isEmpty())
                            throw new RuntimeException("unexpected text content: " + inputNode.getNodeValue());
                    } else {
                        throw new RuntimeException("invalid child node: " + inputNode.getNodeName());
                    }
                }
            }
            else if ("datasets".equals(node.getNodeName())) {
                NodeList datasetNodes = node.getChildNodes();

                for (int j = 0; j < datasetNodes.getLength(); ++j) {
                    Node datasetNode = datasetNodes.item(j);

                    if ("datasets".equals(datasetNode.getNodeName()))
                        loadItems(datasetNode, analysis);
                    else if ("#text".equals(datasetNode.getNodeName())) {
                        if (!datasetNode.getNodeValue().trim().isEmpty())
                            throw new RuntimeException("unexpected text content: " + datasetNode.getNodeValue());
                    }
                    else
                        throw new RuntimeException("invalid child node: " + datasetNode.getNodeName());
                }
            }
            else if ("chartSheets".equals(node.getNodeName())) {
                int n = XmlUtils.getElementsWithTag(node, "chartSheets").size();
                if (n > 0)
                    errors.add("Chart sheets not supported, " + n + " chart sheet(s) were discarded.");
            }
            else if ("#text".equals(node.getNodeName())) {
                if (!node.getNodeValue().trim().isEmpty())
                    throw new RuntimeException("unexpected text content: " + node.getNodeValue());
            }
            else {
                throw new RuntimeException("invalid child node: " + node.getNodeName());
            }
        }

        return analysis;
    }

}
