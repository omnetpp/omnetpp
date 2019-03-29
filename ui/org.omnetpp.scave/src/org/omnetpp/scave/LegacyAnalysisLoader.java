package org.omnetpp.scave;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.XmlUtils;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.python.ChartTemplateRegistry;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class LegacyAnalysisLoader {

    private static final ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

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

        DataVecOp(String type, String operation, Map<String, String> params) {
            this.type = type;
            this.operation = operation;
            this.params = params;
        }

        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append(type);
            sb.append(":");
            sb.append(operation);

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


    private static String makeFilterString(ArrayList<DataOp> ops, ArrayList<DataFilter> filters) {
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

    private static Chart makeLegacyChart(ArrayList<DataOp> ops, ArrayList<DataVecOp> vecOps, String datasetName, Node chartNode, String chartType, ArrayList<String> errors) {
        Chart chart = null;

        if ("scave:BarChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(ChartTemplateRegistry.barChartInternal);
        else if ("scave:HistogramChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(ChartTemplateRegistry.histogramChart);
        else if ("scave:ScatterChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(ChartTemplateRegistry.scatterChartInternal);
        else if ("scave:LineChart".equals(chartType))
            chart = ScaveModelUtil.createChartFromTemplate(ChartTemplateRegistry.lineChartInternal);
        else
            throw new RuntimeException("unknown chart type: " + chartType);

        chart.setName(chartNode.getAttributes().getNamedItem("name").getNodeValue());

        ArrayList<DataFilter> filters = new ArrayList<DataFilter>();

        NodeList props = chartNode.getChildNodes();

        for (int k = 0; k < props.getLength(); ++k) {
            Node propNode = props.item(k);
            if ("properties".equals(propNode.getNodeName())) {
                Property prop = factory.createProperty();
                prop.setName(propNode.getAttributes().getNamedItem("name").getNodeValue());
                prop.setValue(propNode.getAttributes().getNamedItem("value").getNodeValue());
                chart.getProperties().add(prop);
            } else if ("filters".equals(propNode.getNodeName())) {
                filters.add(new DataFilter(propNode.getAttributes().getNamedItem("xsi:type").getNodeValue(), propNode.getAttributes().getNamedItem("filterPattern").getNodeValue()));
            }
        }


        if ("scave:BarChart".equals(chartType)) {
            setProperty(chart, "Graph.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));

            ArrayList<String> bars = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "barFields");
            setProperty(chart, "bars", StringUtils.join(bars, ","));

            ArrayList<String> groups = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "groupByFields");
            setProperty(chart, "groups", StringUtils.join(groups, ","));

            // NOTE: averagedFields elements are not used, because they belong "to the rest"
        }
        else if ("scave:HistogramChart".equals(chartType)) {
            setProperty(chart, "Graph.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));
        }
        else if ("scave:ScatterChart".equals(chartType)) {
            String xDataPattern = chartNode.getAttributes().getNamedItem("xDataPattern").getNodeValue();
            Node avgRepls = chartNode.getAttributes().getNamedItem("averageReplications");
            String averageReplications = avgRepls != null ? avgRepls.getNodeValue() : "true";
            ArrayList<String> isoDataPatterns = XmlUtils.collectChildTextsFromElementsWithTag(chartNode, "isoDataPattern");

            setProperty(chart, "Graph.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));
            setProperty(chart, "x_pattern", xDataPattern);
            setProperty(chart, "iso_patterns", StringUtils.join(isoDataPatterns, ";"));
            setProperty(chart, "average_replications", averageReplications);
        }
        else if ("scave:LineChart".equals(chartType)) {
            setProperty(chart, "Graph.Title", chart.getName());
            setProperty(chart, "filter", makeFilterString(ops, filters));

            ArrayList<String> vecOpStrings = new ArrayList<String>();
            for (DataVecOp vo : vecOps)
                vecOpStrings.add(vo.toString());

            setProperty(chart, "vector_operations", StringUtils.join(vecOps, ";"));
        }
        else
            throw new RuntimeException("unknown chart type: " + chartType);

        return chart;
    }

    private static void setProperty(Chart chart, String name, String value) {
        Property prop = ScaveModelUtil.getChartProperty(chart, name);
        if (prop == null) {
            prop = factory.createProperty();
            chart.getProperties().add(prop);
            prop.setName(name);
        }
        prop.setValue(value);
    }

    static Map<String, String> extractVectorOpParams(Node operationNode) {
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

    public static void loadItems(Node parentNode, Analysis analysis, ArrayList<String> errors) {
        loadItems(parentNode, analysis, errors, new ArrayList<DataOp>(), new ArrayList<DataVecOp>());
    }

    public static void loadItems(Node parentNode, Analysis analysis, ArrayList<String> errors, ArrayList<DataOp> startingOps, ArrayList<DataVecOp> startingVecOps) {

        Node nameNode = parentNode.getAttributes().getNamedItem("name");

        String datasetName = nameNode.getNodeValue();
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
                    ops.add(new DataOp("scave:Add".equals(itemType) ? "add" : "discard", filterNode.getNodeValue(), opType));
                }
                else if ("scave:Apply".equals(itemType) || "scave:Compute".equals(itemType)) {
                    Node operationNode = itemNode.getAttributes().getNamedItem("operation");
                    vecOps.add(new DataVecOp("scave:Apply".equals(itemType) ? "apply" : "compute", operationNode.getNodeValue(), extractVectorOpParams(itemNode)));
                }
                else if ("scave:BarChart".equals(itemType) || "scave:HistogramChart".equals(itemType) || "scave:ScatterChart".equals(itemType) || "scave:LineChart".equals(itemType))
                    analysis.getCharts().getItems().add(makeLegacyChart(ops, vecOps, datasetName, itemNode, itemType, errors));
                else if ("scave:Group".equals(itemType)) {
                    loadItems(itemNode, analysis, errors, ops, vecOps);
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

    public static Analysis loadLegacyAnalysis(Node rootNode, ArrayList<String> errors) {

        Analysis analysis = factory.createAnalysis();
        analysis.setInputs(factory.createInputs());
        analysis.setCharts(factory.createCharts());

        NodeList topLevelNodes = rootNode.getChildNodes();

        // TODO: add proper error reporting

        for (int i = 0; i < topLevelNodes.getLength(); ++i) {
            Node node = topLevelNodes.item(i);

            if ("inputs".equals(node.getNodeName())) {
                NodeList inputNodes = node.getChildNodes();
                for (int j = 0; j < inputNodes.getLength(); ++j) {
                    Node inputNode = inputNodes.item(j);
                    if ("inputs".equals(inputNode.getNodeName())) {
                        InputFile input = factory.createInputFile();

                        Node nameNode = inputNode.getAttributes().getNamedItem("name");

                        input.setName(nameNode.getNodeValue());

                        analysis.getInputs().getInputs().add(input);
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
                        loadItems(datasetNode, analysis, errors);
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
