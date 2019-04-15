package org.omnetpp.scave;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class AnalysisLoader {

    private static final ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

    private static Element getFirstElementByName(Node parent, String type) {
        NodeList children = parent.getChildNodes();

        for (int k = 0; k < children.getLength(); ++k) {
            Node childNode = children.item(k);
            if (childNode instanceof Element && type.equals(childNode.getNodeName()))
                return (Element)childNode;
        }

        return null;
    }

    public static Analysis loadNewAnalysis(Node rootNode) {

        Analysis analysis = factory.createAnalysis();
        analysis.setInputs(factory.createInputs());
        analysis.setCharts(factory.createCharts());

        Node versionNode = rootNode.getAttributes().getNamedItem("version");

        if (versionNode == null)
            throw new RuntimeException("unspecified analysis file version");

        if (!"2".equals(versionNode.getNodeValue()))
            throw new RuntimeException("invalid analysis file version: " + versionNode.getNodeValue());

        NodeList topLevelNodes = rootNode.getChildNodes();

        for (int i = 0; i < topLevelNodes.getLength(); ++i) {
            Node node = topLevelNodes.item(i);

            if ("inputs".equals(node.getNodeName())) {
                NodeList inputNodes = node.getChildNodes();
                for (int j = 0; j < inputNodes.getLength(); ++j) {
                    Node inputNode = inputNodes.item(j);
                    if ("input".equals(inputNode.getNodeName())) {
                        InputFile input = factory.createInputFile();

                        Node patternNode = inputNode.getAttributes().getNamedItem("pattern");

                        input.setName(patternNode.getNodeValue());

                        analysis.getInputs().getInputs().add(input);
                    } else if ("#text".equals(inputNode.getNodeName())) {
                        if (!inputNode.getNodeValue().trim().isEmpty())
                            throw new RuntimeException("unexpected text content: " + inputNode.getNodeValue());
                    } else {
                        throw new RuntimeException("invalid child node: " + inputNode.getNodeName());
                    }
                }
            }
            else if ("charts".equals(node.getNodeName())) {
                NodeList chartNodes = node.getChildNodes();

                for (int j = 0; j < chartNodes.getLength(); ++j) {
                    Node chartNode = chartNodes.item(j);

                    if ("chart".equals(chartNode.getNodeName())) {

                        Node typeNode = chartNode.getAttributes().getNamedItem("type");

                        String chartType = typeNode.getNodeValue();

                        Chart chart;
                        if ("MatplotlibChart".equals(chartType))
                            chart = factory.createMatplotlibChart();
                        else if ("BarChart".equals(chartType))
                            chart = factory.createBarChart();
                        else if ("LineChart".equals(chartType))
                            chart = factory.createLineChart();
                        else if ("ScatterChart".equals(chartType))
                            chart = factory.createScatterChart();
                        else if ("HistogramChart".equals(chartType))
                            chart = factory.createHistogramChart();
                        else
                            throw new RuntimeException("unknown chart type: " + chartType);

                        chart.setName(chartNode.getAttributes().getNamedItem("name").getNodeValue());

                        Node templateNode = chartNode.getAttributes().getNamedItem("template");
                        if (templateNode != null)
                            chart.setTemplateID(templateNode.getNodeValue());

                        Element scriptElement = getFirstElementByName(chartNode, "script");
                        Element formElement = getFirstElementByName(chartNode, "form");

                        Node scriptAttrNode = chartNode.getAttributes().getNamedItem("script");
                        if (scriptAttrNode != null)
                            chart.setScript(scriptAttrNode.getNodeValue());
                        else {
                            if (scriptElement != null) {
                                chart.setScript(scriptElement.getTextContent());
                                if (formElement != null)
                                    chart.setForm(formElement.getTextContent());
                            }
                            else
                                chart.setScript(StringUtils.stripEnd(chartNode.getTextContent(), " "));
                        }

                        NodeList props = chartNode.getChildNodes();
                        for (int k = 0; k < props.getLength(); ++k) {
                            Node propNode = props.item(k);
                            if ("property".equals(propNode.getNodeName())) {
                                Property prop = factory.createProperty();
                                prop.setName(propNode.getAttributes().getNamedItem("name").getNodeValue());
                                prop.setValue(propNode.getAttributes().getNamedItem("value").getNodeValue());
                                chart.getProperties().add(prop);
                            }
                        }

                        analysis.getCharts().getItems().add(chart);
                    }
                    else if ("#text".equals(chartNode.getNodeName())) {
                        if (!chartNode.getNodeValue().trim().isEmpty())
                            throw new RuntimeException("unexpected text content: " + chartNode.getNodeValue());
                    } else {
                        throw new RuntimeException("invalid child node: " + chartNode.getNodeName());
                    }
                }
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
