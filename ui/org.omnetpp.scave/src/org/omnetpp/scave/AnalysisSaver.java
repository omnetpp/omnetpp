/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave;

import java.io.ByteArrayInputStream;
import java.util.HashMap;
import java.util.Map;

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
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/**
 * Save the Analysis models into an Eclipse IFile, as an XML-based .anf file.
 */
public class AnalysisSaver {

    /**
     * Serializes and writes an analysis model into a file.
     */
    public static void saveAnalysis(Analysis analysis, IFile file) throws CoreException {
        saveAnalysis(analysis, file, new HashMap<Chart, String>());
    }

    /**
     * Serializes and writes an analysis model into a file, replacing the scripts
     * of some Charts with the contents given in editedChartScripts.
     *
     * This is used when the Analysis is open in a ScaveEditor, and there
     * are some Charts open in ChartScriptEditors, with edited script contents.
     * These ChartSctriptEditors are not marked as "dirty", but the script
     * in them is not yet written back into the Chart object itself, to avoid
     * spamming the "main" (model) CommandStack of the ScaveEditor.
     */
    public static void saveAnalysis(Analysis analysis, IFile file, Map<Chart, String> editedChartScripts) throws CoreException {

        DocumentBuilder db;
        try {
            db = DocumentBuilderFactory.newInstance().newDocumentBuilder();

            Document document = db.newDocument();

            Element anal = document.createElement("analysis");
            anal.setAttribute("version", "2");
            document.appendChild(anal);

            Element inputs = document.createElement("inputs");
            anal.appendChild(inputs);

            Element charts = document.createElement("charts");
            anal.appendChild(charts);

            for (InputFile i : analysis.getInputs().getInputs()) {
                Element elem = document.createElement("input");
                inputs.appendChild(elem);
                elem.setAttribute("pattern", i.getName());
            }

            for (AnalysisItem item : analysis.getRootFolder().getItems())
                saveAnalysisItem(item, document, charts, editedChartScripts);

            String content = XmlUtils.serialize(document);
            if (!file.exists())
                file.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
            else
                file.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
        }
        catch (ParserConfigurationException | TransformerException e) {
            throw ScavePlugin.wrapIntoCoreException(e);
        }
    }

    protected static void saveAnalysisItem(AnalysisItem item, Document document, Element parent, Map<Chart, String> editedChartScripts) {
            if (item instanceof Chart) {
                Chart chart = (Chart) item;
                Element chartElem = document.createElement("chart");
                parent.appendChild(chartElem);

                chartElem.setAttribute("id", Integer.toString(chart.getId()));
                chartElem.setAttribute("name", chart.getName());
                chartElem.setAttribute("template", chart.getTemplateID());

                Element scriptElem = document.createElement("script");
                String script = editedChartScripts.containsKey(chart) ? editedChartScripts.get(chart) : chart.getScript();
                scriptElem.appendChild(document.createCDATASection(script));
                chartElem.appendChild(scriptElem);

                for (DialogPage page : chart.getDialogPages()) {
                    Element pg = document.createElement("dialogPage");
                    pg.setAttribute("id", page.id);
                    pg.setAttribute("label", page.label);
                    pg.appendChild(document.createCDATASection(page.xswtForm));
                    chartElem.appendChild(pg);
                }

                chartElem.setAttribute("type", chart.getType().toString());
                chartElem.setAttribute("icon", chart.getIconPath());

                for (Property p : chart.getProperties()) {
                    Element e = document.createElement("property");
                    e.setAttribute("name", p.getName());
                    e.setAttribute("value", p.getValue());
                    chartElem.appendChild(e);
                }
            }
            else if (item instanceof Folder) {
                Folder folder = (Folder) item;
                Element folderElem = document.createElement("folder");
                parent.appendChild(folderElem);
                folderElem.setAttribute("id", Integer.toString(folder.getId()));
                folderElem.setAttribute("name", folder.getName());

                for (AnalysisItem child : folder.getItems())
                    saveAnalysisItem(child, document, folderElem, editedChartScripts);
            }
            else {
                Debug.println("Unsupported analysis item type '" + item.getClass().getSimpleName() + "', not saved");
            }
    }
}
