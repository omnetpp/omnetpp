package org.omnetpp.simulation.editors;

import java.io.File;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.simulation.SimulationPlugin;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

/**
 *
 * @author Andras
 */
public class InspectorList {
    private String id;
    private List<InspectorMemento> inspectorMementos;

    public InspectorList(String id, List<InspectorMemento> inspectorMementos) {
        this.id = id;
        this.inspectorMementos = inspectorMementos;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public List<InspectorMemento> getItems() {
        return inspectorMementos;
    }

    /**
     * Saves the inspector list to file.
     */
    public void save() throws CoreException {
        try {
            // build DOM tree
            DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            Document doc = docBuilder.newDocument();
            Element root = doc.createElement("inspectors");
            root.setAttribute("version", "5.0");
            doc.appendChild(root);

            for (InspectorMemento inspectorMemento : inspectorMementos) {
                Element e = doc.createElement("inspector");
                e.setAttribute("objectFullPath", inspectorMemento.objectFullPath);
                e.setAttribute("objectClassName", inspectorMemento.objectClassName);
                e.setAttribute("inspectorId", inspectorMemento.inspectorId);
                Rectangle r = inspectorMemento.bounds;
                e.setAttribute("bounds", r.width + "x" + r.height + "+" + r.x + "+" + r.y);
                root.appendChild(e);
            }

            // serialize
            TransformerFactory factory = TransformerFactory.newInstance();
            Transformer transformer = factory.newTransformer();
            transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            StringWriter writer = new StringWriter();
            transformer.transform(new DOMSource(doc), new StreamResult(writer));
            String content = writer.toString();

            // save it
            File file = getStorageFileFor(id);
            FileUtils.writeTextFile(file, content, "UTF8");
        }
        catch (Exception e) {
            throw SimulationPlugin.wrapIntoCoreException("Cannot save inspector list for id=" + id, e);
        }
    }

    public static InspectorList read(String id) throws CoreException {
        File file = null;
        try {
            file = getStorageFileFor(id);
            if (!file.isFile())
                return null;
            DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            Document doc = docBuilder.parse(file);
            Element root = doc.getDocumentElement();
            NodeList elements = root.getElementsByTagName("inspector");
            List<InspectorMemento> inspectorMementos = new ArrayList<InspectorMemento>();
            for (int i=0; i<elements.getLength(); i++) {
                Element e = (Element)elements.item(i);
                InspectorMemento inspectorMemento = new InspectorMemento(
                        e.getAttribute("objectFullPath"), e.getAttribute("objectClassName"),
                        e.getAttribute("inspectorId"), parseRect(e.getAttribute("bounds")));
                inspectorMementos.add(inspectorMemento);
            }
            return new InspectorList(id, inspectorMementos);
        }
        catch (Exception e) {
            throw SimulationPlugin.wrapIntoCoreException("Cannot load inspector list for id=" + id + " from " + file, e);
        }
    }

    private static File getStorageFileFor(String id) {
        IPath pluginStateLocation = SimulationPlugin.getDefault().getStateLocation();
        String fname = id + ".insp";
        fname = fname.replaceAll("[^ -~]", ""); // remove non-ascii and non-printable chars
        fname = fname.replaceAll("[/\\:?*]", ""); // remove chars typically forbidden in file names
        return pluginStateLocation.append(fname).toFile();
    }

    private static Rectangle parseRect(String attr) {
        // format: width x height + x + y
        String s[] = attr.split("[+x]");
        return new Rectangle(Integer.parseInt(s[2]), Integer.parseInt(s[3]), Integer.parseInt(s[0]), Integer.parseInt(s[1]));
    }

    @Override
    public String toString() {
        return id;
    }
}
