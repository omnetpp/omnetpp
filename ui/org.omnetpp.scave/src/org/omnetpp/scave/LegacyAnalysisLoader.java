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

public class LegacyAnalysisLoader {

    public static final String BARCHART_ID = "barchart_native";
    public static final String LINECHART_ID = "linechart_native";
    public static final String HISTOGRAMCHART_ID = "histogramchart_native";
    public static final String SCATTERCHART_ID = "scatterchart_legacy";


    static class DataOp {
        String op;
        String filter;
        String type;

        DataOp(String op, String filter, String type) {
            this.op = op;
            this.filter = filter;
            this.type = type;
        }
    }

    static class DataVecOp {
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

    static class DataFilter {
        String type;
        String filter;

        DataFilter(String type, String filter) {
            this.type = type;
            this.filter = filter;
        }
    }

    private ChartTemplateRegistry chartTemplateRegistry;
    private ArrayList<String> errors;

    public LegacyAnalysisLoader(ChartTemplateRegistry chartTemplateRegistry, ArrayList<String> errors) {
        this.chartTemplateRegistry = chartTemplateRegistry;
        this.errors = errors;
    }

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
            setProperty(chart, "Plot.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));

            ArrayList<String> bars = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "barFields");
            setProperty(chart, "series", bars.isEmpty() ? "name,measurement" : StringUtils.join(bars, ","));

            ArrayList<String> groups = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "groupByFields");
            setProperty(chart, "groups", groups.isEmpty() ? "module,experiment" : StringUtils.join(groups, ","));

            // NOTE: averagedFields elements are not used, because they belong "to the rest"
        }
        else if ("scave:HistogramChart".equals(chartType)) {
            setProperty(chart, "Plot.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));
        }
        else if ("scave:ScatterChart".equals(chartType)) {
            String xDataPattern = chartNode.getAttributes().getNamedItem("xDataPattern").getNodeValue();
            xDataPattern = FilterUtil.translateLegacyFilterExpression(xDataPattern);
            Node avgRepls = chartNode.getAttributes().getNamedItem("averageReplications");
            String averageReplications = avgRepls != null ? avgRepls.getNodeValue() : "true";
            ArrayList<String> isoDataPatterns = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "isoDataPattern");
            for (int i = 0; i < isoDataPatterns.size(); ++i)
                isoDataPatterns.set(i, FilterUtil.translateLegacyFilterExpression(isoDataPatterns.get(i)));

            setProperty(chart, "Plot.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));
            setProperty(chart, "x_pattern", xDataPattern);
            setProperty(chart, "iso_patterns", StringUtils.join(isoDataPatterns, ";"));
            setProperty(chart, "average_replications", averageReplications);
        }
        else if ("scave:LineChart".equals(chartType)) {
            setProperty(chart, "Plot.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));

            ArrayList<String> vecOpStrings = new ArrayList<String>();
            for (DataVecOp vo : vecOps)
                vecOpStrings.add(vo.toString());

            setProperty(chart, "vector_operations", StringUtils.join(vecOps, "\n"));
        }
        else
            throw new RuntimeException("unknown chart type: " + chartType);

        return chart;
    }

    private void setProperty(Chart chart, String name, String value) {
        Property prop = chart.getProperty(name);
        if (prop == null) {
            prop = new Property(name, value);
            chart.addProperty(prop);
        } else
            prop.setValue(value);
    }

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

    private void loadItems(Node parentNode, Analysis analysis) {
        loadItems(parentNode, analysis, new ArrayList<DataOp>(), new ArrayList<DataVecOp>());
    }

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

    public Analysis loadLegacyAnalysis(Node rootNode) {

        Analysis analysis = new Analysis();

        NodeList topLevelNodes = rootNode.getChildNodes();

        // TODO: add proper error reporting

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
