package org.omnetpp.scave;

import java.io.ByteArrayInputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.XmlUtils;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class AnalysisSaver {
    public static void saveAnalysis(Analysis analysis, IFile file) throws CoreException {

        DocumentBuilder db;
        try {
            db = DocumentBuilderFactory.newInstance().newDocumentBuilder();

            Document d = db.newDocument();

            Element anal = d.createElement("analysis");
            anal.setAttribute("version", "2");
            d.appendChild(anal);

            Element inputs = d.createElement("inputs");
            anal.appendChild(inputs);

            Element charts = d.createElement("charts");
            anal.appendChild(charts);

            for (InputFile i : analysis.getInputs().getInputs()) {
                Element elem = d.createElement("input");
                inputs.appendChild(elem);
                elem.setAttribute("pattern", i.getName());
            }

            for (AnalysisItem a : analysis.getCharts().getCharts()) {
                if (a instanceof Chart) {
                    Chart chart = (Chart) a;
                    Element chartElem = d.createElement("chart");
                    charts.appendChild(chartElem);

                    chartElem.setAttribute("id", Integer.toString(chart.getId()));
                    chartElem.setAttribute("name", chart.getName());
                    chartElem.setAttribute("template", chart.getTemplateID());

                    Element script = d.createElement("script");
                    script.appendChild(d.createCDATASection(chart.getScript()));
                    chartElem.appendChild(script);

                    for (DialogPage page : chart.getDialogPages()) {
                        Element pg = d.createElement("dialogPage");
                        pg.setAttribute("id", page.id);
                        pg.setAttribute("label", page.label);
                        pg.appendChild(d.createCDATASection(page.xswtForm));
                        chartElem.appendChild(pg);
                    }

                    chartElem.setAttribute("type", chart.getType().toString());
                    chartElem.setAttribute("icon", chart.getIconPath());

                    for (Property p : chart.getProperties()) {
                        Element e = d.createElement("property");
                        e.setAttribute("name", p.getName());
                        e.setAttribute("value", p.getValue());
                        chartElem.appendChild(e);
                    }
                } else {
                    // TODO: handle better
                    Debug.println("Analysis item '" + a.getName()
                            + "' is not a chart, ignored (dropped) upon saving");
                }
            }

            String content = XmlUtils.serialize(d);
            if (!file.exists())
                file.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
            else
                file.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
        } catch (ParserConfigurationException | TransformerException e) {
            throw ScavePlugin.wrapIntoCoreException(e);
        }
    }
}
