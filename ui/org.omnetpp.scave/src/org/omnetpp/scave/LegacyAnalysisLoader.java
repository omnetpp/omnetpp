package org.omnetpp.scave;

import java.util.ArrayList;

import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.XmlUtils;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelFactory;
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
        for (DataOp o : ops)
            if (opsExpr.isEmpty())
                opsExpr = "(" + o.filter + ")";
            else // TODO: take operation kind into account
                opsExpr = opsExpr + "\n  OR (" + o.filter + ")";

        String filtersExpr = "";
        for (DataFilter f : filters)
            if (filtersExpr.isEmpty())
                filtersExpr = "(" + f.filter + ")";
            else // TODO: take filter type into account
                filtersExpr = filtersExpr + "\n  OR (" + f.filter + ")";

        return "(" + opsExpr + ") AND (" + filtersExpr + ")";
    }

    private static String makeHistogramChartScript(Chart chart, Node chartNode, ArrayList<DataOp> ops, ArrayList<DataFilter> filters) {
        StringBuilder sb = new StringBuilder();

        sb.append("import results\n\n");

        String filter = makeFilterString(ops, filters);

        sb.append("df = results.getHistograms(\"\"\"" + filter + "\"\"\")\n\n");

        sb.append("chart.plotHistograms(df)\n\n");

        sb.append("chart.setProperty('Graph.Title', chart.getName())\n\n");

        sb.append("chart.setProperties(chart.getProperties())\n");

        return sb.toString();
    }


    private static String makeScatterChartScript(Chart chart, Node chartNode, ArrayList<DataOp> ops, ArrayList<DataFilter> filters, ArrayList<String> errors) {
        StringBuilder sb = new StringBuilder();

        String filter = makeFilterString(ops, filters);

        sb.append("import results\n\n");

        sb.append("filter_string = \"\"\"" + filter + "\"\"\"\n\n");
        sb.append("df = results.getScalars(filter_string)\n\n");

        // sb.append("pd.set_option('display.width', 180)\n");
        // sb.append("pd.set_option('display.max_colwidth', 100)\n");

        Node xDataNode = chartNode.getAttributes().getNamedItem("xDataPattern");

        if (xDataNode == null)
            throw new RuntimeException("No xDataPattern in scatter chart " + chart.getName());

        String xdata = StringUtils.substringBefore(StringUtils.substringAfter(xDataNode.getNodeValue(), "name("), ")");

        String iso_column = null;

        NodeList children = chartNode.getChildNodes();
        for (int i = 0; i < children.getLength(); ++i) {
            Node child = children.item(i);
            if ("isoDataPattern".equals(child.getNodeName())) {
                if (iso_column == null)
                    iso_column = child.getTextContent();
                else {
                    // TODO: handle better
                    Debug.println("Warning: More than one isoDataPattern.");
                    errors.add(chart.getName() + ": More than one isoDataPattern found, this not yet supported, second and further ones were dropped.");
                    break;
                }
            }
        }

        if (iso_column != null)
            iso_column = StringUtils.substringBefore(StringUtils.substringAfter(iso_column, "name("), ")");

        sb.append("xdata = '" + xdata + "'\n");
        sb.append("iso_column = " + (iso_column == null ? "None" : ("'" + iso_column.replace("(*)", "") + "'")));
        sb.append("\n\n");

        sb.append("names = chart.plotScatter(df, xdata=xdata, iso_column=iso_column)\n\n");

        sb.append("chart.setProperty('Y.Axis.Title', ', '.join(names))\n");
        sb.append("chart.setProperty('X.Axis.Title', '" + xdata + "')\n");
        sb.append("chart.setProperty('Graph.Title', chart.getName())\n\n");

        sb.append("chart.setProperties(chart.getProperties())\n");

        return sb.toString();
    }

    private static String makeLineChartScript(Chart chart, Node chartNode, ArrayList<DataOp> ops, ArrayList<DataFilter> filters) {
        StringBuilder sb = new StringBuilder();

        String filter = makeFilterString(ops, filters);

        sb.append("import results\n\n");

        sb.append("filter_string = \"\"\"" + filter + "\"\"\"\n\n");

        sb.append("df = results.getVectors(filter_string)\n\n");

        // sb.append("pd.set_option('display.width', 180)\n");
        // sb.append("pd.set_option('display.max_colwidth', 100)\n");

        sb.append("chart.plotVectors(df)\n\n");

        sb.append("chart.setProperty('X.Axis.Title', 'Time')\n");
        sb.append("chart.setProperty('Graph.Title', chart.getName())\n\n");

        sb.append("chart.setProperties(chart.getProperties())\n");

        return sb.toString();
    }

    private static String makeBarChartScript(Chart chart, Node chartNode, ArrayList<DataOp> ops, ArrayList<DataFilter> filters) {
        StringBuilder sb = new StringBuilder();

        String filter = makeFilterString(ops, filters);

        sb.append("import results\n\n");

        sb.append("filter_string = \"\"\"" + filter + "\"\"\"\n\n");

        sb.append("df = results.getScalars(filter_string)\n\n");

        // sb.append("pd.set_option('display.width', 180)\n");
        // sb.append("pd.set_option('display.max_colwidth', 100)\n");

        sb.append("chart.plotScalars(df)\n\n");

        sb.append("chart.setProperty('Graph.Title', chart.getName())\n\n");

        sb.append("chart.setProperties(chart.getProperties())\n");

        return sb.toString();
    }

    private static Chart makeLegacyChart(ArrayList<DataOp> ops, String datasetName, Node chartNode, String chartType, ArrayList<String> errors) {
        Chart chart = null;

        if ("scave:BarChart".equals(chartType))
            chart = factory.createBarChart();
        else if ("scave:HistogramChart".equals(chartType))
            chart = factory.createHistogramChart();
        else if ("scave:ScatterChart".equals(chartType))
            chart = factory.createScatterChart();
        else if ("scave:LineChart".equals(chartType))
            chart = factory.createLineChart();
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


        if ("scave:BarChart".equals(chartType))
            chart.setScript(makeBarChartScript(chart, chartNode, ops, filters));
        else if ("scave:HistogramChart".equals(chartType))
            chart.setScript(makeHistogramChartScript(chart, chartNode, ops, filters));
        else if ("scave:ScatterChart".equals(chartType))
            chart.setScript(makeScatterChartScript(chart, chartNode, ops, filters, errors));
        else if ("scave:LineChart".equals(chartType))
            chart.setScript(makeLineChartScript(chart, chartNode, ops, filters));
        else
            throw new RuntimeException("unknown chart type: " + chartType);

        return chart;
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

                    if ("datasets".equals(datasetNode.getNodeName())) {
                        Node nameNode = datasetNode.getAttributes().getNamedItem("name");

                        String datasetName = nameNode.getNodeValue();
                        NodeList itemNodes = datasetNode.getChildNodes();

                        ArrayList<DataOp> ops = new ArrayList<DataOp>();

                        for (int k = 0; k < itemNodes.getLength(); k++) {
                            Node itemNode = itemNodes.item(k);
                            if ("items".equals(itemNode.getNodeName())) {
                                Node chartTypeNode = itemNode.getAttributes().getNamedItem("xsi:type");

                                String itemType = chartTypeNode.getNodeValue();

                                // these are null for charts
                                Node filterNode = itemNode.getAttributes().getNamedItem("filterPattern");
                                Node opTypeNode = itemNode.getAttributes().getNamedItem("type");
                                String opType = "SCALAR";
                                if (opTypeNode != null)
                                    opType = opTypeNode.getNodeValue();

                                if ("scave:Add".equals(itemType))
                                    ops.add(new DataOp("Add", filterNode.getNodeValue(), opType));
                                else if ("scave:BarChart".equals(itemType) || "scave:HistogramChart".equals(itemType) || "scave:ScatterChart".equals(itemType) || "scave:LineChart".equals(itemType))
                                    analysis.getCharts().getItems().add(makeLegacyChart(ops, datasetName, itemNode, itemType, errors));
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
                    else if ("#text".equals(datasetNode.getNodeName())) {
                        if (!datasetNode.getNodeValue().trim().isEmpty())
                            throw new RuntimeException("unexpected text content: " + datasetNode.getNodeValue());
                    } else {
                        throw new RuntimeException("invalid child node: " + datasetNode.getNodeName());
                    }
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
